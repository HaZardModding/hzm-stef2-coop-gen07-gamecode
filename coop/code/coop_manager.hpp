#pragma once

#ifdef ENABLE_COOP

#include "../../dlls/game/_pch_cpp.h"
#include "../../dlls/game/mp_manager.hpp"
#include "../../dlls/game/mp_modeBase.hpp"
#include "../../dlls/game/puzzleobject.hpp"
#include <qcommon/gameplaymanager.h>
#include "coop_config.hpp"
#include "coop_generalstrings.hpp"
#include "coop_playdialog.hpp"
#include "coop_circlemenu.hpp"
#include "coop_forcefield.hpp"

extern Event EV_Player_DeactivateWeapon;
extern Event EV_Player_ActivateNewWeapon;
extern Event EV_World_coop_configstrRemove;
extern Event EV_Trigger_GetLastActivatingEntity;

//windows/linux+other handle
#ifdef WIN32
#include <windows.h>
#define DEBUG_LOG(...) do { OutputDebugStringA(va(__VA_ARGS__)); } while(0)
#else
#define DEBUG_LOG(...) do { gi.Printf(va(__VA_ARGS__));} while (0)
#endif

struct coopManager_clientIniData_s
{
	str clientId = "";
	str mapName = "";
	int health = 0;
	int armor = 0;
	int phaser = 0;
	int plasma = 0;
	int fed = 0;
	int idryll = 0;
	int deathTime = 0;
	int lmsDeaths = 0;
};

struct coopManager_client_persistant_s
{
    float		coopSetupNextCheckTime = -999.0f;
    short		coopSetupTries = 0;
    bool		coopSetupStarted = false;
    bool		coopSetupDone = false;
    bool		coopClientIdDone = false;
    bool        objectiveSetupDone = false;

	float       entityTargetedSince = 0.0f;
	Entity*     entityTargetedLast = nullptr;
    
    bool        coopTricorderPuzzleing = false;
	bool        coopTricorderScanHudOn = false;
	float       coopTricorderScanLastSend = 0.0f;
	str         coopTricorderScanData1 = "";
	str         coopTricorderScanData2 = "";
	str         coopTricorderScanData3 = "";

    str			coopClass = "Technician";
    float       coopClassLastTimeApplied = -99.1f;
    float       coopClassLastTimeChanged = -98.2f;
    float       coopClassLastTimeUsedMsg = -97.3f;
	float       coopClasslastTimeUpdatedStat = -96.4f;
    int         coopClassRegenerationCycles = 0;
    bool		coopClassLocked = false;

	float       revivedStepLasttime = -799.0f;
	int         revivedStepCounter = 0;
    float       usingStepLasttime = 0.0f;

    str         coopClientId = "";
    int         coopVersion = 0;
    bool        coopAdmin = false;
    int         coopAdminAuthAttemps = 0;
    str         coopAdminAuthString = "";
    bool        coopAdminAuthStarted = false;
    int         coopAdminAuthStringLengthLast = 0;

	float       cinematicEscapePressLastTime = 0.0f;
    bool        coopUpdateNoticeSend = false;

    bool        targetedShow = false;
    int         targetedLastEntNum = -1;

    str         communicatorSendNames[_COOP_SETTINGS_PLAYER_SUPPORT_MAX]{ "", "", "", "", "", "", "", "" };
    bool        disconnecting = false;

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

    short       circleMenuActive = 0;
    float       circleMenuActivatingTime = 0.0f;
    Vector      circleMenuViewAngle = Vector(0, 0, 0);
    short       circleMenuNumOfSegments = CIRCLEMENU_MAX_OPTIONS;
    Vector      circleMenuLastViewAngle = Vector(0, 0, 0);
    Vector      circleMenuLongtimeViewangle = Vector(0, 0, 0);
    float       circleMenuLastThinkTime = 0.0f;
    float       circleMenuLastMessageTime = 0.0f;
    str         circleMenuLastWidget = "";
    int         circleMenuLastSegment = -1;
    str         circleMenuLastWeapon = "None";
    bool        circleMenuHoldingRightButton = false;
    bool        circleMenuHoldingLeftButton = false;
    str         circleMenuOptionIconLastSend[CIRCLEMENU_MAX_OPTIONS] = { "", "", "", "" };
    str         circleMenuOptionTextLastSend[CIRCLEMENU_MAX_OPTIONS] = { "", "", "", "" };

