#pragma once
#ifdef ENABLE_COOP

#include "../../dlls/game/_pch_cpp.h"
#include "coop_generalstrings.hpp"

constexpr auto _COOP_THIS_VERSION = 70200;
constexpr auto _COOP_CLIENT_MINIMUM_COMPATIBELE_VERSION = 70200;

constexpr auto _COOP_SCRIPT_main = "globalCoop_main";

constexpr auto _COOP_FILE_main = "co-op/matrix/main.scr";
constexpr auto _COOP_FILE_noscript = "noscript.scr";
constexpr auto _COOP_FILE_multioptions = "multioptions.scr";
constexpr auto _COOP_FILE_multioptions4 = "mom4_coopInput.scr";
constexpr auto _COOP_FILE_mrm = "mom_mrm.scr";

constexpr auto _COOP_FILE_maplist = "co-op/config/maplist.ini";
constexpr auto _COOP_FILE_settings = "co-op/config/settings.ini";
constexpr auto _COOP_FILE_userlist = "co-op/config/userlist.ini";
constexpr auto _COOP_FILE_validPlayerModels = "co-op/config/skinlist.ini";
constexpr auto _COOP_FILE_voteList = "co-op/config/votelist.ini";
constexpr auto _COOP_FILE_scorelist = "co-op/config/scorelist.ini";
constexpr auto _COOP_FILE_deathlist = "co-op/config/deathlist.ini";
constexpr auto _COOP_FILE_spawninventory = "co-op/config/spawninventory.cfg";

constexpr auto _COOP_UI_NAME_communicator = "coop_com";

constexpr auto _COOP_INI_CAT_default = "categories";

constexpr auto _COOP_USERLIST_CAT_session = "session";

constexpr auto _COOP_MAPLIST_CAT_coopcompatible = "coopCompatible";
constexpr auto _COOP_MAPLIST_CAT_included = "coopIncluded";
constexpr auto _COOP_MAPLIST_CAT_coopSpMission = "singlePlayerMission";
constexpr auto _COOP_MAPLIST_CAT_coopSpIgm = "singlePlayerIgm";
constexpr auto _COOP_MAPLIST_CAT_coopSpSecret = "singlePlayerSecret";
constexpr auto _COOP_MAPLIST_CAT_multiplayerOnly = "multiPlayer";
constexpr auto _COOP_MAPLIST_CAT_singleplayerOnly = "singlePlayer";

constexpr auto _COOP_SETTINGS_CAT_boot = "boot";
constexpr auto _COOP_SETTINGS_CAT_level = "level";
constexpr auto _COOP_SETTINGS_CAT_vote = "vote";
constexpr auto _COOP_SETTINGS_CAT_gameplay = "gameplay";
constexpr auto _COOP_SETTINGS_CAT_missionstatus = "missionstatus";

constexpr auto _COOP_SETTINGS_server_configname = "server";

constexpr auto _COOP_SETTINGS_ADMIN_LOGIN_TRIES = 5;
constexpr auto _COOP_SETTINGS_ADMIN_LOGIN_AUTH_STRING_LENGTH_MIN = 3;
constexpr auto _COOP_SETTINGS_ADMIN_LOGIN_AUTH_STRING_LENGTH_MAX = 10;
constexpr auto _COOP_SETTINGS_SETUP_TRIES = 15;
constexpr auto _COOP_SETTINGS_MISSION_HUDS_MAX = 8;
constexpr auto _COOP_SETTINGS_OBJECTIVES_MAX = 8;
constexpr auto _COOP_SETTINGS_WEAPONS_MAX = 11;
constexpr auto _COOP_SETTINGS_RADAR_BLIPS_MAX = 9;
constexpr auto _COOP_SETTINGS_RADAR_BLIPS_OBJ_MAX = (_COOP_SETTINGS_RADAR_BLIPS_MAX - 1);
constexpr auto _COOP_SETTINGS_RADAR_CIRCLE_START = 10;
constexpr auto _COOP_SETTINGS_RADAR_HUD_REAL_WIDTH = 768;
constexpr auto _COOP_SETTINGS_RADAR_HUD_SCALE_WIDTH = 128;
constexpr auto _COOP_SETTINGS_RADAR_BLIP_SIZE = 30;
constexpr auto _COOP_SETTINGS_RADAR_MAX_RADIUS = 55;
constexpr auto _COOP_SETTINGS_RADAR_TIMECYCLE = 0.05f; //used to be 0.1
constexpr auto _COOP_SETTINGS_RADAR_SCALE_FACTOR = 25000; //used to be 40000
constexpr auto _COOP_SETTINGS_RADAR_SCALE_MIN = 1;
constexpr auto _COOP_SETTINGS_RADAR_SCALE_MAX = 6;
constexpr auto _COOP_SETTINGS_HEADHUD_CHARS_LINE_ACCEPTABLE = 50;
constexpr auto _COOP_SETTINGS_HEADHUD_LINES_ACCEPTABLE = 3;
constexpr auto _COOP_SETTINGS_PLAYER_SUPPORT_MAX = 8;
constexpr auto _COOP_SETTINGS_PLAYER_SPAWNSPOT_MAX = _COOP_SETTINGS_PLAYER_SUPPORT_MAX;
constexpr auto _COOP_SETTINGS_PLAYER_WEAPON_RANGE_THROW = 300.0f;
constexpr auto _COOP_SETTINGS_PLAYER_WEAPON_RANGE_DROW = 100.0f;
constexpr auto _COOP_SETTINGS_PLAYER_WEAPON_DROP_REMOVE = 120.0f;
constexpr auto _COOP_SETTINGS_PLAYER_BLOCK_NOTSOLID_TIME = 7.0f;
constexpr auto _COOP_SETTINGS_PLAYER_PENALTY_BADKILL = 4;
constexpr auto _COOP_SETTINGS_DIALOG_FAILSAFE_TIMEOUT = 45.0f;

