//--------------------------------------------------------------
// COOP Generation 7.000 - Gamemode based on mp_modeTeamDM - chrissstrahl
//--------------------------------------------------------------

#ifdef ENABLE_COOP

#include "../../dlls/game/_pch_cpp.h"
#include "../../dlls/game/mp_manager.hpp"
#include "../../dlls/game/player.h"
#include "../../dlls/game/gamefix.hpp"
#include "coop_config.hpp"
#include "coop_gametype.hpp"
#include "coop_manager.hpp"
#include "coop_class.hpp"


CLASS_DECLARATION(ModeTeamDeathmatch, ModeCoop, NULL)
{
	{ NULL, NULL }
};

ModeCoop::ModeCoop()
{
	//_redTeam = AddTeam("Red");
	_redTeam = nullptr;
	_blueTeam = AddTeam("Blue");
	//_useTeamSpawnpoints = false;
}

ModeCoop::~ModeCoop()
{
}

//Executed each level load in multiplayer
//Executed from: MultiplayerManager::initMultiplayerGame()
void ModeCoop::init(int maxPlayers)
{
	//we don't want any time or point limit for now
	mp_pointlimit->integer = 0;
	mp_timelimit->integer = 0;

	ModeTeamBase::init(maxPlayers);
	readMultiplayerConfig(_COOP_FILE_spawninventory);

	//allow 3rd person aiming
	gi.cvar_set("g_aimviewangles", "1");

	gi.Printf("ModeCoop: Coop gametype initialized.\n");
}

//Executed from: MultiplayerManager::initItems - only if gamemode is coop
//Related: AwardSystem::initItems
void ModeCoop::initItems(void)
{
	// Setup spawn points
	getSpawnpoints();
	resetSpawnpoints();

	// Setup the start time
	_matchStartTime = multiplayerManager.getTime();
	_gameStartTime = multiplayerManager.getTime();

	_played5MinWarning = false;
	_played2MinWarning = false;
	_played1MinWarning = false;
}

bool ModeCoop::isEndOfMatch(void)
{
	// See if we have a gone over the point limit
	if (getPointLimit() > 0){
		if (_redTeam->getPoints() >= getPointLimit())
			return true;
		else if (_blueTeam->getPoints() >= getPointLimit())
			return true;
	}

	// See if we have a gone over the time limit
	if ((getTimeLimit() > 0.0f) && (multiplayerManager.getTime() - _gameStartTime > getTimeLimit()))
		return true;

	return false;
}

void ModeCoop::_giveInitialConditions(Player* player)
{
	//_weaponList - was  
	int idx;
	// Give all the weapons to the player
	for (idx = 1; idx <= _weaponList.NumObjects(); idx++){
		multiplayerManager.givePlayerItem(player->entnum, _weaponList.ObjectAt(idx));
	}

	// Give all the ammo to the player
	for (idx = 1; idx <= _ammoList.NumObjects(); idx++){
		Event* ev = new Event("ammo");
		ev->AddString(_ammoList.ObjectAt(idx)->type);
		ev->AddInteger(_ammoList.ObjectAt(idx)->amount);
		player->ProcessEvent(ev); // deletes the created Event
	}

	// Start the player with the appropriate weapon
	if (_startingWeaponName.length()){
		Event* ev = new Event("use");
		ev->AddString(_startingWeaponName);
		player->ProcessEvent(ev); // deletes the created Event
	}
}

float ModeCoop::playerDamaged(Player* damagedPlayer, Player* attackingPlayer, float damage, int meansOfDeath)
{
	// Always take telefrag damage

	if (meansOfDeath == MOD_TELEFRAG || meansOfDeath == MOD_FALLING)
		return damage;

	// Player can always hurt himself

	if (damagedPlayer == attackingPlayer)
		return damage;

	if (attackingPlayer) {
		multiplayerManager.gameFixAPI_getMultiplayerAwardSystem()->playerFired(attackingPlayer);
	}

	if (!attackingPlayer) {
		damage = CoopManager::Get().getSkillBasedDamage(damage);
	}

	//return friendly fire multiplicator settings
	if (coopSettings.getSetting_friendlyFireMultiplicator() != 0.0f) {
		return damage *= coopSettings.getSetting_friendlyFireMultiplicator();
	}

	// If on same team and not allowing team damage
	if ((_playerGameData[damagedPlayer->entnum]._currentTeam == _playerGameData[attackingPlayer->entnum]._currentTeam) &&
		(!multiplayerManager.checkFlag(MP_FLAG_FRIENDLY_FIRE)))
		return 0;
	else
		return damage;
}

