#include "../../dlls/game/level.h"
#include "../../dlls/game/gamefix.hpp"
#include "coop_manager.hpp"

CoopManager& CoopManager::Get() {
    static CoopManager instance;
    return instance;
}

void CoopManager::Init() {
    try {
        //Print Init Status Info
        ///////////////////////////////////////////////////
        gi.Printf(_COOP_INFO_INIT_game);

        LoadSettingsFromINI();

        //Print Init Status Info
        ///////////////////////////////////////////////////
        gi.Printf(_COOP_INFO_INIT_gamedone);
    }
    catch (const char* error) {
        gi.Printf(_COOP_ERROR_fatal, error);
        G_ExitWithError(error);
    }
}

void CoopManager::InitWorld() {
    try {
        //Print Init WORLD Status Info
        ///////////////////////////////////////////////////
        gi.Printf(_COOP_INFO_INIT_world, level.mapname.c_str());

        DetectMapType();

        str coopStatus = "inactive";
        str sAllowSpMaps = "0";

        //Decide if we want to enable Coop
        ///////////////////////////////////////////////////
        //this early we can't use API - multiplayer manager has not been started yet
        if (mapFlags.coopMap && g_gametype->integer == GT_MULTIPLAYER) { 
            sAllowSpMaps = "1";
            coopStatus = "ACTIVE";
            coopEnabled = true;
        }
        gi.cvar_set("gfix_allowSpMaps", sAllowSpMaps);

        if (IsCoopEnabled() || IsRpgEnabled()) {
            OverrideMultiplayer();
        }

        //Print TEST Status Info
        ///////////////////////////////////////////////////
        gi.Printf(_COOP_INFO_INIT_status, coopStatus.c_str(), level.mapname.c_str());
    }
    catch (const char* error) {
        gi.Printf(_COOP_ERROR_fatal, error);
        G_ExitWithError(error);
    }
}

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

void CoopManager::Shutdown() {
    // Clean up anything coop-related if needed
}

void CoopManager::EndMap() {
    // Cleanup or save state
}

bool CoopManager::IsCoopEnabled() const {
    return coopEnabled;
}

bool CoopManager::IsRpgEnabled() const {
    return rpgEnabled;
}

void CoopManager::OverrideMultiplayer() {
    if (!coopEnabled && !rpgEnabled)
        return;

    // TODO: Hook or disable multiplayerManager behavior as needed
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

void CoopManager::LoadSettingsFromINI() {
    str contents;
    if (gamefix_getFileContents(_COOP_FILE_settings, contents, true)) {
        enemyKillPoints     = atoi(gamefix_iniKeyGet(_COOP_FILE_settings, contents, "enemyKillPoints", "0"));
        playerKillPenalty   = atoi(gamefix_iniKeyGet(_COOP_FILE_settings, contents, "playerKillPenalty", "0"));
        friendlyFire        = gamefix_iniKeyGet(_COOP_FILE_settings, contents, "friendlyFire", "false") == "true";
        giveSpawnItems      = gamefix_iniKeyGet(_COOP_FILE_settings, contents, "giveSpawnItems", "false") == "true";
    }
}
