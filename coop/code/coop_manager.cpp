#ifdef ENABLE_COOP

#include "../../dlls/game/gamefix.hpp"
#include "../../dlls/game/level.h"
#include "../../dlls/game/mp_manager.hpp"
#include "coop_objectives.hpp"
#include "coop_armory.hpp"
#include "coop_radar.hpp"
#include "coop_manager.hpp"

coopManager_client_persistant_s coopManager_client_persistant_t[MAX_CLIENTS];
coopManager_mapSettings_s coopManager_mapSettings_t;

CoopManager& CoopManager::Get() {
    static CoopManager instance;
    return instance;
}


bool CoopManager::IsCoopEnabled() const {
    return coopEnabled;
}

void CoopManager::DisableCoop() {
    coopEnabled = false;
}

bool CoopManager::IsCoopLevel() {
    return mapFlags.coopMap;
}

bool CoopManager::IsRpgEnabled() const {
    return rpgEnabled;
}

//check if specific coop files are included
//this is required because certain global_scripts
//need to be included differehtly then
str CoopManager::IncludeScriptReplace(str sLex) {
   
    //check for known coop script files
    IncludeScriptCheck(sLex);
    
    if (IncludedScriptCoop()) {
        if (gamefix_findString(sLex.c_str(), "maps/global_scripts/", false) != -1) {
            str sFile = gamefix_getFileName(sLex);
            str sReplace = "co-op/maps/global_scripts/";
            sReplace += sFile;

            if (gi.FS_ReadFile(sReplace.c_str(), NULL, true) != -1) {
                //DEBUG_LOG(va("%s used from /co-op/ folder\n", sFile.c_str()));
                gi.Printf(va("%s used from /co-op/ folder\n", sFile.c_str()));
                return sReplace;
            }
        }
    }
    return "";
}

void CoopManager::IncludeScriptCheck(str &sLex) {
    if (Q_stricmp(sLex,_COOP_FILE_main) == 0) {
        mapFlags.scriptIncludedCoopMain = true;
        return;
    }
    if (Q_stricmp(sLex, _COOP_FILE_noscript) == 0) {
        mapFlags.scriptIncludedCoopNoscript = true;
        return;
    }
    if (Q_stricmp(sLex, _COOP_FILE_multioptions) == 0) {
        mapFlags.scriptIncludedCoopMom = true;
        return;
    }
    if (Q_stricmp(sLex, _COOP_FILE_multioptions4) == 0) {
        mapFlags.scriptIncludedCoopMom4 = true;
        return;
    }
    if (Q_stricmp(sLex, _COOP_FILE_mrm) == 0) {
        mapFlags.scriptIncludedCoopMrm = true;
        return;
    }
}

bool CoopManager::IncludedScriptCoop() {
    return mapFlags.scriptIncludedCoopMain;
}

str CoopManager::playerScriptCallValidateThreadname(str threadName)
{
    str threadToCall = "";
    for (unsigned int i = 0; i < strlen(threadName); i++) {
        if (threadName[i] == '"' ||
            threadName[i] == '%' ||
            threadName[i] == ';' ||
            threadName[i] == ' ')
        {
            break;
        }
        threadToCall += threadName[i];
    }
    return threadToCall;
}

//checks if this player is allowed to call this thread at this time and circumstance - security measurs
bool CoopManager::playerScriptCallExecute(Entity* entPlayer, str commandName, str threadName, Entity* entUsed)
{
    if (g_gametype->integer == GT_SINGLE_PLAYER) {
        return true;
    }

    if (!entPlayer || !multiplayerManager.inMultiplayer() || !CoopManager::Get().IsCoopEnabled() || !entPlayer->isSubclassOf(Player)) {
        return false;
    }


    threadName = playerScriptCallValidateThreadname(threadName);
    

    Player* player = (Player*)entPlayer;
    /*Deactivated for testing
    if (player->coop_isAdmin()) {
        ExecuteThread(threadName, true,entPlayer);
        return true;
    }*/

    for (int i = CoopSettings_playerScriptThreadsAllowList.NumObjects(); i > 0; i--) {
        CoopSettings_clientThreads_s threadListTemp;
        threadListTemp = CoopSettings_playerScriptThreadsAllowList.ObjectAt(i);

        //command match
        if (Q_stricmp(commandName.c_str(), threadListTemp.command.c_str()) == 0) {
            //dialog active check
            if (entUsed && entUsed->isSubclassOf(Actor) && Q_stricmpn(threadListTemp.command.c_str(), "dialogrunthread", strlen(threadListTemp.command.c_str()))) {
                Actor* actor = (Actor*)entUsed;
                str sDialogName = actor->coop_getBranchDialogName();
                if (!strlen(sDialogName)) {
                    return false;
                }
            }
            //thread match
            if (strlen(threadListTemp.thread) && Q_stricmpn(threadName.c_str(), threadListTemp.thread.c_str(), strlen(threadListTemp.thread.c_str())) == 0) {
                //item check
                if (strlen(threadListTemp.item) && threadListTemp.item != "None") {
                    str itemName = "";
                    weaponhand_t	hand = WEAPON_ANY;
                    player->getActiveWeaponName(hand, itemName);
                    //mismatch - just skip
                    if (Q_stricmpn(threadListTemp.item.c_str(), threadListTemp.item.c_str(), strlen(threadListTemp.item.c_str())) != 0) {
                        return false;
                    }
                }
                ExecuteThread(threadName, true, entPlayer);
                return true;
            }
        }
    }
    return false;
}

bool CoopManager::entityUservarGetKillMessage(Entity* inflictor, str& killmessageEng, str& killmessageDeu)
{
    int uservarsNumHandeled = 10;

    if (!inflictor) {
        killmessageEng = "Inflictor-Missing-???";
        killmessageDeu = "Inflictor-Fehlt-???";
        return true;
    }
    killmessageEng = "";
    killmessageDeu = "";

    str entityValue="";
    ScriptVariable* entityData = nullptr;
    for (int i = 1; i <= uservarsNumHandeled; i++) {
        entityData = inflictor->entityVars.GetVariable(va("uservar%d", i));       
        if (!entityData) {
            continue;
        }

        entityValue = entityData->stringValue();
        if (Q_stricmpn("killmessage", entityValue.c_str(), 11) == 0) {
            //"uservar1" "killmessage AbC"
            if (entityValue.length() > 12) {
                killmessageEng = "";
                for (int j = 12; j < entityValue.length(); j++) {
                    killmessageEng += entityValue[j];
                }
            }
        }

        entityValue = entityData->stringValue();
        if (Q_stricmpn("killmessage_de", entityValue.c_str(), 14) == 0) {
            //"uservar1" "killmessage_de AbC"
            if (entityValue.length() > 15) {
                killmessageDeu = "";
                for (int j = 15; j < entityValue.length(); j++) {
                    killmessageDeu += entityValue[j];
                }
            }
        }
    }
    if (!killmessageDeu.length() && killmessageEng.length()) {
        killmessageDeu = killmessageEng;
    }

    if (!killmessageEng.length() && killmessageDeu.length()) {
        killmessageEng = killmessageDeu;
    }

    if (killmessageEng.length()) {
        return true;
    }
    return false;
}

bool CoopManager::entityUservarGetName(Entity* inflictor, str& killmessageEng, str& killmessageDeu)
{
    int uservarsNumHandeled = 10;

    if (!inflictor) {
        killmessageEng = "Inflictor-Missing-???";
        killmessageDeu = "Inflictor-Fehlt-???";
        return true;
    }
    killmessageEng = "";
    killmessageDeu = "";

    str entityValue = "";
    ScriptVariable* entityData = nullptr;
    for (int i = 1; i <= uservarsNumHandeled; i++) {
        entityData = inflictor->entityVars.GetVariable(va("uservar%d", i));
        if (!entityData) {
            continue;
        }

        entityValue = entityData->stringValue();
        if (Q_stricmpn("name", entityValue.c_str(), 4) == 0) {
            //"uservar1" "killmessage AbC"
            if (entityValue.length() > 5) {
                killmessageEng = "";
                for (int j = 6; j < entityValue.length(); j++) {
                    killmessageEng += entityValue[j];
                }
            }
        }

        if (!killmessageDeu.length() && killmessageEng.length()) {
            killmessageDeu = killmessageEng;
        }

        if (killmessageEng.length()) {
            return true;
        }
    }
    return false;
}

int CoopManager::entityUservarContains(Entity* ent, const str &find)
{
    int uservarsNumHandeled = 10;

    str entityValue = "";
    ScriptVariable* entityData = nullptr;
    for (int i = 1; i <= uservarsNumHandeled; i++) {
        entityData = ent->entityVars.GetVariable(va("uservar%d", i));
        if (!entityData) {
            continue;
        }

        entityValue = entityData->stringValue();
        if (gamefix_findString(entityValue.c_str(), find.c_str(), false)) {
            return i;
        }
    }

    //return value 1-10 is true
    return 0;
}

void CoopManager::ClientThink(Player *player) {
    playerSetup(player);
    playerAdminThink(player);
    coop_objectivesUpdatePlayer(player);
    coop_radarUpdate(player);
}

