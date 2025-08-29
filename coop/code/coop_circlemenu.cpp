#include "../../dlls/game/_pch_cpp.h"
#include "../../dlls/game/mp_manager.hpp"
#include "../../dlls/game/gamefix.hpp"
#include "coop_manager.hpp"
#include "coop_class.hpp"
#include "coop_config.hpp"
#include "coop_generalstrings.hpp"
#include "coop_circlemenu.hpp"

CoopCircleMenu coopCircleMenu;

extern Event EV_Player_ActivateNewWeapon;
extern Event EV_Player_DeactivateWeapon;


void CoopCircleMenu::circleMenuCall(Player* player, int iType)
{
	if (!player || player->getHealth() <= 0 || gameFixAPI_isSpectator_stef2(player) || level.cinematic) {
		return;
	}

	//prevent premature re-opening
	if (coopManager_client_persistant_t[player->entnum].circleMenuActive <= 0 && (coopManager_client_persistant_t[player->entnum].circleMenuActivatingTime + 0.5) >= level.time) {
		player->hudPrint("c open/close tosoon\n");
		return;
	}

	//make sure bad value is catched
	if (iType <= 0 || iType > 2) {
		gi.Printf(va("%s coopCircleMenu( %i ) <- unsupported Menu Type number in parameter 1\n", player->client->pers.netname, iType));
		player->hudPrint(va("%s coopCircleMenu( %i ) <- unsupported Menu Type number in parameter 1\n", player->client->pers.netname, iType));
		return;
	}

	coopManager_client_persistant_t[player->entnum].circleMenuNumOfSegments = 4;
	coopManager_client_persistant_t[player->entnum].circleMenuActivatingTime = level.time;

	if (coopManager_client_persistant_t[player->entnum].circleMenuActive <= 0) {
		//hudPrint("coopCircleMenu - start\n");//hzm coopdebug circlemenu
		//reset holding buttons - precent acidental activation
		coopManager_client_persistant_t[player->entnum].circleMenuHoldingLeftButton = true;
		coopManager_client_persistant_t[player->entnum].circleMenuHoldingRightButton = true;

		coopManager_client_persistant_t[player->entnum].circleMenuLastSegment = 0; //reset selected segment

		coopManager_client_persistant_t[player->entnum].circleMenuViewAngle = player->getViewAngles();
		coopManager_client_persistant_t[player->entnum].circleMenuLastViewAngle = player->getViewAngles();

		coopManager_client_persistant_t[player->entnum].circleMenuActive = iType;
		circleMenuHud(player, true);

		//[b60021] chrissstrahl - stop weapon from fireing
		player->getActiveWeaponName(WEAPON_ANY, coopManager_client_persistant_t[player->entnum].circleMenuLastWeapon);

		Event* StopFireEvent;
		StopFireEvent = new Event(EV_Sentient_StopFire);
		StopFireEvent->AddString("dualhand");
		player->ProcessEvent(StopFireEvent);

		Event* deactivateWeaponEv;
		deactivateWeaponEv = new Event(EV_Player_DeactivateWeapon);
		deactivateWeaponEv->AddString("dualhand");
		player->PostEvent(deactivateWeaponEv, 0.05);

		player->disableInventory();

		Event* StopFireEvent2;
		StopFireEvent2 = new Event(EV_Sentient_StopFire);
		StopFireEvent2->AddString("dualhand");
		player->PostEvent(StopFireEvent2, 0.1);

		//upgPlayerDisableUseWeapon(true);
	}
	else {
		//hudPrint("coopCircleMenu - end\n");//hzm coopdebug circlemenu
		gamefix_playerDelayedServerCommand(player->entnum, "-attackLeft");

		circleMenuHud(player, false);
		coopManager_client_persistant_t[player->entnum].circleMenuActive = 0;

		player->enableInventory();

		//restore prev weapon
		Event* useWeaponEv;
		useWeaponEv = new Event(EV_Player_UseItem);
		useWeaponEv->AddString(coopManager_client_persistant_t[player->entnum].circleMenuLastWeapon);
		useWeaponEv->AddString("dualhand");
		player->ProcessEvent(useWeaponEv);

		Event* StopFireEvent;
		StopFireEvent = new Event(EV_Sentient_StopFire);
		StopFireEvent->AddString("dualhand");
		player->PostEvent(StopFireEvent, 0.1f);

		Event* activateWeaponEv;
		activateWeaponEv = new Event(EV_Player_ActivateNewWeapon);
		player->PostEvent(activateWeaponEv, 0.1f);

		//reset to normal circlemenu items - !ability, !communicator, !objectives
		circleMenuSetup(player);
	}
}

