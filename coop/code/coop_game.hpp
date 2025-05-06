#pragma once
#include "../../dlls/game/_pch_cpp.h"
#include "coop_generalstrings.hpp"
#include "coop_config.hpp"


//current map info
struct coop_MapFlags_s {
    bool rpgMap = false;
    bool coopMap = false;
    bool coopIncluded = false;
    bool coopSpMission = false;
    bool coopSpIgm = false;
    bool coopSpSecret = false;
    bool multiplayerOnly = false;
    bool singleplayerOnly = false;
    bool stockMap = false;
};
static coop_MapFlags_s coop_MapFlags;


namespace CoopGame {
    //define coop release number
    extern int          COOP_INFO_STATIC_RELEASE_NUMBER;
    //allowed by config setting
    extern bool         COOP_CONFIG_COOP_ALLOWED;
    

    void    Allowed();
    void    AllowedNot();
    bool    IsAllowed();
    
    bool    IsSupported();

    void    Init();
    void    InitWorld();
    void    DetectMapType();
}