bool CoopManager::callvoteManager(const str& _voteString) { 
    if (!_voteString.length()) {
        return false;
    }
    Container<str> voteStringList;
    gamefix_listSeperatedItems(voteStringList, _voteString," ");

    if (voteStringList.NumObjects() >= 2) {
        if (Q_stricmp(voteStringList.ObjectAt(1).c_str(), "coop_ff") == 0) {
            float firedlyFireVal = atof(voteStringList.ObjectAt(2));
            coopSettings.setSetting_friendlyFireMultiplicator(firedlyFireVal);
            callvoteUpdateUi("Friendly Fire",va("%.2f", firedlyFireVal), "coopGpoFF");
            return true;
        }
        if (Q_stricmp(voteStringList.ObjectAt(1).c_str(), "coop_airaccelerate") == 0) {
            unsigned int airAccellerate = atoi(voteStringList.ObjectAt(2));
            sv_airaccelerate->integer = airAccellerate;
            callvoteUpdateUi("Air Accelerate",va("%d", airAccellerate), "coopGpoAa");
            return true;
        }
        if (Q_stricmp(voteStringList.ObjectAt(1).c_str(), "coop_maxspeed") == 0) {
            unsigned int maxSpeed = atoi(voteStringList.ObjectAt(2));
            callvoteUpdateUi("Movement Speed",va("%d", maxSpeed), "coopGpoMvSpd");
            world->setPhysicsVar("maxspeed", maxSpeed);
            return true;
        }
        if (Q_stricmp(voteStringList.ObjectAt(1).c_str(), "coop_skill") == 0) {
            unsigned int skillValue = atoi(voteStringList.ObjectAt(2));
            callvoteUpdateUi("Difficulty",va("%d", skillValue), "coopGpoSkill");
            skill->integer = skillValue;
            return true;
        }
        if (Q_stricmp(voteStringList.ObjectAt(1).c_str(), "coop_awards") == 0) {
            bool awards = (bool)atoi(voteStringList.ObjectAt(2));
            callvoteUpdateUi("Coop Awards",va("%d", awards), "coopGpoAw");
            coopSettings.setSetting_awards(awards);
            return true;
        }
    }

    return false;
}

extern qboolean G_SetWidgetTextOfPlayer(const gentity_t* ent, const char* widgetName, const char* widgetText);
void CoopManager::callvoteUpdateUiPlayer(Player *player, str sValue, str sWidget)
{
    if (!player) {
        return;
    }
    if (player->coop_hasCoopInstalled()) {
        gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand %s title %s", sWidget.c_str(), sValue.c_str()));
    }
}

void CoopManager::callvoteUpdateUi(str sText, str sValue, str sWidget)
{
    if (!CoopManager::Get().IsCoopEnabled()) {
        return;
    }

    Player* player = NULL;
    for (int i = 0; i < maxclients->integer; i++) {
        player = (Player*)g_entities[i].entity;
        if (player && player->client && player->isSubclassOf(Player)) {
            multiplayerManager.HUDPrint(player->entnum, va("^5INFO^8: %s set to^5 %s\n", sText.c_str(), sValue.c_str()));
            if (player->coop_hasCoopInstalled()) {
                callvoteUpdateUiPlayer(player, sValue, sWidget);
            }
        }
    }
}

//return player quanity based on certain parameters
int CoopManager::getNumberOfPlayers(bool noDead, bool noSpectator) {
    int iNumberOfPlayer = 0;
    Player* player = nullptr;
    for (int i = 0; i < gameFixAPI_maxClients(); i++) {
        player = gamefix_getPlayer(i);

        if (!player) {
            continue;
        }

        if (noDead && gameFixAPI_isDead((Entity*)player) || noSpectator && gameFixAPI_isSpectator_stef2((Entity*)player)) {
            continue;
        }

        if (gamefix_checkNotarget((Entity*)player)) {
            continue;
        }
        iNumberOfPlayer++;
    }
    return iNumberOfPlayer;
}

//executed once, only on game server start/load
//executed before world and other entities exist
void CoopManager::Init() {
    try {
        gi.Printf(_COOP_INFO_INIT_game);
        DEBUG_LOG(_COOP_INFO_INIT_game);

        LoadSettingsFromINI();
        LoadMapListFromINI();
        LoadPlayerModelsFromINI();

        gi.Printf(_COOP_INFO_INIT_gamedone);
    }
    catch (const char* error) {
        gi.Printf(_COOP_ERROR_fatal, error);
        G_ExitWithError(error);
    }
}

//executed once, only on game server start/load
//loads coop valid model list for players from ini
void CoopManager::LoadPlayerModelsFromINI() {

    //default fallback hardcoded model - if coop gamefiles are missing
    str skinName = "models/char/munro.tik";
    coopManager_validPlayerModels.AddUniqueObject(skinName);

    str contents;
    if (gamefix_getFileContents(_COOP_FILE_validPlayerModels, contents, true)) {
        str sectionContents = gamefix_iniSectionGet(_COOP_FILE_validPlayerModels, contents, "coopSkins");
        
        //DEBUG_LOG("Valid Player Models\n");       
        //DEBUG_LOG("%s\n",skinName.c_str());

        //get models by number, starting with 1
        int i = 1;
        while (skinName.length()) {
            skinName = gamefix_iniKeyGet(_COOP_FILE_validPlayerModels, sectionContents,va("%d",i), "");
            if (skinName.length()) {
                coopManager_validPlayerModels.AddUniqueObject(skinName);
                
                //DEBUG_LOG("%s\n",skinName.c_str());
            }
            i++;
        }
    }
}

//executed once, only on game server start/load
//loads coop settings from ini
void CoopManager::LoadSettingsFromINI() {
    str contents;
    if (gamefix_getFileContents(_COOP_FILE_settings, contents, true)) {
        enemyKillPoints = atoi(gamefix_iniKeyGet(_COOP_FILE_settings, contents, "enemyKillPoints", "0"));
        playerKillPenalty = atoi(gamefix_iniKeyGet(_COOP_FILE_settings, contents, "playerKillPenalty", "0"));
        friendlyFire = gamefix_iniKeyGet(_COOP_FILE_settings, contents, "friendlyFire", "false") == "true";
        giveSpawnItems = gamefix_iniKeyGet(_COOP_FILE_settings, contents, "giveSpawnItems", "false") == "true";
    }
}

//executed once, only on game server start/load
//loads coop settings from ini
void CoopManager::LoadMapListFromINI() {
    if (!gamefix_getFileContents(_COOP_FILE_maplist, coopManager_maplist_contents, true)) {
        //Print maplist file warning
        ///////////////////////////////////////////////////
        gi.Printf(va(_COOP_WARNING_FILE_failed, _COOP_FILE_maplist));
        return;
    }
}

//executed each level load - always
//requires the world to be already spawned
//Other: CoopManager::LevelStart()
void CoopManager::InitWorld() {
    try {
        gi.Printf(_COOP_INFO_INIT_world, level.mapname.c_str());
        DEBUG_LOG(_COOP_INFO_INIT_world, level.mapname.c_str());

        coopSettings.serverConfigCheck();

        SetMapType();

        str coopStatus = "inactive";
        str sAllowSpMaps = "0";

        //this early we can't use API - multiplayer manager has not been started yet
        if (mapFlags.coopMap && g_gametype->integer == GT_MULTIPLAYER) { 
            sAllowSpMaps = "1";
            coopStatus = "ACTIVE";
            coopEnabled = true;
        }
        gi.cvar_set("gfix_allowSpMaps", sAllowSpMaps);

        //notify scripts - is a coop level
        levelVars.SetVariable("isCoopLevel",float(int(coopEnabled)));


        if (coopEnabled) {
            if (world) {
                world->setPhysicsVar("maxSpeed",coopSettings.getSetting_maxSpeed());
            }

            coopSettings.playerCommandsAllow();
            coopSettings.playerScriptThreadsAllow();
            coopSettings.loadScoreList();
            coopSettings.loadDeathList();

            gi.Printf(_COOP_INFO_INIT_status, coopStatus.c_str(), level.mapname.c_str());
        }
        else {
            assert(!coopEnabled);
            gi.Printf("NOT A CO-OP LEVEL but coop is active!!?\n");
        }
    }
    catch (const char* error) {
        gi.Printf(_COOP_ERROR_fatal, error);
        G_ExitWithError(error);
    }
}

//detects what kind of gametype the level
//support and sets related parameters
coopManager_mapSettings_s CoopManager::DetectMapType(str mapName) {

    str mapNameClean = gamefix_cleanMapName(mapName);

    coopManager_mapSettings_s struct_currentMap;
    struct_currentMap.cleanName = mapNameClean;

    //remember if it is a standard map - do this always
    //check if it is a coop or rpg map
    if (gameFixAPI_mapIsStock(mapNameClean))         struct_currentMap.stockMap = true;
    if (mapNameClean.icmpn("coop_", 5) == 0)         struct_currentMap.coopMap = true;
    else if (mapNameClean.icmpn("rpg_", 4) == 0)     struct_currentMap.rpgMap = true;

    struct_currentMap.checkPoint = "";

    if (!coopManager_maplist_contents.length()) {
        return struct_currentMap;
    }
    const str fileContents = coopManager_maplist_contents;


    //Get name of the section the map is listed in
    str sectionOfCurrentMap = gamefix_iniFindSectionByValue(_COOP_FILE_maplist, fileContents, mapNameClean.c_str());
    if (sectionOfCurrentMap == _COOP_MAPLIST_CAT_included) { struct_currentMap.coopIncluded = true; struct_currentMap.coopMap = true; }
    if (sectionOfCurrentMap == _COOP_MAPLIST_CAT_coopSpMission) { struct_currentMap.coopSpMission = true; struct_currentMap.coopMap = true; }
    if (sectionOfCurrentMap == _COOP_MAPLIST_CAT_coopSpIgm) { struct_currentMap.coopSpIgm = true; struct_currentMap.coopMap = true; }
    if (sectionOfCurrentMap == _COOP_MAPLIST_CAT_coopSpSecret) { struct_currentMap.coopSpSecret = true; struct_currentMap.coopMap = true; }
    if (sectionOfCurrentMap == _COOP_MAPLIST_CAT_multiplayerOnly) { struct_currentMap.multiplayerOnly = true; }
    if (sectionOfCurrentMap == _COOP_MAPLIST_CAT_singleplayerOnly) { struct_currentMap.singleplayerOnly = true; }

    if (struct_currentMap.coopMap == false && struct_currentMap.multiplayerOnly == false && struct_currentMap.singleplayerOnly == false) {
        str sectionContents = gamefix_iniSectionGet(_COOP_FILE_maplist, fileContents, _COOP_MAPLIST_CAT_coopcompatible);
        str CoopCompatible = gamefix_iniKeyGet(_COOP_FILE_maplist, sectionContents, sectionOfCurrentMap, "");

        if (CoopCompatible.length() && CoopCompatible == "true") {
            struct_currentMap.coopMap = true;
        }
    }
    return struct_currentMap;
}