void ModeCoop::playerKilled(Player* killedPlayer, Player* attackingPlayer, Entity* inflictor, int meansOfDeath)
{
	Team* team;
	int points;
	bool goodKill;

	if (attackingPlayer && (killedPlayer != attackingPlayer) &&
		(_playerGameData[attackingPlayer->entnum]._currentTeam != _playerGameData[killedPlayer->entnum]._currentTeam))
		goodKill = true;
	else
		goodKill = false;

	//let the code know that this player is being respawned
	CoopManager::Get().setPlayerData_respawnMe(killedPlayer, true);

	//respawn at regular spawn or respawn location - MEANS OF DEATH
	if (meansOfDeath == MOD_VAPORIZE ||
		meansOfDeath == MOD_DROWN ||
		meansOfDeath == MOD_CRUSH ||
		meansOfDeath == MOD_CRUSH_EVERY_FRAME ||
		meansOfDeath == MOD_IMPALE ||
		meansOfDeath == MOD_UPPERCUT ||
		meansOfDeath == MOD_TELEFRAG ||
		meansOfDeath == MOD_LAVA ||
		meansOfDeath == MOD_SLIME ||
		meansOfDeath == MOD_FALLING ||
		meansOfDeath == MOD_ELECTRICWATER ||
		meansOfDeath == MOD_GAS ||
		meansOfDeath == MOD_VEHICLE ||
		meansOfDeath == MOD_GIB ||
		meansOfDeath == MOD_EAT ||
		meansOfDeath == MOD_VAPORIZE ||
		meansOfDeath == MOD_VAPORIZE_COMP ||
		meansOfDeath == MOD_VAPORIZE_DISRUPTOR ||
		meansOfDeath == MOD_VAPORIZE_PHOTON ||
		meansOfDeath == MOD_SNIPER)
	{
		CoopManager::Get().setPlayerData_respawnLocationSpawn(killedPlayer, true);
	}

	//respawn at regular spawn or respawn location - KILLED BY TRIGGER
	if(inflictor) {
		if ( Q_stricmp(inflictor->getClassname(), "TriggerHurt") == 0	||
			CoopManager::Get().entityUservarContains(inflictor, "badspot") > 0)
		{
			CoopManager::Get().setPlayerData_respawnLocationSpawn(killedPlayer, true);
		}
	}

	if (!CoopManager::Get().getPlayerData_respawnLocationSpawn(killedPlayer)) {
		Vector vView = killedPlayer->getViewAngles();
		vView[0] = 0.0f;
		vView[2] = 0.0f;
		CoopManager::Get().setPlayerData_lastValidViewAngle(killedPlayer, vView);
		CoopManager::Get().setPlayerData_lastValidLocation(killedPlayer, killedPlayer->origin);
	}


	coopClass.playerKilled(killedPlayer);
	handleKill(killedPlayer, attackingPlayer, inflictor, meansOfDeath, goodKill);
	//MultiplayerModeBase::playerKilled( killedPlayer, attackingPlayer, inflictor, meansOfDeath );


	if (!attackingPlayer)
		attackingPlayer = killedPlayer;

	team = _playerGameData[attackingPlayer->entnum]._currentTeam;

	if (team)
	{
		if (multiplayerManager.checkRule("team-pointsForKills", true, attackingPlayer))
		{
			// Setup the default points for this kill - 1 point to the attacking player's team (if victim's not 
			//   on same team), or lose 1 point if on the same team

			if (_playerGameData[killedPlayer->entnum]._currentTeam != _playerGameData[attackingPlayer->entnum]._currentTeam)
			{
				points = _defaultPointsPerKill;

				// Get the real points to give (the modifiers can change it)

				points = multiplayerManager.getPointsForKill(killedPlayer, attackingPlayer, inflictor, meansOfDeath, points);

				// Give points to the team

				team->addPoints(attackingPlayer, points);
			}
		}
	}
}

void ModeCoop::handleKill(Player* killedPlayer, Player* attackingPlayer, Entity* inflictor, int meansOfDeath, bool goodKill)
{
	if (!killedPlayer) {
		return;
	}

	// Record the death
	_playerGameData[killedPlayer->entnum]._numDeaths++;
	_playerGameData[killedPlayer->entnum]._lastKillerOfPlayerMOD = meansOfDeath;
	
	// Save off some important info
	if (attackingPlayer) {
		_playerGameData[attackingPlayer->entnum]._numKills++;
		_playerGameData[killedPlayer->entnum]._lastKillerOfPlayer = attackingPlayer->entnum;

		//Killed by different team or in deathmatch
		if (goodKill) {
			addPoints(attackingPlayer->entnum, multiplayerManager.getPointsForKill(killedPlayer, attackingPlayer, inflictor, meansOfDeath, _defaultPointsPerKill));
			_playerGameData[killedPlayer->entnum]._lastKillerOfPlayer = attackingPlayer->entnum;
		}
		else {
			addPoints(killedPlayer->entnum, -_COOP_SETTINGS_PLAYER_PENALTY_BADKILL);
		}
	}

	//suicide
	if (	!inflictor && !attackingPlayer ||
			!inflictor && attackingPlayer == killedPlayer ||
			inflictor && inflictor->entnum == killedPlayer->entnum)
	{
		//addPoints(killedPlayer->entnum, -_defaultPointsPerTakenAwayForSuicide);
		_playerGameData[killedPlayer->entnum]._lastKillerOfPlayer = killedPlayer->entnum;
	}

	// Print out an obituary
	obituary(killedPlayer, attackingPlayer, inflictor, meansOfDeath);
}

