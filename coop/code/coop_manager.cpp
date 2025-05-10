#include "../../dlls/game/level.h"
#include "../../dlls/game/gamefix.hpp"
#include "coop_manager.hpp"


CoopManager& CoopManager::Get() {
    static CoopManager instance;
    return instance;
}

bool CoopManager::IsCoopEnabled() const {
    return coopEnabled;
}

//executed once, only on game server start/load
//executed before world and other entities exist
void CoopManager::Init() {
    try {
        gi.Printf(_COOP_INFO_INIT_game);

        LoadSettingsFromINI();

        gi.Printf(_COOP_INFO_INIT_gamedone);
    }
    catch (const char* error) {
        gi.Printf(_COOP_ERROR_fatal, error);
        G_ExitWithError(error);
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

//executed each level load - always
//requires the world to be already spawned
//Other: CoopManager::LevelStart()
void CoopManager::InitWorld() {
    try {
        gi.Printf(_COOP_INFO_INIT_world, level.mapname.c_str());

        DetectMapType();

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
            //...
        }
        
        gi.Printf(_COOP_INFO_INIT_status, coopStatus.c_str(), level.mapname.c_str());
    }
    catch (const char* error) {
        gi.Printf(_COOP_ERROR_fatal, error);
        G_ExitWithError(error);
    }
}

//detects what kind of gametype the level
//support and sets related parameters
void CoopManager::DetectMapType() {
    mapFlags.rpgMap = false;
    mapFlags.coopMap = false;
    mapFlags.coopIncluded = false;
    mapFlags.coopSpMission = false;
    mapFlags.coopSpIgm = false;
    mapFlags.coopSpSecret = false;
    mapFlags.multiplayerOnly = false;
    mapFlags.singleplayerOnly = false;
    mapFlags.stockMap = false;

    str mapNameClean = gamefix_cleanMapName(level.mapname);

    //remember if it is a standard map - do this always
    //check if it is a coop or rpg map
    if (gameFixAPI_mapIsStock(mapNameClean))         mapFlags.stockMap = true;
    if (mapNameClean.icmpn("coop_", 5) == 0)         mapFlags.coopMap = true;
    else if (mapNameClean.icmpn("rpg_", 4) == 0)     mapFlags.rpgMap = true;

    str fileContents;
    if (!gamefix_getFileContents(_COOP_FILE_maplist, fileContents, true)) {
        //Print maplist file warning
        ///////////////////////////////////////////////////
        gi.Printf(va(_COOP_WARNING_FILE_failed, _COOP_FILE_maplist));
        return;
    }

    //Get name of the section the map is listed in
    str sectionOfCurrentMap = gamefix_iniFindSectionByValue(_COOP_FILE_maplist, fileContents, level.mapname.c_str());
    if (sectionOfCurrentMap == _COOP_MAPLIST_CAT_included) { mapFlags.coopIncluded = true; mapFlags.coopMap = true; }
    if (sectionOfCurrentMap == _COOP_MAPLIST_CAT_coopSpMission) { mapFlags.coopSpMission = true; mapFlags.coopMap = true; }
    if (sectionOfCurrentMap == _COOP_MAPLIST_CAT_coopSpIgm) { mapFlags.coopSpIgm = true; mapFlags.coopMap = true; }
    if (sectionOfCurrentMap == _COOP_MAPLIST_CAT_coopSpSecret) { mapFlags.coopSpSecret = true; mapFlags.coopMap = true; }
    if (sectionOfCurrentMap == _COOP_MAPLIST_CAT_multiplayerOnly) { mapFlags.multiplayerOnly = true; }
    if (sectionOfCurrentMap == _COOP_MAPLIST_CAT_singleplayerOnly) { mapFlags.singleplayerOnly = true; }

    if (mapFlags.coopMap == false && mapFlags.multiplayerOnly == false && mapFlags.singleplayerOnly == false) {
        str sectionContents = gamefix_iniSectionGet(_COOP_FILE_maplist, fileContents, _COOP_MAPLIST_CAT_coopcompatible);
        str CoopCompatible = gamefix_iniKeyGet(_COOP_FILE_maplist, sectionContents, sectionOfCurrentMap, "");

        if (CoopCompatible.length() && CoopCompatible == "true") {
            mapFlags.coopMap = true;
        }
    }
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
    scriptFileDir.AddObject("coop/maps/");
    scriptFileDir.AddObject("maps/");
    int scriptFileDirectories = scriptFileDir.NumObjects();
    
    str dir,ext;
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

    gi.Printf("Level *.scr or *.script file not found in coop/maps or maps: '%s'\n", mapname.c_str());
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
}

//Executed if level is exited/changed/restarted - but not on first load/game start
//Cleans up stuff while world and entities still exist - Not executed if game server is quit
void CoopManager::LevelEndCleanup(qboolean temp_restart) {
    //coop only
    if (coopEnabled) {
        //...
    }
}

//Executed each time a level is starting - scripts are started
//Executed from: Level::Start - always
//Other: CoopManager::InitWorld
void CoopManager::LevelStart(CThread* gamescript) {
    if (coopEnabled) {
        //start coop main function automatically
        if (gamescript && gamescript->labelExists(_COOP_SCRIPT_main)) {
            CThread* coopMain = Director.CreateThread(_COOP_SCRIPT_main);
            if (coopMain) {
                coopMain->DelayedStart(0.0f);
            }
        }
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