void CoopCircleMenu::circleMenuHud(Player* player, bool show)
{
	str sMenu;
	str sWidgetName;
	int iSegments;
	str sCommand = "ui_removehud";

	coopManager_client_persistant_t[player->entnum].circleMenuLastWidget = "";

	if (show == true) { sCommand = "ui_addhud"; }

	switch (coopManager_client_persistant_t[player->entnum].circleMenuActive)
	{
	case 3:
		sMenu = "coop_circle8";
		iSegments = 8;
		break;
	case 2:
		sMenu = "coop_circleD";
		iSegments = 4;
		break;
	default:
		sMenu = "coop_circle";
		iSegments = 4;
		break;
	}

	for (int i = 0; i < iSegments; i++) {
		G_SendCommandToPlayer(player->edict, va("globalwidgetcommand %s shadercolor 0.5 0.5 0.5 0.8", circleMenuGetWidgetName(player, i).c_str()));
	}

	gi.SendServerCommand(player->entnum, va("stufftext \"%s %s\"\n", sCommand.c_str(), sMenu.c_str()));
}

// Description:	Calculates the desired move direction based on the last direction moves         
// Returns:     Circle Menu Widgetname for given direction
str CoopCircleMenu::circleMenuGetWidgetName(Player *player, int iSegment)
{
	str currentWidget = "coop_circle";
	int iSegments = coopManager_client_persistant_t[player->entnum].circleMenuNumOfSegments;

	//if circlemenu is of type dialog
	if (coopManager_client_persistant_t[player->entnum].circleMenuActive == 2) {
		currentWidget = "coop_circleD";
	}

	switch (iSegments)
	{
	case 4: //0=up,1=right,2=down,3=left
		switch (iSegment) {
		case 0:
			currentWidget += "T";
			break;
		case 1:
			currentWidget += "R";
			break;
		case 2:
			currentWidget += "B";
			break;
		case 3:
			currentWidget += "L";
			break;
		default:
			gi.Printf(va("circleMenuGetWidgetName(%i) %i - No widget name avialable for parameter 1\n", iSegment, iSegments));
		}
		break;
	case 8: //0=Top,1=TopRight 2=right,3=BottomRight, 4=Bottom,5=BottomLeft, 6=Left,7=TopLeft
		currentWidget = "coop_circle8";
		switch (iSegment) {
		case 0:
			currentWidget += "T";
			break;
		case 1:
			currentWidget += "TR";
			break;
		case 2:
			currentWidget += "R";
			break;
		case 3:
			currentWidget += "BR";
			break;
		case 4:
			currentWidget += "B";
			break;
		case 5:
			currentWidget += "BL";
			break;
		case 6:
			currentWidget += "L";
			break;
		case 7:
			currentWidget += "TL";
			break;
		default:
			gi.Printf(va("circleMenuGetWidgetName(%i) %i - No widget name avialable for parameter 1\n", iSegment, iSegments));
		}
	default:
		gi.Printf(va("circleMenuGetWidgetName(%i) %i - No widget name avialable for parameter 1\n", iSegment, iSegments));
	}
	return currentWidget;
}

