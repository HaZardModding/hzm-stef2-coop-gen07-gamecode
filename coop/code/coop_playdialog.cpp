#ifdef ENABLE_COOP

#include "coop_playdialog.hpp"
#include "../../dlls/game/gamefix.hpp"
#include "../../dlls/game/level.h"
#include "../../dlls/game/mp_manager.hpp"
#include "../../dlls/game/actor.h"
#include "coop_manager.hpp"


CoopPlaydialog coopPlaydialog;
Container<CoopPlaydialog_dialogData> CoopPlaydialog_dialogListContainer_eng;
Container<CoopPlaydialog_dialogData> CoopPlaydialog_dialogListContainer_deu;


void CoopPlaydialog::readDialogFile(str pathName, str levelName, Container<CoopPlaydialog_dialogData>& data)
{
	str fileContents = "";
	str currentDialogFile = va("%s%s.dlg", pathName.c_str(), levelName.c_str());
	if (!gamefix_getFileContents(currentDialogFile,fileContents,false)) {
		return;
	}

	if (!fileContents.length()) {
		DEBUG_LOG("CoopPlaydialog::readDialogFile - Empty file: %s\n", currentDialogFile.c_str());
		return;
	}

	Container<str> dialogLines;
	gamefix_listSeperatedItems(dialogLines, fileContents,"\n\r");
	fileContents = "";

	for (int i = dialogLines.NumObjects(); i > 0; i--) {
		int dialogTiming = 0;
		str dialogSegment;
		str textSegment;
		str numberSegment = "";
		str currentLine = gamefix_trimWhitespace(dialogLines.ObjectAt(i),false);
		int fileExtensionPos = gamefix_findString(currentLine.c_str(),".mp3",false);
		
		if (fileExtensionPos < 0) {
			continue;
		}

		dialogSegment = gamefix_getStringUntil(currentLine, 0, fileExtensionPos + 4);
		textSegment = gamefix_trimWhitespace(gamefix_getStringUntil(currentLine, fileExtensionPos + 4, currentLine.length()),true);
		
		str temp_numberSegment = "";
		for (int x = 0; x < dialogSegment.length();x++) {
			temp_numberSegment = dialogSegment[x];
			if (temp_numberSegment.isNumeric()) {
				numberSegment += temp_numberSegment;
			}
			else {
				break;
			}
		}
		if (numberSegment.length()) {
			dialogSegment = gamefix_trimWhitespace(gamefix_getStringUntil(currentLine, numberSegment.length(), fileExtensionPos + 4),true);
			dialogTiming = atoi(numberSegment.c_str());
		}
		
		CoopPlaydialog_dialogData tempLineData;
		if (dialogTiming > 0) {
			tempLineData.dialogTiming = dialogTiming;
		}
		tempLineData.dialogPath = dialogSegment;
		tempLineData.dialogText = textSegment;

		data.AddObject(tempLineData);
		
		//DEBUG_LOG("# [%d] t:%d - %s\n%s\n", i, dialogTiming, dialogSegment.c_str(), textSegment.c_str());
	}
}