//detects what kind of gametype the level
//support and sets related parameters
void CoopManager::SetMapType() {
    coopManager_mapSettings_s struct_currentMap = DetectMapType(level.mapname);

    mapFlags.rpgMap = struct_currentMap.rpgMap;
    mapFlags.coopMap = struct_currentMap.coopMap;
    mapFlags.coopIncluded = struct_currentMap.coopIncluded;
    mapFlags.coopSpMission = struct_currentMap.coopSpMission;
    mapFlags.coopSpIgm = struct_currentMap.coopSpIgm;
    mapFlags.coopSpSecret = struct_currentMap.coopSpSecret;
    mapFlags.multiplayerOnly = struct_currentMap.multiplayerOnly;
    mapFlags.singleplayerOnly = struct_currentMap.singleplayerOnly;
    mapFlags.stockMap = struct_currentMap.stockMap;
    mapFlags.scriptIncludedCoopMain = false;
    mapFlags.scriptIncludedCoopNoscript = false;
    mapFlags.scriptIncludedCoopMom = false;
    mapFlags.scriptIncludedCoopMom4 = false;
    mapFlags.scriptIncludedCoopMrm = false;
}

//Executes the level script
//prefers coop/maps/ over maps/ , prefers .script over .scr
void CoopManager::LoadLevelScript(str mapname) {
    mapname = gamefix_cleanMapName(mapname);
    
    Container<str> scriptFileExt;
    scriptFileExt.AddObject("script");
    scriptFileExt.AddObject("scr");
    int scriptFileExtensions = scriptFileExt.NumObjects();

    Container<str> scriptFileDir;
    scriptFileDir.AddObject("co-op/maps/");
    scriptFileDir.AddObject("maps/");
    int scriptFileDirectories = scriptFileDir.NumObjects();
    
    str dir,ext;
    //multiplayer prioritise files from co-op folder and .script files
    if (gameFixAPI_inMultiplayer()) {
        for (int j = 1; j <= scriptFileDirectories; j++) {
            dir = scriptFileDir.ObjectAt(j);
            for (int i = 1; i <= scriptFileExtensions; i++) {
                ext = scriptFileExt.ObjectAt(i);
                //construct directory mapname and file extension
                str sFile = va("%s%s.%s", dir.c_str(), mapname.c_str(), ext.c_str());
                if (gi.FS_ReadFile(sFile.c_str(), NULL, true) != -1) {
                    gi.Printf("Adding script: '%s'\n", sFile.c_str());
                    // just set the script, we will start it in G_Spawn
                    level.SetGameScript(sFile.c_str());
                    return;
                }
            }
        }
    }
    //singleplayer prioritise files from maps folder and .scr files
    else {
        for (int j = scriptFileDirectories; j > 0; j--) {
            dir = scriptFileDir.ObjectAt(j);
            for (int i = scriptFileExtensions; i > 0; i--) {
                ext = scriptFileExt.ObjectAt(i);
                //construct directory mapname and file extension
                str sFile = va("%s%s.%s", dir.c_str(), mapname.c_str(), ext.c_str());
                if (gi.FS_ReadFile(sFile.c_str(), NULL, true) != -1) {
                    gi.Printf("Adding script: '%s'\n", sFile.c_str());
                    // just set the script, we will start it in G_Spawn
                    level.SetGameScript(sFile.c_str());
                    return;
                }
            }
        }
    }

    gi.Printf("Level *.scr or *.script file not found in co-op/maps or maps: '%s'\n", mapname.c_str());
    level.SetGameScript("");
}

//Executed if game is shut down
//Cleans up stuff while world and entities still exist
//Executed ONLY on game shutdown
void CoopManager::Shutdown() {

    if (coopEnabled) {
        CoopSettings_playerScriptThreadsAllowList.FreeObjectList();
    }

    coopManager_validPlayerModels.FreeObjectList();
}

//Executed each time a level is starting - scripts are started
//Executed from: Level::Start - always
//Other: CoopManager::InitWorld
void CoopManager::LevelStart(CThread* gamescript) {
    //start coop main function automatically, will auto-handle SP/MP
    if (gamescript && gamescript->labelExists(_COOP_SCRIPT_main)) {
        CThread* coopMain = Director.CreateThread(_COOP_SCRIPT_main);
        if (coopMain) {
            coopMain->DelayedStart(0.0f);
        }
    }
}

//Executed if level is exited/changed/restarted - but not on first load/game start
//Cleans up stuff while world and entities still exist - Not executed if game server is quit
void CoopManager::LevelEndCleanup(qboolean temp_restart) {
    coop_objectives_reset();

    Player* player = nullptr;
    for (int i = 0; i < gameFixAPI_maxClients(); i++) {
        player = gamefix_getPlayer(i);
        if (player) {
            setPlayerData_spawnLocationSpawnForced(player, true);
            setPlayerData_respawnLocationSpawn(player, false);
            setPlayerData_lastValidLocation(player, Vector(0.0f, 0.0f, 0.0f));
            setPlayerData_objectives_reset(player);
        }
    }

    if (coopEnabled) {
        CoopSettings_deathList.FreeObjectList();
        CoopSettings_scoreKillList.FreeObjectList();
        CoopSettings_playerScriptThreadsAllowList.FreeObjectList();
    }
}

void CoopManager::MissionFailed(const str & reason) {
    if (g_gametype->integer == GT_SINGLE_PLAYER) { return; }

    str sReason = reason;
    if (reason.length() == 0) {
        sReason = "DefaultFailure";
    }

    //-- Intentionally no $$ wanted
    if (sReason[0] == '#') {
        sReason = gamefix_getStringUntil(sReason, 1, 120);
    }
    //-- If it is just a single word and has no $$ it is extreemly likley a local string, so add $$
    else if (gamefix_findString(sReason.c_str(), "$$") == -1 && gamefix_findString(sReason.c_str(), " ") == -1) {
        sReason = va("$$%s$$", sReason.c_str());
    }

    gentity_t* other;
    for (int j = 0; j < game.maxclients; j++) {
        other = &g_entities[j];
        if (other->inuse && other->client && other->entity) {
            Player* player = (Player*)other->entity;
            //show hud for coop clients and text for others
            if (player->coop_hasCoopInstalled()) {
                gamefix_playerDelayedServerCommand(player->entnum, va("set ui_failureReason %s\n", sReason.c_str()));
                gamefix_playerDelayedServerCommand(player->entnum, "pushmenu coop_failure");
            }
            else {
                gamefix_playerDelayedServerCommand(player->entnum, "hudprint ^1=============^3$$MISSIONFAILED$$^1=============\n");
                gamefix_playerDelayedServerCommand(player->entnum, va("hudprint ^3%s\n", sReason.c_str()));
            }
        }
    }
}

str CoopManager::MissionFailureConfigString(const str& reason)
{
    str newReason = reason;
    //if # is used it means set string without $$
    if (reason[0] == '#') {
        newReason = gamefix_getStringUntil(reason, 1, 120);
        newReason = va("\n\n%s^0", newReason.c_str());
    }
    return newReason;
}

void CoopManager::MissionFailureLoadMap()
{
    if (!multiplayerManager.inMultiplayer()) { return; }

    Event* ev_loadMap = new Event(EV_World_coop_loadMap);
    str sParameters = "";

    //Checkpoints - do this only if we know we have a valid scriptfile for this map, cuz scriptmaster keeps values of last script file - chrissstrahl
    if (level.game_script != "") {
        sParameters = program.coop_getStringVariableValue("coop_string_checkpointParameters");
    }

    if (sParameters != "") {
        ev_loadMap->AddString(va("%s$%s", level.mapname.c_str(), sParameters.c_str()));
    } else {
        ev_loadMap->AddString(level.mapname.c_str());
    }

    world->PostEvent(ev_loadMap, 6.0f);
}

//called by ClientThink
void CoopManager::playerSetup(Player* player) {
    if (!player) {
        return;
    }

    if (getPlayerData_coopSetupDone(player) && getPlayerData_coopClientIdDone(player)) {
        return;
    }

    if (getPlayerData_coopSetupNextCheckTime(player) > level.time) {
        return;
    }

    setPlayerData_coopSetupNextCheckTime_update(player);
    setPlayerData_coopSetupTries_update(player);

    if (!getPlayerData_coopSetupStarted(player)) {
        //Client ID
        gamefix_playerDelayedServerCommand(player->entnum, "vstr coop_cId");
        //Coop Version
        gamefix_playerDelayedServerCommand(player->entnum, "exec co-op/cfg/detect.cfg");
        setPlayerData_coopSetupStarted(player, true);
    }

    //handle client id
    playerSetupClId(player);
    playerSetupCoop(player);
}

