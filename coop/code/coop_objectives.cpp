//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING OBJECTIVES RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------

#ifdef ENABLE_COOP

#include "../../dlls/game/gamefix.hpp"

#include "coop_manager.hpp"
#include "coop_objectives.hpp"


#include "../../dlls/game/_pch_cpp.h"
#include "../../dlls/game/level.h"
#include "../../dlls/game/player.h"
#include "../../dlls/game/weapon.h"
#include "../../dlls/game/mp_manager.hpp"


struct coopObjectives_s coopObjectives_t;

str coop_objectivesStoryGet(str lang)
{
	if (lang == "Deu") {
		return coopObjectives_t.story_deu;
	}
	return coopObjectives_t.story;
}

void coop_objectivesStorySet(str sStory, str lang)
{
	coopObjectives_t.lastUpdated_story = level.time;
	if (lang == "Deu") {
		coopObjectives_t.story_deu = sStory;
	}
	else {
		coopObjectives_t.story = sStory;
	}
}


int coop_getObjectivesCycle()
{
	return coopObjectives_t.objectiveCycle;
}

void coop_setObjectivesCycle()
{
	coopObjectives_t.objectiveCycle++;
}

str coop_getObjectivesCvarName(int iObjectivesItem)
{
	if (iObjectivesItem < 2) {
		iObjectivesItem = 1;
	}
	else if (iObjectivesItem > 11) {
		iObjectivesItem = 11;
	}

	str sCvar;
	switch (iObjectivesItem)
	{
	case 1:
		sCvar = "coop_one";
		break;
	case 2:
		sCvar = "coop_two";
		break;
	case 3:
		sCvar = "coop_three";
		break;
	case 4:
		sCvar = "coop_four";
		break;
	case 5:
		sCvar = "coop_five";
		break;
	case 6:
		sCvar = "coop_six";
		break;
	case 7:
		sCvar = "coop_seven";
		break;
	case 8:
		sCvar = "coop_eight";
		break;
	case 9:
		sCvar = "coop_t1";
		break;
	case 10:
		sCvar = "coop_t2";
		break;
	default:
		sCvar = "coop_t3";
		break;
	}
	return sCvar;
}



//================================================================    
// Description: Sets story for Coop Objectives       
//================================================================
void coop_objectivesStorySet(Player* player)
{
	if (!player) { return; }

	//Set story if there is none set already by script
	//this is just a backup if someone forgot to set it or if the var gets inizialised late
	str storyEng = coop_objectivesStoryGet("Eng");
	str storyDeu = coop_objectivesStoryGet("Deu");
	if (!storyEng.length() || Q_stricmp(storyEng.c_str(), "$$Empty$$") == 0) {
		coop_objectivesStorySet(program.coop_getStringVariableValue("coop_string_story"),"Eng");
	}
	if (!storyDeu.length() || Q_stricmp(storyDeu.c_str(), "$$Empty$$") == 0) {
		coop_objectivesStorySet( program.coop_getStringVariableValue("coop_string_story_deu"),"Deu");
	}


	//get localized story
	str sStory = "";
	if (gamefix_getLanguage(player) == "Deu" && coop_objectivesStoryGet("Deu").length()) {
		sStory = coop_objectivesStoryGet("Deu");
	}
	//if that failed or player has not a german version, set english story
	if (!sStory.length() && coop_objectivesStoryGet("Eng").length()) {
		sStory = coop_objectivesStoryGet("Eng");
	}
	//if that failed set empty
	if (!sStory.length()) {
		sStory = "$$Empty$$";
	}

	//send story
	gamefix_playerDelayedServerCommand(player->entnum, va("set coop_story %s", sStory.c_str()), 0.0f);
}

