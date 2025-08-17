//-----------------------------------------------------------------------------
//
//  $Logfile:: /EF2/Code/DLLs/game/gamecmds.cpp                              $
// $Revision:: 54                                                             $
//     $Date:: 9/26/03 2:36p                                                  $
//
// Copyright (C) 1999 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source is may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
//
//
// DESCRIPTION:
//

#include "_pch_cpp.h"
#include "gamecmds.h"
#include "camera.h"
#include "viewthing.h"
#include "soundman.h"
#include "navigate.h"
#include "mp_manager.hpp"
#include "CinematicArmature.h"
#include <qcommon/gameplaymanager.h>
#include "botmenudef.h"


//--------------------------------------------------------------
// GAMEFIX - Added: to make gamefix functionality available - chrissstrahl
//--------------------------------------------------------------
#include "gamefix.hpp"


//--------------------------------------------------------------
// COOP Generation 7.000 - Added Include - chrissstrahl
//--------------------------------------------------------------
#ifdef ENABLE_COOP
#include "../../coop/code/coop_manager.hpp"
#include "../../coop/code/coop_radar.hpp"
#include "../../coop/code/coop_class.hpp"
#endif


typedef struct
{
	const char  *command;
	qboolean    ( *func )( const gentity_t *ent );
	qboolean    allclients;
} consolecmd_t;

consolecmd_t G_ConsoleCmds[] =
{
	//   command name       function             available in multiplayer?


#ifdef ENABLE_COOP
	//--------------------------------------------------------------
	// COOP Generation 7.000 - Added: coop console command specific functions - chrissstrahl
	//--------------------------------------------------------------
	{ "coopinstalled",coop_playerCoopDetected,true },
	{ "coopcid",coop_playerClientId,true },
	{ "coopinput",coop_playerInput,true },
	{ "coopradarscale",coop_playerRadarScale,true },

	{ "!thread",coop_playerThread,true },
	{ "!testspawn",coop_playerTestSpawn,true },
	{ "!follow",coop_playerFollowMe,true },
	{ "!leader",coop_playerLeader,true },
	{ "!login",coop_playerLogin,true },
	{ "!logout",coop_playerLogout,true },
	{ "!kill",coop_playerKill,true },
	{ "!origin",coop_playerOrigin,true },
	{ "!noclip",coop_playerNoclip,true },
	{ "!stuck",coop_playerStuck,true },
	{ "!transport",coop_playerTransport,true },
	{ "!notransport",coop_playerNotransport,true },
	{ "!showspawn",coop_playerShowspawn,true },
	{ "!transferlife",coop_playerTransferlife,true },
	{ "!ability",coop_playerAbility,true },
	{ "!targetnames",coop_playerTargetnames,true },
	{ "!levelend",coop_playerLevelend,true },
	{ "!drop",coop_playerDrop,true },
	{ "!skill",coop_playerSkill,true },
	{ "!info",coop_playerInfo,true },
	{ "!block",coop_playerBlock,true },
	{ "!mapname",coop_playerMapname,true },
	{ "!class",coop_playerClass,true },
	{ "!help",coop_playerHelp,true },

	{ "script",G_ScriptCmd,true },
	{ "clientrunthread",G_ClientRunThreadCmd,true },
	{ "dialogrunthread",G_DialogRunThread,true },
	//{ "serverthreadtorun",???,true },
#else
	{ "script",G_ScriptCmd,false },
	{ "dialogrunthread",	G_DialogRunThread,		false },
	{ "clientrunthread",	G_ClientRunThreadCmd,	false },
#endif


	//--------------------------------------------------------------
	// GAMEFIX - Added: Commands for german/english Language detection - G_ConsoleCmds - chrissstrahl
	//--------------------------------------------------------------
	{ "Eng",				gamefix_languageEng,	true },
	{ "Deu",				gamefix_languageDeu,	true },


	{ "vtaunt",				G_VTaunt,				true },
//	{ "vsay_team",			G_VTaunt,				true },
//	{ "vosay_team",			G_VTaunt,				true },
//	{ "vtell",				G_VTaunt,				true },
	{ "vsay",				G_SayCmd,				true },
	{ "vosay",				G_SayCmd,				true },
	{ "tell",				G_TellCmd,				true },
	{ "vtell",				G_TellCmd,				true },
	{ "say",				G_SayCmd,				true },
	{ "taunt",				G_TauntCmd,				true },
	{ "vosay_team",			G_TeamSayCmd,			true },
	{ "vsay_team",			G_TeamSayCmd,			true },
	{ "tsay",				G_TeamSayCmd,			true },
	{ "say_team",			G_TeamSayCmd,			true }, // added for BOTLIB
	{ "eventlist",			G_EventListCmd,			false },
	{ "pendingevents",		G_PendingEventsCmd,		false },
	{ "eventhelp",			G_EventHelpCmd,			false },
	{ "dumpevents",			G_DumpEventsCmd,		false },
	{ "classevents",		G_ClassEventsCmd,		false },
	{ "dumpclassevents",	G_DumpClassEventsCmd,	false },
	{ "dumpallclasses",		G_DumpAllClassesCmd,	false },
	{ "classlist",			G_ClassListCmd,			false },
	{ "classtree",			G_ClassTreeCmd,			false },
	{ "cam",				G_CameraCmd,			false },
	{ "snd",				G_SoundCmd,				false },
	{ "cin",				G_CinematicCmd,			false },
//	{ "showvar",			G_ShowVarCmd,			false },

	{ "clientsetvar",		G_ClientSetVarCmd,		false },
	{ "levelvars",			G_LevelVarsCmd,			false },
	{ "gamevars",			G_GameVarsCmd,			false },
	{ "loc",				G_LocCmd,				false },
	{ "warp",				G_WarpCmd,				false },
	{ "mask",				G_MaskCmd,				false },
	{ "setgameplayfloat",	G_SetGameplayFloatCmd,	false },
	{ "setgameplaystring",	G_SetGameplayStringCmd, false },
	{ "purchaseSkill",		G_PurchaseSkillCmd,		false },
	{ "swapItem",			G_SwapItemCmd,			false },
	{ "dropItem",			G_DropItemCmd,			false },
	//--------------------------------------------------------------
	// GAMEFIX - Fixed: warning: converting to non-pointer type int from NULL [-Wconversion-null] - chrissstrahl
	//--------------------------------------------------------------
	{ NULL,					NULL,					false }
};

void G_InitConsoleCommands( void )
{
	consolecmd_t *cmds;
	
	//
	// the game server will interpret these commands, which will be automatically
	// forwarded to the server after they are not recognized locally
	//
	gi.AddCommand( "give" );
	gi.AddCommand( "god" );
	gi.AddCommand( "notarget" );
	gi.AddCommand( "noclip" );
	gi.AddCommand( "kill" );
	gi.AddCommand( "script" );
	
	for( cmds = G_ConsoleCmds; cmds->command != NULL; cmds++ )
	{
		gi.AddCommand( cmds->command );
	}
}

qboolean	G_ConsoleCommand( void )
{
	gentity_t *ent;
	qboolean result;
	
	result = false;
	try
	{
		if ( dedicated->integer )
		{
			const char *cmd;

			cmd = gi.argv( 0 );


			//--------------------------------------------------------------
			// GAMEFIX - Fixed: Warning C4996 stricmp: The POSIX name for this item is deprecated. Using Q_stricmp instead. - chrissstrahl
			//--------------------------------------------------------------
			if ( Q_stricmp( cmd, "say" ) == 0 )
			{
				G_Say( NULL, false, false );
				result = true;
			}
		}


		//--------------------------------------------------------------
		// GAMEFIX - Added: Check if it is a valid server command, because it would otherwise print out commands send to server at G_ProcessClientCommand - daggolin - chrissstrahl
		//--------------------------------------------------------------
		if (!result){
			result = G_ServerCommand();
		}


		if ( !result )
		{
			ent = &g_entities[ 0 ];
			result = G_ProcessClientCommand( ent );
		}
	}
	
	catch( const char *error )
	{
		G_ExitWithError( error );
	}
	
	return result;
}

void G_ClientCommand( gentity_t *ent )
{
	try
	{
		//--------------------------------------------------------------
		// GAMEFIX - Added: sv_floodProtect replacement functions - chrissstrahl
		//--------------------------------------------------------------
		if (ent && ent->entity && !gamefixAPI_commandsUpdate(ent->entity->entnum, gi.argv(0))) {
			return;
		}


		if ( ent && !G_ProcessClientCommand( ent ) )
		{
			//--------------------------------------------------------------
			// GAMEFIX - Added: Convince Feature to strip leading /,^ or \ from console commands - chrissstrahl
			// Once the command has been stripped it will be send back to the client console it will then decide what to do with it
			//--------------------------------------------------------------
			const char* cmd;
			cmd = gi.argv(0);
			bool bFiltered = false;
			while (*cmd == '^' || *cmd == '/' || *cmd == '\\') {
				cmd++;
				bFiltered = true;
			}

			if (bFiltered && ent->entity && gi.GetNumFreeReliableServerCommands(ent->entity->entnum) > 32)
			{
				int cmdNum = 1;
				str cmdGlueUp = "";
				char* checkCmdPresent;

				cmdGlueUp = va("%s", cmd);

				checkCmdPresent = gi.argv(cmdNum);
				while (checkCmdPresent && strlen(checkCmdPresent) > 0) {
					cmdGlueUp += va(" %s", checkCmdPresent);
					cmdNum++;
					checkCmdPresent = gi.argv(cmdNum);
				}
				//send back to client and client console will decide it it wants to send it again
				gi.SendServerCommand(ent->entity->entnum, va("stufftext \"%s\"\n", cmdGlueUp.c_str()));
				return;
			}


			// anything that doesn't match a command will be a chat
			G_Say( ent, false, true );
		}
	}
	
	catch( const char *error )
	{
		G_ExitWithError( error );
	}
}

qboolean G_ProcessClientCommand( gentity_t *ent )
{
	const char	   *cmd;
	consolecmd_t   *cmds;
	int			   i;
	int            n;
	Event			   *ev;
	
	cmd = gi.argv( 0 );

	if ( !ent || !ent->client || !ent->entity || !ent->inuse )
	{
		// not fully in game yet
		return false;
	}
	
	for( cmds = G_ConsoleCmds; cmds->command != NULL; cmds++ )
	{
		// if we have multiple clients and this command isn't allowed by multiple clients, skip it

		//--------------------------------------------------------------
		// GAMEFIX - Fixed: sv_maxclient > 1 having a impact on script console commands in Singleplayer - chrissstrahl
		//--------------------------------------------------------------
		if ( ( game.maxclients > 1 && g_gametype->integer != GT_SINGLE_PLAYER ) && ( !cmds->allclients ) && !sv_cheats->integer )
		{
			continue;
		}

		
		if ( !Q_stricmp( cmd, cmds->command ) )
		{
			return cmds->func( ent );
		}
	}
	
	if ( Event::Exists( cmd ) )
	{
		ev = new Event( cmd );
		ev->SetSource( EV_FROM_CONSOLE );
		ev->SetConsoleEdict( ent );
		
		n = gi.argc();
		for( i = 1; i < n; i++ )
		{
			ev->AddToken( gi.argv( i ) );
		}
		
		if ( !Q_stricmpn( cmd, "ai_", 2 ) )
		{
			return thePathManager.ProcessEvent( ev );
		}
		else if ( !Q_stricmpn( cmd, "view", 4 ) )
		{
			return Viewmodel.ProcessEvent( ev );
		}
		else
		{
			if( ent && ent->entity )
				return ent->entity->ProcessEvent( ev );
		}
	}
	
	return false;
}