void ModeCoop::obituary(Player* killedPlayer, Player* attackingPlayer, Entity* inflictor, int meansOfDeath)
{
	const char* s1 = NULL, * s2 = NULL;
	str printString;
	bool suicide;
	bool printSomething;
	char color;
	bool sameTeam;

	suicide = false;
	printSomething = false;

	sameTeam = false;

	if (attackingPlayer && (killedPlayer != attackingPlayer))
	{
		Team* killedPlayersTeam;
		Team* attackingPlayersTeam;

		killedPlayersTeam = multiplayerManager.getPlayersTeam(killedPlayer);

		attackingPlayersTeam = multiplayerManager.getPlayersTeam(attackingPlayer);

		if (killedPlayersTeam && attackingPlayersTeam && (killedPlayersTeam == attackingPlayersTeam))
		{
			sameTeam = true;
		}
	}

	if (killedPlayer == attackingPlayer || !attackingPlayer && !inflictor || inflictor == killedPlayer /* && meansOfDeath != MOD_FALLING*/)
	{
		suicide = true;
		printSomething = true;

		switch (meansOfDeath)
		{
		case MOD_SUICIDE:
			s1 = "$$MOD_SUICIDE$$";
			break;
		case MOD_DROWN:
			s1 = "$$MOD_DROWN$$";
			break;
		case MOD_LAVA:
			s1 = "$$MOD_LAVA$$";
			break;
		case MOD_SLIME:
			s1 = "$$MOD_SLIME$$";
			break;
		case MOD_FALLING:
			s1 = "$$MOD_FALLING$$";
			break;
		default:
			s1 = "$$MOD_SUICIDE$$";
			break;
		}
	}



	// Killed by another player
	if (attackingPlayer && attackingPlayer->isClient() && (killedPlayer != attackingPlayer))
	{
		printSomething = true;

		switch (meansOfDeath)
		{
		case MOD_CRUSH:
		case MOD_CRUSH_EVERY_FRAME:
			s1 = "$$MOD_CRUSH$$";
			break;
		case MOD_TELEFRAG:
			s1 = "$$MOD_TELEFRAG$$";
			break;
		case MOD_EXPLODEWALL:
		case MOD_EXPLOSION:
		case MOD_POO_EXPLOSION:
			s1 = "$$MOD_EXPLOSION$$";
			break;
		case MOD_ELECTRICWATER:
		case MOD_ELECTRIC:
		case MOD_CIRCLEOFPROTECTION:
			s1 = "$$MOD_ELECTRIC$$";
			break;
		case MOD_IMPACT:
		case MOD_THROWNOBJECT:
			s1 = "$$MOD_IMPACT$$";
			s2 = "$$MOD_IMPACT2$$";
			break;
		case MOD_BEAM:
			s1 = "$$MOD_BEAM$$";
			break;
		case MOD_ROCKET:
			s1 = "$$MOD_ROCKET$$";
			s2 = "$$MOD_ROCKET2$$";
			break;
		case MOD_GAS_BLOCKABLE:
		case MOD_GAS:
			s1 = "$$MOD_GAS$$";
			break;
		case MOD_ACID:
			s1 = "$$MOD_ACID$$";
			break;
		case MOD_SWORD:
			s1 = "$$MOD_SWORD$$";
			break;
		case MOD_PLASMA:
		case MOD_PLASMABEAM:
		case MOD_PLASMASHOTGUN:
			s1 = "$$MOD_ASSULT_RIFLE$$";
			break;
		case MOD_RADIATION:
			s1 = "$$MOD_PLASMA$$";
			break;
		case MOD_STING:
		case MOD_STING2:
			s1 = "$$MOD_STING$$";
			break;
		case MOD_BULLET:
		case MOD_FAST_BULLET:
			s1 = "$$MOD_BULLET$$";
			break;
		case MOD_VEHICLE:
			s1 = "$$MOD_VEHICLE$$";
			break;
		case MOD_FIRE:
		case MOD_FIRE_BLOCKABLE:
		case MOD_ON_FIRE:
			s1 = "$$MOD_FIRE$$";
			break;
		case MOD_LIFEDRAIN:
			s1 = "$$MOD_LIFEDRAIN$$";
			break;
		case MOD_FLASHBANG:
			s1 = "$$MOD_FLASHBANG$$";
			break;
		case MOD_AXE:
			s1 = "$$MOD_AXE$$";
			s2 = "$$MOD_AXE2$$";
			break;
		case MOD_CHAINSWORD:
			s1 = "$$MOD_CHAINSWORD$$";
			break;
		case MOD_FIRESWORD:
			s1 = "$$MOD_FIRESWORD$$";
			break;
		case MOD_ELECTRICSWORD:
			s1 = "$$MOD_ELECTRICSWORD$$";
			s2 = "$$MOD_ELECTRICSWORD2$$";
			break;
		case MOD_LIGHTSWORD:
			s1 = "$$MOD_LIGHTSWORD$$";
			s2 = "$$MOD_LIGHTSWORD2$$";
			break;
		case MOD_IMPALE:
			s1 = "$$MOD_IMPALE$$";
			break;
		case MOD_UPPERCUT:
			s1 = "$$MOD_UPPERCUT$$";
			break;
		case MOD_POISON:
			s1 = "$$MOD_POISON$$";
			break;
		case MOD_PHASER:
			s1 = "$$MOD_PHASER$$";
			break;
		case MOD_COMP_RIFLE:
			s1 = "$$MOD_COMP_RIFLE$$";
			break;
			//case MOD_ASSULT_RIFLE:

			//case MOD_IMOD:
			//	s1 = "$$MOD_IMOD$$";
			//	break;

		case MOD_VAPORIZE:
		case MOD_VAPORIZE_COMP:
		case MOD_VAPORIZE_DISRUPTOR:
		case MOD_VAPORIZE_PHOTON:
			s1 = "$$MOD_VAPORIZE$$";
			break;
		default:
			s1 = "$$MOD_DEFAULT$$";
			break;
		}
	}
	
	//Killed by ACTOR or something else
	//go through the list by priority from very specialized to very broad
	//this allowes to use different text for each map
	else if(inflictor) {
		//grab owner if possible
		if (inflictor->isSubclassOf(Projectile)) {
			Projectile* proj = (Projectile*)inflictor;
			if (proj->owner > -1 && proj->owner < maxentities->integer) {
				unsigned int ownerNumber = proj->owner;
				Entity* ent = G_GetEntity(ownerNumber);
				if (ent) {
					inflictor = ent;
				}
			}
		}

		str mapName = level.mapname;
		mapName = mapName.tolower();
		str killmessageEng = "???";
		str killmessageDeu = "";
		str className = inflictor->getClassname();
		str modelName = inflictor->model;
		str targetName = inflictor->targetname;
		str typeName = inflictor->getName();
		bool foundText = false;
		bool isTiki = true;

		if (typeName.length() == 0) {
			typeName = inflictor->getArchetype();
		}

		//not a tiki model, but a BSP model
		if (modelName.length()) {
			if (modelName[0] == '*') {
				isTiki = false;
			}
		}

		foundText = CoopManager::Get().entityUservarGetKillMessage(inflictor, killmessageEng, killmessageDeu);
		if (!foundText) {
			foundText = CoopManager::Get().entityUservarGetName(inflictor, killmessageEng, killmessageDeu);
		}

		//NOTHING FOUND - USE DATA EXTRACTED FROM deathlist.ini
		//NOTHING FOUND - USE DATA EXTRACTED FROM deathlist.ini
		//NOTHING FOUND - USE DATA EXTRACTED FROM deathlist.ini

		if (!foundText && inflictor->isSubclassOf(Actor)) {
			//actorname specified for this map 
			for (int i = 1; !foundText && i <= CoopSettings_deathList.NumObjects(); i++) {
				if (CoopSettings_deathList.ObjectAt(i).type == va("actornames@%s", mapName.c_str()) && CoopSettings_deathList.ObjectAt(i).name == typeName) {
					killmessageEng = CoopSettings_deathList.ObjectAt(i).text;
					foundText = true;
					break;
				}
			}
		}
		
		if (!foundText && inflictor->isSubclassOf(Projectile)) {
			//projectiles specified for this map 
			for (int i = 1; !foundText && i <= CoopSettings_deathList.NumObjects(); i++) {
				if (CoopSettings_deathList.ObjectAt(i).type == va("projectiles@%s", mapName.c_str()) && CoopSettings_deathList.ObjectAt(i).name == modelName) {
					killmessageEng = CoopSettings_deathList.ObjectAt(i).text;
					foundText = true;
					break;
				}
			}
			//projectiles specified in general
			for (int i = 1; !foundText && i <= CoopSettings_deathList.NumObjects(); i++) {
				if (CoopSettings_deathList.ObjectAt(i).type == "projectiles" && CoopSettings_deathList.ObjectAt(i).name == modelName) {
					killmessageEng = CoopSettings_deathList.ObjectAt(i).text;
					foundText = true;
					break;
				}
			}
		}

		//targetnames specified for this map 
		for (int i = 1; !foundText && i <= CoopSettings_deathList.NumObjects(); i++) {
			if (CoopSettings_deathList.ObjectAt(i).type == va("targetnames@%s", mapName.c_str()) && CoopSettings_deathList.ObjectAt(i).name == targetName) {
				killmessageEng = CoopSettings_deathList.ObjectAt(i).text;
				foundText = true;
				break;
			}
		}
		//models specified for this map - only check tikis (excludes bsp models like *41)
		for (int i = 1; !foundText && isTiki && i <= CoopSettings_deathList.NumObjects(); i++) {
			if (CoopSettings_deathList.ObjectAt(i).type == va("models@%s", mapName.c_str()) && CoopSettings_deathList.ObjectAt(i).name == modelName) {
				killmessageEng = CoopSettings_deathList.ObjectAt(i).text;
				foundText = true;
				break;
			}
		}
		//class specified for this map 
		for (int i = 1; !foundText && i <= CoopSettings_deathList.NumObjects(); i++) {
			if (CoopSettings_deathList.ObjectAt(i).type == va("classes@%s", mapName.c_str())) {
				if (CoopSettings_deathList.ObjectAt(i).name == className) {
					killmessageEng = CoopSettings_deathList.ObjectAt(i).text;
					foundText = true;
					break;
				}
			}
		}

		//targetnames specified
		for (int i = 1; !foundText && i <= CoopSettings_deathList.NumObjects(); i++) {
			if (CoopSettings_deathList.ObjectAt(i).type == "targetnames" && CoopSettings_deathList.ObjectAt(i).name == targetName) {
				killmessageEng = CoopSettings_deathList.ObjectAt(i).text;
				foundText = true;
				break;
			}
		}
		//models specified - only check tikis (excludes bsp models like *41)
		for (int i = 1; !foundText && isTiki && i <= CoopSettings_deathList.NumObjects(); i++) {
			if (CoopSettings_deathList.ObjectAt(i).type == "models" && CoopSettings_deathList.ObjectAt(i).name == modelName) {
				killmessageEng = CoopSettings_deathList.ObjectAt(i).text;
				foundText = true;
				break;
			}
		}
		if (!foundText && inflictor->isSubclassOf(Actor)) {
			//actorname specified
			for (int i = 1; !foundText && i <= CoopSettings_deathList.NumObjects(); i++) {
				if (CoopSettings_deathList.ObjectAt(i).type == "actornames" && CoopSettings_deathList.ObjectAt(i).name == typeName) {
					killmessageEng = CoopSettings_deathList.ObjectAt(i).text;
					foundText = true;
					break;
				}
			}
		}
		//class specified - very broad
		for (int i = 1; !foundText && i <= CoopSettings_deathList.NumObjects(); i++) {
			if (CoopSettings_deathList.ObjectAt(i).type == "classes") {
				if (CoopSettings_deathList.ObjectAt(i).name == className) {
					killmessageEng = CoopSettings_deathList.ObjectAt(i).text;
					foundText = true;
					break;
				}
			}
		}
		
		//tell players
		if (foundText) {
			Player* player = nullptr;
			for (int i = 0; i < gameFixAPI_maxClients(); i++) {
				player = gamefix_getPlayer(i);
				if (!player) {
					continue;
				}
				if (player->coop_hasLanguageGerman() && killmessageDeu.length()) {
					multiplayerManager.HUDPrint(i, va("%s - %s\n", killedPlayer->client->pers.netname, killmessageDeu.c_str()));
				}
				else {
					multiplayerManager.HUDPrint(i, va("%s - %s\n", killedPlayer->client->pers.netname, killmessageEng.c_str()));
				}
			}
			return;
		}
	}

	if (killedPlayer == attackingPlayer || !attackingPlayer)
	{
		//multiplayerManager.centerPrint(killedPlayer->entnum, va("^%c$$YouKilledYourself$$^%c", COLOR_RED, COLOR_NONE), CENTERPRINT_IMPORTANCE_NORMAL);
		multiplayerManager.HUDPrintAllClients(va("%s ^%c$$MOD_SUICIDE$$^%c\n", killedPlayer->client->pers.netname, COLOR_RED, COLOR_NONE));
		return;
	}

	if (printSomething)
	{
		Player* currentPlayer;

		// Print to the dedicated console

		if (dedicated->integer)
		{
			if (killedPlayer) {
				if (suicide)
				{
					printString = va("%s %s", killedPlayer->client->pers.netname, s1);
				}
				else if (s2)
				{
					printString = va("%s %s %s %s", killedPlayer->client->pers.netname, s1, attackingPlayer->client->pers.netname, s2);
				}
				else
				{
					printString = va("%s %s %s", killedPlayer->client->pers.netname, s1, attackingPlayer->client->pers.netname);
				}
			}

			if (sameTeam)
			{
				printString += " ($$SameTeam$$)";
			}

			printString += "\n";

			gi.Printf(printString.c_str());
		}

		// Print to all of the players
		for (unsigned int i = 0; i < _maxPlayers; i++)
		{
			currentPlayer = multiplayerManager.getPlayer(i);

			if (!currentPlayer)
				continue;

			// Figure out which color to use 
			if (killedPlayer && (unsigned)killedPlayer->entnum == i)
				color = COLOR_RED;
			else if (attackingPlayer && (unsigned)attackingPlayer->entnum == i)
				color = COLOR_GREEN;
			else
				color = COLOR_NONE;

			// Build the death string
			if (killedPlayer) {
				if (suicide)
				{
					printString = va("%s ^%c%s^8", killedPlayer->client->pers.netname, color, s1);
				}
				else if (s2)
				{
					printString = va("%s ^%c%s^8 %s ^%c%s^8", killedPlayer->client->pers.netname, color, s1, attackingPlayer->client->pers.netname, color, s2);
				}
				else
				{
					printString = va("%s ^%c%s^8 %s", killedPlayer->client->pers.netname, color, s1, attackingPlayer->client->pers.netname);
				}
			}


			if (sameTeam)
			{
				printString += " (^";
				printString += COLOR_RED;
				printString += "$$SameTeam$$^8)";
			}

			printString += "\n";

			// Print out the death string
			if (gi.GetNumFreeReliableServerCommands(currentPlayer->edict - g_entities) > 32)
			{
				multiplayerManager.HUDPrint(currentPlayer->entnum, printString.c_str());
			}

			//multiplayerManager.HUDPrintAllClients( printString.c_str() );
		}
	}
}