void CoopPlaydialog::handleDialog(Actor* actor, const char* dialog_name, qboolean headDisplay, float dialogLength)
{
	if (!headDisplay) {
		return;
	}

	currentDialogTextEng_containerPos = 1;
	currentDialogTextDeu_containerPos = 1;
	coopPlaydialog.currentDialogText_nextPrint = 0.0f;
	currentDialogTextEng.FreeObjectList();
	currentDialogTextDeu.FreeObjectList();
	currentDialogTextSnippetsEng.FreeObjectList();
	currentDialogTimeSnippetsEng.FreeObjectList();
	world->CancelEventsOfType(EV_World_coop_playDialogShowTextTextline);

	str dialogTextDeu;
	for (int j = 1; j <= CoopPlaydialog_dialogListContainer_deu.NumObjects(); j++) {
		if (Q_stricmp(CoopPlaydialog_dialogListContainer_deu.ObjectAt(j).dialogPath.c_str(),dialog_name) == 0) {
			dialogTextDeu = CoopPlaydialog_dialogListContainer_deu.ObjectAt(j).dialogText;
			//DEBUG_LOG("# DEU [%d] %s %s\n", containerDebugPos, CoopPlaydialog_dialogListContainer_deu.ObjectAt(j).dialogPath.c_str(), dialogTextDeu.c_str());
		}
	}
	str dialogTextEng;
	for (int j = 1; j <= CoopPlaydialog_dialogListContainer_eng.NumObjects(); j++) {
		if (Q_stricmp(CoopPlaydialog_dialogListContainer_eng.ObjectAt(j).dialogPath.c_str(),dialog_name) == 0) {
			dialogTextEng = CoopPlaydialog_dialogListContainer_eng.ObjectAt(j).dialogText;
			//DEBUG_LOG("# ENG [%d] %s %s\n", containerDebugPos, CoopPlaydialog_dialogListContainer_eng.ObjectAt(j).dialogPath.c_str(), dialogTextEng.c_str());
		}
	}

	if (!dialogTextDeu.length()) {
		dialogTextDeu = dialogTextEng;
	}
	if (!dialogTextEng.length()) {
		dialogTextEng = dialogTextDeu;
	}

	if (!dialogTextEng.length() && !dialogTextDeu.length()) {
		return;
	}

	//cut dialog into snippets and give each snippet a duration based on char length or snippet count
	gamefix_listSeperatedItems(currentDialogTextSnippetsEng, dialogTextEng, ".");
	gamefix_listSeperatedItems(currentDialogTextSnippetsDeu, dialogTextDeu, ".");

	splitTextIntoLines("Eng",currentDialogTextSnippetsEng, currentDialogTimeSnippetsEng, dialogLength, dialogTextEng.length());
	//DEBUG_LOG("# handleDialog: %f * %d = %f/%f - %s", dialogLengthActual, currentDialogTextSnippetsEng.NumObjects(), dialogLengthActual * currentDialogTextSnippetsEng.NumObjects(), dialogLength, dialogText.c_str());
	splitTextIntoLines("Deu",currentDialogTextSnippetsDeu, currentDialogTimeSnippetsDeu, dialogLength, dialogTextDeu.length());

	showNextTextLine("Eng", currentDialogTextSnippetsEng, currentDialogTextEng_containerPos);
	showNextTextLine("Deu", currentDialogTextSnippetsDeu, currentDialogTextDeu_containerPos);
}

void CoopPlaydialog::showNextTextLine(const str &language, Container<str> &currentDialogTextSnippets, int &currentDialogContainerPos)
{
	str dialogText = "";
	if (currentDialogContainerPos <= currentDialogTextSnippets.NumObjects()) {
		//grab last line of dialog text and add it infront of the current text
		if (currentDialogContainerPos > 1) {
			unsigned int oldStartPos = 0;
			if ((currentDialogContainerPos - 1) > 0 && (currentDialogContainerPos - 1) <= currentDialogTextSnippets.NumObjects()) {
				str tempOldText = currentDialogTextSnippets.ObjectAt(currentDialogContainerPos - 1);
				if (tempOldText.length() > _COOP_SETTINGS_HEADHUD_CHARS_LINE_ACCEPTABLE) {
					const char* oldTextPtr = tempOldText.c_str();
					unsigned oldTextCutEndAt = _COOP_SETTINGS_HEADHUD_CHARS_LINE_ACCEPTABLE - (tempOldText.length() - 1);
					unsigned oldTextCutStartingFrom = (tempOldText.length() - 1);
					oldStartPos = gamefix_findCharsReverse(oldTextPtr, " ", oldTextCutEndAt, oldTextCutStartingFrom);
					dialogText += gamefix_getStringLength(tempOldText, oldStartPos, _COOP_SETTINGS_HEADHUD_CHARS_LINE_ACCEPTABLE);
				}
				else {
					//if we are dealing with short text, like a countdown, we can add a third line
					if ((currentDialogContainerPos - 2) > 0 && (currentDialogContainerPos - 2) <= currentDialogTextSnippets.NumObjects()) {
						str tempOldText_2 = currentDialogTextSnippets.ObjectAt(currentDialogContainerPos - 2);
						if (tempOldText_2.length() < _COOP_SETTINGS_HEADHUD_CHARS_LINE_ACCEPTABLE) {
							dialogText += va("%s~",tempOldText_2.c_str());
						}
					}

					dialogText += currentDialogTextSnippets.ObjectAt(currentDialogContainerPos - 1);
				}

				if (dialogText.length()) {
					dialogText += "~"; //add a line break
				}
			}
			else {
				DEBUG_LOG("# CoopPlaydialog::showNextTextLine - CONTAINETR OUT OF RANGE\n");
			}
		}
		
		dialogText += currentDialogTextSnippets.ObjectAt(currentDialogContainerPos);
	}
	else {
		DEBUG_LOG("# [%d] CoopPlaydialog::showNextTextLine bounds for %s\n", currentDialogContainerPos, dialogText.c_str());
		return;
	}
	
	currentDialogContainerPos++;

	if (!dialogText.length()) {
		return;
	}

	dialogText = gamefix_replaceUmlautAndSpecials(dialogText);
	coopPlaydialog.replaceForDialogText(dialogText);

	//DEBUG_LOG("# showNextTextLine p[%d] l[%s] %s\n", currentDialogContainerPos, language.c_str(), dialogText.c_str());

	Player* player = nullptr;
	for (int i = 0; i < gameFixAPI_maxClients(); i++) {
		player = gamefix_getPlayer(i);
		if (player) {
			if (gameFixAPI_getLanguage(player) == language) {
				gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand coop_dCns labeltext %s", dialogText.c_str()));
			}
		}
	}
}