/*
==================
Cmd_Say_f
==================
*/
void G_Say( const gentity_t *ent, bool team, qboolean arg0 )
{
	str text;
	const char	*p;
	
	if ( gi.argc() < 2 && !arg0 )
	{
		return;
	}
	
	if ( arg0 )
	{
		text = gi.argv( 0 );
		text += " ";
		text += gi.args();
	}
	else
	{
		p = gi.args();
		
		if ( *p == '"' )
		{
			p++;
			text = p;
			text[ text.length() - 1 ] = 0;
		}
		else
		{
			text = p;
		}
	}
	
	if ( ent && ent->entity && ent->entity->isSubclassOf( Player ) )
	{
		multiplayerManager.say( (Player *)ent->entity, text, team );
	}
	else
	{
		multiplayerManager.say( NULL, text, false );
	}
}

qboolean G_CameraCmd( const gentity_t *ent )
{
	Event *ev;
	const char *cmd;
	int   i;
	int   n;
	
	n = gi.argc();
	if ( !n )
	{
		gi.WPrintf( "Usage: cam [command] [arg 1]...[arg n]\n" );
		return true;
	}
	
	cmd = gi.argv( 1 );
	if ( Event::Exists( cmd ) )
	{
		ev = new Event( cmd );
		ev->SetSource( EV_FROM_CONSOLE );
		ev->SetConsoleEdict( NULL );
		
		for( i = 2; i < n; i++ )
		{
			ev->AddToken( gi.argv( i ) );
		}
		
		CameraMan.ProcessEvent( ev );
	}
	else
	{
		gi.WPrintf( "Unknown camera command '%s'.\n", cmd );
	}
	
	return true;
}

//===============================================================
// Name:		G_CinematicCmd
// Class:		
//
// Description: Takes a cinematic command from the console, rips
//				off the first argument and looks up the second
//				as a cinematic armature event.  If found, sends
//				it off to the armature along with the rest of the
//				event tokens.
// 
// Parameters:	gentity_t* -- entity sending the event
//
// Returns:		qboolean -- true if processed.
// 
//===============================================================
qboolean G_CinematicCmd( const gentity_t *ent )
{
	Event		*ev ;
	const char	*cmd ;
	int			 n		= gi.argc();
	
	if ( !n )
	{
		gi.WPrintf( "Usage: cin <command> <arg1> ... <argn>\n");
		return true ;
	}
	
	cmd = gi.argv( 1 );
	if ( Event::Exists( cmd ) )
	{
		ev = new Event( cmd );
		ev->SetSource( EV_FROM_CONSOLE );
		ev->SetConsoleEdict( NULL );
		
		for (int i = 2; i < n; i++ )
		{
			ev->AddToken( gi.argv( i ) );
		}
		
		theCinematicArmature.ProcessEvent( ev );
	}
	else
	{
		gi.WPrintf( "Unknown cinematic armature command '%s'.\n", cmd );
	}
	
	return true ;
}

qboolean G_SoundCmd( const gentity_t *ent )
{
	Event *ev;
	const char *cmd;
	int   i;
	int   n;
	
	n = gi.argc();
	if ( !n )
	{
		gi.WPrintf( "Usage: snd [command] [arg 1]...[arg n]\n" );
		return true;
	}
	
	cmd = gi.argv( 1 );
	if ( Event::Exists( cmd ) )
	{
		ev = new Event( cmd );
		ev->SetSource( EV_FROM_CONSOLE );
		ev->SetConsoleEdict( NULL );
		
		for( i = 2; i < n; i++ )
		{
			ev->AddToken( gi.argv( i ) );
		}
		
		SoundMan.ProcessEvent( ev );
	}
	else
	{
		gi.WPrintf( "Unknown sound command '%s'.\n", cmd );
	}
	
	return true;
}

char *ClientName(int client, char *name, int size);
qboolean G_VTaunt( const gentity_t *ent )
{
// NOTE: vtant, vsay, vosay are q3's specific hotkey chat mechanisms.  we don't have these so this is a stub
// function right now.
/*
	gentity_t *who;
	int i;

	if (!ent->client) {
		return qfalse;
	}


	Player *player = (Player *)ent->entity;
	Player *enemy = multiplayerManager.getLastKilledByPlayer(player);

	// insult someone who just killed you
	if (ent->enemy && ent->enemy->client && ent->enemy->client->lastkilled_client == ent->s.number) {
		// i am a dead corpse
		if (!(ent->enemy->r.svFlags & SVF_BOT)) {
			G_Voice( ent, ent->enemy, SAY_TELL, VOICECHAT_DEATHINSULT, qfalse );
		}
		if (!(ent->r.svFlags & SVF_BOT)) {
			G_Voice( ent, ent,        SAY_TELL, VOICECHAT_DEATHINSULT, qfalse );
		}
		ent->enemy = NULL;
		return;
	}
	// insult someone you just killed
	if (ent->client->lastkilled_client >= 0 && ent->client->lastkilled_client != ent->s.number) {
		who = g_entities + ent->client->lastkilled_client;
		if (who->client) {
			// who is the person I just killed
			if (who->client->lasthurt_mod == MOD_GAUNTLET) {
				if (!(who->r.svFlags & SVF_BOT)) {
					G_Voice( ent, who, SAY_TELL, VOICECHAT_KILLGAUNTLET, qfalse );	// and I killed them with a gauntlet
				}
				if (!(ent->r.svFlags & SVF_BOT)) {
					G_Voice( ent, ent, SAY_TELL, VOICECHAT_KILLGAUNTLET, qfalse );
				}
			} else {
				if (!(who->r.svFlags & SVF_BOT)) {
					G_Voice( ent, who, SAY_TELL, VOICECHAT_KILLINSULT, qfalse );	// and I killed them with something else
				}
				if (!(ent->r.svFlags & SVF_BOT)) {
					G_Voice( ent, ent, SAY_TELL, VOICECHAT_KILLINSULT, qfalse );
				}
			}
			ent->client->lastkilled_client = -1;
			return;
		}
	}

	if (gametype >= GT_TEAM) {
		// praise a team mate who just got a reward
		for(i = 0; i < MAX_CLIENTS; i++) {
			who = g_entities + i;
			if (who->client && who != ent && who->client->sess.sessionTeam == ent->client->sess.sessionTeam) {
				if (who->client->rewardTime > level.time) {
					if (!(who->r.svFlags & SVF_BOT)) {
						G_Voice( ent, who, SAY_TELL, VOICECHAT_PRAISE, qfalse );
					}
					if (!(ent->r.svFlags & SVF_BOT)) {
						G_Voice( ent, ent, SAY_TELL, VOICECHAT_PRAISE, qfalse );
					}
					return;
				}
			}
		}
	}
*/
	// just say something
/*
	cmd = "vchat";
	}

	trap_SendServerCommand( other-g_entities, va("%s %d %d %d %s", cmd, voiceonly, ent->s.number, color, id));
*/

//	trap_SendServerCommand( other-g_entities, va("%s %d %d %d %s", cmd, voiceonly, ent->s.number, color, id));
/*
	GameplayManager
	gi.SendServerCommand(NULL,va("%s %d %d %d %s", "taunt", voiceonly, ent->s.number, color, id));
	//	G_Say( ent,false, va("%s","taunt") ); // VOICECHAT_TAUNT)); //  NULL, SAY_ALL, VOICECHAT_TAUNT, qfalse );
	char bogoname[1024];

	gi.SendServerCommand(ent->s.clientNum,va("hudsay \"%s: %s \"",ClientName(ent->s.clientNum,bogoname,1024),"taunt")); // ClientName(ent->s.clientNum,bogoname,1024),
*/

	return true;
}

qboolean G_TellCmd( const gentity_t *ent )
{
	str text;
	const char	*p;
	int i;
	int entnum;
	
	if ( gi.argc() < 3 )
	{
		return true;
	}

	entnum = atoi( gi.argv( 1 ) );
	
	for ( i = 2 ; i < gi.argc() ; i++ )
	{
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Command tell discarding space char, gluing words together - chrissstrahl
		//--------------------------------------------------------------
		if (i != 2) {
			text += " ";
		}

		
		p = gi.argv( i );
	
		if ( *p == '"' )
		{
			p++;
			text += p;
			text[ text.length() - 1 ] = 0;
		}
		else
		{
			text += p;
		}

	}
	
	if ( ent->entity && ent->entity->isSubclassOf( Player ) )
	{
		multiplayerManager.tell( (Player *)ent->entity, text, entnum );
	}

	return true;
}

qboolean G_SayCmd( const gentity_t *ent )
{
	G_Say( ent, false, false );

	return true;
}

qboolean G_TeamSayCmd( const gentity_t *ent )
{
	G_Say( ent, true, false );

	return true;
}

qboolean G_TauntCmd( const gentity_t *ent )
{
	//--------------------------------------------------------------
	// GAMEFIX - Added: Player Taunts not playing during cinematic - chrissstrahl
	//--------------------------------------------------------------
	if (level.cinematic) {
		return true;
	}
	
	if ( gi.argc() < 2 )
	{
		return true;
	}

	str tauntName;
	tauntName = "taunt";
	tauntName += gi.argv( 1 );
	
	if ( ent->entity && ent->entity->isSubclassOf( Player ) )
	{
		Player *player = (Player *)ent->entity;

		if ( multiplayerManager.inMultiplayer() && !multiplayerManager.isPlayerSpectator( player ) )
		{
			//--------------------------------------------------------------
			// GAMEFIX - Fixed: MP-Taunt Cooldown, the variable was there but nothing was implemented - chrissstrahl
			//--------------------------------------------------------------
			if (multiplayerManager.getNextTauntTime(player) <= level.time) {
				multiplayerManager.setNextTauntTime(player);


				//ent->entity->Sound( tauntName, CHAN_TAUNT, DEFAULT_VOL, LEVEL_WIDE_MIN_DIST );
				ent->entity->Sound(tauntName, CHAN_TAUNT, DEFAULT_VOL, 250.0f);
			}
		}
	}

	return true;
}

qboolean G_LocCmd( const gentity_t *ent )
{
	if ( ent )
	{
		gi.Printf( "Origin = ( %f, %f, %f )   Angles = ( %f, %f, %f )\n", 
			ent->currentOrigin[0], ent->currentOrigin[1], ent->currentOrigin[2],
			ent->currentAngles[0], ent->currentAngles[1], ent->currentAngles[0] );
	}
	
	return true;
}

qboolean G_WarpCmd( const gentity_t *ent )
{
	Vector pos;

	if ( sv_cheats->integer == 0 )
	{
		return true;
	}
	
	if ( ent )
	{
		Entity *entity;
		
		if ( ent->entity )
		{
			// Get the new position
			
			if ( gi.argc() == 2 )
			{
				pos = Vector(gi.argv( 1 ));
			}
			else if ( gi.argc() == 4 )
			{
				pos[ 0 ] = atof( gi.argv( 1 ) );
				pos[ 1 ] = atof( gi.argv( 2 ) );
				pos[ 2 ] = atof( gi.argv( 3 ) );
			}
			else
			{
				gi.Printf( "Incorrect parms\n" );
				return false;
			}
			
			// Move the entity to the new position
			
			entity = ent->entity;
			entity->setOrigin( pos );
		}
	}
	
	return true;
}

qboolean G_MaskCmd( const gentity_t *ent )
{
	Vector pos;
	
	// Check parms
	
	if ( gi.argc() != 2 )
	{
		gi.Printf( "Incorrect parms\n" );
		return true;
	}
	
	if ( ent )
	{
		Entity *entity;
		
		if ( ent->entity )
		{
			entity = ent->entity;
			
			// Get and set new mask
			

			//--------------------------------------------------------------
			// GAMEFIX - Fixed: Warning C4996 stricmp: The POSIX name for this item is deprecated. Using Q_stricmp instead. - chrissstrahl
			// 
			// Replaced 2 times
			//--------------------------------------------------------------
			if ( Q_stricmp( gi.argv( 1 ), "monster" ) == 0 )
			{
				entity->edict->clipmask	 = MASK_MONSTERSOLID;
			}
			else if ( Q_stricmp( gi.argv( 1 ), "player" ) == 0 )
			{
				entity->edict->clipmask	 = MASK_PLAYERSOLID;
			}
			else
			{
				gi.Printf( "Unknown mask name - %s\n", gi.argv( 1 ) );
			}
		}
	}
	
	return true;
}