int ModeCoop::getTeamPoints(Player* player)
{
	// Return the points for this team
	Team *team = _playerGameData[player->entnum]._currentTeam;

	if (team)
		return team->getPoints();
	else
		return 0;
}

void ModeCoop::setupMultiplayerUI(Player* player)
{
	if (multiplayerManager.inMultiplayer()) {
		gi.SendServerCommand(player->entnum, "stufftext \"ui_removehuds all\"\n");	
		
		//multiplayerManager.gameFixAPI_setTeamHud(player, true);
		Team* team = getPlayersTeam(player);
		
		if (gameFixAPI_isSpectator_stef2(player)) {
			if (!team)
				multiplayerManager.setTeamHud(player, "mp_teamspec");
			else if (team->getName() == "Red")
				multiplayerManager.setTeamHud(player, "mp_teamredspec");
			else
				multiplayerManager.setTeamHud(player, "mp_teambluespec");
		}

		gamefix_playerDelayedServerCommand(player->entnum, "ui_addhud mp_console");

		if (mp_timelimit->integer) {
			gamefix_playerDelayedServerCommand(player->entnum, "globalwidgetcommand dmTimer enable");
		}
		else {
			gamefix_playerDelayedServerCommand(player->entnum, "globalwidgetcommand dmTimer disable");
		}
	}
}