//================================================================          
// Description: updates the objective marker entities to sync with their master             
//================================================================
void coop_objectivesMarkerUpdate( void )
{
	gi.Printf("coop_objectivesMarkerUpdate - NOT IMPLEMENTED\n");
	/*
	int i;
	Entity *eEntity = NULL;
	Entity *eMarker = NULL;
	for ( i = 0; i < globals.max_entities; i++ ){
		eEntity = G_GetEntity( i );
		if ( eEntity && eEntity->myCoopMarker ){

			eMarker = eEntity->myCoopMarker;
			float fScale = 0.05;

			if ( !level.cinematic ){
				//hzm coop mod chrissstrahl - manaual bind to real objective, has many advantages (will not be hidden or scaled)
				if ( eMarker->origin != eEntity->origin ){
					eMarker->setOrigin( eEntity->origin );
				}

				//hzm coop mod chrissstrahl - scale our marker relative to the original marker
				int playerNum;
				
				Entity *player;
				Vector vCalc;
				float fLength = 999999;
				for ( playerNum = 0; playerNum < maxclients->integer; playerNum++ ){
					player = g_entities[playerNum].entity;
					if ( player && player->client && player->isSubclassOf( Player ) ){
						vCalc = ( ( player->origin ) - ( eMarker->origin ) );

						if ( vCalc.length() < fLength ){
							fLength = vCalc.length();
						}
					}
				}

				if ( fLength > 12000.0f ){
					fLength = 12000.0f;
				}

				fLength = ( fLength / 3000 );
				fScale += fLength;
				//eMarker->edict->s.scale
			}
			else{ //make the markers very very small, avoid them to be visible during cinematic sequences
				fScale = 0.005;
			}
			

			eMarker->setScale( fScale );
		}
	}
	*/
}

//================================================================        
// Description: spawns marker for given objectives entity             
//================================================================
void coop_objectivesMarker( Entity *eMaster )
{

	if ( !CoopManager::Get().IsCoopEnabled() || g_gametype->integer == GT_SINGLE_PLAYER)return;

	Entity		   *entMiObj;
	Entity		   *entSpawn;

	entMiObj = G_GetEntity( eMaster->entnum );

	// create a new entity
	SpawnArgs args;
	args.setArg( "classname" , "ScriptModel" );
	//args.setArg( "model" , "sysimg/icons/mp/team_red.spr" );
	//args.setArg( "model" , "sysimg/icons/mp/actionhero_icon.spr" );
	args.setArg( "model" , "models/hud/radar_ground-plane.tik" );
	args.setArg( "origin" , va( "%f %f %f" , entMiObj->origin[0] , entMiObj->origin[1] , entMiObj->origin[2] ) );
	args.setArg( "scale" , "0.25" );
	args.setArg( "angle" , "-1" );
	args.setArg( "notsolid" , "1" );
	args.setArg( "rotateZ" , "200" );
	//args.setArg( "rotateX" , "999" );
	args.setArg( "rendereffects" , "+depthhack" );
	args.setArg( "targetname" , va("%s_objMarker" , entMiObj->targetname.c_str()) );//use original name plus additional
	entSpawn = args.Spawn();

	//hzm coop mod chrissstrahl - used to store the entity that marks this missionObjective
	gi.Printf("coop_objectivesMarker - NOT IMPLEMENTED\n");
	//eMaster->myCoopMarker = entSpawn;
}

//================================================================         
// Description: removes marker for given objectives entity 
//================================================================
void coop_objectivesMarkerRemove( Entity *eMaster )
{
	gi.Printf("coop_objectivesMarkerRemove - NOT IMPLEMENTED\n");

	if ( !eMaster )return;

	/*
	if ( eMaster->myCoopMarker != NULL ){
		Entity *eMarker = eMaster->myCoopMarker;
		G_RemoveEntityFromExtraList( eMarker->entnum );
		eMarker->PostEvent( EV_Remove , 0.0f );
	}
	*/
}


