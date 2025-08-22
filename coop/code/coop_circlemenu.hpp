//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:	chrissstrahl@yahoo.de
// 
// Circle Menu Related Code, used to add New Mewnu Type to the game, like known from modern games.
//-----------------------------------------------------------------------------------
#pragma once

#include "../../dlls/game/_pch_cpp.h"
#include "coop_manager.hpp"

//void PmoveSingle (pmove_t *pmove)
// Clear movement flags
//client->ps.pm_flags &= ~( PMF_FLIGHT | PMF_FROZEN | PMF_NO_PREDICTION | PMF_NO_MOVE | PMF_HAVETARGET | PMF_NO_GRAVITY );

#define CIRCLEMENU_STARTNUM 0
#define CIRCLEMENU_MAX_OPTIONS 4
#define CIRCLEMENU_MAX_OPTIONSDIALOG 4

extern Event EV_Player_circleMenu;
extern Event EV_Player_circleMenuDialogSet;
extern Event EV_Player_circleMenuDialogClear;
extern Event EV_Player_coop_circleMenuSet;
extern Event EV_Player_circleMenuClear;

class CoopCircleMenu
{
public:
	int		active = 0;
	int		numOfSegments = 4; //[b60021] chrissstrahl - changed so we can SETUP circlemenu without inizialising/opening it first
	//viewangle - stored upon opening the menu
	Vector	viewAngle = Vector(0, 0, 0);
	//last time the menu did think
	float	thinkTime = 0.0f;
	//time when the menu was activated - used to prevent premature opening/closeing
	float	activatingTime = 0.0f;
//time when the circlemenu was last active - used to prevent weapon fireing in next frame on menu closing
//float	lastTimeActive = 0.0f;
	//time a message was last send to player
	float	lastMessageTime = 0.0f;
	//remember stuff so we can compare
	Vector	lastViewangle = Vector(0, 0, 0);
	Vector	longtimeViewangle = Vector(0, 0, 0);
	str		lastWidget = "";
	int		lastSegment = -1;
	
	//[b60021] chrissstrahl
	str		lastWeapon = "None";
	bool	holdingRightButton = false;
	bool	holdingLeftButton = false;
	str		optionTextLastSend[CIRCLEMENU_MAX_OPTIONS];
	str		optionIconLastSend[CIRCLEMENU_MAX_OPTIONS];

	//circlemenu options
	str		optionThreadOrCommand	[CIRCLEMENU_MAX_OPTIONS];
	str		optionText				[CIRCLEMENU_MAX_OPTIONS];
	str		optionIcon				[CIRCLEMENU_MAX_OPTIONS];
	bool	optionIsScript			[CIRCLEMENU_MAX_OPTIONS];
	int		optionAmmount			[CIRCLEMENU_MAX_OPTIONS];
	int		optionCost				[CIRCLEMENU_MAX_OPTIONS];
	str		optionCostType			[CIRCLEMENU_MAX_OPTIONS];

	//circlemenu dialog options  - used for dialog selection
	str		optionDialogThread	[CIRCLEMENU_MAX_OPTIONSDIALOG];
	str		optionDialogText	[CIRCLEMENU_MAX_OPTIONSDIALOG];
	str		optionDialogIcon	[CIRCLEMENU_MAX_OPTIONSDIALOG];

	void circleMenuCall(Player* player, int iType);
	void circleMenuHud(Player* player, bool show);
	str circleMenuGetWidgetName(Player *player, int iSegment);
	void circleMenuSet(Player* player, int iOption, str sText, str sThread, str sImage, bool bThread, int iAmmount, int iCost, str sCostType);
	void circleMenuReset(Player* player);
	void circleMenuSetup(Player* player);
	void circleMenuSwitchWidgets(Player* player, str widget1, str widget2, str widget1Cmd, str widget2Cmd);
	bool circleMenuIsActive(Player* player);
	float circleMenuLastTimeActive(Player* player);
	int circleMenuGetSegmentNumForAngle(float fAngle);
	void circleMenuThink(Player* player);
	void circleMenuSelect(Player* player, int iOption);
	void circleMenuClear(Player* player, int iOption);
	void circleMenuDialogSet(Player* player, int iOption, str sText, str sThread, str sImage);
	void CoopCircleMenu::circleMenuDialogClear(Player* player, int iOption);
};
extern CoopCircleMenu coopCircleMenu;