qboolean G_EventListCmd( const gentity_t *ent )
{
	const char *mask;
	
	mask = NULL;
	if ( gi.argc() > 1 )
	{
		mask = gi.argv( 1 );
	}
	
	Event::ListCommands( mask );
	
	return true;
}

qboolean G_PendingEventsCmd( const gentity_t *ent )
{
	const char *mask;
	
	mask = NULL;
	if ( gi.argc() > 1 )
	{
		mask = gi.argv( 1 );
	}
	
	Event::PendingEvents( mask );
	
	return true;
}

qboolean G_EventHelpCmd( const gentity_t *ent )
{
	const char *mask;
	
	mask = NULL;
	if ( gi.argc() > 1 )
	{
		mask = gi.argv( 1 );
	}
	
	Event::ListDocumentation( mask, false );
	
	return true;
}

qboolean G_DumpEventsCmd( const gentity_t *ent )
{
	const char *mask;
	
	mask = NULL;
	if ( gi.argc() > 1 )
	{
		mask = gi.argv( 1 );
	}
	
	Event::ListDocumentation( mask, true );
	
	return true;
}

qboolean G_ClassEventsCmd( const gentity_t *ent )
{
	const char *className;
	
	className = NULL;
	if ( gi.argc() < 2 )
	{
		gi.WPrintf( "Usage: classevents [className]\n" );
		className = gi.argv( 1 );
	}
	else
	{
		className = gi.argv( 1 );
		ClassEvents( className );
	}
	return true;
}

qboolean G_DumpClassEventsCmd( const gentity_t *ent )
{
	const char *className;
	
	className = NULL;
	if ( gi.argc() < 2 )
	{
		gi.WPrintf( "Usage: dumpclassevents [className]\n" );
		className = gi.argv( 1 );
	}
	else
	{
		className = gi.argv( 1 );
		ClassEvents( className, true );
	}
	return true;
}

qboolean G_DumpAllClassesCmd( const gentity_t *ent )
{
	const char *tmpstr = NULL;
	const char *filename = NULL;
	int typeFlag = EVENT_ALL;
	int outputFlag = OUTPUT_ALL;
	
	if ( gi.argc() > 1 )
	{   
		tmpstr = gi.argv( 1 );
		if ( !strcmp("tiki", tmpstr ) )
			typeFlag = EVENT_TIKI_ONLY;
		if ( !strcmp("script", tmpstr ) )
			typeFlag = EVENT_SCRIPT_ONLY;
	}
	if ( gi.argc() > 2 )
	{
		tmpstr = gi.argv( 2 );
		if ( !strcmp("html", tmpstr ) )
			outputFlag = OUTPUT_HTML;
		if ( !strcmp("cmdmap", tmpstr ) )
			outputFlag = OUTPUT_CMD;
	}
	if ( gi.argc() > 3 )
	{
		filename = gi.argv( 3 );
	}
	
	DumpAllClasses(typeFlag, outputFlag, filename);
	
	return true;
}

qboolean G_ClassListCmd( const gentity_t *ent )
{
	listAllClasses();
	
	return true;
}

qboolean G_ClassTreeCmd( const gentity_t *ent )
{
	if ( gi.argc() > 1 )
	{
		listInheritanceOrder( gi.argv( 1 ) );
	}
	else
	{
		gi.SendServerCommand( ent - g_entities, "print \"Syntax: classtree [classname].\n\"" );
	}
	
	return true;
}

/*
qboolean G_ShowVarCmd( gentity_t *ent )
{
	ScriptVariable *var;

	var = Director.GetExistingVariable( gi.argv( 1 ) );
	if ( var )
	{
		gi.SendServerCommand( ent - g_entities, "print \"%s = '%s'\n\"", gi.argv( 1 ), var->stringValue() );
	}
	else
	{
		gi.SendServerCommand( ent - g_entities, "print \"Variable '%s' does not exist.\"", gi.argv( 1 ) );
	}
  
	return true;
} */

qboolean G_ScriptCmd( const gentity_t *ent )
{
	int i, argc;
	const char *argv[ 32 ];
	char args[ 32 ][ 64 ];
	
	argc = 0;
	for( i = 1; i < gi.argc(); i++ )
	{
		if ( argc < 32 )
		{
			strncpy( args[ argc ], gi.argv( i ), 64 );
			argv[ argc ] = args[ argc ];
			argc++;
		}
	}



	if ( argc > 0 )
	{


#ifdef ENABLE_COOP
		//--------------------------------------------------------------
		// COOP Generation 7.000 - Added: script functions execution - chrissstrahl
		// make tricorderpuzzel an other default gamplay components from singleplayer work
		//--------------------------------------------------------------
		if (gameFixAPI_inMultiplayer()) {
			if (ent->entity && ent->entity->isSubclassOf(Player)) {
				if (argc > 1 && Q_stricmp("thread", gi.argv(1)) == 0) {
					str ssds = gi.argv(2);
					if (CoopManager::Get().playerScriptCallExecute(ent->entity, "script", ssds, nullptr)) {
						return true;
					}
				}
				Player* player = (Player*)ent->entity;
				if (!player->coop_isAdmin()) {
					return false;
				}
			}
		}
#endif


		level.consoleThread->ProcessCommand( argc, argv );
	}
	
	return true;
}

qboolean G_ClientRunThreadCmd( const gentity_t *ent )
{
	str		threadName;
	CThread	*thread;
	
	// Get the thread name
	
	if ( !gi.argc() )
		return true;
	
	threadName = gi.argv( 1 );
	
	
	// Check to make sure player is allowed to run this thread
	
	// Need to do this part
	
	// Run the thread
	
	if ( !threadName.length() )
		return true;


#ifdef ENABLE_COOP
	//--------------------------------------------------------------
	// COOP Generation 7.000 - Added: script functions execution - chrissstrahl
	// make tricorderpuzzel an other default gamplay components from singleplayer work
	//--------------------------------------------------------------
	if (gameFixAPI_inMultiplayer()) {
		if (CoopManager::Get().playerScriptCallExecute(ent->entity, "clientrunthread", threadName, nullptr)) {
			return true;
		}
		return false;
	}
#endif
	

	thread = Director.CreateThread( threadName );
	
	if ( thread )
		thread->DelayedStart( 0.0f );
	
	return true;
}

qboolean G_ClientSetVarCmd( const gentity_t *ent )
{
	str		varName;
	str		value;
	
	
	if ( gi.argc() != 3 )
		return true;
	
	// Get the variable name
	
	varName = gi.argv( 1 );
	
	// Get the variable value
	
	value = gi.argv( 2 );
	
	// Check to make sure player is allowed to set this variable
	
	// Need to do this part
	
	// Set the variable
	
	if ( varName.length() && value.length() )
	{
		levelVars.SetVariable( varName, value );
	}
	
	return true;
}

//===============================================================
// Name:		G_SendCommandToAllPlayers
// Class:		None
//
// Description: Sends the specified command to all connected
//				clients (players).
// 
// Parameters:	const char* -- the command to send
//
// Returns:		qboolean -- qtrue if successfully sent.
// 
//===============================================================
qboolean G_SendCommandToAllPlayers( const char *command )
{
	bool retVal = true ;


	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Warning C4018: Signed/unsigned mismatch. - chrissstrahl
	//--------------------------------------------------------------
	for( unsigned int clientIdx = 0; clientIdx < (unsigned)maxclients->integer; ++clientIdx )
	{
		gentity_t *ent = g_entities + clientIdx ;
		if ( !ent->inuse || !ent->client || !ent->entity ) continue;

		if ( !G_SendCommandToPlayer( ent, command ) ) retVal = false ;
	}


	return retVal ;
}


//===============================================================
// Name:		G_SendCommandToPlayer
// Class:		
//
// Description: Sends the specified command to the specified player.
// 
// Parameters:	Player* -- the player to send it to
//				str		-- the command
//
// Returns:		None
// 
//===============================================================
qboolean G_SendCommandToPlayer( const gentity_t *ent, const char *command )
{
	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Added NULL check to prevent crash - chrissstrahl
	//--------------------------------------------------------------
	if (!ent) {
		gi.Printf("G_SendCommandToPlayer - gentity_t/Player was NULL\n");
		return true;
	}

	assert( ent );
	Entity *entity = ent->entity ;
	
	assert( entity );
	assert( entity->isSubclassOf( Player ) );
	Player *player = ( Player* )entity ;
	
	str		builtCommand("stufftext \"");
	builtCommand += command ;
	builtCommand += "\"\n";
	gi.SendServerCommand( player->edict - g_entities, builtCommand.c_str() );
	
	return true ;
}


//===============================================================
// Name:		G_EnableWidgetOfPlayer
// Class:		
//
// Description: Enables (or disables) the specified widget of 
//				the specified player.
// 
// Parameters:	const gentity_t*	-- the player gentity_t to send this command to.
//				const char*			-- the name of the widget to enable/disable
//				bool				-- true means enable
//
// Returns:		None
// 
//===============================================================
qboolean G_EnableWidgetOfPlayer( const gentity_t *ent, const char *widgetName, qboolean enableFlag )
{
	assert( widgetName );

	str command("globalwidgetcommand ");
	command += widgetName ;
	command += ( enableFlag ) ? " enable" : " disable" ;

	return G_SendCommandToPlayer( ent, command.c_str());
}


//===============================================================
// Name:		G_SetWidgetTextOfPlayer
// Class:		
//
// Description: Sets the widget text of the widget for the speicifed player
// 
// Parameters:	const gentity_t*	-- the player gentity_t to send this command to.
//				const char*			-- the name of the widget to set the text on
//				const char*			-- text the put on the widget
//
// Returns:		None
// 
//===============================================================
qboolean G_SetWidgetTextOfPlayer( const gentity_t *ent, const char *widgetName, const char *widgetText )
{
	assert( widgetName );
	char tmpstr[4096];
	if ( strlen(widgetText) > 4095 )
		assert(0);
	
	strcpy(tmpstr, widgetText);
	

	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Warning C4018: Signed/unsigned mismatch. - chrissstrahl
	//--------------------------------------------------------------
	for (unsigned int i=0; i<strlen(widgetText); i++ )
	{
		if ( tmpstr[i] == '\n' )
			tmpstr[i] = '~';
		if ( tmpstr[i] == ' ' )
			tmpstr[i] = '^';
	}
	

	str command("globalwidgetcommand ");
	command += widgetName ;
	command += " labeltext " ;
	command += tmpstr ;
	
	return G_SendCommandToPlayer( ent, command.c_str());
}

void PrintVariableList( ScriptVariableList * list )
{
	ScriptVariable *var;
	int i;
	
	for( i = 1; i <= list->NumVariables(); i++ )
	{
		var = list->GetVariable( i );
		gi.Printf( "%s = %s\n", var->getName(), var->stringValue() );
	}
	gi.Printf( "%d variables\n", list->NumVariables() );
}

qboolean G_LevelVarsCmd( const gentity_t *ent )
{
	gi.Printf( "Level Variables\n" );
	PrintVariableList( &levelVars );
	
	return true;
}

qboolean G_GameVarsCmd( const gentity_t *ent )
{
	gi.Printf( "Game Variables\n" );
	PrintVariableList( &gameVars );
	
	return true;
}

//--------------------------------------------------------------
//
// Name:			G_SetGameplayFloatCmd
// Class:			None
//
// Description:		Sets properties on the gameplay database.
//
// Parameters:		const gentity_t *ent -- Entity, not used
//
// Returns:			qboolean
//
//--------------------------------------------------------------
qboolean G_SetGameplayFloatCmd( const gentity_t *ent )
{
	str objname;
	str propname;
	str create = "0";
	float value = 1.0f;

	// Check for not enough args
	if ( gi.argc() < 4 )
		return qfalse;

	objname = gi.argv( 1 );
	propname = gi.argv( 2 );
	value = (double)atof(gi.argv( 3 ));
	if ( gi.argc() > 4 )
		create = gi.argv( 4 );

	if ( create == "0" )
		GameplayManager::getTheGameplayManager()->setFloatValue(objname, propname, value);
	else
		GameplayManager::getTheGameplayManager()->setFloatValue(objname, propname, value, true);

	gi.Printf("Gameplay Modified -- %s's %s changed to %g\n", objname.c_str(), propname.c_str(), value);

	return qtrue;
}


