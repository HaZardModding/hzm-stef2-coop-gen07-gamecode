//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:	chrissstrahl@yahoo.de
//
// CONTAINING PLAYER WEAPON/ARMORY RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------

#include "../../dlls/game/_pch_cpp.h"

/*
#include "level.h"
#include "player.h"
#include "weapon.h"
#include "mp_manager.hpp"
#include "mp_modeDm.hpp"
#include "mp_modeTeamDm.hpp"
#include "mp_modeCtf.hpp"
#include "powerups.h"*/


#include "../../dlls/game/gamefix.hpp"
#include "coop_manager.hpp"
#include "coop_objectives.hpp"
#include "coop_armory.hpp"


//================================================================           
// Description: remembers the weapon the player used last, restore on mapchange
//================================================================
void coop_armoryRememberLastUsed( Player *player, str sWeapon )
{
	if (!CoopManager::Get().IsCoopEnabled())return;
	gamefix_playerDelayedServerCommand( player->entnum , va( "set coop_weap use %s" , sWeapon.c_str() ) );
}

//================================================================
// Description: Equips player with starting weapons for coop, sepcified by script 
//================================================================
void coop_armoryEquipPlayer(Player *player)
{
	if (!player || gameFixAPI_isSpectator_stef2(player)) { return; }
//	bool bGiveSecretWeapons=true;
	int iWeapons=0;
	int iTemporary;
	str sTricorderName , sWeaponName , sPhaserName, sVariableName , sTikiName, sUseThisWeapon;

	sUseThisWeapon = "";

	//hzm coop mod chrissstrahl - equip the player, give weapons specified by script
	//[b60025] chrissstrahl - fixed armory equiping only 8 weapons - M11l1a was missing weapons if player collected secret weapons
	for ( iTemporary = _COOP_SETTINGS_WEAPONS_MAX; iTemporary > 0; iTemporary-- ){
		sVariableName = "coop_string_weapon";
		sVariableName += iTemporary;
		sTikiName = program.coop_getVariableValueAsString( sVariableName.c_str() );

		if ( sTikiName.length() > 4 && sTikiName != "<NULL>") { //[b60011] Chrissstrahl - fix handling if <NULL> string is returned
			iWeapons++;

			//make this work in coop and singleplayer
			if(g_gametype->integer != GT_SINGLE_PLAYER ){
				multiplayerManager.givePlayerItem( player->entnum , sTikiName.c_str() );
			}else{
				Event *event;
				event = new Event( EV_Player_GiveCheat );
				event->AddString( sTikiName );
				player->ProcessEvent( event );
			}
//gi.Printf(va("COOPDEBUG: Equiping %s with %s\n", player->client->pers.netname, sTikiName.c_str()));

			str sTempName = coop_armoryReturnWeaponName( sTikiName );
			sTikiName = sTikiName.tolower();
			sTempName = sTempName.tolower();

			//[b60012] chrissstrahl - fix missing .c_str()
			if (Q_stricmp( sTikiName.c_str(), "models/weapons/worldmodel-tricorder-stx.tik") == 0 ||
				Q_stricmp( sTikiName.c_str(), "models/weapons/worldmodel-rom-datapad.tik" ) == 0||
				Q_stricmp( sTikiName.c_str(), "models/weapons/worldmodel-tricorder.tik" ) == 0)
			{
				sTricorderName = sTikiName;
			}
			else
			{
				//[b60012] chrissstrahl - fix missing .c_str()
				if (Q_stricmp( sTikiName.c_str(), "models/weapons/worldmodel-phaser.tik" ) == 0 ||
					Q_stricmp( sTikiName.c_str(), "models/weapons/worldmodel-phaser-stx.tik" ) == 0)
				{
					sPhaserName = sTikiName;
				}
				else {
					sWeaponName = sTikiName;
				}
			}
		}
	}

	//[b610] chrissstrahl - print out how many weapons player got at start
	//gi.Printf(va("COOPDEBUG: %i script-specified Weapons given to %s\n", iWeapons, player->client->pers.netname));

	//hzm coop mod chrissstrahl - exit here if there are no weapons to handle
	if ( iWeapons == 0 ){
		return;
	}

	//hzm coop mod chrissstrahl - check if there is no active weapon, fall back to tricoder if it exists
	//this should fix the problem that in many missions players spawn with the tricorder, instead
	//with a real weapon in their hands, making some respawn situations a angering issue
	//Priority:
	//- any other real weapon
	//- phaser
	//- tricorder
	if ( !Q_stricmp( sUseThisWeapon.c_str(), "")) {
		if ( sWeaponName.length() < 4 && sPhaserName.length() > 3 ){
			sTikiName = sPhaserName;
		}
		else if ( sWeaponName.length() < 4 && sTricorderName.length() > 3 ){
			sTikiName = sTricorderName;
		}
		else{
			sTikiName = sWeaponName;
		}
		sUseThisWeapon = coop_armoryReturnWeaponName( sTikiName );
	}

	if ( sUseThisWeapon.length() > 0 ){
		Event *ev = new Event( "use" );
		ev->AddString( sUseThisWeapon.c_str() );
		player->ProcessEvent( ev );			
	}
	gamefix_playerDelayedServerCommand( player->entnum , "vstr coop_weap" );
}