//================================================================   
// Description: setup the objectives for given player
//================================================================
void coop_objectivesSetup( Player *player)
{
	if ( !player || gameFixAPI_isBot(player) || CoopManager::Get().getPlayerData_objectives_setupDone(player) || !player->coop_hasCoopInstalled()) {
		return;
	}

	str sCvar;
//reset objectives and tactical info
	for (int i = 1; i < 12; i++ ){
		sCvar = coop_getObjectivesCvarName( i );
		if ( i < 9){
			gamefix_playerDelayedServerCommand( player->entnum , va( "set %s_s 0" , sCvar.c_str() ) );
		}else{
			gamefix_playerDelayedServerCommand( player->entnum , va( "set %s ^0" , sCvar.c_str() ) );
		}
	}
	
	if (gameFixAPI_inSingleplayer()) {
		//make sure the coop objectives hud is displayed when we play a custom (coop) map
		if (CoopManager::Get().getMapFlags().stockMap) {
			gamefix_playerDelayedServerCommand(player->entnum, "set coop_oExc score");
		}
		else {
			gamefix_playerDelayedServerCommand(player->entnum,"set coop_oExc pushmenu coop_obj");
		}
	}
	
	//standard maps always have ritual entertainment as author
	if (gameFixAPI_mapIsStock(gamefix_cleanMapName(level.mapname))) {
		coopObjectives_t.levelAuthor = "Ritual Entertainment";
	}
	else {
		if (!coopObjectives_t.levelAuthor.length()) {
			coopObjectives_t.levelAuthor = program.coop_getStringVariableValue( "coop_string_levelAuthor" );
		}
		if (!coopObjectives_t.levelAuthor.length()){
			coopObjectives_t.levelAuthor = "$$Empty$$";
		}
	}
	gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand coop_objAuthor labeltext %s", gamefix_replaceForLabelText(coopObjectives_t.levelAuthor).c_str()));
	gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand coop_objMap title %s", level.mapname.c_str()));
	gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand coop_objSkill title %d", skill->integer));
	
	//hzm coop mod chrissstrahl - set story right away
	coop_objectivesStorySet( player );

	CoopManager::Get().setPlayerData_objectives_setupDone(player);
}


//================================================================         
// Description: called when a uservar is set and variables have to be updated           
//================================================================
void coop_objectivesUpdateUservar( int iUservar )
{
	if ( level.mission_failed )
		return;

	int i;
	gentity_t *gentity;
	Player *player = NULL;

	//hzm coop mod chrissstrahl - return in sp directly
	if ( g_gametype->integer == GT_SINGLE_PLAYER ){
		player = ( Player * )g_entities[0].entity;
		if ( player ){
			if ( iUservar == 1){
				gamefix_playerDelayedServerCommand( player->entnum , va( "globalwidgetcommand coop_objAuthor title %s" , coopObjectives_t.levelAuthor.c_str() ) );
			}else if ( iUservar == 2){
				gamefix_playerDelayedServerCommand( player->entnum , va( "set coop_story %s" , coopObjectives_t.story.c_str() ) );
			}else if ( iUservar == 3 ){
				if ( player->coop_hasLanguageGerman() ){
					gamefix_playerDelayedServerCommand( player->entnum , va( "set coop_story %s" , coopObjectives_t.story_deu.c_str() ) );
				}
			}
		}
		return;
	}

	gi.Printf( va( "Uservar updated on world, uservar%d\n" , iUservar ) );

	for ( i = 0; i < maxclients->integer; i++ ){
		gentity = &g_entities[i];
		if ( gentity->inuse && gentity->entity && gentity->client && gentity->entity->isSubclassOf( Player ) ){
			player = ( Player * )gentity->entity;
			if ( player && player->coop_hasCoopInstalled() ){
				if ( iUservar == 1){
					gamefix_playerDelayedServerCommand( player->entnum , va( "globalwidgetcommand coop_objAuthor title %s" , coopObjectives_t.levelAuthor.c_str() ) );
				}
				else if ( iUservar == 2 ){
					gamefix_playerDelayedServerCommand( player->entnum , va( "set coop_story %s" , coopObjectives_t.story.c_str() ) );
				}
				else if ( iUservar == 3 ){
					if ( player->coop_hasLanguageGerman() ){
						gamefix_playerDelayedServerCommand( player->entnum , va( "set coop_story %s" , coopObjectives_t.story_deu.c_str() ) );
					}
				}
			}
		}
	}
}