//--------------------------------------------------------------
//
// Name:			G_SetGameplayStringCmd
// Class:			None
//
// Description:		Sets properties on the gameplay database.
//
// Parameters:		const gentity_t *ent -- Entity, not used
//
// Returns:			qboolean
//
//--------------------------------------------------------------
qboolean G_SetGameplayStringCmd( const gentity_t *ent )
{
	str objname;
	str propname;
	str valuestr;
	str create = "0";

	// Check for not enough args
	if ( gi.argc() < 4 )
		return qfalse;

	objname = gi.argv( 1 );
	propname = gi.argv( 2 );
	valuestr = gi.argv( 3 );
	if ( gi.argc() > 4 )
		create = gi.argv( 4 );

	if ( create == "0" )
		GameplayManager::getTheGameplayManager()->setStringValue(objname, propname, valuestr);
	else
		GameplayManager::getTheGameplayManager()->setStringValue(objname, propname, valuestr, true);

	gi.Printf("Gameplay Modified -- %s's %s changed to %s\n", objname.c_str(), propname.c_str(), valuestr.c_str());

	return qtrue;
}


//===============================================================
// Name:		G_PurchaseSkillCmd
// Class:		None
//
// Description: Purchases a skill for the current player.  This
//				goes into the gameplay database and looks for a
//				CurrentPlayer object who has a skillPoints property.
//
//				If its found, we attempt to increment the value
//				of the skill specified in the command's first argument.
//				We also decrement by one the number of available skillPoints.
// 
// Parameters:	gentity_t -- the entity issuing the command. Not used.
//
// Returns:		qboolean -- true if the command was executed.
// 
//===============================================================
qboolean G_PurchaseSkillCmd( const gentity_t *ent )
{
	str propname ;

	if ( gi.argc() < 1 ) return qfalse ;

	propname = gi.argv( 1 );

	GameplayManager *gpm = GameplayManager::getTheGameplayManager();
	if ( !gpm ) return false ;

	float skillPoints = gpm->getFloatValue( "CurrentPlayer", "SkillPoints" );
	if ( skillPoints > 0.0f )
	{
		str objName("CurrentPlayer.");
		objName += propname ;
		float skillValue = gpm->getFloatValue( objName, "value" );
		if ( skillValue < gpm->getFloatValue( objName, "max" ) )
		{
			gpm->setFloatValue( objName, "value", skillValue + 1.0f );
			gpm->setFloatValue( "CurrentPlayer", "SkillPoints", skillPoints - 1.0f );

			if ( ent->entity->isSubclassOf( Player ) )
			{
				Player *player = (Player*)ent->entity;
				player->skillChanged( objName );
			}
		}
	}

	return qtrue ;
}


//===============================================================
// Name:		G_SwapItemCmd
// Class:		None
//
// Description: Swaps an inventory item with the currently held item
//				of that type.  This is fairly specific at this point
//				to use the Gameplay Database in a particular way.
//
//				The command takes a single string, which defines an
//				inventory slot object.  We retrieve the name of the
//				item in that inventory slot.  If it isn't found, there
//				isn't an item there and nothing happens.
//
//				If it is found, we determine if the player already has
//				an item of that type in their local inventory (hands).
//				This is done by again checking the database.  If they
//				do have a weapon of that type, we swap this one with
//				that.  Otherwise we just give him this weapon.
// 
// Parameters:	gentity_t -- the entity issuing the command.  This
//							 is the player who issued the command.
//
// Returns:		qboolean -- true if the command was executed.
// 
//===============================================================
qboolean G_SwapItemCmd( const gentity_t *ent )
{
	if ( gi.argc() < 1 ) return qfalse ;
	if ( !ent->entity->isSubclassOf( Player ) ) return qfalse ;
	
	Player		*player			= (Player*)ent->entity ;
	str			 objectName		= gi.argv( 1 );
	str			 propertyName	= gi.argv( 2 );
	str			 heldItem ;
	weaponhand_t hand			= WEAPON_RIGHT;
	
	GameplayManager *gpm = GameplayManager::getTheGameplayManager();
	if ( !gpm ) return false ;
	
	str itemName	= gpm->getStringValue( objectName, propertyName );
	str tikiName	= gpm->getStringValue( itemName, "model" );
	str itemType	= gpm->getStringValue( itemName, "class" );
	str	playerItem("CurrentPlayer." + itemType);
	
	if ( !gpm->hasObject(playerItem) )
	{
		gi.WPrintf( "Warning: Unknown item type %s for item %s", itemType.c_str(), itemName.c_str() );
		return qfalse ;
	}
	
	// Get the currently held item, and replace it with the item we clicked on
	heldItem = gpm->getStringValue( playerItem, "name" );
	gpm->setStringValue( playerItem, "name", itemName );
	
	// Ranged weapons are left hand
	if ( itemType == "Ranged" )
		hand = WEAPON_LEFT ;
	
	// Remove the currently held item from the player's inventory
	player->takeItem( heldItem.c_str() );
	
	// Give the player the new item we clicked on
	player->giveItem( tikiName );
	
	// Use the weapon we clicked on if we picked a weapon of the same type
	// that we have currently equipped.  Otherwise, we just swap out
	// the slots
	Weapon *weap = 0;
	weap = player->GetActiveWeapon(WEAPON_RIGHT);
	if ( !weap )
		weap = player->GetActiveWeapon(WEAPON_LEFT);
	if ( weap )
	{
		str tmpstr = gpm->getStringValue(weap->getArchetype(), "class");
		if ( tmpstr == itemType )
			player->useWeapon( itemType, hand );
	}
	
	// Put the held item in inventory slot the clicked item was in.
	gpm->setStringValue( objectName, propertyName, heldItem );
	
	return qtrue ;
}

//--------------------------------------------------------------
//
// Name:			G_DropItemCmd
// Class:			None
//
// Description:		Drops an item that is clicked on from the inventory
//
// Parameters:		gentity_t -- the entity issuing the command.  This
//							 is the player who issued the command.
//
// Returns:			qboolean -- true if the command was executed.	
//
//--------------------------------------------------------------
qboolean G_DropItemCmd( const gentity_t *ent )
{
	if ( gi.argc() < 1 ) return false ;
	if ( !ent->entity->isSubclassOf( Player ) ) return false ;
	
	Player		*player			= (Player*)ent->entity ;
	str			 objectName		= gi.argv( 1 );
	str			 propertyName	= gi.argv( 2 );
	
	GameplayManager *gpm = GameplayManager::getTheGameplayManager();
	if ( !gpm ) return false ;
	
	str itemName	= gpm->getStringValue( objectName, propertyName );
	if ( itemName == "Empty" )
		return false;
	
	str tikiName	= gpm->getStringValue( itemName, "model" );
	str itemType	= gpm->getStringValue( itemName, "class" );
	str	playerItem("CurrentPlayer." + itemType);
	
	if ( !gpm->hasObject(playerItem) )
	{
		gi.WPrintf( "Warning: Unknown item type %s for item %s", itemType.c_str(), itemName.c_str() );
		return false ;
	}
	
	// Empty the slot
	gpm->setStringValue( objectName, propertyName, "Empty" );
	
	// Give the player the item to drop
	//Item *givenItem = player->giveItem(tikiName);
	
	Weapon *dropweap;
	ClassDef *cls;
	cls = getClass( tikiName.c_str() );
	if ( !cls )
	{
		SpawnArgs args;
		args.setArg( "model", tikiName.c_str() );
		cls = args.getClassDef();
		if ( !cls )
			return false;
	}
	dropweap = ( Weapon * )cls->newInstance();
	dropweap->setModel( tikiName.c_str() );
	dropweap->ProcessPendingEvents();
	dropweap->SetOwner(player);
	dropweap->hideModel();
	dropweap->setAttached(true);
	dropweap->Drop();
	
	return true ;
}

qboolean G_DialogRunThread( const gentity_t *ent )
{
	if (!ent || !ent->entity || !ent->client) {
		return false;
	}

	if (ent->entity->getHealth() <= 0) {
		return false;
	}

	if (g_gametype->integer == GT_SINGLE_PLAYER) {
		// clear out the current dialog actor
		if (ent->entity->isSubclassOf(Player))
		{
			Player* player = (Player*)ent->entity;
			player->clearBranchDialogActor();
		}
		return G_ClientRunThreadCmd( ent );
	}
#ifdef ENABLE_COOP
	//--------------------------------------------------------------
	// COOP Generation 7.000 - Allow Cinematic Dialog Selection Options - chrissstrahl
	//--------------------------------------------------------------
	else if(CoopManager::Get().IsCoopEnabled() && gameFixAPI_inMultiplayer()){
		if (!ent->entity->isSubclassOf(Player)) {
			return false;
		}

		Player* player = (Player*)ent->entity;
		Actor* actor = player->coop_getBranchDialogActor();
		if (actor && gi.argc()) {

			//cancel any old failsafes
			actor->CancelEventsOfType(EV_Actor_coop_branchDialogFailsafe);

			str sGivenThread = gi.argv(1);
			str sDialogName = actor->coop_getBranchDialogName();
			if (sDialogName.length() && sGivenThread.length()) {

				player->clearBranchDialogActor();

				if (CoopManager::Get().playerScriptCallExecute(ent->entity, "dialogrunthread", sGivenThread, (Entity*)actor)) {
					return true;
				}
			}
		}
	}
#endif
	return false;
}


#ifdef ENABLE_COOP
//--------------------------------------------------------------
// COOP Generation 7.000 - Added: coop console command specific functions - chrissstrahl
//--------------------------------------------------------------
qboolean coop_playerCoopDetected(const gentity_t* ent) {
	if (!ent || !ent->entity || !ent->client || g_gametype->integer == GT_SINGLE_PLAYER) {
		return true;
	}

	CoopManager::Get().playerCoopDetected(ent, gi.argv(1));
	return true;
}
qboolean coop_playerClientId(const gentity_t* ent) {
	if (!ent || !ent->entity || !ent->client || g_gametype->integer == GT_SINGLE_PLAYER) {
		return true;
	}

	CoopManager::Get().playerClIdDetected(ent, gi.argv(1));
	return true;
}

qboolean coop_playerThread(const gentity_t* ent)
{
	if (!ent || !ent->inuse || !ent->client || !gi.argc())
		return true;

	if (g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		return true;
	}

	const str threadName = gi.argv(1);
	Player* player = (Player*)ent->entity;

	//needs coopThread_ prefix
	if (!threadName.length() ||	gamefix_findString(threadName.c_str(), "coopThread_",false) != 0) {
		return true;
	}

	if (!gameFixAPI_isHost(player) || !player->coop_isAdmin() ) {
		return true;
	}

	CThread* pThread;
	str sPrint = "^5Succsessfully ran func";

	pThread = ExecuteThread(threadName, true,(Entity*)player);
	if (pThread == nullptr) { sPrint = "^2FAILED to run func"; }
	player->hudPrint(va("%s:^3 %s\n", sPrint.c_str(), threadName.c_str()));	

	return true;
}

