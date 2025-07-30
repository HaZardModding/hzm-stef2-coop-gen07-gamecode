#include "../../dlls/game/gamefix.hpp"
#include "coop_config.hpp"

CoopSettings coopSettings;
Container<CoopSettings_clientThreads_s> CoopSettings_playerScriptThreadsAllowList;
Container<CoopSettings_killScoreActornames_s> CoopSettings_scoreKillActornameList;
Container<CoopSettings_killScoreActornames_s> CoopSettings_scoreKillTargetnameList;


void CoopSettings::serverConfigCheck()
{
	//make sure dedicated server is not using the same config as the player on windows
	if ( gameFixAPI_inSingleplayer() || !gameFixAPI_isWindowsServer() || gameFixAPI_isDedicatedServer()) {
		return;
	}

	str userName = gamefix_getCvar("username");
	if (Q_stricmp(userName.c_str(), gamefix_getCvar("config").c_str()) == 0 && Q_stricmp(userName.c_str(), "server") != 0) {
		gi.Printf(va(_COOP_INFO_INIT_server_config, userName.c_str(), _COOP_SETTINGS_server_configname));
		gi.cvar_set("config", _COOP_SETTINGS_server_configname);
	}
}

void CoopSettings::playerCommandsAllow()
{
	//client console commands that will not be flood filtered
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

	gameFixAPI_clCmdsWhitheListAdd("coopinstalled");
	gameFixAPI_clCmdsWhitheListAdd("coopcid");
	gameFixAPI_clCmdsWhitheListAdd("coopinput");
	gameFixAPI_clCmdsWhitheListAdd("coopradarscale");
	gameFixAPI_clCmdsWhitheListAdd("clientrunthread");
	gameFixAPI_clCmdsWhitheListAdd("dialogrunthread");
}
void CoopSettings::playerScriptThreadsAllow()
{
	//Allowed script threads in coop multiplayer
	CoopSettings_clientThreads_s trirteClick;
	trirteClick.command = "clientrunthread";
	trirteClick.thread = "trirteClick";
	CoopSettings_playerScriptThreadsAllowList.AddObject(trirteClick);

	CoopSettings_clientThreads_s exitRoutine;
	exitRoutine.command = "clientrunthread";
	exitRoutine.thread = "exitRoutine";
	CoopSettings_playerScriptThreadsAllowList.AddObject(exitRoutine);

	CoopSettings_clientThreads_s tricorderMod_;
	tricorderMod_.command = "clientrunthread";
	tricorderMod_.thread = "tricorderMod_";
	CoopSettings_playerScriptThreadsAllowList.AddObject(tricorderMod_);

	CoopSettings_clientThreads_s tricorderKeypad_;
	tricorderKeypad_.command = "clientrunthread";
	tricorderKeypad_.thread = "tricorderKeypad_";
	CoopSettings_playerScriptThreadsAllowList.AddObject(tricorderKeypad_);

	CoopSettings_clientThreads_s useLibraryTerminal;
	useLibraryTerminal.command = "clientrunthread";
	useLibraryTerminal.thread = "useLibraryTerminal";
	CoopSettings_playerScriptThreadsAllowList.AddObject(useLibraryTerminal);

	CoopSettings_clientThreads_s tricorderBaseCancel;
	tricorderBaseCancel.command = "clientrunthread";
	tricorderBaseCancel.thread = "tricorderBaseCancel";
	CoopSettings_playerScriptThreadsAllowList.AddObject(tricorderBaseCancel);

	CoopSettings_clientThreads_s trirteClick_2;
	trirteClick_2.command = "ServerThreadToRun";
	trirteClick_2.thread = "trirteClick";
	CoopSettings_playerScriptThreadsAllowList.AddObject(trirteClick_2);

	CoopSettings_clientThreads_s globalTricorder1;
	globalTricorder1.command = "script";
	globalTricorder1.command2 = "thread";
	globalTricorder1.thread = "globalTricorder_TT";
	CoopSettings_playerScriptThreadsAllowList.AddObject(globalTricorder1);

	CoopSettings_clientThreads_s trirteClick_3;
	trirteClick_3.command = "script";
	trirteClick_3.command2 = "thread";
	trirteClick_3.thread = "trirteClick";
	CoopSettings_playerScriptThreadsAllowList.AddObject(trirteClick_3);


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

	CoopSettings_clientThreads_s dialogrunthread_Choice;
	dialogrunthread_Choice.command = "dialogrunthread";
	dialogrunthread_Choice.thread = "Choice";
	CoopSettings_playerScriptThreadsAllowList.AddObject(dialogrunthread_Choice);

	CoopSettings_clientThreads_s dialogrunthread__DialogChoice;
	dialogrunthread__DialogChoice.command = "dialogrunthread";
	dialogrunthread__DialogChoice.thread = "_DialogChoice";
	CoopSettings_playerScriptThreadsAllowList.AddObject(dialogrunthread__DialogChoice);

	CoopSettings_clientThreads_s dialogrunthread_Option;
	dialogrunthread_Option.command = "dialogrunthread";
	dialogrunthread_Option.thread = "Option";
	CoopSettings_playerScriptThreadsAllowList.AddObject(dialogrunthread_Option);

	CoopSettings_clientThreads_s dialogrunthread_cinematicArm;
	dialogrunthread_cinematicArm.command = "dialogrunthread";
	dialogrunthread_cinematicArm.thread = "cinematicArm";
	CoopSettings_playerScriptThreadsAllowList.AddObject(dialogrunthread_cinematicArm);

	CoopSettings_clientThreads_s dialogrunthread_failedBranch;
	dialogrunthread_failedBranch.command = "dialogrunthread";
	dialogrunthread_failedBranch.thread = "failedBranch";
	CoopSettings_playerScriptThreadsAllowList.AddObject(dialogrunthread_failedBranch);

	CoopSettings_clientThreads_s dialogrunthread_successBranch;
	dialogrunthread_successBranch.command = "dialogrunthread";
	dialogrunthread_successBranch.thread = "successBranch";
	CoopSettings_playerScriptThreadsAllowList.AddObject(dialogrunthread_successBranch);

	CoopSettings_clientThreads_s dialogrunthread_entDeck7cIGM;
	dialogrunthread_entDeck7cIGM.command = "dialogrunthread";
	dialogrunthread_entDeck7cIGM.thread = "entDeck7cIGM";
	CoopSettings_playerScriptThreadsAllowList.AddObject(dialogrunthread_entDeck7cIGM);

	CoopSettings_clientThreads_s dialogrunthread_IGM7_DialogChoice;
	dialogrunthread_IGM7_DialogChoice.command = "dialogrunthread";
	dialogrunthread_IGM7_DialogChoice.thread = "IGM7_DialogChoice";
	CoopSettings_playerScriptThreadsAllowList.AddObject(dialogrunthread_IGM7_DialogChoice);

	CoopSettings_clientThreads_s dialogrunthread_entDeck1IGM;
	dialogrunthread_entDeck1IGM.command = "dialogrunthread";
	dialogrunthread_entDeck1IGM.thread = "entDeck1IGM";
	CoopSettings_playerScriptThreadsAllowList.AddObject(dialogrunthread_entDeck1IGM);

	CoopSettings_clientThreads_s dialogrunthread_entDeck8IGM;
	dialogrunthread_entDeck8IGM.command = "dialogrunthread";
	dialogrunthread_entDeck8IGM.thread = "entDeck8IGM";
	CoopSettings_playerScriptThreadsAllowList.AddObject(dialogrunthread_entDeck8IGM);

	CoopSettings_clientThreads_s dialogrunthread_careerOption;
	dialogrunthread_careerOption.command = "dialogrunthread";
	dialogrunthread_careerOption.thread = "careerOption";
	CoopSettings_playerScriptThreadsAllowList.AddObject(dialogrunthread_careerOption);
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
	}
	catch (const char* error) {
		gi.Printf(_COOP_ERROR_fatal, error);
		G_ExitWithError(error);
	}
}