void CoopManager::playerSetupClId(Player* player) {
    if (getPlayerData_coopClientIdDone(player)) {
        return;
    }
    
    if (getPlayerData_coopSetupTries(player) > _COOP_SETTINGS_SETUP_TRIES) {
        playerClIdSet(player);
        setPlayerData_coopClientIdDone(player, true);

        DEBUG_LOG("# playerSetupClId Failed\n");
        return;
    }
}

void CoopManager::playerSetupCoop(Player* player) {
    if (getPlayerData_coopSetupDone(player)) {
        return;
    }
    
    if (getPlayerData_coopSetupTries(player) > _COOP_SETTINGS_SETUP_TRIES) {
        coopManager_client_persistant_t[player->entnum].coopVersion = 0;

        setPlayerData_coopSetupDone(player, true);
        setPlayerData_objectives_setupDone(player);

        DEBUG_LOG("# playerSetupCoop Failed\n");
        return;
    }
}

void CoopManager::playerSetupCoopUi(Player* player) {
    if (!player || !player->coop_hasCoopInstalled()) {
        return;
    }
    //populate ui
    gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand coopGpoSkill title %d", skill->integer));
    gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand coopGpoAw title %d", coopSettings.getSetting_awards()));
    gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand coopGpoMvSpd title %d", coopSettings.getSetting_maxSpeed()));
    gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand coopGpoFF title %.2f", coopSettings.getSetting_friendlyFireMultiplicator()));
    gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand coopGpoRspwt title %s", "NOT_IMPL"));
    gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand coopGpoLms title %s", "NOT_IMPL"));
    gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand coopGpoDb title %s", "NOT_IMPL"));
    gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand coopGpoSt title %s", "NOT_IMPL"));
    gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand coopGpoAa title %d", sv_airaccelerate->integer));
}

//called by console command "coopinstalled"
void CoopManager::playerCoopDetected(const gentity_t* ent, const char* coopVer) {
    if (!ent || !ent->entity || !ent->client || !coopVer || !strlen(coopVer)) {
        return;
    }
    Player* player = (Player*)ent->entity;

    if (getPlayerData_coopSetupDone(player)) {
        return;
    }   
    
    int iVer =  atoi(coopVer);
    setPlayerData_coopSetupDone(player, true);
    if (iVer < _COOP_CLIENT_MINIMUM_COMPATIBELE_VERSION) {
        DEBUG_LOG("# COOP DETECTED = OUTDATED =: %d\n", iVer);
        return;
    }

    setPlayerData_coopVersion(player,iVer);

    //run coop setup
    DEBUG_LOG("# COOP DETECTED: %d (WAITED: %d)\n", iVer, getPlayerData_coopSetupTries(player));
}

//called by console command "coopcid"
void CoopManager::playerClIdDetected(const gentity_t* ent, const char* clientId) {
    if (!ent || !ent->entity || !ent->client || !clientId || !strlen(clientId)) {
        return;
    }
    Player* player = (Player*)ent->entity;

    const char* cCClientId = gi.argv(1);
    str sId = va("%s", cCClientId);

    if (getPlayerData_coopClientIdDone(player)) {
        player->hudPrint("COOPDEBUG: coop_cId - Timed Out: Rejected!\n");
        gi.Printf(va("COOPDEBUG: coop_cId - Timed Out: Rejected! For: %s\n", player->client->pers.netname));
        DEBUG_LOG("# CId TIMED OUT: %s\n", sId.c_str());
        return;
    }

    if (!sId.length()) {
        gi.Printf(va("COOPDEBUG: coop_cId - Bad or Empty: Rejected! For: %s\n", player->client->pers.netname));
        return;
    }

    DEBUG_LOG("# CId RECIVED: %s (WAITED %d)\n", sId.c_str(), getPlayerData_coopSetupTries(player));

    str sClientId = "";
    sId = gamefix_filterChars(sId, ";[]=%");
    sId = gamefix_trimWhitespace(sId,false);

    DEBUG_LOG("# CId CHECK VS INI NOT IMPLEMENTED\n");

    sClientId = sId;
    //sClientId = coop_checkPlayerCoopIdExistInIni(player, sId);
    if (sClientId.length()) {
        //prevent players from cheating lms by reconnecting
        //player->coop_lmsCheckReconnectHack();
        setPlayerData_coopClientIdDone(player,true);
        setPlayerData_coopClientId(player, sId);
    }
}

void CoopManager::playerClIdSet(Player* player)
{
    if (getPlayerData_coopClientIdDone(player)) {
        return;
    }

    //that should create a pretty uniqe player id
    str sId = va("%d%d", (int)gamefix_getTimeStamp(), player->entnum);
    setPlayerData_coopClientId(player, sId);
    gi.SendServerCommand(player->entnum, va("stufftext \"seta coop_cId 0;set coop_cId coopcid %s\"\n", sId.c_str()));
    DEBUG_LOG("# CId NEW CREATED: %s\n", sId.c_str());
}

void CoopManager::playerAddMissionHuds(Player* player)
{
    str hudName = "";
    str hudCommand = "";
    for (int iHuds = 0; iHuds < _COOP_SETTINGS_MISSION_HUDS_MAX; iHuds++) {
        hudName = gamefix_getEntityVarString(world, va("coop_registredHud%d", iHuds));
        if (!hudName.length()) {
            continue;
        }
        gamefix_playerDelayedServerCommand(player->entnum,va("ui_addhud %s", hudName.c_str()));

        //execute command if one is attached - usually to exec a cfg or to handle globalwidgetcommands for ui elements
        hudCommand = gamefix_getEntityVarString(world, va("coop_registredHud%d_command", iHuds));
        if (hudCommand.length()) {
            gamefix_playerDelayedServerCommand(player->entnum, va("%s", hudCommand.c_str()));
        }
    }
}

void CoopManager::playerRemoveMissionHuds(Player* player)
{
    str hudName = "";
    str hudCommand = "";
    for (int iHuds = 0; iHuds < _COOP_SETTINGS_MISSION_HUDS_MAX; iHuds++) {
        hudName = gamefix_getEntityVarString(world, va("coop_registredHud%d", iHuds));
        if (!hudName.length()) {
            continue;
        }
        gamefix_playerDelayedServerCommand(player->entnum,va("ui_removehud %s", hudName.c_str()));
    }
}

bool CoopManager::playerItemPickup(Entity* player, Item* item)
{
    //allow to pick up item if all other players have that item
    if (!player || !item || !player->isSubclassOf(Player)) {
        return false;
    }

    if (!IsCoopEnabled()) {
        return true;
    }

    if (!item->isSubclassOf(Weapon)) {
        return true;
    }

    Sentient* sent = (Sentient*)player;
    if (!sent->coop_hasItem(item->model)) {
        return true;
    }

    Player* other = nullptr;
    for (int i = 0; i < gameFixAPI_maxClients(); i++) {
        other = gamefix_getPlayer(i);

        if (!other || gameFixAPI_isSpectator_stef2((Entity*)other) || gameFixAPI_isDead((Entity*)other)) {
            continue;
        }
        
        if (!(Sentient*)other->coop_hasItem(item->model)) {
            return false;
        }
    }

    return true;
}

void CoopManager::playerAdminThink(Player* player) {
    if (getPlayerData_coopAdmin(player)) {
        return;
    }

    if (!getPlayerData_coopAdminAuthStarted(player)) {
        return;
    }

    if (!getPlayerData_coopAdminAuthString_changed(player)) {
        return;
    }

    str authStringActual = gamefix_getCvar("coop_admin");
    if (strlen(authStringActual) < _COOP_SETTINGS_ADMIN_LOGIN_AUTH_STRING_LENGTH_MIN){
        setPayerData_coopAdminAuthStarted(player,false);
        gamefix_playerDelayedServerCommand(player->entnum, va(_COOP_INFO_adminLogin_auth, gamefix_replaceForLabelText(_COOP_INFO_adminLogin_authCvarToShort).c_str()));
        return;
    }

    if (strlen(authStringActual) > _COOP_SETTINGS_ADMIN_LOGIN_AUTH_STRING_LENGTH_MAX){
        setPayerData_coopAdminAuthStarted(player,false);
        gamefix_playerDelayedServerCommand(player->entnum, va(_COOP_INFO_adminLogin_auth, gamefix_replaceForLabelText(_COOP_INFO_adminLogin_authCvarToLong).c_str()));
        return;
    }

    str authStringPlayer = getPlayerData_coopAdminAuthString(player);
    int authStringLengthNew = strlen(authStringPlayer);
    setPlayerData_coopAdminAuthStringLengthLast(player, authStringLengthNew);

    //total failure
    if (authStringLengthNew > _COOP_SETTINGS_ADMIN_LOGIN_AUTH_STRING_LENGTH_MAX || getPlayerData_coopAdminAuthAttemps(player) > _COOP_SETTINGS_ADMIN_LOGIN_TRIES) {
        setPayerData_coopAdminAuthAttemps_update(player);
        setPayerData_coopAdminAuthStarted(player, false);
        setPlayerData_coopAdminAuthString(player,"");
        if (getPlayerData_coopAdminAuthAttemps(player) > _COOP_SETTINGS_ADMIN_LOGIN_TRIES) {
            gi.SendConsoleCommand(va("kick %d\n", player->entnum));
            return;
        }
        gamefix_playerDelayedServerCommand(player->entnum, "globalwidgetcommand coop_comCmdLoginCode title ''");
        gamefix_playerDelayedServerCommand(player->entnum, va(_COOP_INFO_adminLogin_auth, gamefix_replaceForLabelText(_COOP_INFO_adminLogin_authFailure).c_str()));
        return;
    }

    //success - match
    if (authStringPlayer == authStringActual) {
        setPlayerData_coopAdmin(player, true);
        setPayerData_coopAdminAuthStarted(player, false);
        setPayerData_coopAdminAuthAttemps_reset(player);
        gamefix_playerDelayedServerCommand(player->entnum, va(_COOP_INFO_adminLogin_auth, gamefix_replaceForLabelText(_COOP_INFO_adminLogin_authSuccess).c_str()));
        return;
    }
}

