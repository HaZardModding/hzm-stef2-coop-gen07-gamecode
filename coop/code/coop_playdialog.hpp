#pragma once

#ifdef ENABLE_COOP

#include "../../dlls/game/_pch_cpp.h"
#include "../../dlls/game/mp_manager.hpp"
#include "../../dlls/game/mp_modeBase.hpp"
#include <qcommon/gameplaymanager.h>
#include "coop_config.hpp"
#include "coop_generalstrings.hpp"


struct CoopPlaydialog_dialogData {
	int dialogTiming = 60;
	str dialogPath = "";
	str dialogText;
};

extern Container<CoopPlaydialog_dialogData> CoopPlaydialog_dialogListContainer_eng;
extern Container<CoopPlaydialog_dialogData> CoopPlaydialog_dialogListContainer_deu;

class CoopPlaydialog {
public:
	void readDialogFile(str pathName, str levelName, Container<CoopPlaydialog_dialogData>& data);
	void handleDialog(Actor* actor, const char* dialog_name, qboolean headDisplay, float dialogLength);
	void ActorThink(Actor* actor);
	void stopDialog();
	void replaceForDialogText(str& sPure);
public:
	float currentDialogText_nextPrint = 0;
	int currentDialogTextEng_containerPos = 1;
	int currentDialogTextDeu_containerPos = 1;
	Container<str> currentDialogTextEng;
	Container<str> currentDialogTextDeu;
};
extern CoopPlaydialog coopPlaydialog;

#endif