#include "../../dlls/game/gamefix.hpp"
#include "coop_config.hpp"

CoopSettings coopSettings;
Container<CoopSettings_clientThreads_s> CoopSettings_playerScriptThreadsAllowList;


void CoopSettings::playerCommandsAllow()
{
	//client console commands that will not be flood filtered
	gameFixAPI_clCmdsWhitheListAdd("coopinstalled");
	gameFixAPI_clCmdsWhitheListAdd("coopcid");
	gameFixAPI_clCmdsWhitheListAdd("coopinput");
	gameFixAPI_clCmdsWhitheListAdd("coopradarscale");
	gameFixAPI_clCmdsWhitheListAdd("!thread");
	gameFixAPI_clCmdsWhitheListAdd("!testspawn");
	gameFixAPI_clCmdsWhitheListAdd("!follow");
	gameFixAPI_clCmdsWhitheListAdd("!leader");
	gameFixAPI_clCmdsWhitheListAdd("!login");
	gameFixAPI_clCmdsWhitheListAdd("!logout");
	gameFixAPI_clCmdsWhitheListAdd("!kill");
	gameFixAPI_clCmdsWhitheListAdd("!origin");
	gameFixAPI_clCmdsWhitheListAdd("!noclip");
	gameFixAPI_clCmdsWhitheListAdd("!stuck");
	gameFixAPI_clCmdsWhitheListAdd("!transport");
	gameFixAPI_clCmdsWhitheListAdd("!notransport");
	gameFixAPI_clCmdsWhitheListAdd("!showspawn");
	gameFixAPI_clCmdsWhitheListAdd("!transferlife");
	//gameFixAPI_clCmdsWhitheListAdd("!ability");
	gameFixAPI_clCmdsWhitheListAdd("!targetnames");
	gameFixAPI_clCmdsWhitheListAdd("!levelend");
	gameFixAPI_clCmdsWhitheListAdd("!drop");
	gameFixAPI_clCmdsWhitheListAdd("!skill");
	gameFixAPI_clCmdsWhitheListAdd("!info");
	gameFixAPI_clCmdsWhitheListAdd("!block");
	gameFixAPI_clCmdsWhitheListAdd("!mapname");
	gameFixAPI_clCmdsWhitheListAdd("!class");

	gameFixAPI_clCmdsWhitheListAdd("dialogrunthread");
}
void CoopSettings::playerScriptThreadsAllow()
{
	//Allowed script threads in coop multiplayer
	CoopSettings_clientThreads_s trirteClick;
	trirteClick.command = "clientthread";
	trirteClick.thread = "trirteClick";
	CoopSettings_playerScriptThreadsAllowList.AddObject(trirteClick);

	CoopSettings_clientThreads_s exitRoutine;
	exitRoutine.command = "clientthread";
	exitRoutine.thread = "trirteClick";
	CoopSettings_playerScriptThreadsAllowList.AddObject(exitRoutine);

	CoopSettings_clientThreads_s tricorderMod_;
	tricorderMod_.command = "clientthread";
	tricorderMod_.thread = "tricorderMod_";
	CoopSettings_playerScriptThreadsAllowList.AddObject(tricorderMod_);

	CoopSettings_clientThreads_s tricorderKeypad_;
	tricorderKeypad_.command = "clientthread";
	tricorderKeypad_.thread = "tricorderKeypad_";
	CoopSettings_playerScriptThreadsAllowList.AddObject(tricorderKeypad_);

	CoopSettings_clientThreads_s useLibraryTerminal;
	tricorderKeypad_.command = "clientrunthread";
	tricorderKeypad_.thread = "useLibraryTerminal";
	CoopSettings_playerScriptThreadsAllowList.AddObject(useLibraryTerminal);

	CoopSettings_clientThreads_s tricorderBaseCancel;
	tricorderKeypad_.command = "clientrunthread";
	tricorderKeypad_.thread = "tricorderBaseCancel";
	CoopSettings_playerScriptThreadsAllowList.AddObject(tricorderBaseCancel);

	CoopSettings_clientThreads_s trirteClick_2;
	trirteClick_2.command = "ServerThreadToRun";
	trirteClick_2.thread = "trirteClick";
	CoopSettings_playerScriptThreadsAllowList.AddObject(trirteClick_2);

	CoopSettings_clientThreads_s globalTricorder;
	globalTricorder.command = "script";
	globalTricorder.command2 = "thread";
	globalTricorder.thread = "trirteClick";
	CoopSettings_playerScriptThreadsAllowList.AddObject(globalTricorder);

	CoopSettings_clientThreads_s trirteTT;
	trirteTT.command = "script";
	trirteTT.command2 = "thread";
	trirteTT.thread = "trirteTT";
	CoopSettings_playerScriptThreadsAllowList.AddObject(trirteTT);

	CoopSettings_clientThreads_s _tricorderRoute_;
	_tricorderRoute_.command = "script";
	_tricorderRoute_.command2 = "thread";
	_tricorderRoute_.thread = "_tricorderRoute_";
	CoopSettings_playerScriptThreadsAllowList.AddObject(_tricorderRoute_);

	CoopSettings_clientThreads_s _tricorderBase_;
	_tricorderBase_.command = "script";
	_tricorderBase_.command2 = "thread";
	_tricorderBase_.thread = "_tricorderBase_";
	CoopSettings_playerScriptThreadsAllowList.AddObject(_tricorderBase_);

	CoopSettings_clientThreads_s useLibraryTerminal_2;
	useLibraryTerminal_2.command = "script";
	useLibraryTerminal_2.command2 = "thread";
	useLibraryTerminal_2.thread = "useLibraryTerminal";
	CoopSettings_playerScriptThreadsAllowList.AddObject(useLibraryTerminal_2);
}

void CoopSettings::LoadSettingsFromFile(const str& iniFilePath) {
	try {
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
	catch (const char* error) {
		gi.Printf(_COOP_ERROR_fatal, error);
		G_ExitWithError(error);
	}
}
