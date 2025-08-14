#pragma once

#ifdef ENABLE_COOP

#include "../../dlls/game/_pch_cpp.h"
#include "../../dlls/game/mp_manager.hpp"
#include "../../dlls/game/mp_modeBase.hpp"
#include <qcommon/gameplaymanager.h>
#include "coop_config.hpp"
#include "coop_generalstrings.hpp"


struct CoopPlaydialog_dialogDataPlaying {
	//allow dialog text to be splitup into 64 parts, totally unrealistic but safe
	//long dialog with countdowns ?
	float textDuration[64] = {0};
	str dialogText[64] = {""};
};

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
	void showNextTextLine(const str &language);
	void splitTextIntoLines(const str language, Container<str>& containerText, Container<float>& containerLength, const float& dialogLength, float& dialogLengthActual);
	void stopDialog();
	void replaceForDialogText(str& sPure);
public:
	float currentDialogText_nextPrint = 0;
	int currentDialogTextEng_containerPos = 1;
	int currentDialogTextDeu_containerPos = 1;
	Container<CoopPlaydialog_dialogData> currentDialogTextEng;
	Container<CoopPlaydialog_dialogData> currentDialogTextDeu;

	int containerDebugPos = 0; //debugging variable to see which dialog text is currently used

	Container <str> currentDialogTextSnippetsEng;
	Container <float> currentDialogTimeSnippetsEng;
	Container <str> currentDialogTextSnippetsDeu;
	Container <float> currentDialogTimeSnippetsDeu;
};
extern CoopPlaydialog coopPlaydialog;

#endif