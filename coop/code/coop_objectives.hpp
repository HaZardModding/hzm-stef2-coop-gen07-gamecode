//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING OBJECTIVES RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------
#pragma once


#include "../../dlls/game/_pch_cpp.h"

struct coopObjectives_s
{
	str         levelAuthor = "Ritual Entertainment";
	str         story = "$$Empty$$";
	str         story_deu = "$$Empty$$";
	float       lastUpdated_story = -1;

	int         objectiveCycle = 0;
	str         objectiveItem[8] = { "$$Empty$$","2","3","4","5","6","7","8" };
	str         objectiveItem_deu[8] = { "$$Empty$$","2","3","4","5","6","7","8" };
	int         objectiveItemStatus[8] = { 0,0,0,0,0,0,0,0 };
	int         objectiveItemShow[8] = { 0,0,0,0,0,0,0,0 };
	float       objectiveItemCompletedAt[8] = { -999.0f,-999.0f,-999.0f,-999.0f,-999.0f,-999.0f,-999.0f,-999.0f };
	float       objectiveItemLastTimePrintedTitleAt = -1.0;
	str         objectiveItemLastTimePrintedTitle = "";
};extern coopObjectives_s coopObjectives_t;


int coop_getObjectivesCycle();
void coop_setObjectivesCycle();
void coop_objectivesStorySet(Player* player);
str coop_getObjectivesCvarName(int iObjectivesItem);

void coop_objectivesMarkerUpdate( void );
void coop_objectivesMarker( Entity *eMaster );
void coop_objectivesMarkerRemove( Entity *eMaster );
void coop_objectivesUpdateUservar( int iUservar );
void coop_objectivesNotify( Player* player );
void coop_objectivesUpdatePlayer( Player* player );
void coop_objectivesSetup(Player *player);
void coop_objectivesUpdate( str sObjectiveState , int fObjectiveItem , int fObjectiveShow );
//void coop_objectivesUpdate( str sObjectiveState , str sObjectiveItem , str sObjectiveShow );
void coop_objectivesShow( Player *player , int iObjectiveItem , int iObjectiveState , bool bObjectiveShow );
void coop_objectivesSingleplayer( str sObjectiveState , int iObjectiveNumber , int iShowNow );
void coop_objectives_tacticalShow( Player *player , int iTactical );