//================================================================
// Description: INDICATE HUD AND PLAY NOTIFICATION SOUND
//================================================================
void coop_objectivesNotify( Player* player )
{
	if ( !player || level.mission_failed )
		return;

	if ( coopObjectives_t.objectiveItemStatus[0] == -1 && coopObjectives_t.objectiveItemStatus[1] == -1 &&
		coopObjectives_t.objectiveItemStatus[2] == -1 && coopObjectives_t.objectiveItemStatus[3] == -1 &&
		coopObjectives_t.objectiveItemStatus[4] == -1 && coopObjectives_t.objectiveItemStatus[5] == -1 &&
		coopObjectives_t.objectiveItemStatus[6] == -1 && coopObjectives_t.objectiveItemStatus[7] == -1 )
	{
		return;
	}

	player->coop_hudsAdd( player , "objectivenotifytext" );
	player->Sound( "snd_objectivechanged" , CHAN_LOCAL );
	player->coop_hudsAdd( player , "coop_notify" );//SYMBOL->(|!|)
}

//================================================================
// Description: Checks if a objective should be send to the player
//================================================================
void coop_objectivesUpdatePlayer( Player* player )
{
	if ( !player )
		return;

	if (gameFixAPI_inMultiplayer()){
		if (!CoopManager::Get().getPlayerData_coopSetupDone(player) || !CoopManager::Get().getPlayerData_objectives_setupDone(player)) {
			coop_objectivesSetup(player);
			CoopManager::Get().playerSetupCoopUi(player);
			return;
		}
	}

	if ( g_gametype->integer == GT_SINGLE_PLAYER || multiplayerManager.inMultiplayer()) {
		if (!player->coop_getObjectivesCycle() /* && (CoopManager::Get().getPlayerData_lastSpawned(player) + 3.0f) < level.time*/) {
			player->coop_setObjectivesCycle();

			coop_objectivesNotify( player );
			
			int i , iState;
			for ( i = 9; i < 12; i++ ){
				coop_objectivesShow( player , i , 0 , true );//was false
			}
			for ( i = 1; i < 9; i++ ){
				iState = coopObjectives_t.objectiveItemStatus[( i - 1 )];
				//gi.Printf( va( "status: [%i] -> %i\n", i, iState ) );
				if ( iState != -1 ){//state set, means this objective is being used
					coop_objectivesShow( player , i , iState , true );
				}
			}
		}
	}
}


