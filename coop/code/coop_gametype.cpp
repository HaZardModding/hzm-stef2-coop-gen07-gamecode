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

// I am not sure if there is a better way to handle all this - Chrissstrahl
ModeCoop& ModeCoop::Get() {
	static ModeCoop instance;
	return instance;
}

ModeCoop::ModeCoop()
{
	//_redTeam = AddTeam("Red");
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

bool ModeCoop::shouldStartMatch(void)
{
	int timeRemaining = (int)(_matchStartTime + mp_warmUpTime->value - multiplayerManager.getTime() + 1.0f);
	int numPlayers;
	if ((timeRemaining > 0) && (timeRemaining < 6) && (timeRemaining != _lastTimeRemaining)){
		_lastTimeRemaining = timeRemaining;

		multiplayerManager.centerPrintAllClients(va("%d", _lastTimeRemaining), CENTERPRINT_IMPORTANCE_NORMAL);
	}

	// Make sure we have done our warm up already
	if (multiplayerManager.getTime() < _matchStartTime + mp_warmUpTime->value)
		return false;


	// Make sure we have enough players
	numPlayers = 0;

	for (unsigned int i = 0; i < _maxPlayers; i++){
		if (_playerGameData[i]._playing){
			numPlayers++;
		}
	}


	if (numPlayers < mp_minPlayers->integer)
		return false;

	return true;
}

void ModeCoop::clientThink(Player* player)
{
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
	gamefix_playerSetupUi(player);
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
		if (multiplayerManager.getTime() + 1.0f > _matchStartTime + mp_warmUpTime->value) {
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

Entity* ModeCoop::getSpawnPoint(Player* player)
{
	Entity* spawnPoint = nullptr;

	//Return coop mod targetnamed spawnpoint (ipd1, ipd2, ..., ipd8)
	Entity* ent;
	TargetList* tlist;
	tlist = world->GetTargetList(va("ipd%i", (1 + player->entnum)), false);
	if (tlist) {
		ent = tlist->GetNextEntity(NULL);
		if (ent) {
			return ent;
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