//adds dialog option to circle menu
void CoopCircleMenu::circleMenuSet(Player* player, int iOption, str sText, str sThread, str sImage, bool bThread, int iAmmount, int iCost, str sCostType)
{
	//range 1 to CIRCLEMENU_MAX_OPTIONS
	if (iOption < 1 || iOption > CIRCLEMENU_MAX_OPTIONS) {
		gi.Printf(va("circleMenuSet: Given Option %i is out of Range\n", iOption));
		return;
	}

	//[b60021] chrissstrahl - disabled - we want to use circlemenu in conjunction with class !ability
	//if (coopCircleMenu.active <= 0) {
		//gi.Printf(va("%s.circleMenuSet() - Can only be used while menu active.\n", targetname.c_str()));
	//}

	if (!sImage.length()) { sImage = "weapons/empty"; }
	if (!sText.length()) { sText = "^"; }
	if (iAmmount == -1) { iAmmount = 999999; }

	int iOptionToArrayNum = (iOption - 1); //make it so that players can start with 1 instead of 0, substract 1

	coopManager_client_persistant_t[player->entnum].circleMenuOptionThreadOrCommand[iOptionToArrayNum] = sThread;
	coopManager_client_persistant_t[player->entnum].circleMenuOptionText[iOptionToArrayNum] = sText;
	coopManager_client_persistant_t[player->entnum].circleMenuOptionIcon[iOptionToArrayNum] = sImage;
	coopManager_client_persistant_t[player->entnum].circleMenuOptionIsScript[iOptionToArrayNum] = bThread;
	coopManager_client_persistant_t[player->entnum].circleMenuOptionAmmount[iOptionToArrayNum] = iAmmount;
	coopManager_client_persistant_t[player->entnum].circleMenuOptionCost[iOptionToArrayNum] = iCost;
	coopManager_client_persistant_t[player->entnum].circleMenuOptionCostType[iOptionToArrayNum] = sCostType;

	/*coopCircleMenu.optionThreadOrCommand[iOptionToArrayNum] = sThread;
	coopCircleMenu.optionText[iOptionToArrayNum] = sText;
	coopCircleMenu.optionIcon[iOptionToArrayNum] = sImage;
	coopCircleMenu.optionIsScript[iOptionToArrayNum] = bThread;
	coopCircleMenu.optionAmmount[iOptionToArrayNum] = iAmmount;
	coopCircleMenu.optionCost[iOptionToArrayNum] = iCost;
	coopCircleMenu.optionCostType[iOptionToArrayNum] = sCostType;*/

	//gi.Printf("Player::circleMenuSet()->circleMenuGetWidgetName\n");
	str sWidgetName = circleMenuGetWidgetName(player, iOptionToArrayNum);

	//send commands to menu - make sure not to resend unnessary data
	if (coopManager_client_persistant_t[player->entnum].circleMenuOptionIconLastSend[iOptionToArrayNum] != sImage) { 
		coopManager_client_persistant_t[player->entnum].circleMenuOptionIconLastSend[iOptionToArrayNum] = sImage;
		gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand %sIcon shader %s", sWidgetName.c_str(), sImage.c_str()));
	}
	//else {
		//gi.Printf("circleMenuSet already set: %d\n", iOptionToArrayNum);
	//}

	//replace withespace and newline to make it work with labeltext - make sure not to resend unnessary data
	if (coopManager_client_persistant_t[player->entnum].circleMenuOptionTextLastSend[iOptionToArrayNum] != sText) {
		coopManager_client_persistant_t[player->entnum].circleMenuOptionTextLastSend[iOptionToArrayNum] = sText;
		sText = gamefix_replaceForLabelText(sText);
		gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand %sText labeltext %s", sWidgetName.c_str(), sText.c_str()));
		//DEBUG_LOG(va("circleMenuSet: %sText labeltext %s\n", sWidgetName.c_str(), sText.c_str()));
	}
	//else {
		//gi.Printf("circleMenuSet already set: %d\n", iOptionToArrayNum);
	//}
}

