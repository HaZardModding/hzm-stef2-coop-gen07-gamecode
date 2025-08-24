#ifdef ENABLE_COOP

#include "../../dlls/game/gamefix.hpp"
#include "coop_manager.hpp"
#include "coop_config.hpp"

CoopSettings coopSettings;
Container<CoopSettings_clientThreads_s> CoopSettings_playerScriptThreadsAllowList;
Container<CoopSettings_killScore_s> CoopSettings_scoreKillList;
Container<CoopSettings_deathmessage_s> CoopSettings_deathList;

int CoopSettings::getSetting_awards()
{
	return awards;
}

int CoopSettings::setSetting_awards(bool newValue)
{
	awards = newValue;
	return awards;
}

int CoopSettings::getSetting_maxSpeed()
{
	return moveSpeed;
}

int CoopSettings::setSetting_maxSpeed(int newValue)
{
	if (newValue < 200) {
		newValue = 200;
	}
	if (newValue > 600) {
		newValue = 600;
	}
	moveSpeed = newValue;
	return moveSpeed;
}

float CoopSettings::getSetting_friendlyFireMultiplicator()
{
	return friendlyFireMultiplicator;
}

float CoopSettings::setSetting_friendlyFireMultiplicator(float newValue)
{
	if (newValue < -1) {
		newValue = -1;
	}
	if (newValue > 2) {
		newValue = 2;
	}
	friendlyFireMultiplicator = newValue;
	return friendlyFireMultiplicator;
}

int CoopSettings::getSetting_difficulty()
{
	return difficulty;
}

int CoopSettings::setSetting_difficulty(int newValue)
{
	if (newValue < 0) {
		newValue = 0;
	}
	if (newValue > 3) {
		newValue = 3;
	}
	difficulty = newValue;
	return difficulty;
}

int CoopSettings::getSetting_airaccelerate()
{
	return airaccelerate;
}

int CoopSettings::setSetting_airaccelerate(int newValue)
{
	if (newValue < 0) {
		newValue = 0;
	}
	if (newValue > 4) {
		newValue = 3;
	}
	airaccelerate = newValue;
	return airaccelerate;
}

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
	gameFixAPI_clCmdsWhitheListAdd("!ability");
	gameFixAPI_clCmdsWhitheListAdd("!circle");
	gameFixAPI_clCmdsWhitheListAdd("!targetnames");
	gameFixAPI_clCmdsWhitheListAdd("!levelend");
	gameFixAPI_clCmdsWhitheListAdd("!drop");
	gameFixAPI_clCmdsWhitheListAdd("!skill");
	gameFixAPI_clCmdsWhitheListAdd("!info");
	gameFixAPI_clCmdsWhitheListAdd("!block");
	gameFixAPI_clCmdsWhitheListAdd("!mapname");
	gameFixAPI_clCmdsWhitheListAdd("!class");
	gameFixAPI_clCmdsWhitheListAdd("!help");

	gameFixAPI_clCmdsWhitheListAdd("coopinstalled");
	gameFixAPI_clCmdsWhitheListAdd("coopcid");
	gameFixAPI_clCmdsWhitheListAdd("coopinput");
	gameFixAPI_clCmdsWhitheListAdd("coopradarscale");
	gameFixAPI_clCmdsWhitheListAdd("clientrunthread");
	gameFixAPI_clCmdsWhitheListAdd("dialogrunthread");

	gameFixAPI_clCmdsWhitheListAdd("script");
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

