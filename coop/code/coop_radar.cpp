//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING PLAYER RADAR RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------

#ifdef ENABLE_COOP

#include "coop_radar.hpp"
#include "coop_manager.hpp"
#include "../../dlls/game/gamefix.hpp"

//reset variables - the ui is reset via ea.cfg in coop_playerEnterArena
void coop_radarReset(Player* player)
{
	if (!player)return;

	for (int i = 0; i < _COOP_SETTINGS_RADAR_BLIPS_MAX; i++) {
		CoopManager::Get().setPlayerData_radarUpdatedLast(player, level.time);
		CoopManager::Get().setPlayerData_radarBlipLastPos(player, i ,Vector(float(i), 1.0f, 1.0f));
		CoopManager::Get().setPlayerData_radarBlipActive(player, i, false);
	}
	CoopManager::Get().setPlayerData_radarSelectedActive(player, false);
	CoopManager::Get().setPlayerData_radarUpdatedLast(player, -937.1f);
	CoopManager::Get().setPlayerData_radarAngleLast(player, 987.1f);
	CoopManager::Get().setPlayerData_radarScale(player,1);
}

float coop_radarAngleTo(const Vector& source, const Vector& dest)
{
	if (source == dest) {
		return 0;
	}
	float dot;
	dot = Vector::Dot(source, dest);
	return acosf(dot);
}

float coop_radarSignedAngleTo(const Vector& source, const Vector& dest, const Vector& planeNormal)
{
	float angle = coop_radarAngleTo(source, dest);
	Vector cross;
	cross = Vector::Cross(source, dest);
	float dot;
	dot = Vector::Dot(cross, planeNormal);

	return dot < 0 ? -angle : angle;
}