constexpr auto _COOP_SETTINGS_MOVEMENT_SPEED_DEFAULT = 400;
constexpr auto _COOP_SETTINGS_DAMAGE_MULTIPLAYER_DEFAULT = 0.25f;
constexpr auto _COOP_SETTINGS_KNOCKBACK_NPC_OBJECTS = 0.75f;
constexpr auto _COOP_SETTINGS_KNOCKBACK_PROJECTILE = 0.35f;
constexpr auto _COOP_SETTINGS_KNOCKBACK_PLAYER = 0.85f;

constexpr auto _COOP_SETTINGS_PLAYER_ITEM_ARMOR_MAX = 200;

constexpr auto _COOP_CLASS_HW_AMMO_FED_MAX = 500;
constexpr auto _COOP_CLASS_HW_AMMO_PLASMA_MAX = 500;
constexpr auto _COOP_CLASS_HW_AMMO_IDRYLL_MAX = 500;

const str _COOP_SETTINGS_FORBIDDEN_FILES_INI_READ[] = { "co-op/config/settings.ini" };
const size_t _COOP_SETTINGS_FORBIDDEN_FILES_INI_READ_NUM = sizeof(_COOP_SETTINGS_FORBIDDEN_FILES_INI_READ) / sizeof(_COOP_SETTINGS_FORBIDDEN_FILES_INI_READ[0]);

const str _COOP_SETTINGS_FORBIDDEN_FILES_INI_WRITE[]{_COOP_FILE_settings,_COOP_FILE_maplist,_COOP_FILE_validPlayerModels,_COOP_FILE_scorelist };
const size_t _COOP_SETTINGS_FORBIDDEN_FILES_INI_WRITE_NUM = sizeof(_COOP_SETTINGS_FORBIDDEN_FILES_INI_WRITE) / sizeof(_COOP_SETTINGS_FORBIDDEN_FILES_INI_WRITE[0]);

struct CoopSettings_deathmessage_s {
	str type = "";
	str name = "";
	str text = "";
};
extern Container<CoopSettings_deathmessage_s> CoopSettings_deathList;

struct CoopSettings_killScore_s {
	str type = "";
	str name = "";
	int points = 1;
};
extern Container<CoopSettings_killScore_s> CoopSettings_scoreKillList;

struct CoopSettings_clientThreads_s {
	bool adminRequired = false;
	str command = "";
	str command2 = "";
	str thread = "";
	str item = "";
	float distanceLimit = 0.0f;
	str type = "";
	bool activatingPlayerRequired = false;
};
extern Container<CoopSettings_clientThreads_s> CoopSettings_playerScriptThreadsAllowList;

class CoopSettings {
public:
	bool coopEnabled = false;
	bool rpgEnabled = false;

	str levelPrefixCoop = "";
	str levelPrefixRpg = "";

	Container<str> voteAllowedCommands; // space-separated string
	bool voteRefundOnSuccess = false;

	bool rpgSpawnWeapons = false;
	int  coopLastManStandingLifes = 0;

	float friendlyFireMultiplicator = _COOP_SETTINGS_DAMAGE_MULTIPLAYER_DEFAULT;
	int moveSpeed = _COOP_SETTINGS_MOVEMENT_SPEED_DEFAULT;
	bool awards = false;
	short int difficulty = 0;
	short int airaccelerate = 2;

	int scoreKilledPlayer = 0;
	int scoreKilledFriendly = 0;
	int scoreKilledEnemy = 0;
	int scoreKilledBoss = 0;

	int getSetting_awards();
	int setSetting_awards(bool newValue);
	int getSetting_maxSpeed();
	int setSetting_maxSpeed(int newValue);
	float getSetting_friendlyFireMultiplicator();
	float setSetting_friendlyFireMultiplicator(float newValue);
	int getSetting_difficulty();
	int setSetting_difficulty(int newValue);
	int getSetting_airaccelerate();
	int setSetting_airaccelerate(int newValue);

	void serverConfigCheck();
	void playerCommandsAllow();
	void playerScriptThreadsAllow();
	void saveSettings();
	void loadSettings();
	void loadScoreList();
	void loadDeathList();
};
extern CoopSettings coopSettings;

#endif