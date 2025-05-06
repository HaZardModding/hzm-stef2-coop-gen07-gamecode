#pragma once

class CoopManager {
public:
    static CoopManager& Get();

    void Init();               // Called during game startup
    void Shutdown();           // Optional cleanup
    void StartMap();           // Called when a new map starts
    void EndMap();             // Called when a map ends

    void OverrideMultiplayer();   // Disable/override default MP logic

    bool IsCoopEnabled() const;
    bool IsRPGEnabled() const;

    // Configurable systems
    bool ShouldGiveSpawnItems() const;
    bool IsFriendlyFireEnabled() const;
    int  GetPointsForEnemyKill() const;
    int  GetPenaltyForPlayerKill() const;

private:
    CoopManager() = default;

    void LoadSettingsFromINI(); // Load point values, damage flags, etc.

    bool coopEnabled = false;
    bool rpgEnabled = false;

    bool friendlyFire = false;
    bool giveSpawnItems = false;

    int enemyKillPoints = 0;
    int playerKillPenalty = 0;

    // Possibly other future flags...
};