qboolean coop_playerInput(const gentity_t* ent)
{
	if (!ent || !ent->inuse || !ent->client)
		return qfalse;

	if (ent->entity->getHealth() <= 0) {
		return true;
	}

	if (!gi.argc())
		return true;

	str inputData = gi.argv(1);

	//Grab more data
	for (int i = 2; i < 32; i++) {
		str sGrabMe = gi.argv(i);
		if (sGrabMe.length()) {
			inputData = va("%s %s", inputData.c_str(), sGrabMe.c_str());
		}
	}

	if (!inputData.length())
		return false;

	Player* player = (Player*)ent->entity;
	str authStringPlayerNew = "";

	//if !login is active add input to coopPlayer.adminAuthString instead
	//also update the cvar that is shown in the login menu of the communicator
	if (multiplayerManager.inMultiplayer() && CoopManager::Get().getPlayerData_coopAdminAuthStarted(player)) {
		if (inputData == "clear") {
			CoopManager::Get().setPlayerData_coopAdminAuthString(player,"");
		}
		else {
			authStringPlayerNew = CoopManager::Get().getPlayerData_coopAdminAuthString(player);
			authStringPlayerNew += inputData;
			CoopManager::Get().setPlayerData_coopAdminAuthString(player, authStringPlayerNew);
		}

		gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand coop_comCmdLoginCode title '%s'\n", authStringPlayerNew.c_str()));
		return true;
	}

	//limit of data that can be actually used
	if (inputData.length() > 260) { //(264) make sure we have space for linebreak
		inputData = gamefix_getStringLength(inputData,0,259);
	}

	ent->entity->entityVars.SetVariable("coopInputData", inputData.c_str());

	ExecuteThread("playerInput", true, (Entity*)player);
	return true;
}

qboolean coop_playerTestSpawn(const gentity_t* ent)
{
	if (!ent || !ent->inuse || !ent->client)
		return qfalse;

	if (sv_cinematic->integer) {
		return true;
	}

	Player* player = (Player*)ent->entity;
	//deny usage of command if player executed command to quickly
	if ((gamefix_getEntityVarFloat((Entity*)player, "!testspawn") + 3) > level.time) {
		return true;
	}
	player->entityVars.SetVariable("!testspawn", level.time);

	if (!CoopManager::Get().getPlayerData_coopAdmin(player)) {
		player->hudPrint(va(_COOP_INFO_adminLogin_needAdminUse,"!testspawn"));
		return true;
	}

	ExecuteThread("globalCoop_level_testSpawn", true, (Entity*)player);
	return true;
}

qboolean coop_playerRadarScale(const gentity_t* ent)
{
	if (!ent || !ent->inuse || !ent->client)
		return qfalse;

	if (!gi.argc())
		return true;

	const char* coopRadarScale = gi.argv(1);
	if (strlen(coopRadarScale) == 0) {
		return true;
	}

	float scale = atoi(coopRadarScale);
	if (scale > _COOP_SETTINGS_RADAR_SCALE_MAX) { scale = _COOP_SETTINGS_RADAR_SCALE_MAX;	}
	else if (scale < _COOP_SETTINGS_RADAR_SCALE_MIN) { scale = _COOP_SETTINGS_RADAR_SCALE_MIN; }
	
	//force update of blips in the next frame
	Player* player = (Player*)ent->entity;
	coop_radarReset(player);
	CoopManager::Get().setPlayerData_radarScale(player, scale);
	return qtrue;
}

qboolean coop_playerFollowMe(const gentity_t* ent)
{
	//deny usage of command if player executed command to quickly
	if (!ent || !ent->entity || (gamefix_getEntityVarFloat((Entity*)ent->entity, "!follow") + 1.5) > level.time) {
		return true;
	}

	if (sv_cinematic->integer) {
		return true;
	}

	Player* player = (Player*)ent->entity;
	player->entityVars.SetVariable("!follow", level.time);

	if (player->getHealth() <= 0) {
		return true;
	}

	if (g_gametype->integer == GT_SINGLE_PLAYER || g_gametype->integer == GT_BOT_SINGLE_PLAYER || !CoopManager::Get().IsCoopLevel()) {
		player->hudPrint(_COOP_INFO_coopCommandOnly);
		return true;
	}
	if (sv_cinematic->integer || multiplayerManager.inMultiplayer() && multiplayerManager.isPlayerSpectator(player)) {
		return true;
	}

	//remember if player using command is currently shown on radar (as missionobjective blip)
	bool bDisable = (bool)ent->entity->edict->s.missionObjective;

	//reset missionobjective blip on all players
	gentity_t* gentity;
	for (int i = 0; i < maxclients->integer; i++) {
		gentity = &g_entities[i];
		if (gentity->inuse && gentity->entity && gentity->client && gentity->entity->isSubclassOf(Player)) {
			gentity->entity->edict->s.missionObjective = 0;
		}
	}

	//If blip is enabled for player disable (toggle)
	if (bDisable) {
		str text = _COOP_INFO_radarFollowMarker_off;
		if (player->coop_hasLanguageGerman()) {
			text = _COOP_INFO_radarFollowMarker_off_deu;
		}
		multiplayerManager.HUDPrint(player->entnum, va("%s", text.c_str()));
		ent->entity->edict->s.missionObjective;
		return true;
	}

	//Otherwise enable missionobjective blip for player
	ent->entity->edict->s.missionObjective = 1;

	//print message to all player huds of player being marked
	for (int i = 0; i < maxclients->integer; i++) {
		gentity_t* gentity2 = &g_entities[i];
		if (gentity2->inuse && gentity2->entity && gentity2->client && gentity2->entity->isSubclassOf(Player)) {
			Player* currentPlayer = (Player*)gentity2->entity;
			if (currentPlayer) {
				str text = _COOP_INFO_radarFollowMarker_on;
				if (currentPlayer->coop_hasLanguageGerman()) {
					text = _COOP_INFO_radarFollowMarker_on_deu;
				}
				multiplayerManager.HUDPrint(currentPlayer->entnum, va("%s: %s\n", text.c_str(), player->client->pers.netname));
			}
		}
	}
	return true;
}

qboolean coop_playerLeader(const gentity_t* ent)
{	
	//deny usage of command if player executed command to quickly
	if (!ent || !ent->entity || (gamefix_getEntityVarFloat((Entity*)ent->entity, "!leader") + 3) > level.time) {
		return true;
	}

	if (sv_cinematic->integer) {
		return true;
	}

	Player* player = (Player*)ent->entity;
	player->entityVars.SetVariable("!leader", level.time);

	if (player->getHealth() <= 0) {
		return true;
	}

	if (g_gametype->integer == GT_SINGLE_PLAYER || g_gametype->integer == GT_BOT_SINGLE_PLAYER || !CoopManager::Get().IsCoopLevel()) {
		player->hudPrint(_COOP_INFO_coopCommandOnly);
		return true;
	}
	
	if (sv_cinematic->integer || multiplayerManager.inMultiplayer() && multiplayerManager.isPlayerSpectator(player)) {
		return true;
	}

	player->hudPrint("!leader - Not implemented yet.\n");
	gi.Printf("!leader - Not implemented yet.\n");
	//multiplayerManager.callVote(player, "leader", va("%i", player->entnum));
	return true;
}

qboolean coop_playerLogin(const gentity_t* ent)
{
	//deny usage of command if player executed command to quickly
	if (!ent || !ent->entity || (gamefix_getEntityVarFloat((Entity*)ent->entity, "!login") + 3) > level.time) {
		return true;
	}
	Player* player = (Player*)ent->entity;
	player->entityVars.SetVariable("!login", level.time);

	if (!player->coop_hasCoopInstalled()) {
		player->hudPrint(_COOP_INFO_adminLogin_needLatestCoop);
		return true;
	}

	if (CoopManager::Get().getPlayerData_coopAdmin(player)) {
		player->hudPrint(_COOP_INFO_adminLogin_useLogout);
		return true;
	}
	
	CoopManager::Get().setPayerData_coopAdminAuthStarted(player, true);
	CoopManager::Get().setPlayerData_coopAdminAuthString(player,"");
	CoopManager::Get().setPlayerData_coopAdminAuthStringLengthLast(player,-1);
	gamefix_playerDelayedServerCommand(player->entnum,va("pushmenu %s", _COOP_UI_NAME_communicator));
	gamefix_playerDelayedServerCommand(player->entnum, "globalwidgetcommand coop_comCmdLoginCode title ''");
	gamefix_playerDelayedServerCommand(player->entnum,va(_COOP_INFO_adminLogin_auth, gamefix_replaceForLabelText(_COOP_INFO_adminLogin_loginStartedEnter).c_str()));
	return true;
}
qboolean coop_playerLogout(const gentity_t* ent)
{
	if (!ent || !ent->entity) {
		return true;
	}

	Player* player = (Player*)ent->entity;
	player->entityVars.SetVariable("!logout", level.time);

	str sMessage = _COOP_INFO_adminLogin_logoutAlready;
	if (CoopManager::Get().getPlayerData_coopAdmin(player)) {
		CoopManager::Get().setPlayerData_coopAdmin(player, false);
		player->entityVars.SetVariable("coop_login_authorisation", "*");
		sMessage = _COOP_INFO_adminLogin_logoutDone;
	}

	if ((gamefix_getEntityVarFloat((Entity*)player, "!logout") + 3) > level.time) {
		return true;
	}
	player->entityVars.SetVariable("!logout", level.time);
	player->hudPrint(sMessage.c_str());

	return true;
}

qboolean coop_playerKill(const gentity_t* ent)
{
	if (!ent || !ent->inuse || !ent->client || !ent->entity) {
		return true;
	}

	if (sv_cinematic->integer) {
		return true;
	}

	Player* player = (Player*)ent->entity;
	if (player->getHealth() <= 0) {
		return true;
	}
	
	if (gi.GetNumFreeReliableServerCommands(player->entnum) > 32){
		gi.SendServerCommand(player->entnum, "stufftext \"kill\"\n");
	}
	return true;
}

qboolean coop_playerOrigin(const gentity_t* ent)
{
	if (!ent || !ent->inuse || !ent->client || !ent->entity) {
		return true;
	}

	if (ent->entity->getHealth() <= 0) {
		return true;
	}

	if ((gamefix_getEntityVarFloat(ent->entity, "!origin") + 1) > level.time) {
		return true;
	}
	ent->entity->entityVars.SetVariable("!origin", level.time);


	Player* player = (Player*)ent->entity;
	player->hudPrint(va("^5Your ^3origin is:^8 %i %i %i\n", (int)ent->entity->origin[1], (int)ent->entity->origin[1], (int)ent->entity->origin[2]));

	Entity* targetedEntity = player->GetTargetedEntity();
	if (targetedEntity) {
		player->hudPrint(va("^3Targeted ^5'$%s'^3 origin is:^8 %i %i %i\n", targetedEntity->targetname.c_str(), (int)targetedEntity->origin[0], (int)targetedEntity->origin[1], (int)targetedEntity->origin[2]));
	}
	
	return true;
}

qboolean coop_playerNoclip(const gentity_t* ent)
{
	if (!ent || !ent->inuse || !ent->client || !ent->entity) {
		return true;
	}

	if (ent->entity->getHealth() <= 0) {
		return true;
	}

	if ((gamefix_getEntityVarFloat(ent->entity, "!origin") + 1) > level.time) {
		return true;
	}
	ent->entity->entityVars.SetVariable("!origin", level.time);

	Player* player = (Player*)ent->entity;
	if (!CoopManager::Get().getPlayerData_coopAdmin(player)) {
		player->hudPrint(va(_COOP_INFO_adminLogin_needAdminUse,"!noclip"));
		return true;
	}
	
	player->hudPrint(_COOP_INFO_usedCommand_noclip);
	extern Event EV_Player_DevNoClipCheat;
	player->ProcessEvent(EV_Player_DevNoClipCheat);
	return true;
}

