#include "../../dlls/game/level.h"
#include "../../dlls/game/gamefix.hpp"
#include "coop_manager.hpp"
#include "coop_objectives.hpp"
#include "coop_armory.hpp"

coopManager_client_persistant_s coopManager_client_persistant_t[MAX_CLIENTS];
coopManager_mapSettings_s coopManager_mapSettings_t;


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
    return coopManager_client_persistant_t[player->entnum].coopObjectiveStatus[item];
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
    coopManager_client_persistant_t[player->entnum].coopObjectiveStatus[item] = status;
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
    return coopManager_client_persistant_t[player->entnum].coopObjectiveSend[item];
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
    coopManager_client_persistant_t[player->entnum].coopObjectiveSend[item] = status;
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
    return coopManager_client_persistant_t[player->entnum].coopObjectiveShown[item];
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
    coopManager_client_persistant_t[player->entnum].coopObjectiveShown[item] = status;
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
    return coopManager_client_persistant_t[player->entnum].objectiveItemLastTimePrintedTitleAt;
}
void CoopManager::setPlayerData_objectiveItemLastTimePrintedTitleAt(Player* player, float lastPrintedTitleAt) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_objectiveItemLastTimePrintedTitleAt() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].objectiveItemLastTimePrintedTitleAt = lastPrintedTitleAt;
}

str CoopManager::getPlayerData_objectiveItemLastTimePrintedTitle(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_objectiveItemLastTimePrintedTitle() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].objectiveItemLastTimePrintedTitle;
}
void CoopManager::setPlayerData_objectiveItemLastTimePrintedTitle(Player* player, str lastPrintedTitle) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_objectiveItemLastTimePrintedTitle() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].objectiveItemLastTimePrintedTitle = lastPrintedTitle;
}

void CoopManager::setPlayerData_objectives_reset(Player* player)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPLayerData_objectives_reset() nullptr player");
        return;
    }

    coopManager_client_persistant_t[player->entnum].objectiveItemLastTimePrintedTitle = "";
    coopManager_client_persistant_t[player->entnum].objectiveItemLastTimePrintedTitleAt = 0.0f;
    coopManager_client_persistant_t[player->entnum].objectiveCycle = 0;
    coopManager_client_persistant_t[player->entnum].objectiveSetupDone = false;

    for (int i = 0; i < _COOP_SETTINGS_OBJECTIVES_MAX;i++) {
        setPlayerData_coopObjectiveStatus(player,i,-1);
        setPlayerData_coopObjectiveSend(player, i,0);
        setPlayerData_coopObjectiveShown(player, i,0);
        setPlayerData_objectiveItemCompletedAt(player, i,0.0f);
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

void CoopManager::ClientThink(Player *player) {
    coop_objectivesUpdatePlayer(player);
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
                DEBUG_LOG(va("%s used from /co-op/ folder\n", sFile.c_str()));
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

//executed once, only on game server start/load
//executed before world and other entities exist
void CoopManager::Init() {
    try {
        gi.Printf(_COOP_INFO_INIT_game);

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
    str contents;
    if (gamefix_getFileContents(_COOP_FILE_validPlayerModels, contents, true)) {
        str sectionContents = gamefix_iniSectionGet(_COOP_FILE_validPlayerModels, contents, "coopSkins");
        
        DEBUG_LOG("Valid Player Models\n");

        //default fallback hardcoded model
        str skinName = "models/char/munro.tik";
        coopManager_validPlayerModels.AddUniqueObject(skinName);
       
        DEBUG_LOG("%s\n",skinName.c_str());

        //get models by number, starting with 1
        int i = 1;
        while (skinName.length()) {
            skinName = gamefix_iniKeyGet(_COOP_FILE_validPlayerModels, sectionContents,va("%d",i), "");
            if (skinName.length()) {
                coopManager_validPlayerModels.AddUniqueObject(skinName);
                
                DEBUG_LOG("%s\n",skinName.c_str());
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

        //coop only
        if (coopEnabled) {
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

bool CoopManager::IsCoopLevel() {
    return mapFlags.coopMap;
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
    //coop only
    if (coopEnabled) {
        //...
    }

    coopManager_validPlayerModels.FreeObjectList();
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
        //...
    }
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

void CoopManager::playerReset(Player* player) {
    if (!player) {
        return;
    }
    setPlayerData_coopSetupDone(player,false);
    setPlayerData_coopClass(player,"");
    setPlayerData_coopVersion(player,-1);
    setPlayerData_respawnMe(player,false);
    setPlayerData_spawnLocationSpawnForced(player,true);
    setPlayerData_respawnLocationSpawn(player,false);
    setPlayerData_lastValidLocation(player, Vector(0.0f, 0.0f, 0.0f));
    setPlayerData_lastValidViewAngle(player, Vector(0.0f, 0.0f, 0.0f));
    setPlayerData_lastSpawned(player, -1.0f);
    setPlayerData_objectives_reset(player);
}

void CoopManager::playerConnect(int clientNum) {
    if (clientNum < 0 || clientNum >= MAX_CLIENTS) {
        return;
    }
}

//Executed upon entering server - Always (Multiplayer + Singleplayer)
void CoopManager::playerEntered(gentity_t* ent) {
    if (ent && ent->entity) {
        ExecuteThread("coop_justEntered", true, ent->entity);
    }
}
//Executed on death - Always (Multiplayer + Singleplayer)
void CoopManager::playerLeft(Player* player) {

    if (player) {
        ExecuteThread("coop_justLeft", true, player);
    }
}
//Executed on death - Always (Multiplayer + Singleplayer)
void CoopManager::playerDied(Player *player){
    if (player) {
        ExecuteThread("coop_justDied", true, player);
    }
}
//Executed on death - Always (Multiplayer + Singleplayer)
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

    coop_armoryEquipPlayer(player);

    gamefix_setMakeSolidAsap((Entity*)player, true, 0.0f);
    ExecuteThread("coop_justSpawned", true, player);
}
//Executed on death - Multiplayer
void CoopManager::playerBecameSpectator(Player *player){
    if (player) {
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

//not yet in use
bool CoopManager::IsRpgEnabled() const {
    return rpgEnabled;
}
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
