#pragma once
#include "../../dlls/game/_pch_cpp.h"
#include "coop_config.hpp"
#include "coop_generalstrings.hpp"


class CoopManager {
public:
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
    void DetectMapType();      
    void LoadLevelScript(str mapname);
    void Shutdown();
    void LevelStart(CThread* gamescript);
    void LevelEndCleanup(qboolean temp_restart);
    bool IsCoopEnabled() const;
    bool IsRpgEnabled() const;


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

    void LoadSettingsFromINI();

    MapFlags mapFlags;

    bool coopEnabled = false;
    bool rpgEnabled = false;

    bool friendlyFire = false;
    bool giveSpawnItems = false;

    int enemyKillPoints = 0;
    int playerKillPenalty = 0;

    // Possibly other future flags...
};