qboolean coop_playerStuck(const gentity_t* ent)
{
	if (!ent || !ent->inuse || !ent->client || !ent->entity || g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		return true;
	}

	if (sv_cinematic->integer) {
		return true;
	}

	if (ent->entity->getHealth() <= 0) {
		return true;
	}
	
	if ((gamefix_getEntityVarFloat(ent->entity, "!stuck") + 1) > level.time) {
		return true;
	}
	ent->entity->entityVars.SetVariable("!stuck", level.time);


	Player* player = (Player*)ent->entity;
	if (!CoopManager::Get().IsCoopEnabled()) {
		player->hudPrint(_COOP_INFO_coopCommandOnly);
		return true;
	}

	Vector vOriginB4 = player->origin;
	CoopManager::Get().setPlayerData_spawnLocationSpawnForced(player,true);
	CoopManager::Get().playerMoveToSpawn(player);
	Vector vOriginDATA = player->origin;

	//compare player locations to determin if player is at a bad spawnspot
	//level z-axis out so we ignore falling players
	vOriginDATA[2] = 0;
	vOriginB4[2] = 0;
	int spawnSpotToTry = gamefix_getEntityVarInt((Entity*)player, "!stuck_spot");
	//cycle spawnspots
	spawnSpotToTry++;
	if (spawnSpotToTry < 1) {
		spawnSpotToTry = 1;
	}
	if (spawnSpotToTry == (player->entnum + 1)) {
		spawnSpotToTry++;
	}
	if (spawnSpotToTry > _COOP_SETTINGS_PLAYER_SPAWNSPOT_MAX) {
		spawnSpotToTry = 1;
	}

	if (VectorLength(vOriginB4 - vOriginDATA) < 100 || spawnSpotToTry != 0 && spawnSpotToTry != (player->entnum + 1)) {
		//player->entityVars.SetVariable("!transport", 0.0f);
		//coop_playerTransport(ent);

		ent->entity->entityVars.SetVariable("!stuck_spot", spawnSpotToTry);

		bool moved = CoopManager::Get().playerMoveToSpawnSpecific(player,spawnSpotToTry);
		if (moved) {
			if (gi.GetNumFreeReliableServerCommands(player->entnum) > 32)
			{
				if (player->coop_hasLanguageGerman()) {
					player->hudPrint(va(_COOP_INFO_usedCommand_stuck2_deu, spawnSpotToTry));
				}
				else {
					player->hudPrint(va(_COOP_INFO_usedCommand_stuck2, spawnSpotToTry));
				}
			}
		}
		return true;
	}

	if (gi.GetNumFreeReliableServerCommands(player->entnum) > 32)
	{
		if (player->coop_hasLanguageGerman()) {
			player->hudPrint(_COOP_INFO_usedCommand_stuck1_deu);
		}
		else {
			player->hudPrint(_COOP_INFO_usedCommand_stuck1);
		}
	}
	return true;
}


qboolean coop_playerTransport(const gentity_t* ent)
{
	if (!ent || !ent->inuse || !ent->client || !ent->entity || g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		return true;
	}

	if (sv_cinematic->integer) {
		return true;
	}

	if (ent->entity->getHealth() <= 0) {
		return true;
	}

	Player* player = (Player*)ent->entity;
	if ((gamefix_getEntityVarFloat((Entity*)player, "!transport") + 5) > level.time) {
		if (player->coop_hasLanguageGerman()) {
			player->hudPrint(_COOP_INFO_usedCommand_transport1_deu);
		}
		else {
			player->hudPrint(_COOP_INFO_usedCommand_transport1);
		}
		return true;
	}

	//coop only command
	if (!CoopManager::Get().IsCoopEnabled()) {
		player->hudPrint(_COOP_INFO_coopCommandOnly);
		return true;
	}

	//deny request during cinematic and in spec [b607] chrissstrahl - moved health check here
	if (sv_cinematic->integer || multiplayerManager.isPlayerSpectator(player) || player->health <= 0) {
		return true;
	}

	bool bTransportFailed = false;
	const char* cmd;
	int   n;
	n = gi.argc();
	cmd = gi.argv(1);


	int iPlayer = -1;
	if (n > 1) {
		str sId = va("%s", cmd);

		int i;
		for (i = 0; i < sId.length(); i++) {
			if (isdigit(sId[i])) {
				sId = sId[i];
				iPlayer = atoi(sId.c_str());
			}
		}
	}

	Player* targetPlayer = NULL;
	//beam directly to a given player number
	if (iPlayer >= 0) {
		if (&g_entities[iPlayer] && g_entities[iPlayer].client && g_entities[iPlayer].inuse) {
			targetPlayer = (Player*)g_entities[iPlayer].entity;
		}
	}
	//given player does not exist or no number was given
	if (!targetPlayer) {
		//more than 2 players, transport to closest player
		if (CoopManager::Get().getNumberOfPlayers(true,true) > 1) {
			targetPlayer = gamefix_getClosestPlayer(player);
		}
		else
		{
			if (player->coop_hasLanguageGerman()) {
				player->hudPrint(_COOP_INFO_usedCommand_transport2_deu);
			}
			else {
				player->hudPrint(_COOP_INFO_usedCommand_transport2);
			}
			bTransportFailed = true;
		}
	}

	if (!targetPlayer || multiplayerManager.isPlayerSpectator(targetPlayer) || targetPlayer->health <= 0) {
		bTransportFailed = true;
	}
	//prevent transport to target player if player has !notransport activated
	else {
		if (gamefix_getEntityVarInt((Entity*)targetPlayer, "!notransport_active") == 1) {
			bTransportFailed = true;

			if (player->coop_hasLanguageGerman()) {
				player->hudPrint(_COOP_INFO_usedCommand_transport3_deu);
			}
			else {
				player->hudPrint(_COOP_INFO_usedCommand_transport3);
			}
		}
		//[b60014] chrissstrahl - prevent transport if player has !notransport activated - do not allow terrorising other players
		else {
			if (gamefix_getEntityVarInt((Entity*)player, "!notransport_active") == 1) {
				bTransportFailed = true;

				if (player->coop_hasLanguageGerman()) {
					player->hudPrint(_COOP_INFO_usedCommand_transport4_deu);
				}
				else {
					player->hudPrint(_COOP_INFO_usedCommand_transport4);
				}
			}
		}
	}

	if (targetPlayer == player) {
		if (player->coop_hasLanguageGerman()) {
			player->hudPrint(_COOP_INFO_usedCommand_transport5);
		}
		else {
			player->hudPrint(_COOP_INFO_usedCommand_transport5);
		}
		bTransportFailed = true;
	}

	if (bTransportFailed) {
		return true;
	}

	player->entityVars.SetVariable("!transport", level.time);

	//make notsolid - so nobody gets stuck
	ent->entity->setSolidType(SOLID_NOT);

	//use the circle menu code to make the weapon go away and reapear after transport
	str lastWeapon = "None";
	player->getActiveWeaponName(WEAPON_ANY, lastWeapon);

	Event* StopFireEvent;
	StopFireEvent = new Event(EV_Sentient_StopFire);
	StopFireEvent->AddString("dualhand");
	player->ProcessEvent(StopFireEvent);

	Event* deactivateWeaponEv;
	deactivateWeaponEv = new Event(EV_Player_DeactivateWeapon);
	deactivateWeaponEv->AddString("dualhand");
	player->PostEvent(deactivateWeaponEv, 0.05);

	player->disableInventory();

	//[b60025] chrissstrahl - set code to restore weapon
	Event* useWeaponEv;
	useWeaponEv = new Event(EV_Player_UseItem);
	useWeaponEv->AddString(lastWeapon);
	useWeaponEv->AddString("dualhand");
	player->PostEvent(useWeaponEv, 2.0f);

	StopFireEvent = new Event(EV_Sentient_StopFire);
	StopFireEvent->AddString("dualhand");
	player->PostEvent(StopFireEvent, 1.9f);

	Event* activateWeaponEv;
	activateWeaponEv = new Event(EV_Player_ActivateNewWeapon);
	player->PostEvent(activateWeaponEv, 1.9f);


	player->client->ps.pm_time = 100;
	player->client->ps.pm_flags |= PMF_TIME_TELEPORT;

	player->client->ps.pm_time = 100;
	player->client->ps.pm_flags |= PMF_TIME_TELEPORT;

	Event* newEvent2 = new Event(EV_DisplayEffect);
	newEvent2->AddString("TransportIn");
	newEvent2->AddString("Multiplayer");
	player->PostEvent(newEvent2, 0.0f);

	player->origin = targetPlayer->origin;
	player->SetViewAngles(targetPlayer->angles);

	CoopManager::Get().playerTransported(player);
	return true;
}

qboolean coop_playerNotransport(const gentity_t* ent)
{
	if (!ent || !ent->inuse || !ent->client || !ent->entity || g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		return true;
	}

	Player* player = (Player*)ent->entity;
	if ((gamefix_getEntityVarFloat((Entity*)player, "!notransport") + 3) > level.time) {
		return true;
	}
	player->entityVars.SetVariable("!notransport", level.time);


	//coop only command
	if (!CoopManager::Get().IsCoopEnabled()) {
		player->hudPrint(_COOP_INFO_coopCommandOnly);
		return true;
	}

	if (gamefix_getEntityVarFloat((Entity*)player, "!notransport_active") == 0.0f) {
		player->entityVars.SetVariable("!notransport_active", 1.0f);

		if (player->coop_hasLanguageGerman()) {
			player->hudPrint(_COOP_INFO_usedCommand_notransport1_deu);
		}
		else {
			player->hudPrint(_COOP_INFO_usedCommand_notransport1);
		}
	}
	else {
		player->entityVars.SetVariable("!notransport_active", 0.0f);

		if (player->coop_hasLanguageGerman()) {
			player->hudPrint(_COOP_INFO_usedCommand_notransport1_deu);
		}
		else {
			player->hudPrint(_COOP_INFO_usedCommand_notransport1);
		}
	}

	return true;
}

qboolean coop_playerShowspawn(const gentity_t* ent)
{
	if (!ent || !ent->inuse || !ent->client || !ent->entity || g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		return true;
	}

	if (sv_cinematic->integer) {
		return true;
	}

	if (ent->entity->getHealth() <= 0) {
		return true;
	}

	
	if ((gamefix_getEntityVarFloat(ent->entity, "!showspawn") + 1) > level.time) {
		return qtrue;
	}
	ent->entity->entityVars.SetVariable("!showspawn", level.time);

	Player* player = (Player*)ent->entity;
	if (!CoopManager::Get().getPlayerData_coopAdmin(player)) {
		player->hudPrint(va(_COOP_INFO_adminLogin_needAdminUse,"!showspawn"));
		return qtrue;
	}

	ExecuteThread("globalCoop_level_showSpawn", true, ent->entity);
	return qtrue;
}

