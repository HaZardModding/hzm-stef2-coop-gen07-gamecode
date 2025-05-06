#include "../../dlls/game/gamefix.hpp"
#include "coop_config.hpp"


void CoopSettings::LoadSettingsFromFile(const str& iniFilePath) {
	str contents;
	if (!gamefix_getFileContents(iniFilePath, contents, true)) {
		gi.Printf(_COOP_WARNING_FILE_failed, iniFilePath.c_str());
		return;
	}

	str sTemp;
	str section_contents;

	// Boot section - settings usually managed during first load of dll and game init - requires server reboot
	section_contents = gamefix_iniSectionGet(iniFilePath, contents, _COOP_SETTINGS_CAT_boot);
	coopEnabled = gamefix_iniKeyGet(iniFilePath, section_contents, "coop_enabled", "false") == "true";
	rpgEnabled = gamefix_iniKeyGet(iniFilePath, section_contents, "rpg_enabled", "false") == "true";

	// Level section - settings usually managed each map load
	section_contents = gamefix_iniSectionGet(iniFilePath, contents, _COOP_SETTINGS_CAT_level);
	levelPrefixCoop = gamefix_iniKeyGet(iniFilePath, section_contents, "autoDetect_level_prefix_coop", "coop_");
	levelPrefixRpg = gamefix_iniKeyGet(iniFilePath, section_contents, "autoDetect_level_prefix_rpg", "rpg_");

	// Vote section - vote system related settings
	section_contents = gamefix_iniSectionGet(iniFilePath, contents, _COOP_SETTINGS_CAT_vote);
	voteRefundOnSuccess = gamefix_iniKeyGet(iniFilePath, section_contents, "voteRefundOnSuccsess", "false") == "true";
	sTemp = gamefix_iniKeyGet(iniFilePath, section_contents, "voteAllowedCommands", "");
	gamefix_listSeperatedItems(voteAllowedCommands, sTemp, " "); // using space as separator

	// Gameplay section - settings usually handled on the fly
	section_contents = gamefix_iniSectionGet(iniFilePath, contents, _COOP_SETTINGS_CAT_gameplay);
	rpgSpawnWeapons = gamefix_iniKeyGet(iniFilePath, section_contents, "rpg_spawnWeapons", "false") == "true";
	coopLastManStandingLifes = atoi(gamefix_iniKeyGet(iniFilePath, section_contents, "coop_lastManStandingLifes", "0"));

	// Score section - settings usually handled on the fly
	section_contents = gamefix_iniSectionGet(iniFilePath, contents, _COOP_SETTINGS_CAT_score);
	scoreKilledPlayer = atoi(gamefix_iniKeyGet(iniFilePath, section_contents, "killedPlayer", "-10"));
	scoreKilledFriendly = atoi(gamefix_iniKeyGet(iniFilePath, section_contents, "killedFriendly", "-10"));
	scoreKilledEnemy = atoi(gamefix_iniKeyGet(iniFilePath, section_contents, "killedEnemy", "1"));
	scoreKilledBoss = atoi(gamefix_iniKeyGet(iniFilePath, section_contents, "killedBoss", "10"));
}