//================================================================         
// Description: MANAGES OBJECTIVES FOR SINGLEPLAYER MODE
//================================================================
void coop_objectivesSingleplayer( str sObjectiveState , int iObjectiveNumber , int iShowNow )
{
	gentity_t *gentity;
	Player *player = NULL;
	gentity = &g_entities[0];
	if ( gentity->inuse && gentity->entity && gentity->client && gentity->entity->isSubclassOf( Player ) ){
		player = ( Player * )gentity->entity;
	}
	else{
		return;
	}

	qboolean Show = iShowNow;
	str ObjName;
	ObjName = program.coop_getStringVariableValue( va( "coop_string_objectiveItem%i" , iObjectiveNumber ) );
	if ( ObjName.length() < 1 ){
		gi.Printf( "coop_objectivesSingleplayer: objective name was empty\n" );
		return;
	}

	gi.Printf( va( "OBJECTIVE SP: %s\n" , ObjName.c_str() ) );

	int ObjIndex;
	ObjIndex = gi.MObjective_GetIndexFromName( ObjName.c_str() );
	if ( ObjIndex == 0 ){
		gi.Printf( "coop_objectivesSingleplayer: invalid objective name\n" );
		return;
	}
	
	//[b60012] chrissstrahl - fix missing .c_str()
	if ( !Q_stricmp( sObjectiveState.c_str() , "ObjectiveComplete") ){
		bool complete = true;

		switch ( ObjIndex )
		{
		case OBJECTIVE1:
			player->coop_setObjectiveState(OBJECTIVE1_COMPLETE,complete);
			break;

		case OBJECTIVE2:
			player->coop_setObjectiveState(OBJECTIVE2_COMPLETE, complete);
			break;

		case OBJECTIVE3:
			player->coop_setObjectiveState(OBJECTIVE3_COMPLETE, complete);
			break;

		case OBJECTIVE4:
			player->coop_setObjectiveState(OBJECTIVE4_COMPLETE, complete);
			break;

		case OBJECTIVE5:
			player->coop_setObjectiveState(OBJECTIVE5_COMPLETE, complete);
			break;

		case OBJECTIVE6:
			player->coop_setObjectiveState(OBJECTIVE6_COMPLETE, complete);
			break;

		case OBJECTIVE7:
			player->coop_setObjectiveState(OBJECTIVE7_COMPLETE, complete);
			break;

		case OBJECTIVE8:
			player->coop_setObjectiveState(OBJECTIVE8_COMPLETE, complete);
			break;

		default:
			break;
		}
	}
	//[b60012] chrissstrahl - fix missing .c_str()
	else if ( !Q_stricmp( sObjectiveState.c_str(), "ObjectiveFailed")) {
		bool failed = true;
		switch ( ObjIndex )
		{
		case OBJECTIVE1:
			player->coop_setObjectiveState(OBJECTIVE1_FAILED, failed);
			break;

		case OBJECTIVE2:
			player->coop_setObjectiveState(OBJECTIVE2_FAILED, failed);
			break;

		case OBJECTIVE3:
			player->coop_setObjectiveState(OBJECTIVE3_FAILED, failed);
			break;

		case OBJECTIVE4:
			player->coop_setObjectiveState(OBJECTIVE4_FAILED, failed);
			break;

		case OBJECTIVE5:
			player->coop_setObjectiveState(OBJECTIVE5_FAILED, failed);
			break;

		case OBJECTIVE6:
			player->coop_setObjectiveState(OBJECTIVE6_FAILED, failed);
			break;

		case OBJECTIVE7:
			player->coop_setObjectiveState(OBJECTIVE7_FAILED, failed);
			break;

		case OBJECTIVE8:
			player->coop_setObjectiveState(OBJECTIVE8_FAILED, failed);
			break;

		default:
			break;
		}
	}
	else{
		//Update the objective show flag.
		gi.MObjective_SetShowObjective( ObjName.c_str() , Show );

		//Set the appropriate bit on the flag passed to the client.
		unsigned int bitToChange;

		switch ( ObjIndex )
		{
		case OBJECTIVE1:
			bitToChange = OBJECTIVE1_SHOW;
			break;
		case OBJECTIVE2:
			bitToChange = OBJECTIVE2_SHOW;
			break;
		case OBJECTIVE3:
			bitToChange = OBJECTIVE3_SHOW;
			break;
		case OBJECTIVE4:
			bitToChange = OBJECTIVE4_SHOW;
			break;
		case OBJECTIVE5:
			bitToChange = OBJECTIVE5_SHOW;
			break;
		case OBJECTIVE6:
			bitToChange = OBJECTIVE6_SHOW;
			break;
		case OBJECTIVE7:
			bitToChange = OBJECTIVE7_SHOW;
			break;
		case OBJECTIVE8:
			bitToChange = OBJECTIVE8_SHOW;
			break;
		default:
			bitToChange = 0;
			break;
		}

		if ( Show && !( player->coop_getObjectiveState() & bitToChange)) {
			player->coop_setObjectiveState(bitToChange, true);
			player->Sound( "snd_objectivechanged" , CHAN_LOCAL );
		}
	}
}
     
