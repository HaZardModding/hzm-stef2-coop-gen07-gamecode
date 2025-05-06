#pragma once
#include "../../dlls/game/_pch_cpp.h"
#include "coop_generalstrings.hpp"


constexpr auto _COOP_FILE_maplist = "coop/config/maplist.ini";
constexpr auto _COOP_FILE_settings = "coop/config/settings.ini";

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