void CoopCircleMenu::circleMenuReset(Player *player)
{
	player->CancelEventsOfType(EV_Player_coop_circleMenuSet);

	coopManager_client_persistant_t[player->entnum].circleMenuActive = false;
	coopManager_client_persistant_t[player->entnum].circleMenuNumOfSegments = CIRCLEMENU_MAX_OPTIONS;
	coopManager_client_persistant_t[player->entnum].circleMenuViewAngle = Vector(0, 0, 0);
	coopManager_client_persistant_t[player->entnum].circleMenuActivatingTime = 0.0f;
	coopManager_client_persistant_t[player->entnum].circleMenuLastWeapon = "None";

	coopManager_client_persistant_t[player->entnum].circleMenuLongtimeViewangle = Vector(0, 0, 0);
	coopManager_client_persistant_t[player->entnum].circleMenuLastViewAngle = Vector(0, 0, 0);
	coopManager_client_persistant_t[player->entnum].circleMenuLastThinkTime = 0.0f;
	coopManager_client_persistant_t[player->entnum].circleMenuLastMessageTime = 0.0f;
	coopManager_client_persistant_t[player->entnum].circleMenuLastWidget = "";
	coopManager_client_persistant_t[player->entnum].circleMenuLastSegment = -1;

	coopManager_client_persistant_t[player->entnum].circleMenuHoldingRightButton = false;
	coopManager_client_persistant_t[player->entnum].circleMenuHoldingLeftButton = false;

	for (short i = 0; i < CIRCLEMENU_MAX_OPTIONS; i++) {
		coopManager_client_persistant_t[player->entnum].circleMenuOptionIconLastSend[i] = "";
		coopManager_client_persistant_t[player->entnum].circleMenuOptionTextLastSend[i] = "";
		coopManager_client_persistant_t[player->entnum].circleMenuOptionThreadOrCommand[i] = "";
		coopManager_client_persistant_t[player->entnum].circleMenuOptionText[i] = "";
		coopManager_client_persistant_t[player->entnum].circleMenuOptionIcon[i] = "";
		coopManager_client_persistant_t[player->entnum].circleMenuOptionIsScript[i] = false;
		coopManager_client_persistant_t[player->entnum].circleMenuOptionAmmount[i] = 99999;
		coopManager_client_persistant_t[player->entnum].circleMenuOptionCost[i] = 0;
		coopManager_client_persistant_t[player->entnum].circleMenuOptionCostType[i] = 0;
	}
	
	for (short i = 0; i < CIRCLEMENU_MAX_OPTIONSDIALOG; i++) {
		coopManager_client_persistant_t[player->entnum].circleMenuOptionDialogThread[i] = "";
		coopManager_client_persistant_t[player->entnum].circleMenuOptionDialogText[i] = "";
		coopManager_client_persistant_t[player->entnum].circleMenuOptionDialogIcon[i] = "";
	}
}

void CoopCircleMenu::circleMenuSetup(Player *player)
{
	circleMenuReset(player);

	str circleText1 = "";
	str circleText2_eng = "Mission\nObjective";
	str circleText2_deu = "Missions\nZiele";
	str circleText3_eng = "Communicator";
	str circleText3_deu = "Kommunikator";
	str circleText4 = "";
	str circleCmd1 = "";
	str circleCmd2 = "togglemenu coop_obj";
	str circleCmd3 = "togglemenu coop_com";
	str circleCmd4 = "";
	str circleImg1 = "";
	str circleImg2 = "sysimg/hud/radar/blip-obj.tga";
	str circleImg3 = "sysimg/icons/levelawards/level_award1.tga";
	str circleImg4 = "";

	//class related !ability
	coopClass.coop_classApplayCirleMenu(player);

	Event* evCircleSet2;
	evCircleSet2 = new Event(EV_Player_coop_circleMenuSet);
	evCircleSet2->AddInteger(2);
	if (player->coop_hasLanguageGerman()) {
		evCircleSet2->AddString(circleText2_deu.c_str());
	}
	else {
		evCircleSet2->AddString(circleText2_eng.c_str());
	}
	evCircleSet2->AddString(circleCmd2.c_str());
	evCircleSet2->AddString(circleImg2.c_str());
	evCircleSet2->AddInteger(0);
	player->PostEvent(evCircleSet2,0.15f);

	Event* evCircleSet3;
	evCircleSet3 = new Event(EV_Player_coop_circleMenuSet);
	evCircleSet3->AddInteger(3);
	if (player->coop_hasLanguageGerman()) {
		evCircleSet3->AddString(circleText3_deu.c_str());
	}
	else {
		evCircleSet3->AddString(circleText3_eng.c_str());
	}
	evCircleSet3->AddString(circleCmd3.c_str());
	evCircleSet3->AddString(circleImg3.c_str());
	evCircleSet3->AddInteger(0);
	player->PostEvent(evCircleSet3, 0.20f);

	Event* evCircleSet4;
	evCircleSet4 = new Event(EV_Player_coop_circleMenuSet);
	evCircleSet4->AddInteger(4);
	evCircleSet4->AddString(circleText4.c_str());
	evCircleSet4->AddString(circleCmd4.c_str());
	evCircleSet4->AddString(circleImg4.c_str());
	evCircleSet4->AddInteger(0);
	player->PostEvent(evCircleSet4,0.25);
}
      