qboolean coop_playerTransferlife(const gentity_t* ent)
{
	if (!ent || !ent->inuse || !ent->client || !ent->entity || g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		return true;
	}

	if (sv_cinematic->integer) {
		return true;
	}

	if (ent->entity->getHealth() <= 0) {
		return true;
	}

	
	if ((gamefix_getEntityVarFloat(ent->entity, "!transferlife") + 3) > level.time) {
		return true;
	}
	ent->entity->entityVars.SetVariable("!transferlife", level.time);


	//coop only command
	Player* player = (Player*)ent->entity;
	if (!CoopManager::Get().IsCoopEnabled()) {
		player->hudPrint(_COOP_INFO_coopCommandOnly);
		return true;
	}

	player->hudPrint("!transferlife - Not Implemented\n");
	gi.Printf("!transferlife - Not Implemented\n");

	/*

	//transfer live
	bool hasTransferedLive = false;
	int iLivesRemaining = (coop_lmsGetLives() - player->coopPlayer.lmsDeaths);

	//if not a spectator player needs one live for him self
	if (!multiplayerManager.isPlayerSpectator(player)) {
		iLivesRemaining--;
	}

	//check if player can give away a live at all
	if (iLivesRemaining > 0) {

		Player* playerOther = NULL;
		for (int i = 0; i < maxclients->integer; i++) {
			playerOther = (Player*)g_entities[i].entity;
			if (!hasTransferedLive && playerOther && player != playerOther && playerOther->isSubclassOf(Player) && !playerOther->upgPlayerIsBot()) {
				if (coop_lmsRevivePlayer(playerOther)) {
					//THE RECEIVER
					if (playerOther->upgPlayerHasLanguageGerman()) {
						multiplayerManager.HUDPrint(playerOther->entnum, va("^5Coop ^2Last Man Standing^8 %s ^2hat 1 Leben transferiert.\n", player->client->pers.netname));
					}
					else {
						multiplayerManager.HUDPrint(playerOther->entnum, va("^5Coop ^2Last Man Standing^8 %s ^2has transfered 1 live to you.\n", player->client->pers.netname));
					}
					//print info - of lives
					playerOther->coop_lmsInfo();

					//THE DONOR
					player->coopPlayer.lmsDeaths++;
					if (player->upgPlayerHasLanguageGerman()) {
						multiplayerManager.HUDPrint(player->entnum, va("^5Coop ^2Last Man Standing^5 1 ^2Leben transferiert an^8 %s.\n", playerOther->client->pers.netname));
					}
					else {
						multiplayerManager.HUDPrint(player->entnum, va("^5Coop ^2Last Man Standing^5 1 ^2live transfered to^8 %s.\n", playerOther->client->pers.netname));
					}
					//print info - of lives
					player->coop_lmsInfo();

					hasTransferedLive = true;
					return true;
				}
			}
		}
		//has not transfered live
		if (!hasTransferedLive) {
			//print message to let player know whats going on
			if (player->upgPlayerHasLanguageGerman()) {
				multiplayerManager.HUDPrint(player->entnum, va("^5Coop ^2Last Man Standing - Kein valieden Spieler zum Transfer gefunden.\n"));
			}
			else {
				multiplayerManager.HUDPrint(player->entnum, va("^5Coop ^2Last Man Standing - No valid Player for transfer found.\n"));
			}
		}
	}
	//Can not transfer live
	else {
		//print message to let player know whats going on
		if (player->upgPlayerHasLanguageGerman()) {
			multiplayerManager.HUDPrint(player->entnum, va("^5Coop ^2Last Man Standing - Nicht genug Leben zum Transfer.\n"));
		}
		else {
			multiplayerManager.HUDPrint(player->entnum, va("^5Coop ^2Last Man Standing - Not enough lives for transfer.\n"));
		}
	}*/
	return true;
}

qboolean coop_playerAbility(const gentity_t* ent)
{
	if (!ent || !ent->inuse || !ent->client || !ent->entity || g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		return true;
	}

	if (sv_cinematic->integer) {
		return true;
	}

	if (ent->entity->getHealth() <= 0) {
		return true;
	}

	Player* player = (Player*)ent->entity;
	if (!CoopManager::Get().IsCoopEnabled()) {
		player->hudPrint(_COOP_INFO_coopCommandOnly);
		return true;
	}

	if (multiplayerManager.isPlayerSpectator(player)) {
		return true;
	}
	
	//deny usage of command if player executed command to quickly
	float cooldownTime = gamefix_getEntityVarFloat((Entity*)player, "!ability");
	cooldownTime += COOP_CLASS_REGENERATION_COOLDOWN;
	if ((cooldownTime - level.time) >= 1) { //make sure we don't get the "less than a sec remaining" situation
		//have printout on a cooldown for 3 sec
		float cooldownTimePrintout = gamefix_getEntityVarFloat((Entity*)player, "!abilityPrintout");
		if (cooldownTimePrintout < level.time) {
			player->entityVars.SetVariable("!abilityPrintout", (level.time + 3));
			if (player->coop_hasLanguageGerman()) {
				player->hudPrint(va("^5Coop Klasse Talent^2 am abklang^5 %d ^2sekunden verbleibend.\n", (int)(cooldownTime - level.time)));
			}
			else {
				player->hudPrint(va("^5Coop Class ability^2 in cool-down^5 %d ^2secounds remaining.\n", (int)(cooldownTime - level.time)));
			}
		}
		return true;
	}

	player->entityVars.SetVariable("!ability", level.time);
	player->entityVars.SetVariable("!abilityPrintout", (level.time + 3));

	if (player->coop_hasLanguageGerman()) {
		player->hudPrint(va("^5Coop Klasse Talent^2 eingesetzt, erneut bereit in:^5 %d.\n", (int)COOP_CLASS_REGENERATION_COOLDOWN));
	}
	else {
		player->hudPrint(va("^5Coop Class ability^2 used, ready again in:^5 %d.\n", (int)COOP_CLASS_REGENERATION_COOLDOWN));
	}

	//activate ability now
	coopClass.coop_classAbilityUse(player);
	return true;
}

qboolean coop_playerTargetnames(const gentity_t* ent)
{
	if (!ent || !ent->inuse || !ent->client || !ent->entity || g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		return true;
	}

	if (sv_cinematic->integer) {
		return true;
	}

	if (ent->entity->getHealth() <= 0) {
		return true;
	}

	Player* player = (Player*)ent->entity;
	if ((gamefix_getEntityVarFloat((Entity*)player, "!targetnames") + 3) > level.time) {
		return qtrue;
	}
	player->entityVars.SetVariable("!targetnames", level.time);

	if (!CoopManager::Get().getPlayerData_coopAdmin(player)) {
		player->hudPrint(va(_COOP_INFO_adminLogin_needAdminUse, "!targetnames"));
		return qtrue;
	}

	if (CoopManager::Get().getPlayerData_targetedShow(player)) {
		CoopManager::Get().setPlayerData_targetedShow(player,false);
		player->hudPrint(_COOP_INFO_usedCommand_targetnames1);
	}
	else {
		CoopManager::Get().setPlayerData_targetedShow(player,true);
		player->hudPrint(_COOP_INFO_usedCommand_targetnames2);
	}
	return qtrue;
}

qboolean coop_playerLevelend(const gentity_t* ent)
{
	if (!ent || !ent->inuse || !ent->client || !ent->entity || g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		return true;
	}

	if ((gamefix_getEntityVarFloat(ent->entity, "!levelend") + 10) > level.time) {
		return true;
	}
	ent->entity->entityVars.SetVariable("!levelend", level.time);

	Player* player = (Player*)ent->entity;
	if (!CoopManager::Get().getPlayerData_coopAdmin(player)) {
		player->hudPrint(va(_COOP_INFO_adminLogin_needAdminUse,"!levelend"));
		return true;
	}

	CThread* thread = ExecuteThread("coop_endLevel", true, (Entity*)player);
	if (thread == NULL) {
		player->hudPrint(_COOP_INFO_usedCommand_levelend1);
	}
	else {
		player->hudPrint(_COOP_INFO_usedCommand_levelend2);
	}
	return true;
}
qboolean coop_playerDrop(const gentity_t* ent)
{
	if (!ent || !ent->inuse || !ent->client || !ent->entity || g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		return true;
	}

	Player* player = (Player*)ent->entity;
	if (sv_cinematic->integer || multiplayerManager.inMultiplayer() && multiplayerManager.isPlayerSpectator(player)) {
		return true;
	}

	Weapon *weap = player->GetActiveWeapon(WEAPON_ANY);
	if (!weap) {
		return true;
	}

	if ((gamefix_getEntityVarFloat((Entity*)player, "!drop") + 3) > level.time) {
		return true;
	}
	player->entityVars.SetVariable("!drop", level.time);
	
	if (g_gametype->integer == GT_SINGLE_PLAYER || g_gametype->integer == GT_BOT_SINGLE_PLAYER || !CoopManager::Get().IsCoopEnabled()) {
		player->hudPrint(_COOP_INFO_coopCommandOnly);
		return true;
	}

	str weaponName;
	player->getActiveWeaponName(WEAPON_ANY, weaponName);

	if (!weap->IsDroppable() ||
		Q_stricmpn("None", weaponName.c_str(), 4) == 0 ||
		Q_stricmpn("EnterpriseCannon", weaponName.c_str(), 4) == 0 ||
		Q_stricmpn("Batleth", weaponName.c_str(), 4) == 0 ||
		Q_stricmpn("Phaser", weaponName.c_str(), 6) == 0 ||
		Q_stricmpn("Tricorder", weaponName.c_str(), 9) == 0)
	{
		if (player->coop_hasLanguageGerman()) {
			player->hudPrint(_COOP_INFO_usedCommand_drop1_deu);
		}
		else {
			player->hudPrint(_COOP_INFO_usedCommand_drop1);
		}
		return true;
	}

	
	//Sentient* playerSent = (Sentient*)player;
	//Item* itemNew = playerSent->PrevItem((Item*)weap);

	weap->Drop();

	//Event* ev1;
	//ev1 = new Event(EV_InventoryItem_Use);
	//ev1->AddEntity(player);
	//itemNew->ProcessEvent(ev1);

	//this would make the weapon disapear :(
	//player->animate->ClearTorsoAnim();

	return true;
}

qboolean coop_playerSkill(const gentity_t* ent)
{
	if (!ent || !ent->inuse || !ent->client || !ent->entity || g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		return true;
	}

	if ((gamefix_getEntityVarFloat(ent->entity, "!skill") + 3) > level.time) {
		return true;
	}
	ent->entity->entityVars.SetVariable("!skill", level.time);

	Player* player = (Player*)ent->entity;
	if (!CoopManager::Get().IsCoopEnabled()) {
		player->hudPrint(_COOP_INFO_coopCommandOnly);
		return true;
	}

	//NO ARGUMENT GIVEN
	int n = gi.argc();
	if (n == 1) {
		if (gi.GetNumFreeReliableServerCommands(player->entnum) > 32) {
			int currentSkill = (int)skill->value;
			str printMe = "^5Coop^2: ";
			if (player->coop_hasLanguageGerman()) {
				printMe += "Schwierigkeit bei: ";
			}
			else {
				printMe += "Current SKILL is: ";
			}
			printMe += currentSkill;

			if (currentSkill == 0)
				printMe += " [$$Easy$$]";
			else if (currentSkill == 1)
				printMe += " [$$Normal$$]";
			else if (currentSkill == 2)
				printMe += " [$$Hard$$]";
			else
				printMe += " [$$VeryHard$$]";

			printMe += "\n";
			player->hudPrint(printMe);
		}
		return true;
	}

	//deny request during cinematic
	if (sv_cinematic->integer) {
		return true;
	}

	if (1) {
		player->hudPrint("!skill - Not fully implemented yet.\n");
		gi.Printf("!skill - Not fully implemented yet.\n");
		return true;
	}
	
	//get skill level input
	const char* cmd = gi.argv(1);
	str sVal = cmd[0];
	int iRange = atoi(sVal.c_str());
	if (iRange < 0 || iRange > 3) {
		if (gi.GetNumFreeReliableServerCommands(player->entnum) > 32) {
			if (player->coop_hasLanguageGerman()) {
				player->hudPrint("^5Coop^2: Illegale Angabe! Optionen: 0^8[$$Easy$$]^2, bis 3^8[$$Very Hard$$]\n");
			}
			else {
				player->hudPrint("^5Coop^2: Invalide range! Range is: 0^8[$$Easy$$]^2, to 3^8[$$Very Hard$$]\n");
			}
		}
		return true;
	}

	//hzm coop mod chrissstrahl - callvote if valid skill has been requested
	str command = "stufftext \"callvote skill ";
	command += cmd[0];
	if (gi.GetNumFreeReliableServerCommands(player->entnum) > 32)
	{
		command += "\n";
		gi.SendServerCommand(player->entnum, command);
	}

	return true;
}