//================================================================
// Description: UPDATES A OBJECTIVE, IN RELATION TO COOP LEVEL SCRIPT, needed sice we can't read floats from script in the interpreter
//================================================================
void coop_objectivesUpdate( str sObjectiveState, int iObjectiveNumber, int  bShow  )
{
	int iObjectivteStatus;
	str sObjective;
	str sVariableName = "coop_string_objectiveItem";
	coop_setObjectivesCycle();

//get the mission objective string from script variable, make sure the item range is valid 1-8 (-11 since we gravtactical info here as well)
	if ( iObjectiveNumber < 2 ){
		iObjectiveNumber = 1;
	}
	else if ( iObjectiveNumber > 11 ){
		iObjectiveNumber = 11;
	}

//check for valid objectives state, set incomplete on default
	sObjectiveState = sObjectiveState.tolower();
	
	//[b60012] chrissstrahl - fix missing .c_str()
	if ( !Q_stricmp( sObjectiveState.c_str(), "complete") || !Q_stricmp( sObjectiveState.c_str(), "objectivecomplete") ){
		iObjectivteStatus = 2;
		sObjectiveState = "ObjectiveComplete";
	}else if ( !Q_stricmp( sObjectiveState.c_str(), "failed") || !Q_stricmp( sObjectiveState.c_str(), "objectivefailed") ){
		iObjectivteStatus = 3;
		sObjectiveState = "ObjectiveFailed";
	}else{
		iObjectivteStatus = 1;
		sObjectiveState = "ObjectiveIncomplete";
	}

//update global objective arrays
	if ( iObjectiveNumber < 9){
//make sure that it stays completed once it has been completed!
		if ( coopObjectives_t.objectiveItemStatus[( iObjectiveNumber - 1 )] == 2 ){
			iObjectivteStatus = 2;
			sObjectiveState = "ObjectiveComplete";		
		}else{
			coopObjectives_t.objectiveItemStatus[( iObjectiveNumber - 1 )] = iObjectivteStatus;
		}

		if ( iObjectivteStatus == 2){
			coopObjectives_t.objectiveItemCompletedAt[( iObjectiveNumber - 1 )] = level.time;
		}
	}

	sVariableName += iObjectiveNumber;
	sObjective = program.coop_getStringVariableValue( sVariableName.c_str());

//check is it a valid string
	if ( !sObjective.length() ){
		gi.Printf( va( "CO-OP OBJECTIVE ERROR: script variable %s is empty or to short!\n" , sVariableName.c_str() ) );
		return;
	}

//print status into the game console
	str sLocalString = "$$";
	//do not automatically localize for custom maps!
	if ( !gameFixAPI_mapIsStock(gamefix_cleanMapName(level.mapname)) ){
		sLocalString = "";
	}
	//do not localize if it already is //[b60011] chrissstrahl - update checking alternatively for space
	else if (sObjective[0] == '$' && sObjective[1] == '$' || gamefix_findChar(sObjective.c_str(),' ') != -1) {
		sLocalString = "";
	}

	//phrase text replace umlaute הצהü and ß, allow german text to work right
	sObjective = sObjective;

	gi.Printf( va( "OBJECTIVE $$%s$$ | %s%s%s\n" , sObjectiveState.c_str() , sLocalString.c_str() , sObjective.c_str() , sLocalString.c_str() ) );

	//TEST
	gentity_t* gentity;
	Player* player = nullptr;
	gentity = &g_entities[0];
	if (gentity->inuse && gentity->entity && gentity->client && gentity->entity->isSubclassOf(Player)) {
		player = (Player*)gentity->entity;

		coop_objectivesShow(player, iObjectiveNumber, iObjectivteStatus, 1);
		//coop_objectivesNotify(player);
	}


//handle singleplayer objectives, this only works, if the objectives are default game objectives
/*	if ( player && g_gametype->integer == GT_SINGLE_PLAYER){
		if ( gameFixAPI_mapIsStock(gamefix_cleanMapName(level.mapname)) ){
			coop_objectivesSingleplayer( sObjectiveState , iObjectiveNumber , (int)bShowNow );	
		}else{
			coop_objectivesNotify( player );
			coop_objectivesShow( player , iObjectiveNumber , iObjectivteStatus , bShowNow );
		}
	}
	*/
}