// Description: This sends a command to enable/disable a widget in a single burst
void CoopCircleMenu::circleMenuSwitchWidgets(Player *player, str widget1, str widget2, str widget1Cmd, str widget2Cmd)
{
	if (!player) {
		return;
	}
	str command = "globalwidgetcommand ";
	command += widget1;
	command += " ";
	command += widget1Cmd;
	command += ";globalwidgetcommand ";
	command += widget2;
	command += " ";
	command += widget2Cmd;
	command += "\n";
	G_SendCommandToPlayer(player->edict, command.c_str());
}

bool CoopCircleMenu::circleMenuIsActive(Player* player)
{
	if (player) {
		if (coopManager_client_persistant_t[player->entnum].circleMenuActive > 0) {
			return true;
		}
	}

	return false;
}

float CoopCircleMenu::circleMenuLastTimeActive(Player *player)
{
	if (!player) {
		return 0.0f;
	}

	if (coopManager_client_persistant_t[player->entnum].circleMenuActivatingTime > coopManager_client_persistant_t[player->entnum].circleMenuLastThinkTime) {
		return coopManager_client_persistant_t[player->entnum].circleMenuActivatingTime;
	}
	return coopManager_client_persistant_t[player->entnum].circleMenuLastThinkTime;
}

int CoopCircleMenu::circleMenuGetSegmentNumForAngle(float fAngle)
{
	float fNumSegments = 4;
	float fSegmentDegreeSize = (360 / fNumSegments);
	float fSegmentMaxEnd = (359 - (fSegmentDegreeSize / 2));

	//go step whise in reverse
	float iStep;
	float iHighestSegmentNum;
	float fCurrentMax;
	fCurrentMax = fSegmentMaxEnd;
	iHighestSegmentNum = (fNumSegments - 1);

	for (iStep = iHighestSegmentNum; iStep >= 0; iStep--) {
		//print("getSegmentNumForAngle: '"+fAngle+"'\n");
		if (fCurrentMax > fAngle && fAngle > (fCurrentMax - (fSegmentDegreeSize))) {
			return iStep;
		}
		fCurrentMax = AngleNormalize360(fCurrentMax - fSegmentDegreeSize);
	}
	if (fAngle > fCurrentMax && fAngle <= 359) {
		return 0;
	}
	return -1;
}
       