void CoopSettings::saveSettings() {
	//gameplay options
	//gameplay options
	//gameplay options
	str fileContents;
	str newFileContents;
	gamefix_getFileContents(_COOP_FILE_settings, fileContents, true);
	str sectionContents = gamefix_iniSectionGet(_COOP_FILE_settings, fileContents, _COOP_SETTINGS_CAT_gameplay);

	str newSectionContents;
	newSectionContents = gamefix_iniKeySet(_COOP_FILE_settings, sectionContents, "friendlyFireMultiplicator", va("%.2f", coopSettings.getSetting_friendlyFireMultiplicator()));
	newSectionContents = gamefix_iniKeySet(_COOP_FILE_settings, newSectionContents, "moveSpeed", va("%d",coopSettings.getSetting_maxSpeed()));
	newSectionContents = gamefix_iniKeySet(_COOP_FILE_settings, newSectionContents, "difficulty", va("%d",coopSettings.getSetting_difficulty()));
	newSectionContents = gamefix_iniKeySet(_COOP_FILE_settings, newSectionContents, "airaccelerate", va("%d", coopSettings.getSetting_airaccelerate()));
	
	str awardsString = "false";
	if (coopSettings.getSetting_awards()) {
		awardsString = "true";
	}
	newSectionContents = gamefix_iniKeySet(_COOP_FILE_settings, newSectionContents, "awards", awardsString);
	// Update section in full file content
	newFileContents = gamefix_iniSectionSet(_COOP_FILE_settings, fileContents, _COOP_SETTINGS_CAT_gameplay, newSectionContents);

	//missionstatus data
	//missionstatus data
	//missionstatus data
	sectionContents = gamefix_iniSectionGet(_COOP_FILE_settings, fileContents, _COOP_SETTINGS_CAT_missionstatus);
	newSectionContents = "";
	if (CoopManager::Get().getMapFlags().stockMap && CoopManager::Get().getMapFlags().coopSpMission ||
		CoopManager::Get().getMapFlags().stockMap && CoopManager::Get().getMapFlags().coopSpIgm) {
		ScriptVariable* var = gameVars.GetVariable("globalMissionEnterprise");
		newSectionContents += va("globalMissionEnterprise=%d\n", var ? (int)var->floatValue() : 1);

		newSectionContents += va("globalTurboliftRide=%d\n", (int)gamefix_getGameVarFloat("globalTurboliftRide"));
		newSectionContents += va("globalKleeyaChoice=%d\n", (int)gamefix_getGameVarFloat("globalKleeyaChoice"));
		newSectionContents += va("globalTelsiaChoice=%d\n", (int)gamefix_getGameVarFloat("globalTelsiaChoice"));
		newSectionContents += va("globalNoneChoice=%d\n", (int)gamefix_getGameVarFloat("globalNoneChoice"));
		newSectionContents += va("attrexianWeaponFound=%d\n", (int)gamefix_getGameVarFloat("attrexianWeaponFound"));
		newSectionContents += va("secretWeapon_CompressionRifle=%d\n", (int)gamefix_getGameVarFloat("secretWeapon_CompressionRifle"));
		newSectionContents += va("secretWeapon_IMOD=%d\n", (int)gamefix_getGameVarFloat("secretWeapon_IMOD"));
		newSectionContents += va("secretWeapon_IdryllStaff=%d\n", (int)gamefix_getGameVarFloat("secretWeapon_IdryllStaff"));
		newSectionContents += va("secretWeapon_RomulanExperimental=%d\n", (int)gamefix_getGameVarFloat("secretWeapon_RomulanExperimental"));
		newSectionContents += va("igmRoomsVisited=%d\n", (int)gamefix_getGameVarFloat("igmRoomsVisited"));
		newSectionContents += va("igmHolodeckSpawn=%d\n", (int)gamefix_getGameVarFloat("igmHolodeckSpawn"));
		newSectionContents += va("igmTurboliftSpawn=%d\n", (int)gamefix_getGameVarFloat("igmTurboliftSpawn"));
		newSectionContents += va("statusM5L2CUnlocked=%d\n", (int)gamefix_getGameVarFloat("statusM5L2CUnlocked"));

		// Update section in full file content
		newFileContents = gamefix_iniSectionSet(_COOP_FILE_settings, fileContents, _COOP_SETTINGS_CAT_missionstatus, newSectionContents);
	}

	//finally save ini file
	if (!gamefix_setFileContents(_COOP_FILE_settings, newFileContents)) {
		gi.Printf("CoopSettings::saveSettings() - Could not write: %s\n", _COOP_FILE_settings);
		DEBUG_LOG("CoopSettings::saveSettings() - Could not write: %s", _COOP_FILE_settings);
	}
}