void CoopPlaydialog::splitTextIntoLines(const str language, Container<str>& containerText, Container<float>& containerLength, const float& dialogLength, const int &totalLength)
{
	float dialogLengthActual = dialogLength;
	for (int i = 1; i <= containerText.NumObjects(); i++) {
		str currentSnippet = gamefix_trimWhitespace(containerText.ObjectAt(i), false);

		//add the dot back in, it was removed during the split
		if (currentSnippet[currentSnippet.length() - 1] != '?' &&
			currentSnippet[currentSnippet.length() - 1] != '!' &&
			currentSnippet[currentSnippet.length() - 1] != '-' &&
			currentSnippet[currentSnippet.length() - 1] != '.') {
			currentSnippet += ".";
		}

		containerText.ObjectAt(i) = currentSnippet;
		if (containerText.NumObjects() > 1) {
			
			//Total dialog playtime devided by total num of chars multiplied by number of chars
			//providing a good estimate for how long each line should be visible for
			dialogLengthActual = (dialogLength / totalLength) * currentSnippet.length();

			//Total dialog playtime devided by number of lines
			//dialogLengthActual = dialogLength / containerText.NumObjects();

			float eventFireTime = 0;
			for (int f = i; f > 0; f--) {
				eventFireTime += dialogLengthActual;
			}
			
			//make sure the next line is automatically triggered
			Event* event = new Event(EV_World_coop_playDialogShowTextTextline);
			event->AddString(language.c_str());
			world->PostEvent(event, eventFireTime - 0.1);
		}

		//DEBUG_LOG("# tc[%f] tt[%f] %s\n", dialogLengthActual, dialogLength, currentSnippet.c_str());

		containerLength.AddObject(dialogLengthActual); //make sure the object exists at all
	}
}

void CoopPlaydialog::stopDialog()
{
	Player* player = nullptr;
	for (int i = 0; i < gameFixAPI_maxClients(); i++) {
		player = gamefix_getPlayer(i);
		if (player) {
			gamefix_playerDelayedServerCommand(player->entnum, "globalwidgetcommand coop_dCns clear");
		}
	}
	currentDialogTextEng.FreeObjectList();
	currentDialogTextDeu.FreeObjectList();
}

void CoopPlaydialog::replaceForDialogText(str& sPure)
{
	int len = sPure.length();
	if (len == 0)
		return;

	// Find the last '.' in the string
	int lastDotPos = -1;
	for (int i = 0; i < len; i++)
	{
		if (sPure[i] == '.')
			lastDotPos = i;
	}

	str result = "";

	for (int i = 0; i < len; i++)
	{
		unsigned char ch = (unsigned char)sPure[i];

		if (ch == 0xB4 || ch == 0x92) // ´ or ’
			result += "'";
		else if (ch == '\n' || ch == '#')
			result += "~";
		else if (ch == ' ')
			result += "^";
		/*
		else if (ch == '!')
			result += "!~";
		else if (ch == '?')
			result += "?~";
		else if (ch == '.' && i == lastDotPos)
			result += ".~"; // only last dot gets replaced
			*/
		else
			result += sPure[i];
	}

	sPure = result;
}

#endif