void CoopSettings::loadScoreList() {
	try {
		str contents = "";
		str section_contents = "";

		// Score - points to give and take
		if (!gamefix_getFileContents(_COOP_FILE_score, contents, true)) {
			gi.Printf(_COOP_WARNING_FILE_failed, _COOP_FILE_score);
			return;
		}

		str key = "";
		str value = "";
		str lineValue;
		Container<str> tempLinesContainer;

		section_contents = gamefix_iniSectionGet(_COOP_FILE_score, contents, _COOP_SCORELIST_CAT_actornames);
		gamefix_listSeperatedItems(tempLinesContainer, section_contents, "\n");

		for (int i = 1; i <= tempLinesContainer.NumObjects();i++) {
			lineValue = tempLinesContainer.ObjectAt(i);
			if (!lineValue.length()) {
				continue;
			}

			//extract - key and value
			if (gamefix_iniExtractKeyAndValueFromLine(_COOP_FILE_score, lineValue, key, value)) {
				CoopSettings_killScoreActornames_s addActorNameKillScore;
				addActorNameKillScore.name = key;
				if (value.length()) {
					int setpoints = atoi(value.c_str());
					addActorNameKillScore.points = setpoints;
				}
				CoopSettings_scoreKillActornameList.AddObject(addActorNameKillScore);
			}
		}

		tempLinesContainer.FreeObjectList();
		section_contents = gamefix_iniSectionGet(_COOP_FILE_score, contents, _COOP_SCORELIST_CAT_targetnames);
		gamefix_listSeperatedItems(tempLinesContainer, section_contents, "\n");

		for (int i = 1; i <= tempLinesContainer.NumObjects(); i++) {
			lineValue = tempLinesContainer.ObjectAt(i);
			if (!lineValue.length()) {
				continue;
			}

			//extract - key and value
			if (gamefix_iniExtractKeyAndValueFromLine(_COOP_FILE_score, lineValue, key, value)) {
				CoopSettings_killScoreActornames_s addActorNameKillScore;
				addActorNameKillScore.name = key;
				if (value.length()) {
					int setpoints = atoi(value.c_str());
					addActorNameKillScore.points = setpoints;
				}
				CoopSettings_scoreKillTargetnameList.AddObject(addActorNameKillScore);
			}
		}
		tempLinesContainer.FreeObjectList();

		return;
	}
	catch (const char* error) {
		gi.Printf(_COOP_ERROR_fatal, error);
		G_ExitWithError(error);
	}
}