void CoopSettings::loadSettings() {
	try {
		str contents;
		if (!gamefix_getFileContents(_COOP_FILE_settings, contents, true)) {
			gi.Printf(_COOP_WARNING_FILE_failed, _COOP_FILE_settings);
			return;
		}

		str sTemp;
		str sectionContents;


		if (gameFixAPI_inMultiplayer()) {

			//LOAD gameplay settings
			sectionContents = gamefix_iniSectionGet(_COOP_FILE_settings, contents, _COOP_SETTINGS_CAT_gameplay);
			if (sectionContents.length()) {
				friendlyFireMultiplicator = setSetting_friendlyFireMultiplicator(atof(gamefix_iniKeyGet(_COOP_FILE_settings, sectionContents, "friendlyFireMultiplicator", "0.0")));
				moveSpeed = setSetting_maxSpeed(atoi(gamefix_iniKeyGet(_COOP_FILE_settings, sectionContents, "moveSpeed", "300")));
				awards = setSetting_awards(gamefix_iniKeyGet(_COOP_FILE_settings, sectionContents, "awards", "false") == "true");
				difficulty = setSetting_difficulty(atoi(gamefix_iniKeyGet(_COOP_FILE_settings, sectionContents, "difficulty", "1")));
				airaccelerate = setSetting_airaccelerate(atoi(gamefix_iniKeyGet(_COOP_FILE_settings, sectionContents, "airaccelerate", "1")));
				skill->integer = difficulty;
				sv_airaccelerate->integer = airaccelerate;
				world->setPhysicsVar("maxspeed", moveSpeed);
			}

			//LOAD missionstatus data
			sectionContents = gamefix_iniSectionGet(_COOP_FILE_settings, contents, _COOP_SETTINGS_CAT_missionstatus);
			if (CoopManager::Get().getMapFlags().stockMap && CoopManager::Get().getMapFlags().coopSpMission ||
				CoopManager::Get().getMapFlags().stockMap && CoopManager::Get().getMapFlags().coopSpIgm) {
				gameVars.SetVariable("globalMissionEnterprise", (float)atof(gamefix_iniKeyGet(_COOP_FILE_settings, sectionContents, "globalMissionEnterprise", "1")));
				gameVars.SetVariable("globalTurboliftRide", (float)atof(gamefix_iniKeyGet(_COOP_FILE_settings, sectionContents, "globalTurboliftRide", "0")));
				gameVars.SetVariable("globalKleeyaChoice", (float)atof(gamefix_iniKeyGet(_COOP_FILE_settings, sectionContents, "globalKleeyaChoice", "0")));
				gameVars.SetVariable("globalTelsiaChoice", (float)atof(gamefix_iniKeyGet(_COOP_FILE_settings, sectionContents, "globalTelsiaChoice", "0")));
				gameVars.SetVariable("globalNoneChoice", (float)atof(gamefix_iniKeyGet(_COOP_FILE_settings, sectionContents, "globalNoneChoice", "0")));
				gameVars.SetVariable("attrexianWeaponFound", (float)atof(gamefix_iniKeyGet(_COOP_FILE_settings, sectionContents, "attrexianWeaponFound", "0")));
				gameVars.SetVariable("secretWeapon_CompressionRifle", (float)atof(gamefix_iniKeyGet(_COOP_FILE_settings, sectionContents, "secretWeapon_CompressionRifle", "0")));
				gameVars.SetVariable("secretWeapon_IMOD", (float)atof(gamefix_iniKeyGet(_COOP_FILE_settings, sectionContents, "secretWeapon_IMOD", "0")));
				gameVars.SetVariable("secretWeapon_IdryllStaff", (float)atof(gamefix_iniKeyGet(_COOP_FILE_settings, sectionContents, "secretWeapon_IdryllStaff", "0")));
				gameVars.SetVariable("secretWeapon_RomulanExperimental", (float)atof(gamefix_iniKeyGet(_COOP_FILE_settings, sectionContents, "secretWeapon_RomulanExperimental", "0")));
				gameVars.SetVariable("igmRoomsVisited", (float)atof(gamefix_iniKeyGet(_COOP_FILE_settings, sectionContents, "igmRoomsVisited", "0")));
				gameVars.SetVariable("igmHolodeckSpawn", (float)atof(gamefix_iniKeyGet(_COOP_FILE_settings, sectionContents, "igmHolodeckSpawn", "0")));
				gameVars.SetVariable("igmTurboliftSpawn", (float)atof(gamefix_iniKeyGet(_COOP_FILE_settings, sectionContents, "igmTurboliftSpawn", "0")));
				gameVars.SetVariable("statusM5L2CUnlocked", (float)atof(gamefix_iniKeyGet(_COOP_FILE_settings, sectionContents, "statusM5L2CUnlocked", "0")));
			}
		}

		//lets review that some other time
		if ( 1 ) {
			return;
		}

		// Boot section - settings usually managed during first load of dll and game init - requires server reboot
		sectionContents = gamefix_iniSectionGet(_COOP_FILE_settings, contents, _COOP_SETTINGS_CAT_boot);
		coopEnabled = gamefix_iniKeyGet(_COOP_FILE_settings, sectionContents, "coop_enabled", "false") == "true";
		rpgEnabled = gamefix_iniKeyGet(_COOP_FILE_settings, sectionContents, "rpg_enabled", "false") == "true";

		// Gameplay section - settings usually handled on the fly
		sectionContents = gamefix_iniSectionGet(_COOP_FILE_settings, contents, _COOP_SETTINGS_CAT_gameplay);
		rpgSpawnWeapons = gamefix_iniKeyGet(_COOP_FILE_settings, sectionContents, "rpg_spawnWeapons", "false") == "true";
		coopLastManStandingLifes = atoi(gamefix_iniKeyGet(_COOP_FILE_settings, sectionContents, "coop_lastManStandingLifes", "0"));
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
		if (!gamefix_getFileContents(_COOP_FILE_scorelist, contents, true)) {
			gi.Printf(_COOP_WARNING_FILE_failed, _COOP_FILE_scorelist);
			return;
		}

		str key = "";
		str value = "";
		str lineValue;
		Container<str> tempLinesContainer;

		section_contents = gamefix_iniSectionGet(_COOP_FILE_scorelist, contents, "actornames");
		gamefix_listSeperatedItems(tempLinesContainer, section_contents, "\n");

		for (int i = 1; i <= tempLinesContainer.NumObjects();i++) {
			lineValue = tempLinesContainer.ObjectAt(i);
			if (!lineValue.length()) {
				continue;
			}

			//extract - key and value
			if (gamefix_iniExtractKeyAndValueFromLine(_COOP_FILE_scorelist, lineValue, key, value)) {
				CoopSettings_killScore_s addActorNameKillScore;
				addActorNameKillScore.type = "actornames";
				addActorNameKillScore.name = key;
				if (value.length()) {
					int setpoints = atoi(value.c_str());
					addActorNameKillScore.points = setpoints;
				}
				CoopSettings_scoreKillList.AddObject(addActorNameKillScore);
			}
		}

		tempLinesContainer.FreeObjectList();
		section_contents = gamefix_iniSectionGet(_COOP_FILE_scorelist, contents, "targetnames");
		gamefix_listSeperatedItems(tempLinesContainer, section_contents, "\n");

		for (int i = 1; i <= tempLinesContainer.NumObjects(); i++) {
			lineValue = tempLinesContainer.ObjectAt(i);
			if (!lineValue.length()) {
				continue;
			}

			//extract - key and value
			if (gamefix_iniExtractKeyAndValueFromLine(_COOP_FILE_scorelist, lineValue, key, value)) {
				CoopSettings_killScore_s addActorNameKillScore;
				addActorNameKillScore.type = "targetnames";
				addActorNameKillScore.name = key;
				if (value.length()) {
					int setpoints = atoi(value.c_str());
					addActorNameKillScore.points = setpoints;
				}
				CoopSettings_scoreKillList.AddObject(addActorNameKillScore);
			}
		}
	}
	catch (const char* error) {
		gi.Printf(_COOP_ERROR_fatal, error);
		G_ExitWithError(error);
	}
}