int ModeCoop::getInfoIcon(Player* player)
{
	// Make sure entity is not invisible
	if (player->_affectingViewModes & gi.GetViewModeMask("forcevisible"))
		return 0;

	//For now we don't want any team icons or arrows floating over the players, thats why we end here
	return 0;
}

void ModeCoop::playerChangedModel(Player* player)
{
	CoopManager::Get().playerChangedModel(player);
	updatePlayerSkin(player);
}

void ModeCoop::updatePlayerSkin(Player* player)
{
	player->SurfaceCommand("all", "-skin1");
	player->SurfaceCommand("all", "-skin2");

	//at this time we don't support teams
	//in the future we might with some players
	//playing normal and others controlling ai
	return;
}

bool ModeCoop::canJoinTeam(Player* player, const str& teamName)
{
	if (!multiplayerManager.checkRule("respawnPlayer", true, player)){
		return false;
	}

	// Deny red team
	if (Q_stricmp(teamName,"red") == 0){
		//tell player
		if (multiplayerManager.getTime() + 1.0f > _matchStartTime + gameFixAPI_getMpWarmupTime()) {
			multiplayerManager.centerPrint(player->entnum, _COOP_COOP_allowesOnlyBlue, CENTERPRINT_IMPORTANCE_HIGH);
		}
		return false;
	}

	//Deny same team
	if (_playerGameData[player->entnum]._currentTeam){
		Team* fallback = _findTeamByName(teamName);
		if (_playerGameData[player->entnum]._currentTeam == fallback) {
			return false;
		}
	}

	// Allow Blue and Spectator
	return true;
}

