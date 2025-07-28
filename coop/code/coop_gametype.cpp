//--------------------------------------------------------------
// COOP Generation 7.000 - Gamemode based on mp_modeTeamDM - chrissstrahl
//--------------------------------------------------------------
#include "../../dlls/game/_pch_cpp.h"
#include "../../dlls/game/mp_manager.hpp"
#include "../../dlls/game/player.h"
#include "../../dlls/game/gamefix.hpp"
#include "coop_config.hpp"
#include "coop_gametype.hpp"
#include "coop_manager.hpp"


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
		if (/* coop_checkStringInUservarsOf(ePurp, "badspot") */
			!Q_stricmp(inflictor->getClassname(), "TriggerHurt"))
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