void CoopSettings::loadDeathList() {
	try {
		str mapName = level.mapname.c_str();
		mapName = mapName.tolower();
		
		str contents = "";
		str section_contents = "";

		// Deathlist - Message to print if player gets killed
		if (!gamefix_getFileContents(_COOP_FILE_deathlist, contents, true)) {
			gi.Printf(_COOP_WARNING_FILE_failed, _COOP_FILE_deathlist);
			return;
		}

		//actornames - specific to current level
		str key = "";
		str value = "";
		str lineValue = "";
		Container<str> tempLinesContainer;
		section_contents = gamefix_iniSectionGet(_COOP_FILE_scorelist, contents, va("actornames@%s", mapName.c_str()));
		gamefix_listSeperatedItems(tempLinesContainer, section_contents, "\n");

		for (int i = 1; i <= tempLinesContainer.NumObjects(); i++) {
			lineValue = tempLinesContainer.ObjectAt(i);
			if (!lineValue.length()) {
				continue;
			}

			//extract - key and value
			if (gamefix_iniExtractKeyAndValueFromLine(_COOP_FILE_scorelist, lineValue, key, value)) {
				CoopSettings_deathmessage_s deathMessage;
				deathMessage.type = "actornames";
				deathMessage.name = key;
				if (value.length()) {
					deathMessage.text = value;
				}
				CoopSettings_deathList.AddObject(deathMessage);
			}
		}

		//targetnames
		tempLinesContainer.FreeObjectList();
		section_contents = gamefix_iniSectionGet(_COOP_FILE_scorelist, contents, "targetnames");
		gamefix_listSeperatedItems(tempLinesContainer, section_contents, "\n");

		for (int i = 1; i <= tempLinesContainer.NumObjects(); i++) {
			lineValue = tempLinesContainer.ObjectAt(i);
			if (!lineValue.length()) {
				continue;
			}

			//extract - key and value
			if (gamefix_iniExtractKeyAndValueFromLine(_COOP_FILE_scorelist, lineValue, key, value)) {
				CoopSettings_deathmessage_s deathMessage;
				deathMessage.type = "targetnames";
				deathMessage.name = key;
				if (value.length()) {
					deathMessage.text = value;
				}
				CoopSettings_deathList.AddObject(deathMessage);
			}
		}

		//models
		tempLinesContainer.FreeObjectList();
		section_contents = gamefix_iniSectionGet(_COOP_FILE_scorelist, contents, "models");
		gamefix_listSeperatedItems(tempLinesContainer, section_contents, "\n");

		for (int i = 1; i <= tempLinesContainer.NumObjects(); i++) {
			lineValue = tempLinesContainer.ObjectAt(i);
			if (!lineValue.length()) {
				continue;
			}

			//extract - key and value
			if (gamefix_iniExtractKeyAndValueFromLine(_COOP_FILE_scorelist, lineValue, key, value)) {
				CoopSettings_deathmessage_s deathMessage;
				deathMessage.type = "models";
				deathMessage.name = key;
				if (value.length()) {
					deathMessage.text = value;
				}
				CoopSettings_deathList.AddObject(deathMessage);
			}
		}

		//actornames
		tempLinesContainer.FreeObjectList();
		section_contents = gamefix_iniSectionGet(_COOP_FILE_scorelist, contents, "actornames");
		gamefix_listSeperatedItems(tempLinesContainer, section_contents, "\n");

		for (int i = 1; i <= tempLinesContainer.NumObjects(); i++) {
			lineValue = tempLinesContainer.ObjectAt(i);
			if (!lineValue.length()) {
				continue;
			}

			//extract - key and value
			if (gamefix_iniExtractKeyAndValueFromLine(_COOP_FILE_scorelist, lineValue, key, value)) {
				CoopSettings_deathmessage_s deathMessage;
				deathMessage.type = "actornames";
				deathMessage.name = key;
				if (value.length()) {
					deathMessage.text = value;
				}
				CoopSettings_deathList.AddObject(deathMessage);
			}
		}

		//classes
		tempLinesContainer.FreeObjectList();
		section_contents = gamefix_iniSectionGet(_COOP_FILE_scorelist, contents, "classes");
		gamefix_listSeperatedItems(tempLinesContainer, section_contents, "\n");

		for (int i = 1; i <= tempLinesContainer.NumObjects(); i++) {
			lineValue = tempLinesContainer.ObjectAt(i);
			if (!lineValue.length()) {
				continue;
			}

			//extract - key and value
			if (gamefix_iniExtractKeyAndValueFromLine(_COOP_FILE_scorelist, lineValue, key, value)) {
				CoopSettings_deathmessage_s deathMessage;
				deathMessage.type = "classes";
				deathMessage.name = key;
				if (value.length()) {
					deathMessage.text = value;
				}
				CoopSettings_deathList.AddObject(deathMessage);
			}
		}
	}
	catch (const char* error) {
		gi.Printf(_COOP_ERROR_fatal, error);
		G_ExitWithError(error);
	}
}

#endif