#pragma once
#include "../../dlls/game/_pch_cpp.h"
#include "../../dlls/game/mp_manager.hpp"
#include "../../dlls/game/mp_modeBase.hpp"
#include "coop_config.hpp"
#include "coop_generalstrings.hpp"

extern Event EV_Player_DeactivateWeapon;
extern Event EV_Player_ActivateNewWeapon;


//windows/linux+other handle
#ifdef WIN32
#include <windows.h>
#define DEBUG_LOG(...) do { OutputDebugStringA(va(__VA_ARGS__)); } while(0)
#else
#define DEBUG_LOG(...) do { gi.printf(va(__VA_ARGS__));} while (0)
#endif



struct coopManager_client_persistant_s
{
    float		coopSetupNextCheckTime = -999.0f;
    short		coopSetupTries = 0;
    bool		coopSetupStarted = false;
    bool		coopSetupDone = false;
    bool		coopClientIdDone = false;
    bool        objectiveSetupDone = false;

    str         coopClientId = "";
    str			coopClass = "Technician";
    int         coopVersion = 0;
    bool        coopAdmin = false;
    int         coopAdminAuthAttemps = 0;
    str         coopAdminAuthString = "";
    bool        coopAdminAuthStarted = false;
    int         coopAdminAuthStringLengthLast = 0;

    bool        targetedShow = false;
    int         targetedLastEntNum = -1;
    
    Vector      radarBlipPositionLast[_COOP_SETTINGS_RADAR_BLIPS_MAX] = { Vector(0.0f, -999.0f, -999.0f),Vector(0.0f, -999.0f, -999.0f),Vector(0.0f, -999.0f, -999.0f),Vector(0.0f, -999.0f, -999.0f),Vector(0.0f, -999.0f, -999.0f),Vector(0.0f, -999.0f, -999.0f),Vector(0.0f, -999.0f, -999.0f),Vector(0.0f, -999.0f, -999.0f),Vector(0.0f, -999.0f, -999.0f) };
    bool        radarBlipActive[_COOP_SETTINGS_RADAR_BLIPS_MAX] = { false,false,false,false,false,false,false,false,false };
    bool        radarSelectedActive = false;
    float       radarUpdateTimeLast = -976.1f;
    float       radarAngleLast = 976.1f;
    int         radarScale = 1;

    bool        respawnMe = false;
    bool        spawnLocationSpawnForced = true;
    bool        respawnLocationSpawn = false;
    Vector      lastValidLocation = Vector(0.0f, 0.0f, 0.0f);
    Vector      lastValidViewAngle = Vector(0.0f, 0.0f, 0.0f);
    float       lastSpawned = -1.0f;
    int         objectiveCycle = -1;
    int			objectiveItemStatus[_COOP_SETTINGS_OBJECTIVES_MAX] = { 0,0,0,0,0,0,0,0 };
    int			objectiveItemSend[_COOP_SETTINGS_OBJECTIVES_MAX] = { 0,0,0,0,0,0,0,0 };
    int			objectiveItemShown[_COOP_SETTINGS_OBJECTIVES_MAX] = { 0,0,0,0,0,0,0,0 };
    float       objectiveItemCompletedAt[_COOP_SETTINGS_OBJECTIVES_MAX] = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f };
    float       objectiveItemPrintedTitleLastTime = -1.0f;
    str         objectiveItemPrintedTitleLast = "";
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

    bool IsCoopEnabled() const;
    void DisableCoop();
    bool IsCoopLevel();
    bool IsRpgEnabled() const;

    void Init();
    void InitWorld();
    coopManager_mapSettings_s DetectMapType(str mapName);
    void SetMapType();
    void LoadLevelScript(str mapname);
    void Shutdown();
    void LevelStart(CThread* gamescript);
    void LevelEndCleanup(qboolean temp_restart);
    void MissionFailed(const str& reason);
    str MissionFailureString(const str& reason);
    void MissionFailureLoadMap();

    str IncludeScriptReplace(str sLex);
    void IncludeScriptCheck(str &sLex);
    bool IncludedScriptCoop();

    void ClientThink(Player* player);

    int getNumberOfPlayers(bool noDead, bool noSpectator);

    void playerConnect(int clientNum);
    void playerDisconnect(Player* player);
    void playerJoined(Player* player);
    void playerSetup(Player* player);
    void playerSetupClId(Player* player);
    void playerSetupCoop(Player* player);
    void playerCoopDetected(const gentity_t* ent, const char* coopVer);
    void playerClIdDetected(const gentity_t* ent, const char* clientId);
    void playerClIdSet(Player* player);

    void playerAdminThink(Player* player);
    void playerMoveToSpawn(Player* player);
    Entity* getSpawnSpecific(int spotNumber);
    bool playerMoveToSpawnSpecific(Player* player, int spotNumber);
    void playerTargetnames(Player *player, Entity* viewTrace);

    void playerReset(Player* player);
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



    bool getPlayerData_coopAdmin(Player* player);
    void setPlayerData_coopAdmin(Player* player, bool state);
    void setPayerData_coopAdmin_reset(Player* player);
    int getPlayerData_coopAdminAuthAttemps(Player* player);
    void setPayerData_coopAdminAuthAttemps_update(Player* player);
    void setPayerData_coopAdminAuthAttemps_reset(Player* player);
    bool getPlayerData_coopAdminAuthStarted(Player* player);
    void setPayerData_coopAdminAuthStarted(Player* player, bool started);
    str getPlayerData_coopAdminAuthString(Player* player);
    void setPlayerData_coopAdminAuthString(Player* player,str newText);
    bool getPlayerData_coopAdminAuthString_changed(Player* player);
    int getPlayerData_coopAdminAuthStringLengthLast(Player* player);
    void setPlayerData_coopAdminAuthStringLengthLast(Player* player, int strLength);

    Vector getPlayerData_radarBlipLastPos(Player* player, short int blipNum);
    void setPlayerData_radarBlipLastPos(Player* player, short int blipNum, Vector blipLastPos);
    bool getPlayerData_radarBlipActive(Player* player, short int blipNum);
    void setPlayerData_radarBlipActive(Player* player, short int blipNum, bool blipActive);
    bool getPlayerData_radarSelectedActive(Player* player);
    void setPlayerData_radarSelectedActive(Player* player, bool selectedActive);
    float getPlayerData_radarUpdatedLast(Player* player);
    void setPlayerData_radarUpdatedLast(Player* player, float lastUpdate);
    float getPlayerData_radarAngleLast(Player* player);
    void setPlayerData_radarAngleLast(Player* player, float lastAngle);
    int getPlayerData_radarScale(Player* player);
    void setPlayerData_radarScale(Player* player, int radarScale);


    bool getPlayerData_coopClientIdDone(Player* player);
    void setPlayerData_coopClientIdDone(Player* player, bool state);
    bool getPlayerData_coopSetupDone(Player* player);
    void setPlayerData_coopSetupDone(Player* player, bool state);
    str getPlayerData_coopClass(Player* player);
    void setPlayerData_coopClass(Player* player, str className);
    str getPlayerData_coopClientId(Player* player);
    void setPlayerData_coopClientId(Player* player,str sClientId);
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
    bool getPlayerData_targetedShow(Player* player);
    void setPlayerData_targetedShow(Player* player, bool status);

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