extern coopObjectives_s coopObjectives_t;

void coop_armoryEquipPlayerSingleplayer(Player* player)
{

	if (g_gametype->integer != GT_SINGLE_PLAYER || !Director.PlayerReady()){
		return;
	}

	ScriptVariable* var = player->entityVars.GetVariable("_spArmoryEquiped");

	if(var && var->intValue()  == 1) {
		return;
	}

	player->entityVars.SetVariable("_spArmoryEquiped", "1");
	coop_armoryEquipPlayer(player);

	//make sure the coop objectives hud is displayed when we play a custom (coop) map
	if (coopManager_mapSettings_t.stockMap) {
		gi.SendServerCommand(player->entnum, "stufftext \"set coop_oExc score\"\n");
	}
	else {
		gi.SendServerCommand(player->entnum, "stufftext \"set coop_oExc pushmenu coop_obj\"\n");
		gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand coop_objAuthor title %s", coopObjectives_t.levelAuthor.c_str()));
		gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand coop_objMap title %s", level.mapname.c_str()));
		gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand coop_objSkill title %d",skill->integer));
	}
}

//================================================================
// Description: Returns Tiki file location name for given weapon
//================================================================
str coop_armoryReturnWeaponTiki( str sWeaponName )
{
	str sTikiName = "";
	if ( sWeaponName.length() )
	{
		sWeaponName = sWeaponName.tolower();
		if ( Q_stricmpn( "federationsniperrifle" , sWeaponName.c_str(), 21) == 0)
		{
			sTikiName = "models/weapons/worldmodel-Sniperrifle.tik";
		}
		else if ( Q_stricmpn( "fieldassaultrifle" , sWeaponName.c_str(), 17 ) == 0)
		{
			sTikiName = "models/weapons/worldmodel-fieldassaultrifle.tik";
		}
		else if ( Q_stricmpn( "compressionrifle" , sWeaponName.c_str(), 16 ) == 0 )
		{
			sTikiName = "models/weapons/worldmodel-compressionrifle.tik";
		}
		else if ( Q_stricmpn( "burstrifle" , sWeaponName.c_str(), 10 ) == 0 )
		{
			sTikiName = "models/weapons/worldmodel-burstrifle.tik";
		}
		else if ( Q_stricmpn( "batleth" , sWeaponName.c_str(), 7 ) == 0 )
		{
			sTikiName = "models/weapons/worldmodel-batleth.tik";
		}
		else if ( Q_stricmpn( "i-mod" , sWeaponName.c_str(), 5 ) == 0 )
		{
			sTikiName = "models/weapons/worldmodel-IMod.tik";
		}
		else if ( Q_stricmpn( "phaser" , sWeaponName.c_str(), 6 ) == 0 )
		{
			sTikiName = "models/weapons/worldmodel-phaser.tik";
		}
		else if ( Q_stricmpn( "phaser-stx" , sWeaponName.c_str(), 10 ) == 0 )
		{
			sTikiName = "models/weapons/worldmodel-phaser-STX.tik";
		}
		else if ( Q_stricmpn( "tricorder-stx" , sWeaponName.c_str(), 13 ) == 0)
		{
			sTikiName = "models/weapons/worldmodel-tricorder-STX.tik";
		}
		else if ( Q_stricmpn( "tricorder" , sWeaponName.c_str(), 9 ) == 0 )
		{
			sTikiName = "models/weapons/worldmodel-tricorder.tik";
		}
		else if ( Q_stricmpn( "tricorder-rom" , sWeaponName.c_str(), 13 ) == 0 )
		{
			sTikiName = "models/weapons/worldmodel-rom-datapad.tik";
		}
		else if ( Q_stricmpn( "romulandisruptor" , sWeaponName.c_str(), 16 ) == 0)
		{
			sTikiName = "models/weapons/worldmodel-rom-disruptor.tik";
		}
		else if ( Q_stricmpn( "tetryongatlinggun" , sWeaponName.c_str(), 17 ) == 0)
		{
			sTikiName = "models/weapons/worldmodel-tetryon.tik";
		}
		else if ( Q_stricmpn( "attrexianrifle" , sWeaponName.c_str(), 14 ) == 0)
		{
			sTikiName = "models/weapons/worldmodel-attrex-rifle.tik";
		}
		else if ( Q_stricmpn( "drullstaff" , sWeaponName.c_str(), 10 ) == 0)
		{
			sTikiName = "models/weapons/worldmodel-drull-staff.tik";
		}
		else if ( Q_stricmpn( "grenadelauncher" , sWeaponName.c_str(), 15 ) == 0)
		{
			sTikiName = "models/weapons/worldmodel-grenadelauncher.tik";
		}
		else if ( Q_stricmpn( "photonburst" , sWeaponName.c_str(), 11 ) == 0)
		{
			sTikiName = "models/weapons/worldmodel-photon.tik";
		}
		else if ( Q_stricmpn( "romulanradgun" , sWeaponName.c_str(), 13 ) == 0 )
		{
			sTikiName = "models/weapons/worldmodel-rom-radgun.tik";
		}
		else if ( Q_stricmpn( "knife" , sWeaponName.c_str(), 5 ) == 0)
		{
			sTikiName = "models/weapons/worldmodel-knife.tik";
		}
		else if ( Q_stricmpn( "enterprisecannon" , sWeaponName.c_str(), 16 ) == 0 )
		{
			sTikiName = "models/weapons/worldmodel-enterprise.tik";
		}
	}
	return sWeaponName;
}