void coop_radarUpdateBlip(Player* player,
	Entity* eMiObjEntity,
	Entity* target,
	Vector& vRadarCenterPos,
	int& iMiObjEntityItemNumber,
	bool& targetedStillValid)
{
	float	fRealDistance;
	float	fRadarDistance;
	float	fRadarAngle;
	float	fBlipAxisX, fBlipAxisY;
	Vector	vRadarBlipLastPosition(0.0f, 0.0f, 0.0f);
	Vector	vRealDistance;
	Vector	vRadarDistance;
	Vector	vNorth(-1.0f, 0.0f, 0.0f);
	vRealDistance = (eMiObjEntity->origin - player->origin);
	vRealDistance.z = 0.0f;

	fRealDistance = vRealDistance.lengthSquared();
	fRadarDistance = fRealDistance / _COOP_SETTINGS_RADAR_SCALE_FACTOR;

	//make sure the blip does not go outside the actual radar
	if (fRadarDistance > _COOP_SETTINGS_RADAR_MAX_RADIUS) fRadarDistance = _COOP_SETTINGS_RADAR_MAX_RADIUS;

	//set vector to length 1, but keep it pointed into the actual direction (Einheitsvector (Länge = 1))
	vRadarDistance = vRealDistance;
	vRadarDistance.normalize();


	//make real pos of the object to a relative pos of the radar
	//Die reale position des objected wird zu realtiven einer position des radars
	vRadarDistance *= fRadarDistance;

	trace_t trace;
	player->GetViewTrace(trace, MASK_PROJECTILE, 1.0f);

	//[b607] chrissstrahl - fix radar not working when player is in cg_3rd_person
	Vector viewDir = Vector(0, 0, 0);
	Vector vorg = Vector(0, 0, 0);
	if (player->client->ps.pm_flags & PMF_CAMERA_VIEW) { // 3rd person automatic camera
		vorg = player->client->ps.camera_origin;
	}
	else if (!player->coop_checkThirdperson()) { // First person
		vorg = player->origin;
		vorg.z += player->client->ps.viewheight;
	}
	else { // Third person
		vorg = player->client->ps.camera_origin;
	}

	viewDir = trace.endpos - vorg; //[b607] chrissstrahl - use the correct player/camera origin now
	viewDir.z = 0.0f;

	vRealDistance.normalize();
	viewDir.normalize();
	Vector up(0.0f, 0.0f, 1.0f);

	fRadarAngle = coop_radarSignedAngleTo(viewDir, vRealDistance, up);

	//debug
	//gi.Printf( va( "PlayerPos(%f,%f)\n" , player->origin.x , player->origin.y ) );
	//gi.Printf( va( "fRadarAngle : %f\n" , fRadarAngle ) );

	vRadarDistance = Vector(0.0f, -fRadarDistance, 0.0f);

	fRadarAngle -= M_PI / 2;

	fBlipAxisX = vRadarCenterPos.x + ((sinf(fRadarAngle) * vRadarDistance.x) + (cosf(fRadarAngle) * vRadarDistance.y));
	fBlipAxisY = vRadarCenterPos.y + ((cosf(fRadarAngle) * vRadarDistance.x) - (sinf(fRadarAngle) * vRadarDistance.y));

	vRadarBlipLastPosition[0] = fBlipAxisX;
	vRadarBlipLastPosition[1] = fBlipAxisY;

	//[b60025] chrissstrahl - scale for user specific scaled radar hud
	int userScaleFactor = CoopManager::Get().getPlayerData_radarScale(player);
	if (userScaleFactor <= 0) {
		userScaleFactor = 1;
	}
	else if (userScaleFactor > 6) {
		userScaleFactor = 6;
	}
	fBlipAxisX = fBlipAxisX * userScaleFactor;
	fBlipAxisY = fBlipAxisY * userScaleFactor;
	//scale 6 (what we use now as max) total hud size -> 768 924
	//we have to shift (on x axis) huds scaled smaller than 6 to the right to make them align with the right edge
	fBlipAxisX += _COOP_SETTINGS_RADAR_HUD_REAL_WIDTH - (_COOP_SETTINGS_RADAR_HUD_SCALE_WIDTH * userScaleFactor);


	//hzm coop mod chrissstrahl - construct client command
	if (CoopManager::Get().getPlayerData_radarBlipLastPos(player, iMiObjEntityItemNumber) != vRadarBlipLastPosition) {
		CoopManager::Get().setPlayerData_radarBlipLastPos(player, iMiObjEntityItemNumber, vRadarBlipLastPosition);

		//hzm coop mod chrissstrahl - convert floats to int, to reduce traffic
		//[b607] chrissstrahl - if there is a lot send to player, do not update blips
		//The blips are updated every time the player moves or turns, so chances are high
		//no one will ever notice this, and if they do then there are nettraffic issues anyway
		//used to be send via DelayedServerCommand 
		if (gi.GetNumFreeReliableServerCommands(player->entnum) > 120) { //64
			gi.SendServerCommand(player->entnum, va("stufftext \"globalwidgetcommand cr%i rect %i %i %i %i\"\n", iMiObjEntityItemNumber, (int)fBlipAxisX, (int)fBlipAxisY, _COOP_SETTINGS_RADAR_BLIP_SIZE * userScaleFactor, _COOP_SETTINGS_RADAR_BLIP_SIZE * userScaleFactor));
		}

		if (target) {
			if (target == eMiObjEntity) {
				targetedStillValid = true;
				//[b607] chrissstrahl - if there is a lot send to player, do not update blips
				//The blips are updated every time the player moves or turns, so chances are high
				//no one will ever notice this, and if they do then there are nettraffic issues anyway
				//used to be send via DelayedServerCommand 
				if (gi.GetNumFreeReliableServerCommands(player->entnum) > 120) { //64
					gi.SendServerCommand(player->entnum, va("stufftext \"globalwidgetcommand crs rect %i %i %i %i\"\n", (int)fBlipAxisX, (int)fBlipAxisY, _COOP_SETTINGS_RADAR_BLIP_SIZE * userScaleFactor, _COOP_SETTINGS_RADAR_BLIP_SIZE * userScaleFactor));
				}
			}
		}
	}

	//hzm coop mod chrissstrahl - enable active blip if it is not already
	if (CoopManager::Get().getPlayerData_radarBlipActive(player, iMiObjEntityItemNumber) == false) {
		CoopManager::Get().setPlayerData_radarBlipActive(player, iMiObjEntityItemNumber, true);
		//we need it to be reliable
		gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand cr%i enable", iMiObjEntityItemNumber));
	}
}