void CoopManager::playerMoveToSpawn(Player* player) {
    Entity* spawnPoint = multiplayerManager.gameFixAPI_getMultiplayerGame()->getSpawnPoint(player);
    if (!spawnPoint) {
        spawnPoint = gamefix_returnInfoPlayerStart(_GFixEF2_INFO_GAMEFIX_spawnlocations_TeamBaseAddPlayerToTeam);
    }
    if (spawnPoint) {
        player->WarpToPoint(spawnPoint);
    }
}

Entity* CoopManager::getSpawnSpecific(int spotNumber){
    Entity* ent = nullptr;

    if (spotNumber > _COOP_SETTINGS_PLAYER_SPAWNSPOT_MAX || spotNumber < 1) {
        gi.Error(ERR_FATAL, va("CoopManager::getSpawnSpecific() spotNumber '%d' out of range\n", spotNumber));
        return ent;
    }

    //Script based spawnspot
    Vector vAngle = Vector(0.0f, 0.0f, 0.0f);
    Vector vSpawn = program.coop_getVectorVariableValue(va("coop_vector_spawnOrigin%i", spotNumber));
    vAngle[1] = program.coop_getFloatVariableValue(va("coop_float_spawnAngle%d", spotNumber));
    if (vAngle[1] == 0.0f) {
        vAngle[1] = program.coop_getFloatVariableValue("coop_float_spawnAngle0");
    }

    if (vSpawn.length() > 0) {
        ent = G_FindClass(NULL, "info_player_start");
        Vector vOld = ent->origin;
        if (ent) {
            ent->setAngles(vAngle);
            ent->setOrigin(vSpawn);
            ent->NoLerpThisFrame();
            return ent;
        }
    }
    
    //info_player_deathmatch based spawnspot with targetname
    TargetList* tlist;
    tlist = world->GetTargetList(va("ipd%d", spotNumber), false);
    if (tlist) {
        ent = tlist->GetNextEntity(NULL);
        if (ent) {
            return ent;
        }
    }

    gi.Printf(va(_COOP_INFO_spawnspotNumberedNotFound,spotNumber));

    return ent;
}

bool CoopManager::playerMoveToSpawnSpecific(Player* player, int spotNumber)
{
    if (!player) {
        return false;
    }
    if (spotNumber > _COOP_SETTINGS_PLAYER_SPAWNSPOT_MAX || spotNumber < 1) {
        gi.Error(ERR_FATAL, va("CoopManager::playerMoveToSpawnSpecific() spotNumber '%d' out of range\n", spotNumber));
        return false;
    }
    Entity* ent = getSpawnSpecific(spotNumber);
    if (ent) {
        player->WarpToPoint(ent);
        return true;
    }
    return false;
}

void CoopManager::playerTargetnames(Player *player, Entity* viewTrace)
{
    if (!CoopManager::Get().IsCoopEnabled()) {
        return;
    }

    if (!player || player->getHealth() <= 0 || !viewTrace || viewTrace->entnum == world->entnum) {
        return;
    }

    if (multiplayerManager.inMultiplayer() && getPlayerData_targetedShow(player)) {
        Vector vData;
        ScriptVariable* scriptVar = NULL;
        scriptVar = player->entityVars.GetVariable("!targeted");
        if (scriptVar != NULL) {
            vData = scriptVar->vectorValue();
        }
        else {
            vData = Vector(0, 0, 0);
        }
        //0=time,1=entitynum,2=?
        if ((vData[0] + 3) < level.time || (int)vData[1] != viewTrace->entnum) {
            vData[0] = level.time;
            vData[1] = viewTrace->entnum;
            player->entityVars.SetVariable("!targeted", vData);
            player->hudPrint(va("^5Object:^3 $%s, ^5Class:^3 %s\n", viewTrace->targetname.c_str(), viewTrace->getClassname()));
        }
    }
}

//Executed every level restart/reload or when player disconnects
void CoopManager::playerReset(Player* player) {
    if (!player) {
        return;
    }
    
    setPlayerData_targetedShow(player, false);
    setPlayerData_respawnMe(player,false);
    setPlayerData_spawnLocationSpawnForced(player,true);
    setPlayerData_respawnLocationSpawn(player,false);
    setPlayerData_lastValidLocation(player, Vector(0.0f, 0.0f, 0.0f));
    setPlayerData_lastValidViewAngle(player, Vector(0.0f, 0.0f, 0.0f));
    setPlayerData_lastSpawned(player, -1.0f);
    setPlayerData_objectives_reset(player);

    //see also will be cleaned up in: playerLeft
}

//Executed only when player connects first time
void CoopManager::playerConnect(int clientNum) {
    if (clientNum < 0 || clientNum >= MAX_CLIENTS) {
        return;
    }

    //used to minimize the usage of configstrings due to cl_parsegamestate issue
    //if a new player connects mid game
    if (level.time > 30.0f) {
        configstringCleanup();
    }
    

    DEBUG_LOG("# playerConnect\n");
}

//Executed when player disconnects - only upon actual disconnect not on mapchange or relaod
void CoopManager::playerDisconnect(Player* player) {
    if (!player) {
        return;
    }
    DEBUG_LOG("# playerDisconnect\n");

    coop_radarReset(player);

    setPlayerData_coopClientIdDone(player, false);
    setPlayerData_coopAdmin(player,false);
    setPayerData_coopAdminAuthAttemps_reset(player);
    setPlayerData_coopClientId(player,"");
    setPlayerData_coopSetupStarted(player, true);
    setPlayerData_coopSetupTries_reset(player);
    setPlayerData_coopSetupNextCheckTime_reset(player);
    setPlayerData_coopSetupDone(player, false);
    setPlayerData_coopVersion(player, -1);
    setPlayerData_coopClass(player, "");
    
    //rest will be cleaned up in: playerLeft
}

//Executed when player object is detroyed, every map reload or exit or disconnect - Always (Multiplayer + Singleplayer)
void CoopManager::playerLeft(Player* player) {

    if (player) {
        ExecuteThread("coop_justLeft", true, player);
    }

    //reset player data on a l
    playerReset(player);

    DEBUG_LOG("# playerLeft\n");
}

void CoopManager::playerJoined(Player* player) {
    if (!player) {
        return;
    }

    DEBUG_LOG("# playerJoined\n");
}

//Executed upon entering server - Always (Multiplayer + Singleplayer)
void CoopManager::playerEntered(gentity_t* ent) {
    if (ent && ent->entity) {
        ExecuteThread("coop_justEntered", true, ent->entity);
    }
    DEBUG_LOG("# playerEntered\n");
}

//Executed on death - Always (Multiplayer + Singleplayer)
void CoopManager::playerDied(Player *player) {
    if (!player) {
        return;
    }

    playerRemoveMissionHuds(player);
    ExecuteThread("coop_justDied", true, player);
}
//Executed when player gets transported - Always (Multiplayer + Singleplayer)
void CoopManager::playerTransported(Entity *entity){
    if (!entity)
        return;

    if (!entity->isSubclassOf(Sentient))
        return;

    gamefix_setMakeSolidAsap(entity, true, level.time + FRAMETIME);
    
    if (entity->isSubclassOf(Player)) {
        ExecuteThread("coop_justTransported", true, (Player*)entity);
    }
}
//Executed on spawn - Multiplayer
void CoopManager::playerSpawned(Player* player) {
    if (!player)
        return;

    if (!gameFixAPI_isSpectator_stef2(player)) {
        coop_armoryEquipPlayer(player);
        coop_radarReset(player);
        gamefix_playerDelayedServerCommand(player->entnum, "exec co-op/cfg/ea.cfg");
        playerAddMissionHuds(player);
    }

    gamefix_setMakeSolidAsap((Entity*)player, true, 0.0f);
    ExecuteThread("coop_justSpawned", true, player);
}
//Executed on death - Multiplayer
void CoopManager::playerBecameSpectator(Player *player){
    if (player) {
        coopManager_client_persistant_t[player->entnum].spawnLocationSpawnForced = true;
        coopManager_client_persistant_t[player->entnum].respawnLocationSpawn = true;

        ExecuteThread("coop_justBecameSpectator", true, player);
    }
}
//Executed on death - Multiplayer
void CoopManager::playerChangedModel(Player *player){
    if (player) {
        //not a valid model, handle
        str sModel = player->model.c_str();
        if (coopManager_validPlayerModels.IndexOfObject(sModel.tolower()) == 0) {
            DEBUG_LOG("NOT ALLOWED: %s\n", player->model.c_str());
            sModel = multiplayerManager.getDefaultPlayerModel(player);
            multiplayerManager.changePlayerModel(player, va("%s", sModel.c_str()), true);
            return;
        }
        ExecuteThread("coop_justChangedModel", true, player);
    }
}
//Executed on death - Multiplayer
void CoopManager::playerChangedClass(Player* player) {
    if (player) {
        ExecuteThread("coop_justChangedClass", true, player);
    }
}