// Description:	Checks if circle menu is active and manages interactions
void CoopCircleMenu::circleMenuThink(Player *player)
{
	//detect which movedirection the player did move towards
	//on menu show exec reset
	if (coopManager_client_persistant_t[player->entnum].circleMenuActive <= 0 /* || coopCircleMenu.thinkTime > level.time */) {
		return;
	}

	//make sure it can not be abused by spec
	if (player->getHealth() <= 0 || multiplayerManager.inMultiplayer() && multiplayerManager.isPlayerSpectator(player)) {
		if (coopManager_client_persistant_t[player->entnum].circleMenuActive) {
			circleMenuHud(player,false);
			coopManager_client_persistant_t[player->entnum].circleMenuActive = 0;
		}
		return;
	}

	//player is clicking fire
	if (player->GetLastUcmd().buttons & BUTTON_ATTACKLEFT) {
		if (!coopManager_client_persistant_t[player->entnum].circleMenuHoldingLeftButton) {
			coopManager_client_persistant_t[player->entnum].circleMenuHoldingLeftButton = true;
			coopManager_client_persistant_t[player->entnum].circleMenuLastThinkTime = level.time;
			circleMenuSelect(player, coopManager_client_persistant_t[player->entnum].circleMenuLastSegment);
			return;
		}
	}
	else {
		coopManager_client_persistant_t[player->entnum].circleMenuHoldingLeftButton = false;
	}

	//detect and record the mouse move directions
	Vector vDifference = Vector(0, 0, 0);
	Vector vViewangle = Vector(0, 0, 0);
	player->GetPlayerView(NULL, &vViewangle);


	if ((coopManager_client_persistant_t[player->entnum].circleMenuLastThinkTime + 0.05) > level.time) { return; }
	coopManager_client_persistant_t[player->entnum].circleMenuLastThinkTime = level.time;

	str sWidgetName;
	int fSegmentNum = coopManager_client_persistant_t[player->entnum].circleMenuLastSegment;

	//select widget by right click
	if (player->GetLastUcmd().buttons & BUTTON_ATTACKRIGHT) {
		if (coopManager_client_persistant_t[player->entnum].circleMenuHoldingRightButton) {
			return;
		}

		coopManager_client_persistant_t[player->entnum].circleMenuHoldingRightButton = true;
		fSegmentNum = (coopManager_client_persistant_t[player->entnum].circleMenuLastSegment + 1);
		if (fSegmentNum >= 4) {
			fSegmentNum = 0;
		}
	}
	else {
		coopManager_client_persistant_t[player->entnum].circleMenuHoldingRightButton = false;
	}

	sWidgetName = circleMenuGetWidgetName(player, fSegmentNum);
	if (sWidgetName != "" && sWidgetName != coopManager_client_persistant_t[player->entnum].circleMenuLastWidget) {
		str sCmd;
		G_SendCommandToPlayer(player->edict, va("globalwidgetcommand %s shadercolor 0 0 0 1", sWidgetName.c_str()));
		if (coopManager_client_persistant_t[player->entnum].circleMenuLastWidget.length()) {
			G_SendCommandToPlayer(player->edict, va("globalwidgetcommand %s shadercolor 0.5 0.5 0.5 1", coopManager_client_persistant_t[player->entnum].circleMenuLastWidget.c_str()));
		}
	}
	
	coopManager_client_persistant_t[player->entnum].circleMenuLastWidget = sWidgetName;
	coopManager_client_persistant_t[player->entnum].circleMenuLastSegment = fSegmentNum;
}

// Description:	Manages if the player has selected a item on the circle menu
void CoopCircleMenu::circleMenuSelect(Player* player, int iOption)
{
	if (iOption < 0 || iOption >= CIRCLEMENU_MAX_OPTIONS) {
		gi.Printf(va("circleMenuSelect: Given Option %d is out of Range for Client[%d] %s\n", iOption, player->entnum, player->client->pers.netname));
		player->hudPrint("c select outofrange\n");
		return;
	}

	bool	bIsScript = true;
	str		sThread;

	if (coopManager_client_persistant_t[player->entnum].circleMenuActive != 2) {
		bIsScript = coopManager_client_persistant_t[player->entnum].circleMenuOptionIsScript[iOption];
		sThread = coopManager_client_persistant_t[player->entnum].circleMenuOptionThreadOrCommand[iOption];
		if (coopManager_client_persistant_t[player->entnum].circleMenuOptionAmmount[iOption] < 1) {
			float fMessageSilenceTime = 0.5f;
			if ((coopManager_client_persistant_t[player->entnum].circleMenuLastMessageTime + fMessageSilenceTime) < level.time) {
				gi.Printf(va("circleMenuSelect: Given Option %d ammount < 1 for Client[%d] %s\n", iOption, player->entnum, player->client->pers.netname));
			}

			//close menu if no options are set
			int iValidOptions = 0;
			for (int iOptionCheck = (CIRCLEMENU_MAX_OPTIONS - 1); iOptionCheck >= 0; iOptionCheck--) {
				if (coopManager_client_persistant_t[player->entnum].circleMenuOptionAmmount[iOptionCheck] > 0) {
					iValidOptions++;
				}
			}
			if (iValidOptions == 0 && (coopManager_client_persistant_t[player->entnum].circleMenuLastMessageTime + fMessageSilenceTime) < level.time) {
				//Close Menu
				circleMenuCall(player, coopManager_client_persistant_t[player->entnum].circleMenuActive);
				player->hudPrint("Circle Menu - No options set by script, abborting.\n");
				coopManager_client_persistant_t[player->entnum].circleMenuLastMessageTime = level.time;
			}
			return;
		}
	}
	else {
		sThread = coopManager_client_persistant_t[player->entnum].circleMenuOptionDialogThread[iOption];
	}

	if (bIsScript) {
		ExecuteThread(sThread, true, player);
	}
	else {
		gamefix_playerDelayedServerCommand(player->entnum, va("%s", sThread.c_str()));
	}
	//Close Menu
	circleMenuCall(player, coopManager_client_persistant_t[player->entnum].circleMenuActive);
}