    str         circleMenuOptionThreadOrCommand[CIRCLEMENU_MAX_OPTIONS] = { "", "", "", "" };
    str         circleMenuOptionText[CIRCLEMENU_MAX_OPTIONS] = { "", "", "", "" };
    str         circleMenuOptionIcon[CIRCLEMENU_MAX_OPTIONS] = { "", "", "", "" };
    bool        circleMenuOptionIsScript[CIRCLEMENU_MAX_OPTIONS] = { false, false, false, false };
    int         circleMenuOptionAmmount[CIRCLEMENU_MAX_OPTIONS] = { 0, 0, 0, 0 };
    int         circleMenuOptionCost[CIRCLEMENU_MAX_OPTIONS] = { 0, 0, 0, 0 };
    str         circleMenuOptionCostType[CIRCLEMENU_MAX_OPTIONS] = { "none", "none", "none", "none" };
    str         circleMenuOptionDialogThread[CIRCLEMENU_MAX_OPTIONSDIALOG] = { "", "", "", "" };
    str         circleMenuOptionDialogText[CIRCLEMENU_MAX_OPTIONSDIALOG] = { "", "", "", "" };
    str         circleMenuOptionDialogIcon[CIRCLEMENU_MAX_OPTIONSDIALOG] = { "", "", "", "" };
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
    Container<coopManager_clientIniData_s> CoopManager_clientIniData;
    Container<str> CoopManager_configstringList;

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
    bool IsSameEnviroment(str levelCurrent, str levelOther);
    bool getSkippingCinematics();
    void setSkippingCinematics(bool skipping);
    float getskippingCinematicsLast();
    void setskippingCinematicsLast(float timeLast);
    void communicatorTransporterUiUpdate();

    void Init();
    void InitWorld();
    coopManager_mapSettings_s DetectMapType(str mapName);
    void SetMapType();
    void LoadLevelScript(str mapname);
    void Shutdown();
    void LevelStart(CThread* gamescript);
    void LevelEndCleanup(qboolean temp_restart);
    void MissionFailed(const str& reason);
    str MissionFailureConfigString(const str& reason);
    void MissionFailureLoadMap();

    str IncludeScriptReplace(str sLex);
    void IncludeScriptCheck(str &sLex);
    bool IncludedScriptCoop();

    str playerScriptCallValidateThreadname(str threadName);
    bool playerScriptCallExecute(Entity* entPlayer, str commandName, str threadName, Entity* entUsed);

    bool entityUservarGetKillMessage(Entity* inflictor,str &killmessageEng, str &killmessageDeu);
    bool entityUservarGetName(Entity* inflictor,str &killmessageEng, str &killmessageDeu);
    int entityUservarContains(Entity* ent, const str &find);
    void ActorThink(Actor* actor);
    void ClientThink(Player* player);
    void playerAdminThink(Player* player);
    void communicatorUpdateUi();

    bool callvoteManager(const str& _voteString);
    void callvoteUpdateUi(str sText, str sValue, str sWidget);
    void callvoteUpdateUiPlayer(Player* player, str sValue, str sWidget);
    bool callvoteSkipCinematicPlayer(Player* player);

    void puzzleObjectUsedstartthread(PuzzleObject* puzzle);

    MapFlags getMapFlags();
    float getSkillBasedDamage(float currentDamage);
    int getNumberOfPlayers(bool noDead, bool noSpectator);
    Entity* getSpawnSpecific(int spotNumber);

    void ActorDamage(Actor* actor, Entity* enemy, float& damage);

    void playerConnect(int clientNum);
    void playerDisconnect(Player* player);
    void playerJoined(Player* player);
    void playerSetup(Player* player);
    void playerSetupClId(Player* player);
    void playerSetupCoop(Player* player);
    void playerSetupCoopUi(Player* player);
    void playerCoopDetected(const gentity_t* ent, const char* coopVer);
    void playerClIdDetected(const gentity_t* ent, const char* clientId);
    void playerClIdSet(Player* player);
    void playerAddMissionHuds(Player* player);
    void playerRemoveMissionHuds(Player* player);
    void playerUpdateNoticeUi(Player* player);
    void playerTricorderScanUi(Player* player, bool);
    void playerTricorderScanUiHandle(Player* player, Entity* ent);

	bool playerDataReset(Player* player);
	bool playerDataSave(Player* player);
	bool playerDataRestore(Player* player);
    bool playerItemPickup(Entity* player, Item *item);

    void playerMoveToSpawn(Player* player);
    bool playerMoveToSpawnSpecific(Player* player, int spotNumber);
    void playerTargetnames(Player *player, Entity* viewTrace);

    void playerReset(Player* player);
    void playerEntered(gentity_t* ent);
    void playerSpawned(Player* player);
    void playerDied(Player* player);
    void playerLeft(Player* player);
    void playerTransported(Entity* entity);
    void playerBecameSpectator(Player* player);
    void playerChangedModel(Player* player);
    void playerChangedClass(Player* player);
    void playerKilledActor(Player* player, Actor* actor);
    bool playerDamagedCoop(Player* damagedPlayer, Damage& damage);
    void playerSharePickedUpAmmo(const Player* player, const str& itemName, const int& amount, int& amountUsed);
    bool playerOtherThanTargetingEntity(const Player* player, const Entity* entityTarget);
    bool playerHaveArchetypeEntityRespond(Player* player, Entity* entityTarget);
    bool playerGetDoingPuzzle(Player* player);
    void playerSetDoingPuzzle(Player* player,bool doingPuzzle);

    bool sentientHandleStasis(Sentient *attacked, Entity *attacker);

    void flushTikis();
    int configstringRemove(str sRem);
    void configstringCleanup();
	void loadClientIniData();
	void saveClientIniData();

