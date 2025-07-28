//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING PLAYER WEAPON/ARMORY RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------
#pragma once

#include "../../dlls/game/_pch_cpp.h"


void coop_armoryRememberLastUsed( Player *player , str sWeapon );
void coop_armoryEquipPlayer( Player *player );
void coop_armoryEquipPlayerSingleplayer(Player* player);
str coop_armoryReturnWeaponName( str sTikiName );
str coop_armoryReturnWeaponTiki( str sWeaponName );