//================================================================
// Description: Returns name from  given weapon Tiki file location
//================================================================
str coop_armoryReturnWeaponName( str sTikiName )
{
	str sWeaponName = "None";
	if(sTikiName.length())
	{
		sTikiName = sTikiName.tolower();
		
		//not astarting off with model/, add it
		if ( Q_stricmpn(sTikiName.c_str(),"models/",7) )
		{
			sTikiName += "models/" + sTikiName;
		}

		if ( Q_stricmpn( "models/weapons/worldmodel-sniperrifle.tik" , sTikiName.c_str(), 41 ) == 0 )
		{
			sWeaponName = "FederationSniperRifle";
		}
		else if ( Q_stricmpn( "models/weapons/worldmodel-fieldassaultrifle.tik" , sTikiName.c_str(), 47 ) == 0)
		{
			sWeaponName = "FieldAssaultRifle";
		}
		else if ( Q_stricmpn( "models/weapons/worldmodel-compressionrifle.tik" , sTikiName.c_str(), 46 ) == 0)
		{
			sWeaponName = "CompressionRifle";
		}
		else if ( Q_stricmpn( "models/weapons/worldmodel-burstrifle.tik" , sTikiName.c_str(), 40 ) == 0 )
		{
			sWeaponName = "BurstRifle";
		}
		else if ( Q_stricmpn( "models/weapons/worldmodel-batleth.tik" , sTikiName.c_str(), 37 ) == 0 )
		{
			sWeaponName = "Batleth";
		}
		else if ( Q_stricmpn( "models/weapons/worldmodel-imod.tik" , sTikiName.c_str(), 34 ) == 0)
		{
			sWeaponName = "I-Mod";
		}
		else if ( Q_stricmpn( "models/weapons/worldmodel-phaser.tik" , sTikiName.c_str(), 36 ) == 0)
		{
			sWeaponName = "Phaser";
		}
		else if ( Q_stricmpn( "models/weapons/worldmodel-phaser-stx.tik" , sTikiName.c_str(), 40 ) == 0)
		{
			sWeaponName = "Phaser-stx";
		}
		else if ( Q_stricmpn( "models/weapons/worldmodel-tricorder-stx.tik" , sTikiName.c_str(), 43 ) == 0 )
		{
			sWeaponName = "Tricorder-stx";
		}
		else if ( Q_stricmpn( "models/weapons/worldmodel-tricorder.tik" , sTikiName.c_str(), 39 ) == 0 )
		{
			sWeaponName = "Tricorder";
		}
		else if ( Q_stricmpn( "models/weapons/worldmodel-rom-datapad.tik" , sTikiName.c_str(), 41 ) == 0 )
		{
			sWeaponName = "Tricorder-rom";
		}
		else if ( Q_stricmpn( "models/weapons/worldmodel-rom-disruptor.tik" , sTikiName.c_str(), 43 ) == 0 )
		{
			sWeaponName = "RomulanDisruptor";
		}
		else if ( Q_stricmpn( "models/weapons/worldmodel-rom-disruptor-romhands.tik" , sTikiName.c_str(), 52 ) == 0 )
		{
			sWeaponName = "RomulanDisruptor";
		}
		else if ( Q_stricmpn( "models/weapons/worldmodel-tetryon.tik" , sTikiName.c_str(), 37 ) == 0)
		{
			sWeaponName = "TetryonGatlingGun";
		}
		else if ( Q_stricmpn( "models/weapons/worldmodel-attrex-rifle.tik" , sTikiName.c_str(), 42 ) == 0 )
		{
			sWeaponName = "AttrexianRifle";
		}
		else if ( Q_stricmpn( "models/weapons/worldmodel-drull-staff.tik" , sTikiName.c_str(), 41 ) == 0 )
		{
			sWeaponName = "DrullStaff";
		}
		else if ( Q_stricmpn( "models/weapons/worldmodel-grenadelauncher.tik" , sTikiName.c_str(), 45 ) == 0)
		{
			sWeaponName = "GrenadeLauncher";
		}
		else if ( Q_stricmpn( "models/weapons/worldmodel-photon.tik" , sTikiName.c_str(), 36 ) == 0)
		{
			sWeaponName = "PhotonBurst";
		}
		else if ( Q_stricmpn( "models/weapons/worldmodel-rom-radgun.tik" , sTikiName.c_str(), 40 ) == 0)
		{
			sWeaponName = "RomulanRadGun";
		}
		else if ( Q_stricmpn( "models/weapons/worldmodel-rom-radgun-romhands.tik" , sTikiName.c_str(), 49 ) == 0)
		{
			sWeaponName = "RomulanRadGun";
		}
		else if ( Q_stricmpn( "models/weapons/worldmodel-knife.tik" , sTikiName.c_str(), 35 ) == 0)
		{
			sWeaponName = "Knife";
		}
		else if ( Q_stricmpn( "models/weapons/worldmodel-enterprise.tik" , sTikiName.c_str(), 40 ) == 0)
		{
			sWeaponName = "EnterpriseCannon";
		}
	}
	return sWeaponName;
}