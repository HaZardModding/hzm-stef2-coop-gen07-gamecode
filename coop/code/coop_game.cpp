#include "../../dlls/game/_pch_cpp.h"
#include "../../dlls/game/gamefix.hpp"
#include "../../dlls/game/gamefix_strings.hpp"
#include "../../dlls/game/script.h"

#include "coop_game.hpp"

namespace CoopGame {
    //define coop release number
    int          COOP_INFO_STATIC_RELEASE_NUMBER = 7000;
    //allowed by config setting
    bool         COOP_CONFIG_COOP_ALLOWED = true;

    void Init() {
        //Print Init Status Info
        ///////////////////////////////////////////////////
        gi.Printf(_COOP_INFO_INIT_game);
    }


    void Allowed() { COOP_CONFIG_COOP_ALLOWED = true; }
    void AllowedNot() { COOP_CONFIG_COOP_ALLOWED = false; }
    bool IsAllowed() { return COOP_CONFIG_COOP_ALLOWED; }
    
    bool IsSupported() { return coop_MapFlags.coopMap; }


    void InitWorld() {
        //Print Init WORLD Status Info
        ///////////////////////////////////////////////////
        gi.Printf(_COOP_INFO_INIT_world,level.mapname.c_str());

        DetectMapType();
        
        str coopStatus;
        //allow to play singleplayer maps with gamefix in place ?
        if (coop_MapFlags.coopMap)  { gi.cvar_set("gfix_allowSpMaps", "1");   coopStatus = "ACTIVE"; }
        else                        { gi.cvar_set("gfix_allowSpMaps", "0");   coopStatus = "inactive"; }

        //Print TEST Status Info
        ///////////////////////////////////////////////////
        gi.Printf(_COOP_INFO_INIT_status, coopStatus.c_str(), level.mapname.c_str());
    }

    void DetectMapType() {
        coop_MapFlags.rpgMap = false;
        coop_MapFlags.coopMap = false;
        coop_MapFlags.coopIncluded = false;
        coop_MapFlags.coopSpMission = false;
        coop_MapFlags.coopSpIgm = false;
        coop_MapFlags.coopSpSecret = false;
        coop_MapFlags.multiplayerOnly = false;
        coop_MapFlags.singleplayerOnly = false;
        coop_MapFlags.stockMap = false;
        
        str mapNameClean = gamefix_cleanMapName(level.mapname);

        //remember if it is a standard map - do this always
        //check if it is a coop or rpg map
        if (gameFixAPI_mapIsStock(mapNameClean))    coop_MapFlags.stockMap = true;
        if (mapNameClean.icmpn("coop_", 5))         coop_MapFlags.coopMap = true;
        else if (mapNameClean.icmpn("rpg_", 4))     coop_MapFlags.rpgMap = true;

        str fileContents;
        if (!gamefix_getFileContents(_COOP_FILE_maplist, fileContents, true)) {
            //Print maplist file warning
            ///////////////////////////////////////////////////
            gi.Printf(va(_COOP_WARNING_FILE_failed, _COOP_FILE_maplist));
            return;
        }

        //Get name of the section the map is listed in
        str sectionOfCurrentMap = gamefix_iniFindSectionByValue(_COOP_FILE_maplist, fileContents, level.mapname.c_str());
        if (sectionOfCurrentMap == _COOP_MAPLIST_CAT_included)          { coop_MapFlags.coopIncluded = true; coop_MapFlags.coopMap = true; }
        if (sectionOfCurrentMap == _COOP_MAPLIST_CAT_coopSpMission)     { coop_MapFlags.coopSpMission = true; coop_MapFlags.coopMap = true; }
        if (sectionOfCurrentMap == _COOP_MAPLIST_CAT_coopSpIgm)         { coop_MapFlags.coopSpIgm = true; coop_MapFlags.coopMap = true; }
        if (sectionOfCurrentMap == _COOP_MAPLIST_CAT_coopSpSecret)      { coop_MapFlags.coopSpSecret = true; coop_MapFlags.coopMap = true; }
        if (sectionOfCurrentMap == _COOP_MAPLIST_CAT_multiplayerOnly)   { coop_MapFlags.multiplayerOnly = true;  }
        if (sectionOfCurrentMap == _COOP_MAPLIST_CAT_singleplayerOnly)  { coop_MapFlags.singleplayerOnly = true; }

        if (coop_MapFlags.coopMap == false && coop_MapFlags.multiplayerOnly == false && coop_MapFlags.singleplayerOnly == false) {
            str sectionContents = gamefix_iniSectionGet(_COOP_FILE_maplist, fileContents, _COOP_MAPLIST_CAT_coopcompatible);
            str CoopCompatible  = gamefix_iniKeyGet(_COOP_FILE_maplist, sectionContents, sectionOfCurrentMap, "");

            if (CoopCompatible.length() && CoopCompatible == "true") {
                coop_MapFlags.coopMap = true;
            }
        }
    }
}
