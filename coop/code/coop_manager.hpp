#pragma once
#include "../../dlls/game/mp_manager.hpp"
#include "../../dlls/game/_pch_cpp.h"
#include "coop_config.hpp"
#include "coop_generalstrings.hpp"


//windows/linux+other handle
#ifdef WIN32
#include <windows.h>
#define DEBUG_LOG(...) do { OutputDebugStringA(va(__VA_ARGS__)); } while(0)
#else
#define DEBUG_LOG(...) do { gi.printf(va(__VA_ARGS__));} while (0)
#endif



struct coopManager_client_persistant_s
{
    str				coopClass = "Technician";
};
extern coopManager_client_persistant_s coopManager_client_persistant_t[MAX_CLIENTS];


struct coopManager_mapSettings_s
{
    bool rpgMap = false;
    bool coopMap = false;
    bool coopIncluded = false;
    bool coopSpMission = false;
    bool coopSpIgm = false;
    bool coopSpSecret = false;
    bool multiplayerOnly = false;
    bool singleplayerOnly = false;
    bool stockMap = false;
    str checkPoint = "";
    str cleanName = "";
};
extern coopManager_mapSettings_s coopManager_mapSettings_t;


class CoopManager {
public:
    str coopManager_maplist_contents = "";
    Container<str> coopManager_validPlayerModels;

    class MapFlags {
    public:
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

public:
    static CoopManager& Get();

    void Init();
    void InitWorld();
    bool IsCoopLevel();
    coopManager_mapSettings_s DetectMapType(str mapName);
    void SetMapType();
    void LoadLevelScript(str mapname);
    void Shutdown();
    void LevelStart(CThread* gamescript);
    void LevelEndCleanup(qboolean temp_restart);
    bool IsCoopEnabled() const;
    void DisableCoop();
    bool IsRpgEnabled() const;


    void ClientThink(Player* player);


    void playerEntered(gentity_t* ent);
    void playerSpawned(Player* player);
    void playerDied(Player* player);
    void playerLeft(Player* player);
    void playerTransported(Entity* entity);
    void playerChangedClass(Player* player);
    void playerChangedModel(Player* player);
    void playerBecameSpectator(Player* player);

    // Configurable systems
    bool ShouldGiveSpawnItems() const;
    bool IsFriendlyFireEnabled() const;
    int  GetPointsForEnemyKill() const;
    int  GetPenaltyForPlayerKill() const;

private:
    CoopManager() = default;

    void LoadPlayerModelsFromINI();
    void LoadSettingsFromINI();
    void LoadMapListFromINI();

    MapFlags mapFlags;

    bool coopEnabled = false;
    bool rpgEnabled = false;

    bool friendlyFire = false;
    bool giveSpawnItems = false;

    int enemyKillPoints = 0;
    int playerKillPenalty = 0;

    // Possibly other future flags...
};