void CoopManager::playerKilledActor(Player* player, Actor* actor) {
    if (!player || !actor || !player->isSubclassOf(Player)) {
        return;
    }

    //HACK, set attacker as enemy, so we can retrive it
    actor->enemyManager->SetCurrentEnemy((Entity*)player);
    
    //change player statistics
    multiplayerManager.addKill(player->entnum, 1);

    if (actor->actortype == IS_CIVILIAN || actor->actortype == IS_FRIEND || actor->actortype == IS_TEAMMATE) {
        multiplayerManager.addPoints(player->entnum, -_COOP_SETTINGS_PLAYER_PENALTY_BADKILL);
    }
    else {
        //use - scorelist with actor names
        int pointsEarned = 1;

        str actorArcheType = actor->getArchetype();
        str actorTargetName = actor->targetname;
        if(actorTargetName.length()) {
            for (int i = 1; i <= CoopSettings_scoreKillList.NumObjects(); i++) {
                CoopSettings_killScore_s addKillScore;
                addKillScore = CoopSettings_scoreKillList.ObjectAt(i);
                if (addKillScore.name == actorTargetName && addKillScore.type == "targetnames" || addKillScore.name == actorArcheType && addKillScore.type == "actornames") {
                    pointsEarned = addKillScore.points;
                    break;
                }
            }
        }
        multiplayerManager.addPoints(player->entnum, pointsEarned);

        if (coopSettings.getSetting_awards()) {
            multiplayerManager.gameFixAPI_getMultiplayerAwardSystem()->coop_awardEnemyKilled(player,(Entity*)actor);
        }
    }
}

bool CoopManager::sentientHandleStasis(Sentient* attacked, Entity* attacker)
{
    if (!attacked || !attacker) {
        return false;
    }

    if (attacked == attacker) {
        return false;
    }

    if (IsCoopEnabled()) {
        if (!attacker->isSubclassOf(Sentient)) {
            return true;
        }

        if (attacked->isSubclassOf(Player) && attacker->isSubclassOf(Player)) {
            Player* attackerPlayer = (Player*)attacker;
            Player* attackedPlayer = (Player*)attacked;
            Team* teamAttacker;
            Team* teamAttacked;
            teamAttacker = multiplayerManager.getPlayersTeam(attackerPlayer);
            teamAttacked = multiplayerManager.getPlayersTeam(attackedPlayer);

            //different teams
            if (teamAttacker && teamAttacked) {
                if (teamAttacker != teamAttacked) {
                    return true;
                }
                return true;
            }
            return true;
        }
        if (attacked->isSubclassOf(Actor) && attacker->isSubclassOf(Player)) {
            Actor *attackedActor = (Actor*)attacked;
            if (attackedActor->actortype == IS_TEAMMATE ||
                attackedActor->actortype == IS_CIVILIAN ||
                attackedActor->actortype == IS_FRIEND )
            {
                return false;
            }
        }
    }

    return true;
}

//used to minimize the usage of configstrings due to cl_parsegamestate issue
int CoopManager::configstringRemove(str sRem)
{
    if (!sRem.length()) {
        return 0;
    }

    int iRem = 0;
    char* s;
    for (int i = 1; i < MAX_CONFIGSTRINGS; i++) {
        s = gi.getConfigstring(i);
        str ss = "";
        ss += s;

        if (ss.length() > 0) {
            //if this is a dialog try to handle german and english localized strings as well
            if (!Q_stricmpn(ss.c_str(), "localization/", 13)) {
                //regular dialog
                if (Q_stricmp(ss.c_str(), sRem.c_str()) == 0) {
                    DEBUG_LOG(va("#REMOVED CS: #%i: %s\n", i, ss.c_str()));
                    gi.setConfigstring(i, "");
                    iRem++;
                }

                //handle deu version of dialog
                char unlocal[96]; //MAX_QPATH + 5 <- did not work!
                memset(unlocal, 0, sizeof(unlocal));
                Q_strncpyz(unlocal, va("loc/deu/%s", sRem.c_str() + 13), sizeof(unlocal));
                if (Q_stricmp(ss.c_str(), unlocal) == 0) {
                    DEBUG_LOG(va("#REMOVED CS: #%i: %s\n", i, ss.c_str()));
                    gi.setConfigstring(i, "");
                    iRem++;
                }

                //handle eng version of dialog
                memset(unlocal, 0, sizeof(unlocal));
                Q_strncpyz(unlocal, va("loc/eng/%s", sRem.c_str() + 13), sizeof(unlocal));
                if (Q_stricmp(ss.c_str(), unlocal) == 0) {
                    DEBUG_LOG(va("#REMOVED CS: #%i: %s\n", i, ss.c_str()));
                    gi.setConfigstring(i, "");
                    iRem++;
                }
            }
            else {
                if (Q_stricmp(ss.c_str(), sRem.c_str()) == 0) {
                    DEBUG_LOG(va("#REMOVED CS: #%i: %s\n", i, ss.c_str()));
                    gi.setConfigstring(i, "");
                    iRem++;
                }
            }
        }
    }
    return iRem;
}

//used to minimize the usage of configstrings due to cl_parsegamestate issue
void CoopManager::configstringCleanup()
{
    if (g_gametype->integer == GT_SINGLE_PLAYER || !IsCoopEnabled()) {
        return;
    }
    //mp taunts we do not use in coop
    configstringRemove("localization/sound/dialog/dm/mp_andor1.mp3");
    configstringRemove("localization/sound/dialog/m10l1/romgate3_cred.mp3");
    configstringRemove("localization/sound/dialog/m10l1/outrom1_ohno.mp3");

    configstringRemove("localization/sound/dialog/dm/mp_andor2.mp3");
    configstringRemove("localization/sound/dialog/dm/mp_attrexf1.mp3");
    configstringRemove("localization/sound/dialog/dm/mp_attrexf2.mp3");
    configstringRemove("localization/sound/dialog/dm/mp_attrexm1.mp3");
    configstringRemove("localization/sound/dialog/dm/mp_attrexm2.mp3");
    configstringRemove("localization/sound/dialog/dm/mp_borgf1.mp3");
    configstringRemove("localization/sound/dialog/dm/mp_borgf2.mp3");
    configstringRemove("localization/sound/dialog/dm/mp_borgm1.mp3");
    configstringRemove("localization/sound/dialog/dm/mp_borgm2.mp3");
    configstringRemove("localization/sound/dialog/dm/mp_kleeya1.mp3");
    configstringRemove("localization/sound/dialog/dm/mp_kleeya2.mp3");
    configstringRemove("localization/sound/dialog/dm/mp_krindo1.mp3");
    configstringRemove("localization/sound/dialog/dm/mp_krindo2.mp3");
    configstringRemove("localization/sound/dialog/dm/mp_omag1.mp3");
    configstringRemove("localization/sound/dialog/dm/mp_omag2.mp3");
    configstringRemove("localization/sound/dialog/dm/mp_lurok1.mp3");
    configstringRemove("localization/sound/dialog/dm/mp_lurok2.mp3");
    configstringRemove("localization/sound/dialog/dm/mp_klingf1.mp3");
    configstringRemove("localization/sound/dialog/dm/mp_klingf2.mp3");
    configstringRemove("localization/sound/dialog/dm/mp_naus1.mp3");
    configstringRemove("localization/sound/dialog/dm/mp_naus2.mp3");
    configstringRemove("localization/sound/dialog/dm/mp_picard1.mp3");
    configstringRemove("localization/sound/dialog/dm/mp_picard2.mp3");
    configstringRemove("localization/sound/dialog/dm/mp_inform1.mp3");
    configstringRemove("localization/sound/dialog/dm/mp_inform2.mp3");
    configstringRemove("localization/sound/dialog/dm/mp_rene1.mp3");
    configstringRemove("localization/sound/dialog/dm/mp_rene2.mp3");
    configstringRemove("localization/sound/dialog/dm/mp_rom1.mp3");
    configstringRemove("localization/sound/dialog/dm/mp_rom2.mp3");
    configstringRemove("localization/sound/dialog/dm/mp_tuvok1.mp3");
    configstringRemove("localization/sound/dialog/dm/mp_tuvok2.mp3");

    //mp player models we do not allow in coop
    configstringRemove("models/char/dm_andorian_merc-male.tik");
    configstringRemove("models/char/dm_picard.tik");

    configstringRemove("models/char/dm_attrexian_command-female.tik");
    configstringRemove("models/char/dm_attrexian_security-male.tik");
    configstringRemove("models/char/dm_borg_female.tik");
    configstringRemove("models/char/dm_borg_male.tik");
    configstringRemove("models/char/dm_drull_kleeya.tik");
    configstringRemove("models/char/dm_drull_krindo.tik");
    configstringRemove("models/char/dm_ferengi_oolpax.tik");
    configstringRemove("models/char/dm_klingon_merc-boss.tik");
    configstringRemove("models/char/dm_klingon_merc-female.tik");
    configstringRemove("models/char/dm_nausicaan_male-merc.tik");
    configstringRemove("models/char/dm_romulan_informant-boss.tik");
    configstringRemove("models/char/dm_romulan_rebel-commander.tik");
    configstringRemove("models/char/dm_romulan_rebel-guard-snow.tik");
    configstringRemove("models/char/dm_romulan_stx-female.tik");
    configstringRemove("models/char/dm_stalker.tik");
    configstringRemove("models/char/dm_tuvok.tik");

    //awards we do not use in coop
    configstringRemove("sysimg/icons/mp/award_sharpshooter");
    configstringRemove("sysimg/icons/mp/award_untouchable");
    configstringRemove("sysimg/icons/mp/award_logistics");
    configstringRemove("sysimg/icons/mp/award_tactician");
    configstringRemove("sysimg/icons/mp/award_demolitionist");
    configstringRemove("sysimg/icons/mp/award_mvp");
    configstringRemove("sysimg/icons/mp/award_defender");
    configstringRemove("sysimg/icons/mp/award_warrior");
    configstringRemove("sysimg/icons/mp/award_carrier");
    configstringRemove("sysimg/icons/mp/award_interceptor");
    configstringRemove("sysimg/icons/mp/award_bravery");
    configstringRemove("sysimg/icons/mp/award_firstStrike");

    //mp computer voice we do not use in coop
    configstringRemove("localization/sound/dialog/dm/comp_5mins.mp3");
    configstringRemove("localization/sound/dialog/dm/comp_2mins.mp3");
    configstringRemove("localization/sound/dialog/dm/comp_1min.mp3");
    configstringRemove("localization/sound/dialog/dm/comp_500pointsleft.mp3");
    configstringRemove("localization/sound/dialog/dm/comp_100pointsleft.mp3");
    configstringRemove("localization/sound/dialog/dm/comp_25pointsleft.mp3");
    configstringRemove("localization/sound/dialog/dm/comp_10pointsleft.mp3");
    configstringRemove("localization/sound/dialog/dm/comp_5pointsleft.mp3");
    configstringRemove("localization/sound/dialog/dm/comp_4pointsleft.mp3");
    configstringRemove("localization/sound/dialog/dm/comp_3pointsleft.mp3");
    configstringRemove("localization/sound/dialog/dm/comp_2pointsleft.mp3");
    configstringRemove("localization/sound/dialog/dm/comp_1pointsleft.mp3");
    configstringRemove("localization/sound/dialog/dm/comp_mats.mp3");
    configstringRemove("localization/sound/dialog/dm/comp_tiedfirst.mp3");
    configstringRemove("localization/sound/dialog/dm/comp_winn.mp3");
    configstringRemove("localization/sound/dialog/dm/comp_second.mp3");
    configstringRemove("localization/sound/dialog/dm/comp_third.mp3");
    configstringRemove("localization/sound/dialog/dm/comp_didnotrank.mp3");
    configstringRemove("localization/sound/dialog/dm/comp_matover.mp3");
}