void ModeCoop::joinTeam(Player* player, const str& teamName)
{
	Team* team = nullptr;
	if(canJoinTeam(player, teamName)){
		team = _findTeamByName(teamName);
	}
	else {
		team = _findTeamByName("Blue");
	}

	changeTeams(player, team);
}

void ModeCoop::AddPlayer(Player* player)
{
	// Make sure everything is ok
	if (!player){
		warning("ModeDeathmatch::AddPlayer", "NULL Player\n");
		return;
	}

	// Make sure player hasn't alrady been added

	if (!needToAddPlayer(player))
		return;

	MultiplayerModeBase::AddPlayer(player);

	// Add player to a team or make him a spectator
	if (!multiplayerManager.checkFlag(MP_FLAG_NO_AUTO_JOIN_TEAM) ){
		addPlayerToTeam(player, getTeam("Blue"));
	}
	// Not force joining of teams, start as a spectator
	else{
		addPlayerToTeam(player, nullptr);
	}

	// If the game hasn't started yet just make the player a spectator
	if (!_gameStarted){
		multiplayerManager.makePlayerSpectator(player);
	}
}

void ModeCoop::addPlayerToTeam(Player* player, Team* team)
{
	Team* oldTeam;
	Entity* spawnPoint;

	MultiplayerModeBase::AddPlayer(player);

	oldTeam = _playerGameData[player->entnum]._currentTeam;
	if (oldTeam){
		_playerGameData[player->entnum]._currentTeam = nullptr;
		player->SurfaceCommand("all", "-skin1");
		player->SurfaceCommand("all", "-skin2");
		oldTeam->RemovePlayer(player);
	}

	// Since the player is now on a team add him to the game
	if (team){
		if (_gameStarted)
			multiplayerManager.playerEnterArena(player->entnum, player->health);
		else
			multiplayerManager.makePlayerSpectator(player);

		// Add the player to the team
		team->AddPlayer(player);
		_playerGameData[player->entnum]._currentTeam = team;
		updatePlayerSkin(player);
		multiplayerManager.playerSpawned(player);
		playersLastTeam[player->entnum] = team->getName();
	}
	// No team selected so make the player a spectator
	else{
		multiplayerManager.makePlayerSpectator(player, SPECTATOR_TYPE_FOLLOW, true);
	}

	// Warp player to a spawn point
	spawnPoint = getSpawnPoint(player);
	if (!spawnPoint) {
		spawnPoint = gamefix_returnInfoPlayerStart(_GFixEF2_INFO_GAMEFIX_spawnlocations_TeamBaseAddPlayerToTeam);
	}


	if (spawnPoint){
		player->WarpToPoint(spawnPoint);
	}

	if (team && _gameStarted){
		//KillBox(player);
//MakeSolidASAP
		ActivatePlayer(player);
	}
}