qboolean coop_playerInfo(const gentity_t* ent)
{
	if (!ent || !ent->inuse || !ent->client || !ent->entity || g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		return true;
	}

	if (sv_cinematic->integer) {
		return true;
	}

	Player* player = (Player*)ent->entity;
	if (gi.GetNumFreeReliableServerCommands(player->entnum) < 48 ||
		multiplayerManager.inMultiplayer() && multiplayerManager.isPlayerSpectator(player)) {
		return true;
	}

	if ((gamefix_getEntityVarFloat((Entity*)player, "!info") + 10) > level.time) {
		return true;
	}
	player->entityVars.SetVariable("!info", level.time);

	if (g_gametype->integer == GT_SINGLE_PLAYER || g_gametype->integer == GT_BOT_SINGLE_PLAYER || !CoopManager::Get().IsCoopEnabled()) {
		player->hudPrint(_COOP_INFO_coopCommandOnly);
		return true;
	}

	str s, s2;
	//[b60014] chrissstrahl printout the info to menu
	if (CoopManager::Get().getPlayerData_coopVersion(player) >= 60014) {
		str sInfoPrint = va("Vers: %i - Client: %i\n", CoopManager::Get().getPlayerData_coopVersion(player), player->entnum);
		int iTemp_do_delete_me_upon_completing_all_implementatiosn = 1;
		sInfoPrint += va("Class: %s - Lives: %d of %d\n", CoopManager::Get().getPlayerData_coopClass(player).c_str(), iTemp_do_delete_me_upon_completing_all_implementatiosn /*coop_lmsGetLives() - player->coopPlayer.lmsDeaths*/, iTemp_do_delete_me_upon_completing_all_implementatiosn /*coop_lmsGetLives()*/);
		sInfoPrint += va("Lang: %s - Entered: %.2f\n", gamefix_getLanguage(player).c_str(), player->client->pers.enterTime);
		sInfoPrint += va("Personal Id: %s\n", CoopManager::Get().getPlayerData_coopClientId(player).c_str());

		sInfoPrint += "\nSERVER:\n";
#ifdef WIN32
		str sys2 = "Win";
#else
		str sys2 = "Lin";
#endif
		sInfoPrint += va("%d %s [%s %s]\n", _COOP_THIS_VERSION, sys2.c_str(), __DATE__, __TIME__);
		if (skill->integer == 0)
			s2 = "Easy";
		else if (skill->integer == 1)
			s2 = "Normal";
		else if (skill->integer == 2)
			s2 = "Hard";
		else
			s2 = "VeryHard";

		sInfoPrint += va("Lang: %s - Skill: %s - FriendlyFire: %.2f\n", local_language->string, s2.c_str(), coopSettings.getSetting_friendlyFireMultiplicator());
		sInfoPrint += va("Map: %s\n", level.mapname.c_str());
		str sChecksum = "ERROR";
		cvar_t* var;
		var = gi.cvar("sv_mapchecksum", "", 0);
		if (var) {
			if (var->string) {
				sChecksum = var->string;
			}
		}
		sInfoPrint += va("sv_mapchecksum: %s\n", sChecksum.c_str());
		sInfoPrint = gamefix_replaceForLabelText(sInfoPrint);
		gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand coop_comCmdI0 labeltext %s", sInfoPrint.c_str()));
		return true;
	}

	//coop not installed
	player->hudPrint(_COOP_INFO_usedCommand_info1);
	player->hudPrint(va("^5Version^8: %i, \n", CoopManager::Get().getPlayerData_coopVersion(player)));
	player->hudPrint(va("^5Class^8: %s, ", CoopManager::Get().getPlayerData_coopClass(player).c_str()));
	player->hudPrint(va("^5Lang^8: %s, ^5Client:^8 %d, ^5Entered:^8 %.2f", gamefix_getLanguage(player).c_str(), player->entnum, player->client->pers.enterTime));
	player->hudPrint(va("^5Personal Id^8: %s\n", CoopManager::Get().getPlayerData_coopClientId(player).c_str()));

	player->hudPrint("===SERVER ===\n");
	player->hudPrint(va("^5Map:^8 %s\n", level.mapname.c_str()));
	player->hudPrint(va("^5Language:^8 %s",local_language->string));

	if (skill->integer == 0)
		s = " [$$Easy$$]";
	else if (skill->integer == 1)
		s = " [$$Normal$$]";
	else if (skill->integer == 2)
		s = " [$$Hard$$]";
	else
		s = " [$$VeryHard$$]";
	player->hudPrint(va("^5Dificulty:^8 %d %s\n", skill->integer, s.c_str()));
	player->hudPrint(va("^5Friendly Fire Multiplier:^8 %.2f\n", coopSettings.getSetting_friendlyFireMultiplicator()));
#ifdef WIN32
	str sys = "Windows";
#else
	str sys = "Linux";
#endif
	player->hudPrint(va("^5HZM Coop Mod [ %i ]^8 [ %s ] - ^3Compiled:^8 %s %s\n", _COOP_THIS_VERSION, sys.c_str(), __DATE__, __TIME__));
	return true;
}

qboolean coop_playerBlock(const gentity_t* ent)
{
	if (!ent || !ent->inuse || !ent->client || !ent->entity || g_gametype->integer == GT_SINGLE_PLAYER || sv_cinematic->integer || !multiplayerManager.inMultiplayer() || g_gametype->integer == GT_BOT_SINGLE_PLAYER ) {
		return true;
	}

	Player* player = (Player*)ent->entity;
	if (multiplayerManager.isPlayerSpectator(player)) {
		return true;
	}
	if (!CoopManager::Get().IsCoopEnabled()) {
		player->hudPrint(_COOP_INFO_coopCommandOnly);
		return true;
	}

	constexpr auto COOP_COOLDOWN_CMD_BLOCK = 9;
	float fCoolDownTime = gamefix_getEntityVarFloat((Entity*)player, "!block");
	if ((fCoolDownTime + COOP_COOLDOWN_CMD_BLOCK) > level.time) {
		player->hudPrint(va("^5!hudprint^8, has a Cooldown please wait %d sec\n", ((fCoolDownTime + COOP_COOLDOWN_CMD_BLOCK) - level.time)));
		return true;
	}
	player->entityVars.SetVariable("!block", level.time);

	//hzm coop mod chrissstrahl - allow to walk trugh a player that is currently blocking, this player needs to aim at the blocking player
	Entity* target;
	target = player->GetTargetedEntity();
	if ((target) && target->health > 0 && target->isSubclassOf(Player)) {
		Player* targetPlayer = (Player*)target;
		targetPlayer->setSolidType(SOLID_NOT);
		
		gamefix_setMakeSolidAsap(targetPlayer,true, (level.time + _COOP_SETTINGS_PLAYER_BLOCK_NOTSOLID_TIME));

		if (player->coop_hasLanguageGerman()) {
			player->hudPrint(_COOP_INFO_usedCommand_block1_deu);
		}
		else {
			player->hudPrint(_COOP_INFO_usedCommand_block1);
		}
	}
	else {
		if (player->coop_hasLanguageGerman()) {
			player->hudPrint(_COOP_INFO_usedCommand_block2_deu);
		}
		else {
			player->hudPrint(_COOP_INFO_usedCommand_block2);
		}
	}
	return true;
}

qboolean coop_playerMapname(const gentity_t* ent)
{
	if (!ent || !ent->inuse || !ent->client || !ent->entity || g_gametype->integer == GT_SINGLE_PLAYER || sv_cinematic->integer || !multiplayerManager.inMultiplayer() || g_gametype->integer == GT_BOT_SINGLE_PLAYER) {
		return true;
	}
	
	if ((gamefix_getEntityVarFloat(ent->entity, "!mapname") + 3) > level.time) {
		return true;
	}
	ent->entity->entityVars.SetVariable("!mapname", level.time);

	Player* player = (Player*)ent->entity;
	if (gi.GetNumFreeReliableServerCommands(player->entnum) > 32)
	{
		if (player->coop_hasLanguageGerman()) {
			player->hudPrint(va(_COOP_INFO_usedCommand_mapname_deu, level.mapname.c_str()));
		}
		else {
			player->hudPrint(va(_COOP_INFO_usedCommand_mapname, level.mapname.c_str()));
		}
	}
	return true;
}

qboolean coop_playerClass(const gentity_t* ent)
{
	if (!ent || !ent->inuse || !ent->client || !ent->entity || g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer() || g_gametype->integer == GT_BOT_SINGLE_PLAYER) {
		return true;
	}

	Player* player = (Player*)ent->entity;
	if (!CoopManager::Get().IsCoopEnabled()) {
		player->hudPrint(_COOP_INFO_coopCommandOnly);
		return true;
	}

	//locked
	if (CoopManager::Get().getPlayerData_coopClassLocked(player)) {
		if (player->coop_hasLanguageGerman()) {
			player->hudPrint(_COOP_INFO_usedCommand_class1_deu);
		}
		else {
			player->hudPrint(_COOP_INFO_usedCommand_class1);
		}

		DEBUG_LOG(va("#coop_playerClass can't change class anymore for %s\n", player->client->pers.netname));
		gi.Printf(va("#coop_playerClass can't change class anymore for %s\n", player->client->pers.netname));

		return true;
	}
	
	//NO ARGUMENT GIVEN - show current
	int n = gi.argc();
	if (n == 1) {
		//[b60012][cleanup] chrissstrahl - this could be put into a func
		if (gi.GetNumFreeReliableServerCommands(player->entnum) > 32) {
			if (player->coop_hasLanguageGerman()) {
				player->hudPrint(va("%s %s\n", _COOP_INFO_usedCommand_class2_deu, CoopManager::Get().getPlayerData_coopClass(player).c_str()));
			}
			else {
				player->hudPrint(va("%s %s\n", _COOP_INFO_usedCommand_class2, CoopManager::Get().getPlayerData_coopClass(player).c_str()));
			}
		}
		return true;
	}

	//[b60021] chrissstrahl - disabled saving of client data here, why would we save here, also this saves imidiately after joining the game, which we don't want
	//hzm coop mod chrissstrahl - remember current health/armor/ammo status
	//coop_serverSaveClientData(player);

	//grab intended class
	str classSelected = gi.argv(1);
	classSelected.tolower();

	switch (classSelected[0]) {
	case 'h':
		classSelected = _COOP_NAME_CLASS_heavyWeapons;
		break;
	case 'm':
		classSelected = _COOP_NAME_CLASS_medic;
		break;
	case 't':
		classSelected = _COOP_NAME_CLASS_technician;
		break;
	default:
		classSelected = _COOP_NAME_CLASS_technician;


		if (gi.GetNumFreeReliableServerCommands(player->entnum) >= 32) {
			if (player->coop_hasLanguageGerman()) {
				player->hudPrint(_COOP_INFO_usedCommand_class3);
			}
			else {
				player->hudPrint(_COOP_INFO_usedCommand_class3_deu);
			}
		}
	}

	coopClass.coop_classSet(player, classSelected);
	coopClass.coop_classApplayAttributes(player, true);

	return true;
}

qboolean coop_playerHelp(const gentity_t* ent)
{
	if (!ent || !ent->inuse || !ent->client || !ent->entity || g_gametype->integer == GT_SINGLE_PLAYER || sv_cinematic->integer || !multiplayerManager.inMultiplayer() || g_gametype->integer == GT_BOT_SINGLE_PLAYER) {
		return true;
	}
	
	if ((gamefix_getEntityVarFloat(ent->entity, "!help") + 3) > level.time) {
		return true;
	}
	ent->entity->entityVars.SetVariable("!help", level.time);

	Player* player = (Player*)ent->entity;
	if (gi.GetNumFreeReliableServerCommands(player->entnum) > 32)
	{
		int printedItems_max = 5;
		int printedItems = 0;
		str sPrintString = "";
		player->hudPrint(_COOP_INFO_availableExclmationmarkCommands);
		for (int i = 1; i < gameFixAPI_AllowedCmdsContainer.NumObjects();i++) {
			if (gameFixAPI_AllowedCmdsContainer.ObjectAt(i).length()) {
				if (gameFixAPI_AllowedCmdsContainer.ObjectAt(i)[0] == '!') {
					if (printedItems >= printedItems_max) {
						sPrintString = "";
						printedItems = 0;
					}
					if (sPrintString.length()) {
						sPrintString += ", ";
					}
					printedItems++;
					sPrintString += gameFixAPI_AllowedCmdsContainer.ObjectAt(i);

					if (printedItems >= printedItems_max) {
						player->hudPrint(va("%s\n", sPrintString.c_str()));
					}
				}
			}
		}
		if (printedItems > 0) {
			player->hudPrint(va("%s\n", sPrintString.c_str()));
		}
	}
	return true;
}
#endif


