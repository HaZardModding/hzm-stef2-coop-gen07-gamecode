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


    void Init();               // Called during game startup
    void InitWorld();          // Called when a map is spawned
    void DetectMapType();      




    void Shutdown();           // Optional cleanup
    void StartMap();           // Called when a new map starts
    void EndMap();             // Called when a map ends

    void OverrideMultiplayer();   // Disable/override default MP logic

    bool IsCoopEnabled() const;
    bool IsRpgEnabled() const;

    // Configurable systems
    bool ShouldGiveSpawnItems() const;
    bool IsFriendlyFireEnabled() const;
    int  GetPointsForEnemyKill() const;
    int  GetPenaltyForPlayerKill() const;

private:
    CoopManager() = default;

    void LoadSettingsFromINI(); // Load point values, damage flags, etc.

    MapFlags mapFlags;

    bool coopEnabled = false;
    bool rpgEnabled = false;

    bool friendlyFire = false;
    bool giveSpawnItems = false;

    int enemyKillPoints = 0;
    int playerKillPenalty = 0;

    // Possibly other future flags...
};