//this always returns a entity, it uses info_player_start which has to be on every map
//it moves it around if script varaiables are used instead of actual info_player_deathmatch spawnlocations
Entity* ModeCoop::getSpawnPoint(Player* player)
{
	if (!player) {
		return nullptr;
	}

	Entity* spawnPoint = nullptr;

	ScriptVariable *entityData = nullptr, *entityData2 = nullptr;
	if (	!CoopManager::Get().getPlayerData_respawnLocationSpawn(player) &&
			!CoopManager::Get().getPlayerData_spawnLocationSpawnForced(player))
	{
		//check if script has set all players to respawn at respawn spots
		entityData = world->entityVars.GetVariable("coop_respawnAtRespawnpoint");
		//check if script or code wants this player to respawn at respawn location
		entityData2 = player->entityVars.GetVariable("coop_respawnAtRespawnpoint");
		if (entityData && entityData->floatValue() == 1.0f ||
			entityData2 && entityData2->floatValue() == 1.0f) {
			CoopManager::Get().setPlayerData_respawnLocationSpawn(player,true);
		}
	}

	//RESPAN LOCATION VAR
	if (CoopManager::Get().getPlayerData_respawnLocationSpawn(player) || CoopManager::Get().getPlayerData_spawnLocationSpawnForced(player)) {
		
		//reset forced location
		if (!gameFixAPI_isSpectator_stef2((Entity*)player) && gameFixAPI_getMpMatchStarted()) {
			player->entityVars.SetVariable("coop_respawnAtRespawnpoint", 0.0f);
			CoopManager::Get().setPlayerData_spawnLocationSpawnForced(player, false);
			CoopManager::Get().setPlayerData_respawnLocationSpawn(player, false);
		}
		
		str respawnLoc = va("coop_vector_respawnOrigin%i", (1 + player->entnum));
		Vector vRespawnSpawn = program.coop_getVectorVariableValue(respawnLoc.c_str());
		if (vRespawnSpawn.length() > 0) {
			Entity* respawnLocEnity;
			respawnLocEnity = G_FindClass(NULL, "info_player_start");
			Vector vOld = respawnLocEnity->origin;
			if (respawnLocEnity) {
				respawnLoc = va("coop_float_spawnAngle%i", (1 + player->entnum));
				Vector vAngle = Vector(0.0f, 0.0f, 0.0f);
				vAngle[1] = program.coop_getFloatVariableValue(respawnLoc.c_str());
				//no player specific angle, get general angle
				if (vAngle[1] == 0.0f) {
					vAngle[1] = program.coop_getFloatVariableValue("coop_float_spawnAngle0");
				}
				
				respawnLocEnity->setAngles(vAngle);
				respawnLocEnity->setOrigin(vRespawnSpawn);
				respawnLocEnity->NoLerpThisFrame();
				return respawnLocEnity;
			}
		} 

		//SPAN LOCATION VAR
		str s = va("coop_vector_spawnOrigin%i", (1 + player->entnum));
		Vector vSpawn = program.coop_getVectorVariableValue(s.c_str());
		if (vSpawn.length() > 0) {
			Entity* ent;
			ent = G_FindClass(NULL, "info_player_start");
			Vector vOld = ent->origin;
			if (ent) {
				s = va("coop_float_spawnAngle%i", (1 + player->entnum));
				Vector vAngle = Vector(0.0f, 0.0f, 0.0f);
				vAngle[1] = program.coop_getFloatVariableValue(s.c_str());
				//no player specific angle, get general angle
				if (vAngle[1] == 0.0f) {
					vAngle[1] = program.coop_getFloatVariableValue("coop_float_spawnAngle0");
				}

				ent->setAngles(vAngle);
				ent->setOrigin(vSpawn);
				ent->NoLerpThisFrame();
				return ent;
			}
		}
	}
	//respawn at deathspot - last location
	else if(CoopManager::Get().getPlayerData_respawnMe(player)) {
		Entity* lastLoc;
		lastLoc = G_FindClass(NULL, "info_player_start");
		lastLoc->setAngles(CoopManager::Get().getPlayerData_lastValidViewAngle(player));
		lastLoc->setOrigin(CoopManager::Get().getPlayerData_lastValidLocation(player));
		lastLoc->NoLerpThisFrame();
		return lastLoc;
	}

	//reset forced location
	if (!gameFixAPI_isSpectator_stef2((Entity*)player) && gameFixAPI_getMpMatchStarted()) {
		player->entityVars.SetVariable("coop_respawnAtRespawnpoint", 0.0f);
		CoopManager::Get().setPlayerData_spawnLocationSpawnForced(player, false);
		CoopManager::Get().setPlayerData_respawnLocationSpawn(player, false);
	}

	//Return coop mod targetnamed spawnpoint (ipd1, ipd2, ..., ipd8)
	TargetList* tlist;
	tlist = world->GetTargetList(va("ipd%i", (1 + player->entnum)), false);
	if (tlist) {
		spawnPoint = tlist->GetNextEntity(NULL);
		if (spawnPoint) {
			return spawnPoint;
		}
	}

	DEBUG_LOG("Coop Targetnamed ipd%i spawn avialable in level\n", (1 + player->entnum));

	int randomStartingSpot;
	int i;
	int spawnPointIndex;
	int numSpawnPoints;
	bool useAnySpawnPoint;

	numSpawnPoints = getNumNamedSpawnpoints("");

	if (numSpawnPoints == 0){
		useAnySpawnPoint = true;

		numSpawnPoints = getNumSpawnpoints();
	}
	else{
		useAnySpawnPoint = false;
	}

	randomStartingSpot = ((int)(G_Random() * numSpawnPoints));

	for (i = 0; i < numSpawnPoints; i++){
		spawnPointIndex = (randomStartingSpot + i) % numSpawnPoints;

		if (useAnySpawnPoint)
			spawnPoint = getSpawnpointbyIndex(spawnPointIndex);
		else
			spawnPoint = getNamedSpawnpointbyIndex("", spawnPointIndex);

		if (spawnPoint){
			int j;
			int num;
			int touch[MAX_GENTITIES];
			gentity_t* hit;
			Vector min;
			Vector max;
			bool badSpot;

			min = spawnPoint->origin + player->mins + Vector(0, 0, 1);
			max = spawnPoint->origin + player->maxs + Vector(0, 0, 1);

			num = gi.AreaEntities(min, max, touch, MAX_GENTITIES, qfalse);

			badSpot = false;

			for (j = 0; j < num; j++){
				hit = &g_entities[touch[j]];

				if (!hit->inuse || (hit->entity == player) || !hit->entity || (hit->entity == world) || (!hit->entity->edict->solid)){
					continue;
				}

				if (hit->entity->isSubclassOf(Player)){
					Player* hitPlayer;

					hitPlayer = (Player*)hit->entity;

					badSpot = true;
					break;
				}
			}

			if (!badSpot){
				break;
			}
		}

	}

	if (spawnPoint) {
		if (spawnPoint->origin == Vector(0.0f, 0.0f, 0.0f)) {
			gi.Printf(_GFix_INFO_MapError, va(_GFixEF2_ERR_LEVEL_InfoPlayerDeathmatch_AT_ZERO, spawnPoint->targetname.c_str()));
			spawnPoint = nullptr;
		}
	}

	return spawnPoint;
}