    // Configurable systems
    bool ShouldGiveSpawnItems() const;
    bool IsFriendlyFireEnabled() const;
    int  GetPointsForEnemyKill() const;
    int  GetPenaltyForPlayerKill() const;

    // Access to coopManager_client_persistant_t
    bool getPlayerData_coopAdmin(Player* player);
    void setPlayerData_coopAdmin(Player* player, bool state);
    void setPlayerData_coopAdmin_reset(Player* player);
    int getPlayerData_coopSetupTries(Player* player);
    void setPlayerData_coopSetupTries_update(Player* player);
    void setPlayerData_coopSetupTries_reset(Player* player);
    float getPlayerData_coopSetupNextCheckTime(Player* player);
    void setPlayerData_coopSetupNextCheckTime_update(Player* player);
    void setPlayerData_coopSetupNextCheckTime_reset(Player* player);
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

 
    float getPlayerData_cinematicEscapePressLastTime(Player* player);
    void setPlayerData_cinematicEscapePressLastTime(Player* player,float lastTime);
    bool getPlayerData_coopUpdateNoticeSend(Player* player);
    void setPlayerData_coopUpdateNoticeSend(Player* player, bool state);


    bool getPlayerData_coopClientIdDone(Player* player);
    void setPlayerData_coopClientIdDone(Player* player, bool state);
    bool getPlayerData_coopSetupDone(Player* player);
    void setPlayerData_coopSetupDone(Player* player, bool state);
    bool getPlayerData_coopSetupStarted(Player* player);
    void setPlayerData_coopSetupStarted(Player* player, bool state);

    str getPlayerData_coopClass(Player* player);
    void setPlayerData_coopClass(Player* player, str className);
    int getPlayerData_coopClassRegenerationCycles(Player* player);
    void setPlayerData_coopClassRegenerationCycles(Player* player, int cycles);
    void setPlayerData_coopClassRegenerationCycles_update(Player* player);
    bool getPlayerData_coopClassLocked(Player* player);
    void setPlayerData_coopClassLocked(Player* player, bool status);
    float getPlayerData_coopClassLastTimeChanged(Player* player);
    void setPlayerData_coopClassLastTimeChanged(Player* player, float lastTime);
    void setPlayerData_coopClassLastTimeChanged_update(Player* player);
    float getPlayerData_coopClassLastTimeApplied(Player* player);
    void setPlayerData_coopClassLastTimeApplied(Player* player, float lastTime);
    float getPlayerData_coopClasslastTimeUpdatedStat(Player* player);
    void setPlayerData_coopClasslastTimeUpdatedStat(Player* player, float lastTime);
    float getPlayerData_coopClassLastTimeUsedMsg(Player* player);
    void setPlayerData_coopClassLastTimeUsedMsg(Player* player, float lastTime);
    void setPlayerData_coopClassLastTimeUsedMsg_update(Player* player);

    float getPlayerData_revivedStepLasttime(Player* player);
    void setPlayerData_revivedStepLasttime(Player* player, float lastTime);
    void setPlayerData_revivedStepLasttime_update(Player* player);
    int getPlayerData_revivedStepCounter(Player* player);
    void setPlayerData_revivedStepCounter(Player* player, int count);
    void setPlayerData_revivedStepCounter_update(Player* player);

    float getPlayerData_usingStepLasttime(Player* player);
    void setPlayerData_usingStepLasttime(Player* player, float last);
    void setPlayerData_usingStepLasttime_update(Player* player);

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

    void setPlayerData_communicatorSendNames_reset(Player *player);

    void setPlayerData_disconnecting(int clientNum, bool status);
    bool getPlayerData_disconnecting(int clientNum);

    void setPlayerData_entityTargetedSince_reset(Player* player);
    void setPlayerData_entityTargetedSince(Player* player, Entity* lastTarget);
    float getPlayerData_entityTargetedSince(Player* player, Entity* lastTarget);

    void setPlayerData_coopTricorderScanHudOn(Player* player, bool on);
    bool getPlayerData_coopTricorderScanHudOn(Player* player);
    void setPlayerData_coopTricorderScanData_reset(Player* player);
    void setPlayerData_coopTricorderScanData(Player* player, short int dataIndex, str scanData);
    str getPlayerData_coopTricorderScanData(Player* player, short int dataIndex);
    void setPlayerData_coopTricorderPuzzleing(Player* player, bool doingAPuzzle);
    bool getPlayerData_coopTricorderPuzzleing(Player* player);


private:
    CoopManager() = default;

    void CreateConfigstringList();
    void LoadPlayerModelsFromINI();
    void LoadMapListFromINI();

    MapFlags mapFlags;

    bool coopEnabled = false;
    bool rpgEnabled = false;

    float skippingCinematicsLast = -99.0f;
    bool skippingCinematics = false;

    bool friendlyFire = false;
    bool giveSpawnItems = false;

    int enemyKillPoints = 0;
    int playerKillPenalty = 0;
    
	bool communicatorTransporterUiUpdateCheck = true;

    // Possibly other future flags...
};

#endif