//================================================================          
// Description: SHOWS OBJECTIVE TO THE GIVEN PLAYER IF NEEDED          
//================================================================
void coop_objectivesShow( Player *player , int iObjectiveItem , int iObjectiveState , bool bfObjectiveShow )
{
	if ( !player || level.mission_failed || multiplayerManager.inMultiplayer() && !CoopManager::Get().getPlayerData_coopSetupDone(player) || g_gametype->integer == GT_SINGLE_PLAYER && gamefix_getEntityVarFloat(player,"_spArmoryEquiped") != 1.0f) {
		return;
	}

	str sObjective;
	str sStatusName;
	str sVariableName;

//determin objective status
	switch ( iObjectiveState )
	{
		case 0:
			sStatusName = "BAD_OBJSTATE";
			break;
		case 1:
			sStatusName = "^5$$ObjectiveIncomplete$$";
			break;
		case 2:
			sStatusName = "^2$$ObjectiveComplete$$";
			break;
		default:
			sStatusName = "^1$$ObjectiveFailed$$";
			iObjectiveState = 3;
			break;
	}

//get cvarname
	str sCvar = coop_getObjectivesCvarName( iObjectiveItem );
	str sLocalString = "$$";

//do not automatically localize for custom maps!
	if ( !gameFixAPI_mapIsStock(gamefix_cleanMapName(level.mapname)) ){
		sLocalString = "";
	}

	if ( iObjectiveItem < 9){
		sVariableName = "coop_string_objectiveItem";
		sVariableName += iObjectiveItem;

		if ( player->coop_hasLanguageGerman() && gameFixAPI_mapIsStock(gamefix_cleanMapName(level.mapname)) == false ){
			str sTempVar , sTempVar2;
			sTempVar2 = sVariableName;
			sTempVar2 += "_deu";
			sTempVar = program.coop_getStringVariableValue( sTempVar2.c_str() );
			if ( sTempVar.length() > 0 ){
				sObjective = sTempVar;
			}else{
				sObjective = program.coop_getStringVariableValue( sVariableName.c_str() );
				gi.Printf( va( "WARNING: No German objective string set for Objective: %d\n" , iObjectiveItem ) );
			}
		}else{
			sObjective = program.coop_getStringVariableValue( sVariableName.c_str() );
		}
		if ( sObjective.length() > 1 ){
		//make sure it is not localized twiche... //[b60011] chrissstrahl - update checking alternatively for space
			if (sObjective[0] == '$' && sObjective[1] == '$' || gamefix_findChar(sObjective.c_str(), ' ')) {
				sLocalString = "";
			}

			//if player has the mod installed display to menu
			if ( player->coop_hasCoopInstalled() ){
				//phrase text replace umlaute הצהü and ß, allow german text to work right - using loc strings $$u$$
				sObjective = sObjective;

				gamefix_playerDelayedServerCommand( player->entnum , va( "set %s %s%s%s" , sCvar.c_str() , sLocalString.c_str() , sObjective.c_str() , sLocalString.c_str() ) );
				gamefix_playerDelayedServerCommand( player->entnum , va( "set %s_s %i" , sCvar.c_str() , iObjectiveState ) );
			}
			//otherwhise display to chat hud
			else{
				//phrase text replace umlaute הצהü and ß, allow german text to work better - using alias: ue ae oe ss
				sObjective = sObjective;
				//don't display if, objective is complete and has been completed more than 5 secounds ago
				float fAge = coopObjectives_t.objectiveItemCompletedAt[( iObjectiveItem - 1 )];
				if ( ( fAge + 5 ) > level.time || fAge < 0 ){
					if ( gi.GetNumFreeReliableServerCommands( player->entnum ) > 32 ){
						if ((player->coop_getObjectivesPrintedTitleLast() + 3) < level.time) {
							player->hudPrint( "\n\n^8======^5 $$MissionObjectives$$ ^8======\n" );
						}
						player->coop_setObjectivesPrintedTitleLast();
						player->hudPrint( va( "%s:^8 %s%s%s\n" , sStatusName.c_str() , sLocalString.c_str() , sObjective.c_str() , sLocalString.c_str() ) );
					}
				}
			}
		}
	}
	else
	{
		str sHint = "";
		sVariableName = "coop_string_objectiveTacticalInfo";
		sVariableName += (iObjectiveItem - 8);
		str sHintString;
		if ( player->coop_hasLanguageGerman() ){
			sHintString = sVariableName;
			sHintString += "_deu";
			sHint = program.coop_getStringVariableValue( sHintString.c_str() );
		}
		if ( sHint.length() < 1 ){
			sHint = program.coop_getStringVariableValue( sVariableName.c_str() );
		}
		if ( sHint.length() > 0 && player->coop_hasCoopInstalled() == 1 ){
			gamefix_playerDelayedServerCommand( player->entnum , va( "set %s %s%s%s" , sCvar.c_str() , sLocalString.c_str() , sHint.c_str() , sLocalString.c_str() ) );
		}
	}
}