void ModeCoop::score(const Player* player)
{
	char		   string[1400];
	char		   entry[1024];
	int            tempStringlength;
	int            count = 0;
	int            stringlength = 0;
	Team* team;
	str				teamName;
	//int				teamPoints;
	Player* currentPlayer;
	int				redScore;
	int				blueScore;

	assert(player);
	if (!player)
	{
		warning("MultiplayerModeBase::score", "Null Player specified.\n");
		return;
	}

	string[0] = 0;
	entry[0] = 0;

	// This for loop builds a string containing all the players scores.
	for (unsigned int i = 0; i < _maxPlayers; i++)
	{
		currentPlayer = multiplayerManager.getPlayer(i);

		if (!currentPlayer)
			continue;

		team = multiplayerManager.getPlayersTeam(currentPlayer);

		if (team)
		{
			teamName = team->getName();
			//teamPoints = team->getPoints();
		}
		else
		{
			teamName = "spectator";
			//teamPoints = 0;
		}

		Com_sprintf(entry, sizeof(entry), "%i %i %i %i %s %i %i %d %d %d %d %d %d ",
			multiplayerManager.getClientNum(_playerGameData[i]._entnum),
			_playerGameData[i]._points,
			_playerGameData[i]._numKills,
			_playerGameData[i]._numDeaths,
			//0 /*pl->GetMatchesWon() */,
			//0 /*pl->GetMatchesLost()*/, 
			teamName.c_str(),
			//teamPoints,
			(int)(multiplayerManager.getTime() - _playerGameData[i]._startTime),
			multiplayerManager.getClientPing(_playerGameData[i]._entnum),
			multiplayerManager.getScoreIcon(currentPlayer, SCOREICON1),
			multiplayerManager.getScoreIcon(currentPlayer, SCOREICON2),
			multiplayerManager.getScoreIcon(currentPlayer, SCOREICON3),
			multiplayerManager.getScoreIcon(currentPlayer, SCOREICON4),
			multiplayerManager.getScoreIcon(currentPlayer, SCOREICON5),
			multiplayerManager.getScoreIcon(currentPlayer, SCOREICON6));

		tempStringlength = strlen(entry);

		// Make sure the string is not too big (take into account other stuff that gets prepended below also)

		if (stringlength + tempStringlength > 975)
			break;

		strcpy(string + stringlength, entry);

		stringlength += tempStringlength;
		count++;
	}


	redScore = multiplayerManager.getTeamPoints("Red");
	blueScore = multiplayerManager.getTeamPoints("Blue");

	gi.SendServerCommand(player->edict - g_entities, "scores 1 %i %d %d %s", count, redScore, blueScore, string);
}

bool ModeCoop::checkGameType(const char* gameType)
{
	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Warning: C4996 stricmp: The POSIX name for this item is deprecated. Using instead: Q_stricmp - chrissstrahl
	//--------------------------------------------------------------
	if (Q_stricmp(gameType, "teamdm") == 0)
		return true;
	else
		return false;
}

void ModeCoop::ActivatePlayer(Player* player)
{
	if (!player){
		warning("ModeCoop::ActivatePlayer", "NULL Player\n");
		return;
	}

	// Make the player enter the game
	if (!_gameStarted) {
		multiplayerManager.makePlayerSpectator(player);
		return;

	}

	multiplayerManager.playerEnterArena(player->entnum, _startingHealth);
	multiplayerManager.changePlayerModel(player, player->client->pers.mp_playermodel);

	_giveInitialConditions(player);
	multiplayerManager.allowFighting(true);
	player->takedamage = DAMAGE_YES;
	multiplayerManager.playerSpawned(player);
}

void ModeCoop::playerDead(Player* player)
{
	if (!player) {
		warning("MultiplayerModeBase::playerDead", "NULL Player\n");
		return;
	}

	if (gamefix_getEntityVarInt(world,"coop_noBeamout") != 0 ||
		gamefix_getEntityVarInt(player, "coop_noBeamout") != 0) {
		return;
	}
	
	player->ProcessEvent(EV_Player_DeadBody);
	player->hideModel();
}

#endif