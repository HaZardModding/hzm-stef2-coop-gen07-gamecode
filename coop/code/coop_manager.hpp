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
    bool		coopSetupDone = false;
    str			coopClass = "Technician";
    int         coopVersion = 0;
    bool        respawnMe = false;
    bool        spawnLocationSpawnForced = true;
    bool        respawnLocationSpawn = false;
    Vector      lastValidLocation = Vector(0.0f, 0.0f, 0.0f);
    Vector      lastValidViewAngle = Vector(0.0f, 0.0f, 0.0f);
    float       lastSpawned = -1;
    int         objectiveCycle = -1;
    int			coopObjectiveStatus[8] = { 0,0,0,0,0,0,0,0 };
    int			coopObjectiveSend[8] = { 0,0,0,0,0,0,0,0 };
    int			coopObjectiveShown[8] = { 0,0,0,0,0,0,0,0 };
    float       objectiveItemCompletedAt[8] = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f };
    float       objectiveItemLastTimePrintedTitleAt = -1.0;
    str         objectiveItemLastTimePrintedTitle = "";
    bool        objectiveSetupDone = false;
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
        str checkPoint = "";
        str cleanName = "";
        bool scriptIncludedCoopMain = false;
        bool scriptIncludedCoopNoscript = false;
        bool scriptIncludedCoopMom = false;
        bool scriptIncludedCoopMom4 = false;
        bool scriptIncludedCoopMrm = false;
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

    str IncludeScriptReplace(str sLex);
    void IncludeScriptCheck(str &sLex);
    bool IncludedScriptCoop();


    void playerReset(Player* player);
    void playerConnect(int clientNum);
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

    // Access to coopManager_client_persistant_t
    bool getPlayerData_coopSetupDone(Player* player);
    void setPlayerData_coopSetupDone(Player* player, bool state);
    str getPlayerData_coopClass(Player* player);
    void setPlayerData_coopClass(Player* player, str className);
    int getPlayerData_coopVersion(Player* player);
    void setPlayerData_coopVersion(Player* player, int coopVersion);
    bool getPlayerData_respawnMe(Player* player);
    void setPlayerData_respawnMe(Player* player, bool respawn);
    bool getPlayerData_spawnLocationSpawnForced(Player* player);
    void setPlayerData_spawnLocationSpawnForced(Player* player, bool forced);
    bool getPlayerData_respawnLocationSpawn(Player* player);
    void setPlayerData_respawnLocationSpawn(Player* player, bool forced);
    Vector getPlayerData_lastValidLocation(Player* player);
    void setPlayerData_lastValidLocation(Player* player, Vector location);
    Vector getPlayerData_lastValidViewAngle(Player* player);
    void setPlayerData_lastValidViewAngle(Player* player, Vector viewAngle);
    float getPlayerData_lastSpawned(Player* player);
    void setPlayerData_lastSpawned(Player* player, float lastSpawned);
    int getPlayerData_objectiveCycle(Player* player);
    void setPlayerData_objectiveCycle(Player* player, int objectiveCycle);
    int getPlayerData_coopObjectiveStatus(Player* player, int item);
    void setPlayerData_coopObjectiveStatus(Player* player, int item, int status);
    int getPlayerData_coopObjectiveSend(Player* player, int item);
    void setPlayerData_coopObjectiveSend(Player* player, int item, int status);
    int getPlayerData_coopObjectiveShown(Player* player, int item);
    void setPlayerData_coopObjectiveShown(Player* player, int item, int status);
    float getPlayerData_objectiveItemCompletedAt(Player* player, int item);
    void setPlayerData_objectiveItemCompletedAt(Player* player, int item, float status);
    float getPlayerData_objectiveItemLastTimePrintedTitleAt(Player* player);
    void setPlayerData_objectiveItemLastTimePrintedTitleAt(Player* player, float lastPrintedTitleAt);
    str getPlayerData_objectiveItemLastTimePrintedTitle(Player* player);
    void setPlayerData_objectiveItemLastTimePrintedTitle(Player* player, str lastPrintedTitle);
    void setPlayerData_objectives_reset(Player* player);
    bool getPlayerData_objectives_setupDone(Player* player);
    void setPlayerData_objectives_setupDone(Player* player);

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
