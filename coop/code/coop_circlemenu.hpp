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
	void circleMenuDialogClear(Player* player, int iOption);
};
extern CoopCircleMenu coopCircleMenu;