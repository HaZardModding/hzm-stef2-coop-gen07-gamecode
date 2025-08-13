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
		
		//DEBUG_LOG("# '%d' - '%s'\n", dialogTiming, dialogSegment.c_str());
	}
}

void CoopPlaydialog::handleDialog(Actor* actor, const char* dialog_name, qboolean headDisplay, float dialogLength)
{
	if (!headDisplay) {
		return;
	}

	str dialogTextDeu;
	for (int j = 1; j <= CoopPlaydialog_dialogListContainer_deu.NumObjects(); j++) {
		if (Q_stricmp(CoopPlaydialog_dialogListContainer_deu.ObjectAt(j).dialogPath.c_str(),dialog_name) == 0) {
			dialogTextDeu = CoopPlaydialog_dialogListContainer_deu.ObjectAt(j).dialogText;
		}
	}

	str _DEBUG_path = "";
	str dialogTextEng;
	for (int j = 1; j <= CoopPlaydialog_dialogListContainer_eng.NumObjects(); j++) {
		if (Q_stricmp(CoopPlaydialog_dialogListContainer_eng.ObjectAt(j).dialogPath.c_str(),dialog_name) == 0) {
			_DEBUG_path = CoopPlaydialog_dialogListContainer_eng.ObjectAt(j).dialogPath;

			dialogTextEng = CoopPlaydialog_dialogListContainer_eng.ObjectAt(j).dialogText;
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

	DEBUG_LOG("# handleDialog ------\n%s\n%s\n\n", _DEBUG_path.c_str(), dialogTextEng.c_str());

	//reset dialog time lock
	coopPlaydialog.currentDialogText_nextPrint = 0.0f;

	//update current dialog text - we assume that there is never two dialogs with text at the same time
	currentDialogTextEng.FreeObjectList();
	currentDialogTextDeu.FreeObjectList();

	//cut dialog into snippets and give each snippet a duration based on char length or snippet count
	currentDialogTextSnippetsEng.FreeObjectList();
	currentDialogTimeSnippetsEng.FreeObjectList();

	gamefix_listSeperatedItems(currentDialogTextSnippetsEng, dialogTextEng, ".");
	for(int i = 1; i <= currentDialogTextSnippetsEng.NumObjects(); i++) {
		currentDialogTimeSnippetsEng.AddObject(dialogLength); //make sure the object exists at all

		str currentSnippet = gamefix_trimWhitespace(currentDialogTextSnippetsEng.ObjectAt(i), false);
		if (currentSnippet.length() > 0) {
			//add the dot back in, it was removed during the split
			currentSnippet += ".";
			currentDialogTextSnippetsEng.ObjectAt(i) = currentSnippet;
			currentDialogTimeSnippetsEng.ObjectAt(i) = dialogLength / currentDialogTextSnippetsEng.NumObjects();
		}
	}

	currentDialogTextEng_containerPos = 1;
	currentDialogTextDeu_containerPos = 1;
}

void CoopPlaydialog::ActorThink(Actor *actor)
{
	//this can be used to update the dialog text - after a certain time, grabbing the next part of the dialog text and showing it to the player.
	if (actor->GetActorFlag(ACTOR_FLAG_DIALOG_PLAYING)) {
		if (currentDialogText_nextPrint > level.time) {
			return;
		}
		currentDialogText_nextPrint = 9999.0f;

		str dialogTextDeu = "";
		/*if (currentDialogTextDeu_containerPos <= currentDialogTextSnippetsDeu.NumObjects()) {
			dialogTextDeu += currentDialogTextSnippetsDeu.ObjectAt(currentDialogTextDeu_containerPos);
		}
		currentDialogTextDeu_containerPos++;*/

		str dialogTextEng = "";
		if (currentDialogTextEng_containerPos <= currentDialogTextSnippetsEng.NumObjects()) {
			currentDialogText_nextPrint = level.time + currentDialogTimeSnippetsEng.ObjectAt(currentDialogTextEng_containerPos);

			//grab last line of dialog text and add it infront of the current text
			if (currentDialogTextEng_containerPos > 1) {
				unsigned int oldStartPos = 0;
				if ((currentDialogTextEng_containerPos - 1) > 0 && (currentDialogTextEng_containerPos - 1) <= currentDialogTextSnippetsEng.NumObjects()) {
					str tempOldText = currentDialogTextSnippetsEng.ObjectAt(currentDialogTextEng_containerPos - 1);
					if (tempOldText.length() > _COOP_SETTINGS_HEADHUD_CHARS_LINE_ACCEPTABLE) {
						oldStartPos = (tempOldText.length() - 1) - _COOP_SETTINGS_HEADHUD_CHARS_LINE_ACCEPTABLE;
						dialogTextEng += gamefix_getStringLength(tempOldText, oldStartPos, _COOP_SETTINGS_HEADHUD_CHARS_LINE_ACCEPTABLE);
					}
					else {
						dialogTextEng += currentDialogTextSnippetsEng.ObjectAt(currentDialogTextEng_containerPos - 1);
					}

					if (dialogTextEng.length()) {
						dialogTextEng += "~"; //add a line break
					}
				}
				else {
					DEBUG_LOG("# ActorThink - CONTAINETR OUT OF RANGE\n");
				}
			}
		
			dialogTextEng += currentDialogTextSnippetsEng.ObjectAt(currentDialogTextEng_containerPos);
		}
		else {
			DEBUG_LOG("# [%d] dialog ActorThink out of bounds for %s\n", currentDialogTextEng_containerPos, dialogTextEng.c_str());
			return;
		}
		currentDialogTextEng_containerPos++;

		if (!dialogTextEng.length() && !dialogTextDeu.length()) {
			return;
		}

		//add the dot back in, it was removed during the split
		gamefix_trimWhitespace(dialogTextEng, false);
		gamefix_trimWhitespace(dialogTextDeu, false);

		dialogTextDeu = gamefix_replaceUmlautAndSpecials(dialogTextDeu);
		dialogTextEng = gamefix_replaceUmlautAndSpecials(dialogTextEng);

		coopPlaydialog.replaceForDialogText(dialogTextEng);
		coopPlaydialog.replaceForDialogText(dialogTextDeu);

		DEBUG_LOG("# [%d] dialog %s\n", coopPlaydialog.currentDialogTextEng_containerPos, dialogTextEng.c_str());

		Player* player = nullptr;
		for (int i = 0; i < gameFixAPI_maxClients(); i++) {
			player = gamefix_getPlayer(i);
			if (player) {
				if (player->coop_hasLanguageGerman()) {
					if (dialogTextDeu.length()) {
						gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand coop_dCns labeltext %s", dialogTextDeu.c_str()));
					}
				}
				else {
					if (dialogTextEng.length()) {
						gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand coop_dCns labeltext %s", dialogTextEng.c_str()));
					}
				}
			}
		}
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