void coop_radarUpdate(Player* player)
//- CALCULATION CODE BY ALBERT DORN (dorn.albert)
{
	if (!player || !player->coop_hasCoopInstalled()) {
		return;
	}

	gentity_t* ent = player->edict;
	if (ent->svflags & SVF_BOT) {
		return;
	}

	//player dead or in spectator, disable selected blip
	Entity* target = player->GetTargetedEntity();
	if (player->health <= 0.0f || gameFixAPI_isSpectator_stef2(player) || !target)
	{
		if (CoopManager::Get().getPlayerData_radarSelectedActive(player)) {
			CoopManager::Get().setPlayerData_radarSelectedActive(player, false);
			gamefix_playerDelayedServerCommand(player->entnum, "globalwidgetcommand crs disable");
		}
	}

	//don't update radar:
	//- on mission failure
	//- player just entered server
	//- player just entered arena/team
	//- to soon after last update
	//- for spectators
	//- for dead
	//- when doing tricorder puzzle
	if (level.mission_failed ||
		player->health <= 0.0f ||
		gameFixAPI_isSpectator_stef2(player) ||
		//(player->upgPlayerGetLevelTimeEntered() + 3) > level.time ||
		//(player->coop_getSpawnedLastTime() + 1.5) > level.time ||
		(CoopManager::Get().getPlayerData_radarUpdatedLast(player) + _COOP_SETTINGS_RADAR_TIMECYCLE) > level.time ||
		CoopManager::Get().playerGetDoingPuzzle(player))
	{
		return;
	}

	//keep track of last update time
	CoopManager::Get().setPlayerData_radarUpdatedLast(player, level.time);

	//radar _COOP_SETTINGS_RADAR_CIRCLE_START at: 10 10 of hud
	//radar _COOP_SETTINGS_RADAR_BLIP_SIZE /2 = precise blip pos
	//radar center pos = 55 55
	//centerpos (55 55) plus radar circle start (55 + 9 - 55 + 9 = 64 64)
	//blip correction = (64 64 - -12 -12 = 52 52 )
	//radar centerpos = 52 52
	//calculate offset dynamicly
	///////////////////////////////////////////////////////////////////

	Vector	vRadarCenterPos(55.0f, 55.0f, 0.0f);
	vRadarCenterPos[0] = ((vRadarCenterPos[0] + _COOP_SETTINGS_RADAR_CIRCLE_START) - (static_cast<float>(_COOP_SETTINGS_RADAR_BLIP_SIZE) / 2));
	vRadarCenterPos[1] = ((vRadarCenterPos[1] + _COOP_SETTINGS_RADAR_CIRCLE_START) - (static_cast<float>(_COOP_SETTINGS_RADAR_BLIP_SIZE) / 2));

	Entity* eMiObjEntity = NULL;
	int		iMiObjEntityItemNumber = 0;
	bool	targetedStillValid = false;

	//check routine is needed to detect what data needs to be send and not be send
	for (int i = 0; i < maxentities->integer; i++) {
		eMiObjEntity = g_entities[i].entity;

		//not meant to show on radar
		if (!eMiObjEntity || eMiObjEntity->edict->s.missionObjective != 1 || eMiObjEntity->isSubclassOf(Player)) {
			continue;
		}

		//overreach
		if (iMiObjEntityItemNumber >= _COOP_SETTINGS_RADAR_BLIPS_OBJ_MAX) {
			break;
		}

		coop_radarUpdateBlip(player, eMiObjEntity, target, vRadarCenterPos, iMiObjEntityItemNumber, targetedStillValid);

		//hzm coop mod chrissstrahl - keep track of the current Mission Objective Blip Marker
		iMiObjEntityItemNumber++;
	}

	//hzm coop mod chrissstrahl - update radar background compas disc, update it first that way the radar seams more responsive
	int iRadarAngle = (int)player->client->ps.viewangles[1];
	if (iRadarAngle < 0) { iRadarAngle = (359 + iRadarAngle); }

	if (iRadarAngle != CoopManager::Get().getPlayerData_radarAngleLast(player)) {
		CoopManager::Get().setPlayerData_radarAngleLast(player, iRadarAngle);
		//[b607] chrissstrahl - if there is a lot send to player, do not update disc
		//The disc is updated every time the player turns, so chances are high
		//no one will ever notice this, and if they do then there are nettraffic issues anyway
		//used to be send via DelayedServerCommand 
		if (gi.GetNumFreeReliableServerCommands(player->entnum) > 64) {
			//[b60014] chrissstrahl - this needs more network traffic, but reduces cfg saves for client massively, this can help some clients with performance
			//gi.SendServerCommand(player->entnum,va("stufftext \"set coop_r %i\"\n",player->coopPlayer.radarAngleLast));
			int angle = (270 - iRadarAngle);
			gi.SendServerCommand(player->entnum, va("stufftext \"globalwidgetcommand radarBg rendermodelpitch %i\"\n", angle));
		}
	}

	//[b60014] chrissstrahl - disable all other inactive blips
	for (int i = iMiObjEntityItemNumber; i < _COOP_SETTINGS_RADAR_BLIPS_OBJ_MAX; i++) {
		if (CoopManager::Get().getPlayerData_radarBlipActive(player, i)) {
			CoopManager::Get().setPlayerData_radarBlipActive(player, i, false);
			//we need it to be reliable
			gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand cr%i disable", i));
		}
	}

	//[b60014] chrissstrahl - show green player blip
	bool bGreenBlipActive = false;
	iMiObjEntityItemNumber = _COOP_SETTINGS_RADAR_BLIPS_OBJ_MAX;
	for (int i = 0; i < maxclients->integer; i++) {
		gentity_t* gentity = &g_entities[i];

		if (gentity->inuse && gentity->entity && gentity->client && gentity->entity->isSubclassOf(Player)) {
			Player* currentPlayer = (Player*)gentity->entity;
			if (currentPlayer && currentPlayer->edict->s.missionObjective == 1) {
				bGreenBlipActive = true;
				coop_radarUpdateBlip(player, currentPlayer, target, vRadarCenterPos, iMiObjEntityItemNumber, targetedStillValid);
				break;
			}
		}
	}
	//[b60014] chrissstrahl - deactivate green blip
	if (!bGreenBlipActive) {
		for (int i = 0; i < maxclients->integer; i++) {
			gentity_t* gentity = &g_entities[i];
			if (gentity->inuse && gentity->entity && gentity->client && gentity->entity->isSubclassOf(Player)) {
				Player* currentPlayer = (Player*)gentity->entity;
				if (CoopManager::Get().getPlayerData_radarBlipActive(currentPlayer, (_COOP_SETTINGS_RADAR_BLIPS_MAX - 1))) {
					CoopManager::Get().setPlayerData_radarBlipActive(currentPlayer, (_COOP_SETTINGS_RADAR_BLIPS_MAX - 1),false);
					gamefix_playerDelayedServerCommand(gentity->entity->entnum, va("globalwidgetcommand cr%i disable", (_COOP_SETTINGS_RADAR_BLIPS_MAX - 1)));
				}
			}
		}
	}


	//update (enable/disable) selected marker
	//bugfix - chrissstrahl - make sure it is deactivated if not needed
	//[b60013] chrissstrahl - make sure it is not enabled during cinematic
	if (targetedStillValid && !level.cinematic) {
		if (!CoopManager::Get().getPlayerData_radarSelectedActive(player)) {
			CoopManager::Get().setPlayerData_radarSelectedActive(player, true);
			gamefix_playerDelayedServerCommand(player->entnum, "globalwidgetcommand crs enable");
		}
	}
}

#endif