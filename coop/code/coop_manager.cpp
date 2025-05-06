#include "../../dlls/game/level.h"
#include "../../dlls/game/gamefix.hpp"
#include "coop_manager.hpp"


CoopManager& CoopManager::Get() {
    static CoopManager instance;
    return instance;
}

void CoopManager::Init() {
    coopEnabled = coop_MapFlags.coopMap;
    rpgEnabled = coop_MapFlags.rpgMap;

    LoadSettingsFromINI();
    OverrideMultiplayer();
}

void CoopManager::Shutdown() {
    // Clean up anything coop-related if needed
}

void CoopManager::StartMap() {
    // Custom logic on map start
}

void CoopManager::EndMap() {
    // Cleanup or save state
}

bool CoopManager::IsCoopEnabled() const {
    return coopEnabled;
}

bool CoopManager::IsRPGEnabled() const {
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
    if (gamefix_getFileContents("coop/settings.ini", contents, true)) {
        enemyKillPoints = gamefix_iniKeyGet("coop/settings.ini", contents, "enemyKillPoints", "0").toint();
        playerKillPenalty = gamefix_iniKeyGet("coop/settings.ini", contents, "playerKillPenalty", "0").toint();
        friendlyFire = gamefix_iniKeyGet("coop/settings.ini", contents, "friendlyFire", "false") == "true";
        giveSpawnItems = gamefix_iniKeyGet("coop/settings.ini", contents, "giveSpawnItems", "false") == "true";
    }
}