//clears dialog option from circle menu
void CoopCircleMenu::circleMenuClear(Player *player, int iOption)
{
	if (coopManager_client_persistant_t[player->entnum].circleMenuActive <= 0) {
		gi.Printf(va("circleMenuClear($%s) - Can only be used while menu active.\n", player->targetname.c_str()));
		return;
	}

	//ramnge 0 to CIRCLEMENU_MAX_OPTIONS
	if (iOption < 0 || iOption > CIRCLEMENU_MAX_OPTIONS) {
		gi.Printf(va("circleMenuClear($%s) - Out of range: %d.\n", player->targetname.c_str(), iOption));
		return;
	}

	if (iOption != 0) {
		//reset specific
		circleMenuSet(player, iOption, "", "", "", false, 999999, 0, "");
	}
	else {
		//reset all
		for (int i = 1; i <= CIRCLEMENU_MAX_OPTIONS; i++) {
			circleMenuSet(player, i, "", "", "", false, 999999, 0, "");
		}
	}
}

//adds dialog option to circle menu
void CoopCircleMenu::circleMenuDialogSet(Player *player, int iOption, str sText, str sThread, str sImage)
{
	//range 1 to CIRCLEMENU_MAX_OPTIONSDIALOG
	if (iOption < 1 || iOption > CIRCLEMENU_MAX_OPTIONSDIALOG) {
		gi.Printf(va("circleMenuDialogSet: Given Option %i is out of Range\n", iOption));
		return;
	}

	//if (coopCircleMenu.active <= 0) {
		//gi.Printf(va("%s.CircleMenuDialogSet() - Can only be used while menu active.\n", targetname.c_str()));
	//}

	int iOptionToArrayNum = (iOption - 1); //make it so that players can start with 1 instead of 0, substract 1
	coopManager_client_persistant_t[player->entnum].circleMenuOptionDialogThread[iOptionToArrayNum] = sThread;
	coopManager_client_persistant_t[player->entnum].circleMenuOptionDialogText[iOptionToArrayNum] = sText;
	coopManager_client_persistant_t[player->entnum].circleMenuOptionDialogIcon[iOptionToArrayNum] = sImage;

	if (!sImage.length()) { sImage = "weapons/empty"; }
	if (!sText.length()) { sText = "^"; }

	str sWidgetName = circleMenuGetWidgetName(player, iOptionToArrayNum);

	//send commands to menu
	gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand %sIcon shader %s", sWidgetName.c_str(), sImage.c_str()));
	sText = gamefix_replaceForLabelText(sText);
	gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand %sText labeltext %s", sWidgetName.c_str(), sText.c_str()));
}


//hzm gameupdate chrissstrahl [b60011]  - adds dialog option to circle menu for player
void CoopCircleMenu::circleMenuDialogClear(Player *player, int iOption)
{
	if (coopManager_client_persistant_t[player->entnum].circleMenuActive <= 0) {
		gi.Printf(va("circleMenuDialogClear($%s) - Can only be used while menu active.\n", player->targetname.c_str()));
		return;
	}

	//range 1 to CIRCLEMENU_MAX_OPTIONSDIALOG
	if (iOption < 0 || iOption > CIRCLEMENU_MAX_OPTIONSDIALOG) {
		gi.Printf(va("circleMenuDialogClear($%s) - Out of range: %d.\n", player->targetname.c_str(), iOption));
		return;
	}

	if (iOption != 0) {
		//reset specific
		circleMenuDialogSet(player, iOption, "", "", "");
	}
	else {
		//reset all
		for (int i = 1; i <= CIRCLEMENU_MAX_OPTIONSDIALOG; i++) {
			circleMenuDialogSet(player, i, "", "", "");
		}
	}
}