//not yet in use
bool CoopManager::ShouldGiveSpawnItems() const {
    return giveSpawnItems;
}
bool CoopManager::IsFriendlyFireEnabled() const {
    return friendlyFire;
}
int CoopManager::GetPointsForEnemyKill() const {
    return enemyKillPoints;
}
int CoopManager::GetPenaltyForPlayerKill() const {
    return playerKillPenalty;
}



bool CoopManager::getPlayerData_coopAdmin(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopAdmin() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].coopAdmin;
}
void CoopManager::setPlayerData_coopAdmin(Player* player, bool state) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopAdmin() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopAdmin = state;
}
void CoopManager::setPlayerData_coopAdmin_reset(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopAdmin_reset() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopAdmin = false;
    coopManager_client_persistant_t[player->entnum].coopAdminAuthAttemps = 0;
    coopManager_client_persistant_t[player->entnum].coopAdminAuthString = "";
    coopManager_client_persistant_t[player->entnum].coopAdminAuthStarted = false;
    coopManager_client_persistant_t[player->entnum].coopAdminAuthStringLengthLast = 0; 
}

int CoopManager::getPlayerData_coopSetupTries(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopSetupTries() nullptr player");
        return 0;
    }
    return coopManager_client_persistant_t[player->entnum].coopSetupTries;
}
void CoopManager::setPlayerData_coopSetupTries_update(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopSetupTries_update() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopSetupTries++;
}
void CoopManager::setPlayerData_coopSetupTries_reset(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPayerData_coopSetupTries_reset() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopSetupTries = 0;
}

int CoopManager::getPlayerData_coopSetupNextCheckTime(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopSetupNextCheckTime() nullptr player");
        return 0;
    }
    return coopManager_client_persistant_t[player->entnum].coopSetupNextCheckTime;
}
void CoopManager::setPlayerData_coopSetupNextCheckTime_update(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopSetupNextCheckTime_update() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopSetupNextCheckTime = (level.time + 0.15f);
}
void CoopManager::setPlayerData_coopSetupNextCheckTime_reset(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPayerData_coopSetupNextCheckTime_reset() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopSetupNextCheckTime = -999.0f;
}

int CoopManager::getPlayerData_coopAdminAuthAttemps(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopAdminAuthAttemps() nullptr player");
        return 0;
    }
    return coopManager_client_persistant_t[player->entnum].coopAdminAuthAttemps;
}
void CoopManager::setPayerData_coopAdminAuthAttemps_update(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPayerData_coopAdminAuthAttemps_update() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopAdminAuthAttemps++;
}
void CoopManager::setPayerData_coopAdminAuthAttemps_reset(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPayerData_coopAdminAuthAttemps_update() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopAdminAuthAttemps = 0;
}
bool CoopManager::getPlayerData_coopAdminAuthStarted(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopAdminAuthStarted() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].coopAdminAuthStarted;
}
void CoopManager::setPayerData_coopAdminAuthStarted(Player* player, bool started) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPayerData_coopAdminAuthStarted() nullptr player");
        return;
    }

    coopManager_client_persistant_t[player->entnum].coopAdminAuthStarted = started;
}
str CoopManager::getPlayerData_coopAdminAuthString(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopAdminAuthString() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].coopAdminAuthString;
}
void CoopManager::setPlayerData_coopAdminAuthString(Player* player, str newText) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopAdminAuthString() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopAdminAuthString = newText;
}
bool CoopManager::getPlayerData_coopAdminAuthString_changed(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopAdminAuthString_changed() nullptr player");
        return false;
    }
    if (getPlayerData_coopAdminAuthStringLengthLast(player) != (int)strlen(getPlayerData_coopAdminAuthString(player))) {
        int lastAuthStringLength = getPlayerData_coopAdminAuthStringLengthLast(player);
        lastAuthStringLength++;
        setPlayerData_coopAdminAuthStringLengthLast(player,lastAuthStringLength);
        return true;
    }
    return false;
}
int CoopManager::getPlayerData_coopAdminAuthStringLengthLast(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopAdminAuthStringLengthLast() nullptr player");
        return 0;
    }
    return coopManager_client_persistant_t[player->entnum].coopAdminAuthStringLengthLast;
}
void CoopManager::setPlayerData_coopAdminAuthStringLengthLast(Player* player, int strLength) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopAdminAuthStringLengthLast() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopAdminAuthStringLengthLast = strLength;
}

Vector CoopManager::getPlayerData_radarBlipLastPos(Player* player, short int blipNum)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_radarBlipLastPos() nullptr player");
        return Vector(0.0f, 0.0f, 0.0f);
    }
    if (blipNum < 0 || blipNum >= _COOP_SETTINGS_RADAR_BLIPS_MAX) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_radarBlipLastPos() blipNum out of range");
        return Vector(0.0f, 0.0f, 0.0f);
    }
    return coopManager_client_persistant_t[player->entnum].radarBlipPositionLast[blipNum];
}
void CoopManager::setPlayerData_radarBlipLastPos(Player* player,short int blipNum,Vector blipLastPos)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_radarBlipLastPos() nullptr player");
        return;
    }
    if (blipNum < 0 || blipNum >= _COOP_SETTINGS_RADAR_BLIPS_MAX) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_radarBlipLastPos() blipNum out of range");
        return;
    }
    coopManager_client_persistant_t[player->entnum].radarBlipPositionLast[blipNum] = blipLastPos;
}

bool CoopManager::getPlayerData_radarBlipActive(Player* player, short int blipNum)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_radarBlipActive() nullptr player");
        return false;
    }
    if (blipNum < 0 || blipNum >= _COOP_SETTINGS_RADAR_BLIPS_MAX) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_radarBlipActive() blipNum out of range");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].radarBlipActive[blipNum];
}
void CoopManager::setPlayerData_radarBlipActive(Player* player,short int blipNum,bool blipActive)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_radarBlipActive() nullptr player");
        return;
    }
    if (blipNum < 0 || blipNum >= _COOP_SETTINGS_RADAR_BLIPS_MAX) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_radarBlipActive() blipNum out of range");
        return;
    }
    coopManager_client_persistant_t[player->entnum].radarBlipActive[blipNum] = blipActive;
}

bool CoopManager::getPlayerData_radarSelectedActive(Player* player)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_radarSelectedActive() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].radarSelectedActive;
}
void CoopManager::setPlayerData_radarSelectedActive(Player* player,bool selectedActive)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_radarSelectedActive() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].radarSelectedActive = selectedActive;
}

float CoopManager::getPlayerData_radarUpdatedLast(Player* player)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_radarUpdatedLast() nullptr player");
        return 99999.0f;
    }
    return coopManager_client_persistant_t[player->entnum].radarUpdateTimeLast;
}
void CoopManager::setPlayerData_radarUpdatedLast(Player* player,float lastUpdate)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_radarUpdatedLast() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].radarUpdateTimeLast = lastUpdate;
}

float CoopManager::getPlayerData_radarAngleLast(Player* player)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_radarAngleLast() nullptr player");
        return 99999.0f;
    }
    return coopManager_client_persistant_t[player->entnum].radarAngleLast;
}
void CoopManager::setPlayerData_radarAngleLast(Player* player,float lastAngle)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_radarAngleLast() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].radarAngleLast = lastAngle;
}
int CoopManager::getPlayerData_radarScale(Player* player)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_radarScale() nullptr player");
        return 1;
    }
    return coopManager_client_persistant_t[player->entnum].radarScale;
}
void CoopManager::setPlayerData_radarScale(Player* player, int radarScale)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_radarScale() nullptr player");
        return;
    }
    if (radarScale < _COOP_SETTINGS_RADAR_SCALE_MIN) { radarScale = _COOP_SETTINGS_RADAR_SCALE_MIN; }
    if (radarScale > _COOP_SETTINGS_RADAR_SCALE_MAX) { radarScale = _COOP_SETTINGS_RADAR_SCALE_MAX; }
    coopManager_client_persistant_t[player->entnum].radarScale = radarScale;
}



