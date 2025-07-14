#pragma once
#include "../../dlls/game/_pch_cpp.h"
#include "coop_generalstrings.hpp"


constexpr auto _COOP_FILE_main = "co-op/matrix/main.scr";
constexpr auto _COOP_FILE_noscript = "noscript.scr";
constexpr auto _COOP_FILE_multioptions = "multioptions.scr";
constexpr auto _COOP_FILE_multioptions4 = "mom4_coopInput.scr";
constexpr auto _COOP_FILE_mrm = "mom_mrm.scr";

constexpr auto _COOP_FILE_maplist = "co-op/config/maplist.ini";
constexpr auto _COOP_FILE_settings = "co-op/config/settings.ini";
constexpr auto _COOP_FILE_validPlayerModels = "co-op/config/playerskins.ini";
constexpr auto _COOP_FILE_spawninventory = "co-op/config/spawninventory.cfg";

constexpr auto _COOP_INI_CAT_default = "categories";

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
constexpr auto _COOP_SETTINGS_CAT_score = "score";


constexpr auto _COOP_SCRIPT_main = "globalCoop_main";

constexpr auto _COOP_CLIENT_MINIMUM_COMPATIBELE_VERSION = 70000;

constexpr auto _COOP_SETTINGS_SETUP_TRIES = 15;
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

const str _COOP_SETTINGS_FORBIDDEN_FILES_INI_READ[] = { "co-op/config/settings.ini" };
const size_t _COOP_SETTINGS_FORBIDDEN_FILES_INI_READ_NUM = sizeof(_COOP_SETTINGS_FORBIDDEN_FILES_INI_READ) / sizeof(_COOP_SETTINGS_FORBIDDEN_FILES_INI_READ[0]);

const str _COOP_SETTINGS_FORBIDDEN_FILES_INI_WRITE[]{ "co-op/config/settings.ini","co-op/config/maplist.ini","co-op/config/playerskins.ini","co-op/config/maplist.ini" };
const size_t _COOP_SETTINGS_FORBIDDEN_FILES_INI_WRITE_NUM = sizeof(_COOP_SETTINGS_FORBIDDEN_FILES_INI_WRITE) / sizeof(_COOP_SETTINGS_FORBIDDEN_FILES_INI_WRITE[0]);



class CoopSettings {
public:
	bool coopEnabled;
	bool rpgEnabled;

	str levelPrefixCoop;
	str levelPrefixRpg;

	Container<str> voteAllowedCommands; // space-separated string
	bool voteRefundOnSuccess;

	bool rpgSpawnWeapons;
	int  coopLastManStandingLifes;

	int scoreKilledPlayer;
	int scoreKilledFriendly;
	int scoreKilledEnemy;
	int scoreKilledBoss;

	void LoadSettingsFromFile(const str& iniFilePath = _COOP_FILE_settings);
};