//================================================================           
// Description: SHOWS TACTICAL TO THE GIVEN PLAYER           
//================================================================
void coop_objectives_tacticalShow( Player *player , int iTactical )
{
	if ( player && player->coop_hasCoopInstalled() == 1 )
	{
		if ( iTactical < 1 )
		{
			iTactical = 1;
		}
		else if ( iTactical > 3 )
		{
			iTactical = 3;
		}

		str sTacEng = program.coop_getStringVariableValue( va("coop_string_objectiveTacticalInfo%d", iTactical ) );
		str sTacDeu = program.coop_getStringVariableValue( va( "coop_string_objectiveTacticalInfo%d_deu" , iTactical ) );
		if ( player->coop_hasLanguageGerman() && sTacDeu.length() ) {
			gamefix_playerDelayedServerCommand( player->entnum , va( "set coop_t%d %s" , iTactical , sTacDeu.c_str() ) );
		}
		else if( sTacEng.length() ){
			gamefix_playerDelayedServerCommand( player->entnum , va( "set coop_t%d %s" , iTactical , sTacEng.c_str() ) );
		}
	}
}

//================================================================           
// Description: RESETS OBJECTIVES           
//================================================================
void coop_objectives_reset()
{
	coopObjectives_t.levelAuthor = "Ritual Entertainment";
	coopObjectives_t.story = "$$Empty$$";
	coopObjectives_t.story_deu = "$$Empty$$";
	coopObjectives_t.lastUpdated_story = -1;
	coopObjectives_t.objectiveCycle = 0;
	coopObjectives_t.objectiveItemPrintedTitleLastTime = -1.0;
	coopObjectives_t.objectiveItemPrintedTitleLast = "";
	coopObjectives_t.objectiveItem[0] = "$$Empty$$";
	coopObjectives_t.objectiveItem_deu[0] = "$$Empty$$";
	for (int i = 1; i < _COOP_SETTINGS_OBJECTIVES_MAX; i++) { coopObjectives_t.objectiveItem[i] = va("%d",i); }
	for (int i = 1; i < _COOP_SETTINGS_OBJECTIVES_MAX; i++) { coopObjectives_t.objectiveItem_deu[i] = va("%d",i); }
	for (int i = 0; i < _COOP_SETTINGS_OBJECTIVES_MAX; i++) { coopObjectives_t.objectiveItemStatus[i] = -1; }
	for (int i = 0; i < _COOP_SETTINGS_OBJECTIVES_MAX; i++) { coopObjectives_t.objectiveItemShow[i] = 0; }
	for (int i = 0; i < _COOP_SETTINGS_OBJECTIVES_MAX; i++) { coopObjectives_t.objectiveItemCompletedAt[i] = -999.0f; }
}

#endif