bool CoopManager::getPlayerData_coopClientIdDone(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopClientIdDone() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].coopClientIdDone;
}
void CoopManager::setPlayerData_coopClientIdDone(Player* player, bool state) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopClientIdDone() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopClientIdDone = state;
}
str CoopManager::getPlayerData_coopClientId(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopClientId() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].coopClientId;
}
void CoopManager::setPlayerData_coopClientId(Player* player, str coopClientId) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopClientId() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopClientId = coopClientId;
}
bool CoopManager::getPlayerData_coopSetupDone(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopSetupDone() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].coopSetupDone;
}
void CoopManager::setPlayerData_coopSetupDone(Player* player, bool state) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopSetupDone() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopSetupDone = state;
}

bool CoopManager::getPlayerData_coopSetupStarted(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopSetupStarted() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].coopSetupStarted;
}
void CoopManager::setPlayerData_coopSetupStarted(Player* player, bool state) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopSetupStarted() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopSetupStarted = state;
}

str CoopManager::getPlayerData_coopClass(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopClass() nullptr player");
        return "";
    }
    return coopManager_client_persistant_t[player->entnum].coopClass;
}
void CoopManager::setPlayerData_coopClass(Player* player, str className) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopClass() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopClass = className;
}

int CoopManager::getPlayerData_coopVersion(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopVersion() nullptr player");
        return 0;
    }
    return coopManager_client_persistant_t[player->entnum].coopVersion;
}
void CoopManager::setPlayerData_coopVersion(Player* player, int coopVersion) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopVersion() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopVersion = coopVersion;
}

bool CoopManager::getPlayerData_respawnMe(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_respawnMe() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].respawnMe;
}
void CoopManager::setPlayerData_respawnMe(Player* player, bool respawn) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_respawnMe() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].respawnMe = respawn;
}

bool CoopManager::getPlayerData_spawnLocationSpawnForced(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_spawnLocationSpawnForced() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].spawnLocationSpawnForced;
}
void CoopManager::setPlayerData_spawnLocationSpawnForced(Player* player, bool forced) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_spawnLocationSpawnForced() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].spawnLocationSpawnForced = forced;
}

bool CoopManager::getPlayerData_respawnLocationSpawn(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_respawnLocationSpawn() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].respawnLocationSpawn;
}
void CoopManager::setPlayerData_respawnLocationSpawn(Player* player, bool forced) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_respawnLocationSpawn() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].respawnLocationSpawn = forced;
}

Vector CoopManager::getPlayerData_lastValidLocation(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_lastValidLocation() nullptr player");
        return Vector(0.0f, 0.0f, 0.0f);
    }
    return coopManager_client_persistant_t[player->entnum].lastValidLocation;
}
void CoopManager::setPlayerData_lastValidLocation(Player* player, Vector location) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_lastValidLocation() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].lastValidLocation = location;
}

Vector CoopManager::getPlayerData_lastValidViewAngle(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_lastValidViewAngle() nullptr player");
        return Vector(0.0f, 0.0f, 0.0f);
    }
    return coopManager_client_persistant_t[player->entnum].lastValidViewAngle;
}
void CoopManager::setPlayerData_lastValidViewAngle(Player* player, Vector viewAngle) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_lastValidViewAngle() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].lastValidViewAngle = viewAngle;
}

float CoopManager::getPlayerData_lastSpawned(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_lastSpawned() nullptr player");
        return 0.0f;
    }
    return coopManager_client_persistant_t[player->entnum].lastSpawned;
}
void CoopManager::setPlayerData_lastSpawned(Player* player, float lastSpawned) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_lastSpawned() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].lastSpawned = lastSpawned;
}

int CoopManager::getPlayerData_objectiveCycle(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_objectiveCycle() nullptr player");
        return 0;
    }
    return coopManager_client_persistant_t[player->entnum].objectiveCycle;
}
void CoopManager::setPlayerData_objectiveCycle(Player* player, int objectiveCycle) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_objectiveCycle() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].objectiveCycle = objectiveCycle;
}

int CoopManager::getPlayerData_coopObjectiveStatus(Player* player, int item) {
    if (item < 0 || item >= _COOP_SETTINGS_OBJECTIVES_MAX) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopObjectiveStatus() index out of range");
        return -1;
    }
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopObjectiveStatus() nullptr player");
        return -1;
    }
    return coopManager_client_persistant_t[player->entnum].objectiveItemStatus[item];
}
void CoopManager::setPlayerData_coopObjectiveStatus(Player* player, int item, int status) {
    if (item < 0 || item >= _COOP_SETTINGS_OBJECTIVES_MAX) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopObjectiveStatus() index out of range");
        return;
    }
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopObjectiveStatus() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].objectiveItemStatus[item] = status;
}

int CoopManager::getPlayerData_coopObjectiveSend(Player* player, int item) {
    if (item < 0 || item >= _COOP_SETTINGS_OBJECTIVES_MAX) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopObjectiveSend() index out of range");
        return 0;
    }
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopObjectiveSend() nullptr player");
        return 0;
    }
    return coopManager_client_persistant_t[player->entnum].objectiveItemSend[item];
}
void CoopManager::setPlayerData_coopObjectiveSend(Player* player, int item, int status) {
    if (item < 0 || item >= _COOP_SETTINGS_OBJECTIVES_MAX) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopObjectiveSend() index out of range");
        return;
    }
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopObjectiveSend() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].objectiveItemSend[item] = status;
}

int CoopManager::getPlayerData_coopObjectiveShown(Player* player, int item) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopObjectiveShown() nullptr player");
        return 0;
    }
    if (item < 0 || item >= _COOP_SETTINGS_OBJECTIVES_MAX) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopObjectiveShown() index out of range");
        return 0;
    }
    return coopManager_client_persistant_t[player->entnum].objectiveItemShown[item];
}
void CoopManager::setPlayerData_coopObjectiveShown(Player* player, int item, int status) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopObjectiveShown() nullptr player");
        return;
    }
    if (item < 0 || item > _COOP_SETTINGS_OBJECTIVES_MAX) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopObjectiveShown() index out of range");
        return;
    }
    coopManager_client_persistant_t[player->entnum].objectiveItemShown[item] = status;
}

float CoopManager::getPlayerData_objectiveItemCompletedAt(Player* player, int item) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_objectiveItemCompletedAt() nullptr player");
        return 0.0f;
    }
    if (item < 0 || item > _COOP_SETTINGS_OBJECTIVES_MAX) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_objectiveItemCompletedAt() index out of range");
        return 0.0f;
    }
    return coopManager_client_persistant_t[player->entnum].objectiveItemCompletedAt[item];
}
void CoopManager::setPlayerData_objectiveItemCompletedAt(Player* player, int item, float status) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_objectiveItemCompletedAt() nullptr player");
        return;
    }
    if (item < 0 || item > _COOP_SETTINGS_OBJECTIVES_MAX) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_objectiveItemCompletedAt() index out of range");
        return;
    }
    coopManager_client_persistant_t[player->entnum].objectiveItemCompletedAt[item] = status;
}

float CoopManager::getPlayerData_objectiveItemLastTimePrintedTitleAt(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_objectiveItemLastTimePrintedTitleAt() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].objectiveItemPrintedTitleLastTime;
}
void CoopManager::setPlayerData_objectiveItemLastTimePrintedTitleAt(Player* player, float lastPrintedTitleAt) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_objectiveItemLastTimePrintedTitleAt() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].objectiveItemPrintedTitleLastTime = lastPrintedTitleAt;
}

str CoopManager::getPlayerData_objectiveItemLastTimePrintedTitle(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_objectiveItemLastTimePrintedTitle() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].objectiveItemPrintedTitleLast;
}
void CoopManager::setPlayerData_objectiveItemLastTimePrintedTitle(Player* player, str lastPrintedTitle) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_objectiveItemLastTimePrintedTitle() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].objectiveItemPrintedTitleLast = lastPrintedTitle;
}

void CoopManager::setPlayerData_objectives_reset(Player* player)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPLayerData_objectives_reset() nullptr player");
        return;
    }

    coopManager_client_persistant_t[player->entnum].objectiveItemPrintedTitleLast = "";
    coopManager_client_persistant_t[player->entnum].objectiveItemPrintedTitleLastTime = 0.0f;
    coopManager_client_persistant_t[player->entnum].objectiveCycle = 0;
    coopManager_client_persistant_t[player->entnum].objectiveSetupDone = false;

    for (int i = 0; i < _COOP_SETTINGS_OBJECTIVES_MAX; i++) {
        setPlayerData_coopObjectiveStatus(player, i, -1);
        setPlayerData_coopObjectiveSend(player, i, 0);
        setPlayerData_coopObjectiveShown(player, i, 0);
        setPlayerData_objectiveItemCompletedAt(player, i, 0.0f);
    }
}

bool CoopManager::getPlayerData_objectives_setupDone(Player* player)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPLayerData_objectives_setupDone() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].objectiveSetupDone;
}
void CoopManager::setPlayerData_objectives_setupDone(Player* player)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPLayerData_objectives_setupDone() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].objectiveSetupDone = true;
}

bool CoopManager::getPlayerData_targetedShow(Player* player)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_targetedShow() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].targetedShow;
}
void CoopManager::setPlayerData_targetedShow(Player* player, bool status)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_targetedShow() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].targetedShow = status;
}

bool CoopManager::getPlayerData_coopClassLocked(Player* player)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopClassLocked() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].coopClassLocked;
}
void CoopManager::setPlayerData_coopClassLocked(Player* player, bool status)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopClassLocked() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopClassLocked = status;
}

#endif