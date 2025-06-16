//-----------------------------------------------------------------------------
//
//  $Logfile:: /EF2/Code/DLLs/game/globalcmd.cpp                              $
// $Revision:: 76                                                             $
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
// Global commands for scripts
//

#include "_pch_cpp.h"
#include "globalcmd.h"
#include "scriptmaster.h"
#include "player.h"
#include "actor.h"
#include <qcommon/gameplaymanager.h>
#include "CinematicArmature.h"


//--------------------------------------------------------------
// GAMEFIX - Added: to make gamefix functionality available - chrissstrahl
//--------------------------------------------------------------
#include "gamefix.hpp"


//--------------------------------------------------------------
// GAMEFIX - Added: Make player view from the current camera during cinematic, when just entering or switching around - chrissstrahl
//--------------------------------------------------------------
#include "mp_manager.hpp"


//--------------------------------------------------------------
// COOP Generation 7.000 - Added Include - chrissstrahl
//--------------------------------------------------------------
#ifdef ENABLE_COOP
#include "../../coop/code/coop_manager.hpp"
#endif


Event EV_ProcessCommands
( 
	"processCommands",
	EV_CODEONLY,
	NULL,
	NULL,
	"Not used."
);
Event EV_Script_NewOrders
(	
	"newOrders",
	EV_CODEONLY,
	NULL,
	NULL,
	"Inform script that it is about to get new orders."
);
Event EV_ScriptThread_Execute
( 
	"execute",
	EV_CODEONLY,
	NULL,
	NULL,
	"Execute the thread."
);
Event EV_ScriptThread_Callback
( 
	"script_callback",
	EV_CODEONLY,
	"ese",
	"slave name other",
	"Script callback."
);
Event EV_ScriptThread_ThreadCallback
( 
	"thread_callback",
	EV_CODEONLY,
	NULL,
	NULL,
	"Thread callback."
);
Event EV_ScriptThread_DeathCallback
( 
	"death_callback",
	EV_CODEONLY,
	NULL,
	NULL,
	"Death callback."
);
Event EV_ScriptThread_CreateThread
( 
	"thread",
	EV_SCRIPTONLY,
	"@is",
	"threadnumber label",
	"Creates a thread starting at label."
);
Event EV_ScriptThread_CreateThread2
( 
	"runThreadNamed",
	EV_SCRIPTONLY,
	"@is",
	"threadnumber label",
	"Creates a thread starting at label."
);
Event EV_ScriptThread_TerminateThread
( 
	"terminate",
	EV_SCRIPTONLY,
	"i",
	"thread_number",
	"Terminates the specified thread or the current one if none specified."
);
Event EV_ScriptThread_ControlObject
( 
	"control",
	EV_CODEONLY,
	"e",
	"object",
	"Not used."
);
Event EV_ScriptThread_Goto
( 
	"goto",
	EV_SCRIPTONLY,
	"s",
	"label",
	"Goes to the specified label."
);
Event EV_ScriptThread_Pause
( 
	"pause",
	EV_SCRIPTONLY,
	NULL,
	NULL,
	"Pauses the thread."
);
Event EV_ScriptThread_Wait
( 
	"wait",
	EV_SCRIPTONLY,
	"f",
	"wait_time",
	"Wait for the specified amount of time."
);
Event EV_ScriptThread_WaitFor
( 
	"waitfor",
	EV_SCRIPTONLY,
	"e",
	"ent",
	"Wait for the specified entity."
);
Event EV_ScriptThread_WaitForThread
( 
	"waitForThread",
	EV_SCRIPTONLY,
	"i",
	"thread_number",
	"Wait for the specified thread."
);
Event EV_ScriptThread_WaitForDeath
( 
	"waitForDeath",
	EV_SCRIPTONLY,
	"s",
	"name",
	"Wait for death."
);
Event EV_ScriptThread_WaitForSound
( 
	"waitForSound",
	EV_SCRIPTONLY,
	"sf",
	"sound_name delay",
	"Wait for end of the named sound plus an optional delay."
);
Event EV_ScriptThread_WaitForDialog
( 
	"waitForDialog",
	EV_SCRIPTONLY,
	"eF",
	"actor additional_delay",
	"Wait for end of the dialog for the specified actor.\n"
	"If additional_delay is specified, than the actor will\n"
	"wait the dialog length plus the delay."
);
Event EV_ScriptThread_WaitDialogLength
( 
	"waitDialogLength",
	EV_SCRIPTONLY,
	"s",
	"dialogName",
	"Wait for the length of time of the dialog.\n"
);
Event EV_ScriptThread_WaitForAnimation
( 
	"waitForAnimation",
	EV_SCRIPTONLY,
	"esF",
	"entity animName additionalDelay",
	"Waits for time of the animation length plus the additional delayt specified.\n"
);
Event EV_ScriptThread_WaitForPlayer
( 
	"waitForPlayer",
	EV_SCRIPTONLY,
	NULL,
	NULL,
	"Wait for player to be ready."
);
Event EV_ScriptThread_CPrint
(
	"cprint",
	EV_SCRIPTONLY,
	"s",
	"string",
	"Prints a string."
);
Event EV_ScriptThread_Print
(
	"print",
	EV_SCRIPTONLY,
	"sSSSSSSSSSSSSSSS",
	"string string string string string string string string string string string string string string string string",
	"Prints a string."
);
Event EV_ScriptThread_PrintInt
( 
	"printint",
	EV_SCRIPTONLY,
	"i",
	"integer",
	"Print integer."
);
Event EV_ScriptThread_PrintFloat
( 
	"printfloat",
	EV_SCRIPTONLY,
	"f",
	"float",
	"Prints a float."
);
Event EV_ScriptThread_PrintVector
( 
	"printvector",
	EV_SCRIPTONLY,
	"v",
	"vector",
	"Prints a vector."
);
Event EV_ScriptThread_NewLine
( 
	"newline",
	EV_SCRIPTONLY,
	NULL,
	NULL,
	"Prints a newline."
);
Event EV_ScriptThread_Assert
( 
	"assert",
	EV_SCRIPTONLY,
	"f",
	"value",
	"Assert if value is 0."
);
Event EV_ScriptThread_Break
( 
	"break",
	EV_SCRIPTONLY,
	NULL,
	NULL,
	"Asserts so that we can break back into the debugger from script."
);
Event EV_ScriptThread_Clear
( 
	"clear",
	EV_SCRIPTONLY,
	"s",
	"var_group",
	"Clears the specified var group."
);
Event EV_ScriptThread_Trigger
( 
	"trigger",
	EV_SCRIPTONLY,
	"s",
	"entname",
	"Trigger the specified target."
);
Event EV_ScriptThread_TriggerEntity
( 
	"triggerentity",
	EV_SCRIPTONLY,
	"e",
	"entityToTrigger",
	"Trigger the specified entity."
);
Event EV_ScriptThread_Spawn
( 
	"spawn", 
	EV_CHEAT | EV_SCRIPTONLY,
	"@esSSSSSSSS",
	"entity entityname keyname1 value1 keyname2 value2 keyname3 value3 keyname4 value4",
	"Spawn the specified entity."
);
Event EV_ScriptThread_Map
( 
	"map",
	EV_SCRIPTONLY,
	"s",
	"map_name",
	"Starts the specified map."
);
Event EV_ScriptThread_NoIntermission
( 
	"noIntermission",
	EV_SCRIPTONLY,
	NULL,
	NULL,
	"Makes it so no intermission happens at the end of this level."
);
Event EV_ScriptThread_DontSaveOrientation
( 
	"dontSaveOrientation",
	EV_SCRIPTONLY,
	NULL,
	NULL,
	"Makes it so we don't save the player's orientation across this sublevel."
);
Event EV_ScriptThread_PlayerDeathThread
( 
	"playerDeathThread",
	EV_SCRIPTONLY,
	"s",
	"threadName",
	"Sets a thread to run when the player dies."
);
Event EV_ScriptThread_EndPlayerDeathThread
( 
	"endPlayerDeathThread",
	EV_SCRIPTONLY,
	NULL,
	NULL,
	"Ends the player death thread (goes on to mission failed)."
);
Event EV_ScriptThread_GetCvar
(
	"getcvar",
	EV_SCRIPTONLY,
	"@ss",
	"string_value cvar_name",
	"Returns the string value of the specified cvar."
);
Event EV_ScriptThread_GetCvarFloat
(
	"getcvarfloat",
	EV_SCRIPTONLY,
	"@fs",
	"float_value cvar_name",
	"Returns the float value of the specified cvar."
);
Event EV_ScriptThread_GetCvarInt
(
	"getcvarint",
	EV_SCRIPTONLY,
	"@fs",
	"int_value cvar_name",
	"Returns the integer value of the specified cvar."
);
Event EV_ScriptThread_SetCvar
( 
	"setcvar",
	EV_SCRIPTONLY,
	"ss",
	"cvar_name value",
	"Sets the value of the specified cvar."
);
Event EV_ScriptThread_CueCamera
( 
	"cuecamera",
	EV_SCRIPTONLY,
	"eF",
	"entity switchTime",
	"Cue the camera. If switchTime is specified, then the camera\n"
	"will switch over that length of time."
);
Event EV_ScriptThread_CuePlayer
( 
	"cueplayer",
	EV_SCRIPTONLY,
	"F",
	"switchTime",
	"Go back to the normal camera. If switchTime is specified,\n"
	"then the camera will switch over that length of time."
);
Event EV_ScriptThread_FreezePlayer
( 
	"freezeplayer",
	EV_SCRIPTONLY,
	NULL,
	NULL,
	"Freeze the player."
);
Event EV_ScriptThread_ReleasePlayer
( 
	"releaseplayer",
	EV_SCRIPTONLY,
	NULL,
	NULL,
	"Release the player."
);
Event EV_ScriptThread_FakePlayer
( 
	"fakeplayer",
	EV_SCRIPTONLY,
	NULL,
	NULL,
	"Will create a fake version of the player, hide the real one and\n"
	"call the fake one 'fakeplayer'."
);
Event EV_ScriptThread_RemoveFakePlayer
( 
	"removefakeplayer",
	EV_SCRIPTONLY,
	NULL,
	NULL,
	"Will delete the fake version of the player, un-hide the real one and\n"
	"return to the normal game"
);
Event EV_ScriptThread_KillEnt
( 
	"killent", 
	EV_CHEAT | EV_SCRIPTONLY,
	"i",
	"ent_num",
	"Kill the specified entity."
);
Event EV_ScriptThread_KillClass
( 
	"killclass", 
	EV_CHEAT | EV_SCRIPTONLY,
	"sI",
	"class_name except",
	"Kills everything in the specified class except for the specified entity (optional)."
);
Event EV_ScriptThread_RemoveEnt
( 
	"removeent", 
	EV_CHEAT | EV_SCRIPTONLY,
	"i",
	"ent_num",
	"Removes the specified entity."
);
Event EV_ScriptThread_RemoveClass
( 
	"removeclass", 
	EV_CHEAT | EV_SCRIPTONLY,
	"sI",
	"class_name except",
	"Removes everything in the specified class except for the specified entity (optional)."
);
Event EV_ScriptThread_RemoveActorsNamed
(
	"removeActorsNamed",
	EV_CHEAT | EV_SCRIPTONLY,
	"s",
	"class_name",
	"remove all actors in the game that have a matching name field (NOT targetname)"
);

// client/server flow control
Event EV_ScriptThread_StuffCommand
( 
	"stuffcmd",
	EV_SCRIPTONLY,
	"SSSSSS",
	"arg1 arg2 arg3 arg4 arg5 arg6",
	"Server only command."
);

//
// world stuff
//
Event EV_ScriptThread_RegisterAlias
( 
	"alias",
	EV_DEFAULT,
	"ssSSSS",
	"alias_name real_name arg1 arg2 arg3 arg4",
	"Sets up an alias."
);
Event EV_ScriptThread_RegisterAliasAndCache
(
	"aliascache",
	EV_DEFAULT,
	"ssSSSS",
	"alias_name real_name arg1 arg2 arg3 arg4",
	"Sets up an alias and caches the resourse."
);
Event EV_ScriptThread_SetCinematic
( 
	"cinematic",
	EV_SCRIPTONLY,
	NULL,
	NULL,
	"Turns on cinematic."
);
Event EV_ScriptThread_SetNonCinematic
( 
	"noncinematic",
	EV_SCRIPTONLY,
	NULL,
	NULL,
	"Turns off cinematic."
);
Event EV_ScriptThread_SetLevelAI
( 
	"level_ai",
	EV_SCRIPTONLY,
	"b",
	"on_or_off",
	"Turns on or off Level-wide AI for monsters."
);
Event EV_ScriptThread_SetSkipThread
( 
	"skipthread",
	EV_SCRIPTONLY,
	"s",
	"thread_name",
	"Set the thread to skip."
);

// Precache specific
Event EV_ScriptThread_Precache_Cache
( 
	"cache",
	EV_DEFAULT,
	"s",
	"resource_name",
	"Cache the specified resource."
);

// drugged up warping FOV controls
Event EV_ScriptThread_DrugView
( 
	"drugview",
	EV_SCRIPTONLY,
	"b",
	"on_or_off",
	"1 to turn FOV warping on, 0 to turn off."
);

// fades for movies
Event EV_ScriptThread_FadeIn
( 
	"cam_fadein",
	EV_SCRIPTONLY,
	"fffffI",
	"time color_red color_green color_blue alpha mode",
	"Sets up fadein in values."
);
Event EV_ScriptThread_FadeOut
( 
	"cam_fadeout",
	EV_SCRIPTONLY,
	"fffffI",
	"time color_red color_green color_blue alpha mode",
	"Sets up fadeout values."
);
Event EV_ScriptThread_FadeIsActive
(
	"fadeisactive",
	EV_SCRIPTONLY,
	"@i",
	"Result",
	"Returns 1 if a fade is in progress."
);
Event EV_ScriptThread_DoesEntityExist
(
	"doesEntityExist",
	EV_SCRIPTONLY,
	"@fe",
	"return_0_or_1 entity",
	"Returns whether or not an entity exists."
);
Event EV_ScriptThread_GetEntity
( 
	"getentity",
	EV_SCRIPTONLY,
	"@es",
	"entity entityname",
	"Returns the entity with the specified targetname."
);
Event EV_ScriptThread_GetNextEntity
( 
	"getnextentity",
	EV_SCRIPTONLY,
	"@ee",
	"entity currententity",
	"Returns the next entity in the list with the same name as the current entity."
);

// music command
Event EV_ScriptThread_MusicEvent
( 
	"music",
	EV_SCRIPTONLY,
	"sS",
	"current fallback",
	"Sets the current and fallback (optional) music moods."
);
Event EV_ScriptThread_ForceMusicEvent
( 
	"forcemusic",
	EV_SCRIPTONLY,
	"sS",
	"current fallback",
	"Forces the current and fallback (optional) music moods."
);
Event EV_ScriptThread_MusicVolumeEvent
( 
	"musicvolume",
	EV_SCRIPTONLY,
	"ff",
	"volume fade_time",
	"Sets the volume and fade time of the music."
);
Event EV_ScriptThread_RestoreMusicVolumeEvent
( 
	"restoremusicvolume",
	EV_SCRIPTONLY,
	"f",
	"fade_time",
	"Restores the music volume to its previous value."
);
Event EV_ScriptThread_AllowMusicDucking
( 
	"allowMusicDucking",
	EV_SCRIPTONLY,
	"B",
	"boolean",
	"Sets whether or not music ducking is allowed."
);
Event EV_ScriptThread_AllowActionMusic
( 
	"allowActionMusic",
	EV_SCRIPTONLY,
	"B",
	"boolean",
	"Sets whether or not action music is allowed or not."
);
Event EV_ScriptThread_SoundtrackEvent
( 
	"soundtrack",
	EV_SCRIPTONLY,
	"s",
	"soundtrack_name",
	"Changes the soundtrack."
);
Event EV_ScriptThread_RestoreSoundtrackEvent
( 
	"restoresoundtrack",
	EV_SCRIPTONLY,
	NULL,
	NULL,
	"Restores the soundtrack to the previous one."
);
Event EV_ScriptThread_ClearFade
(
	"clearfade",
	EV_SCRIPTONLY,
	NULL,
	NULL,
	"Clear the fade from the screen"
);
Event EV_ScriptThread_Letterbox
( 
	"letterbox",
	EV_SCRIPTONLY,
	"f",
	"time",
	"Puts the game in letterbox mode."
);
Event EV_ScriptThread_ClearLetterbox
( 
	"clearletterbox",
	EV_SCRIPTONLY,
	"f",
	"time",
	"Clears letterbox mode."
);
Event EV_ScriptThread_SetDialogScript
( 
	"setdialogscript" ,
	EV_SCRIPTONLY,
	"s",
	"dialog_script",
	"Set the script to be used when dialog:: is used"
);
Event EV_ScriptThread_CosDegrees
( 
	"cosdegrees",
	EV_SCRIPTONLY,
	"@ff",
	"cosine degrees",
	"Returns the cosine of the angle specified (in degrees)."
);
Event EV_ScriptThread_SinDegrees
( 
	"sindegrees",
	EV_SCRIPTONLY,
	"@ff",
	"sine degrees",
	"Returns the sine of the angle specified (in degrees)."
);
Event EV_ScriptThread_CosRadians
( 
	"cosradians",
	EV_SCRIPTONLY,
	"@ff",
	"cosine radians",
	"Returns the cosine of the angle specified (in radians)."
);
Event EV_ScriptThread_SinRadians
( 
	"sinradians",
	EV_SCRIPTONLY,
	"@ff",
	"sine radians",
	"Returns the sine of the angle specified (in radians)."
);
Event EV_ScriptThread_ArcTanDegrees
(
	"arctandegrees",
	EV_SCRIPTONLY,
	"@fff",
	"arctangent y x",
	"Returns the angle (in degrees) described by a 2d vector pointing to (x,y)."
);
Event EV_ScriptThread_Sqrt
(
	"sqrt",
	EV_SCRIPTONLY,
	"@ff",
	"square_root x",
	"Returns the sign-preserving square root of x."
);
Event EV_ScriptThread_Log
(
	"log",
	EV_SCRIPTONLY,
	"@ff",
	"log x",
	"Returns the log (base 2)."
);
Event EV_ScriptThread_Exp
(
	"exp",
	EV_SCRIPTONLY,
	"@ff",
	"exp x",
	"Returns the exponential."
);
Event EV_ScriptThread_RandomFloat
( 
	"randomfloat",
	EV_SCRIPTONLY,
	"@ff",
	"returnval max_float",
	"Sets the float to a random number."
);
Event EV_ScriptThread_RandomInteger
( 
	"randomint",
	EV_SCRIPTONLY,
	"@ii",
	"returnval max_int",
	"Sets the integer to a random number."
);
Event EV_ScriptThread_CameraCommand
( 
	"cam",
	EV_SCRIPTONLY,
	"sSSSSSS",
	"command arg1 arg2 arg3 arg4 arg5 arg6",
	"Processes a camera command."
);
Event EV_ScriptThread_SetLightStyle
( 
	"setlightstyle" ,
	EV_SCRIPTONLY,
	"is",
	"lightstyleindex lightstyledata",
	"Set up the lightstyle with lightstyleindex to the specified data"
);
Event EV_ScriptThread_KillThreadEvent
( 
	"killthread",
	EV_SCRIPTONLY,
	"s",
	"threadName",
	"kills all threads starting with the specified name"
);
Event EV_ScriptThread_SetThreadNameEvent
( 
	"threadname",
	EV_SCRIPTONLY,
	"s",
	"threadName",
	"sets the name of the thread"
);
Event EV_ScriptThread_CenterPrint
(
	"centerprint",
	EV_SCRIPTONLY,
	"s",
	"stuffToPrint",
	"prints the included message in the middle of all player's screens"
);
Event EV_ScriptThread_IsActorDead
(
	"isactordead",
	EV_SCRIPTONLY,
	"s",
	"actor_name",
	"Checks if an Actor is dead or not"
);
Event EV_ScriptThread_IsActorAlive
(
	"isactoralive",
	EV_SCRIPTONLY,
	"@fs",
	"flag actor_name",
	"Checks if an Actor is alive or not"
);
Event EV_ScriptThread_SendClientCommand
(
	"SendClientCommand",
	EV_SCRIPTONLY,
	"es",
	"player command",
	"Sends a command across to the client"
);
Event EV_ScriptThread_GetNumFreeReliableServerCommands
(
	"GetNumFreeReliableServerCommands",
	EV_SCRIPTONLY,
	"@fe",
	"count player",
	"Returns the number of reliable server command slots that are still available for use"
);
Event EV_ScriptThread_SendClientVar
(
	"SendClientVar",
	EV_SCRIPTONLY,
	"es",
	"player varName",
	"Sends a variable across to the client"
);
Event EV_ScriptThread_GetCurrentEntity
(
	"getcurrententity",
	EV_SCRIPTONLY,
	"@e",
	"entity",
	"Retrieves the current entity"
);
Event EV_ScriptThread_Mod
(
	"mod",
	EV_SCRIPTONLY,
	"@fii",
	"integer_remainder nominator denominator",
	"Returns the integer remainder from nominator / denominator"
);
Event EV_ScriptThread_Div
(
	"div",
	EV_SCRIPTONLY,
	"@fii",
	"integer_result nominator denominator",
	"Returns the integer value from nominator / denominator"
);
Event EV_ScriptThread_VectorScale
(
	"vectorscale",
	EV_SCRIPTONLY,
	"@vvf",
	"scaled_vector vector scale_value",
	"Returns the vector scaled by scale"
);
Event EV_ScriptThread_VectorDot
(
	"vectordot",
	EV_SCRIPTONLY,
	"@fvv",
	"dot_product vector1 vector2",
	"Returns the dot product of vector1 and vector2."
);
Event EV_ScriptThread_VectorCross
(
	"vectorcross",
	EV_SCRIPTONLY,
	"@vvv",
	"cross_vector vector1 vector2",
	"Returns the cross product of vector1 and vector2."
);
Event EV_ScriptThread_VectorNormalize
(
	"vectornormalize",
	EV_SCRIPTONLY,
	"@vv",
	"normalized_vector vector",
	"Returns the normalized vector."
);
Event EV_ScriptThread_VectorLength
(
	"vectorlength",
	EV_SCRIPTONLY,
	"@fv",
	"length vector",
	"Returns the length of the vector."
);
Event EV_ScriptThread_VectorForward
(
	"vectorforward",
	EV_SCRIPTONLY,
	"@vv",
	"forward_vector vector",
	"Returns the forward vector of the vector."
);
Event EV_ScriptThread_VectorLeft
(
	"vectorleft",
	EV_SCRIPTONLY,
	"@vv",
	"left_vector vector",
	"Returns the left vector of the vector."
);
Event EV_ScriptThread_VectorUp
(
	"vectorup",
	EV_SCRIPTONLY,
	"@vv",
	"up_vector vector",
	"Returns the up vector of the vector."
);
Event EV_ScriptThread_VectorGetX
(
	"vectorgetx",
	EV_SCRIPTONLY,
	"@fv",
	"x_value vector",
	"Returns the x of the vector."
);
Event EV_ScriptThread_VectorGetY
(
	"vectorgety",
	EV_SCRIPTONLY,
	"@fv",
	"y_value vector",
	"Returns the y of the vector."
);
Event EV_ScriptThread_VectorGetZ
(
	"vectorgetz",
	EV_SCRIPTONLY,
	"@fv",
	"z_value vector",
	"Returns the z of the vector."
);
Event EV_ScriptThread_VectorSetX
(
	"vectorsetx",
	EV_SCRIPTONLY,
	"@vvf",
	"vector_result vector x_value",
	"Sets the x value of a vector"
);
Event EV_ScriptThread_VectorSetY
(
	"vectorsety",
	EV_SCRIPTONLY,
	"@vvf",
	"vector_result vector y_value",
	"Sets the y value of a vector"
);
Event EV_ScriptThread_VectorSetZ
(
	"vectorsetz",
	EV_SCRIPTONLY,
	"@vvf",
	"vector_result vector z_value",
	"Sets the z value of a vector"
);
Event EV_ScriptThread_VectorToString
(
	"vectorToString",
	EV_SCRIPTONLY,
	"@sv",
	"string_result vector",
	"Converts a vector to a string"
);
Event EV_ScriptThread_TargetOf
(
	"targetof",
	EV_SCRIPTONLY,
	"@se",
	"name entity",
	"Gets the name of the entity's target"
);
Event EV_ScriptThread_Floor
(
	"floor",
	EV_SCRIPTONLY,
	"@ff",
	"return_value float_value",
	"Returns the closest integer not larger than float."
);
Event EV_ScriptThread_Ceil
(
	"ceil",
	EV_SCRIPTONLY,
	"@ff",
	"return_value float_value",
	"Returns the closest integer not smaller than float."
);
Event EV_ScriptThread_Round
(
	"round",
	EV_SCRIPTONLY,
	"@ff",
	"return_value float_value",
	"Returns the rounded value of a float."
);
Event EV_ScriptThread_SetGameplayFloat
(
	"setgameplayfloat",
	EV_SCRIPTONLY,
	"ssf",
	"objname propname value",
	"Sets a property float value on an object"
);
Event EV_ScriptThread_GetGameplayString
(
	"getgameplaystring",
	EV_DEFAULT,
	"@sss",
	"stringvalue objname propname",
	"Returns the string value of the object property."
);
Event EV_ScriptThread_GetGameplayFloat
(
	"getgameplayfloat",
	EV_DEFAULT,
	"@fss",
	"floatvalue objname propname",
	"Returns the float value of the object property."
);
Event EV_ScriptThread_SetGameplayString
(
	"setgameplaystring",
	EV_SCRIPTONLY,
	"sss",
	"objname propname valuestr",
	"Sets a property string value on an object"
);
Event EV_ScriptThread_GetIntegerFromString
(
	"getintfromstring",
	EV_SCRIPTONLY,
	"@fs",
	"returned_int string",
	"Returns the integer found in a string.  Can be called with strings "
	"that contain text, such as enemy12."
);
Event EV_ScriptThread_CreateCinematic
(
	"createCinematic",
	EV_SCRIPTONLY,
	"@es",
	"returnedCinematic cinematicName",
	"Creates a cinematic from disk. Returns the created cinematic."
	"Each call to this function creates a new cinematic, even if a"
	"cinematic with this name already exists."
);
Event EV_ScriptThread_GetTime
(
	"getLevelTime",
	EV_SCRIPTONLY,
	"@f",
	"returnedFloat",
	"Returns how many seconds.milliseconds the current level has been running."
);
Event EV_ScriptThread_ConnectPathnodes
(
	"connectPathnodes",
	EV_SCRIPTONLY,
	"ss",
	"node1 node2",
	"Connects the 2 specified nodes."
);
Event EV_ScriptThread_DisconnectPathnodes
(
	"disconnectPathnodes",
	EV_SCRIPTONLY,
	"ss",
	"node1 node2",
	"Disconnects the 2 specified nodes."
);


CLASS_DECLARATION( Interpreter, CThread, NULL )
{
#ifdef ENABLE_COOP
	//--------------------------------------------------------------
	// COOP Generation 7.000 - coop specific script function - chrissstrahl
	//--------------------------------------------------------------
	{ &EV_ScriptThread_coop_getVectorVariable, &CThread::coop_getVectorVariable },
	{ &EV_ScriptThread_coop_getFloatVariable, &CThread::coop_getFloatVariable },
	{ &EV_ScriptThread_coop_getStringVariable, &CThread::coop_getStringVariable },
	{ &EV_ScriptThread_coop_setVectorVariable, &CThread::coop_setVectorVariable },
	{ &EV_ScriptThread_coop_setFloatVariable, &CThread::coop_setFloatVariable },
	{ &EV_ScriptThread_coop_setStringVariable, &CThread::coop_setStringVariable },

	{ &EV_ScriptThread_coop_configstrRemove,				&CThread::coop_configstrRemove },
	{ &EV_ScriptThread_coop_configstrRemoveCombatSounds,	&CThread::coop_configstrRemoveCombatSounds },

	{ &EV_ScriptThread_coop_subString, &CThread::coop_subString },
	{ &EV_ScriptThread_coop_toLower, &CThread::coop_toLower },
	{ &EV_ScriptThread_coop_toUpper, &CThread::coop_toUpper },
	{ &EV_ScriptThread_coop_length, &CThread::coop_length },
	{ &EV_ScriptThread_coop_find, &CThread::coop_find },

	{ &EV_ScriptThread_coop_isDigit, &CThread::coop_isDigit },

	{ &EV_ScriptThread_coop_getTimeStamp, &CThread::coop_getTimeStamp },
	{ &EV_ScriptThread_coop_getFloat, &CThread::coop_getFloat },
	{ &EV_ScriptThread_coop_getVector, &CThread::coop_getVector },

	{ &EV_ScriptThread_coop_getClassOf, &CThread::coop_getClassOf },
	{ &EV_ScriptThread_coop_getPathnodeOrigin, &CThread::coop_getPathnodeOrigin },

	{ &EV_ScriptThread_coop_getLevelParamater, &CThread::coop_getLevelParamater },

	{ &EV_ScriptThread_coop_getIniData, &CThread::coop_getIniData },
	{ &EV_ScriptThread_coop_getIniDataPlayer, &CThread::coop_getIniDataPlayer },
	{ &EV_ScriptThread_coop_setIniDataPlayer, &CThread::coop_setIniDataPlayer },
	{ &EV_ScriptThread_coop_setIniData, &CThread::coop_setIniData },

	{ &EV_ScriptThread_coop_getMapByServerIp, &CThread::coop_getMapByServerIp },
#endif


	{ &EV_ScriptThread_Execute,						&CThread::ExecuteFunc },
	{ &EV_MoveDone,									&CThread::ObjectMoveDone },
	{ &EV_ScriptThread_Callback,					&CThread::ScriptCallback },
	{ &EV_ScriptThread_ThreadCallback,				&CThread::ThreadCallback },
	{ &EV_ScriptThread_DeathCallback,				&CThread::DeathCallback },
	{ &EV_ScriptThread_CreateThread,				&CThread::CreateThread },
	{ &EV_ScriptThread_CreateThread2,				&CThread::CreateThread },
	{ &EV_ScriptThread_TerminateThread,				&CThread::TerminateThread },
	{ &EV_ScriptThread_Pause,						&CThread::EventPause },
	{ &EV_ScriptThread_Wait,						&CThread::EventWait },
	{ &EV_ScriptThread_WaitFor,						&CThread::EventWaitFor },
	{ &EV_ScriptThread_WaitForThread,				&CThread::EventWaitForThread },
	{ &EV_ScriptThread_WaitForDeath,				&CThread::EventWaitForDeath },
	{ &EV_ScriptThread_WaitForSound,				&CThread::EventWaitForSound },
	{ &EV_ScriptThread_WaitForDialog,				&CThread::EventWaitForDialog },
	{ &EV_ScriptThread_WaitDialogLength,			&CThread::EventWaitDialogLength },
	{ &EV_ScriptThread_WaitForAnimation,			&CThread::EventWaitForAnimation },
	{ &EV_ScriptThread_WaitForPlayer,				&CThread::EventWaitForPlayer },
	{ &EV_ScriptThread_CPrint,						&CThread::CPrint },
	{ &EV_ScriptThread_Print,						&CThread::Print },
	{ &EV_ScriptThread_PrintInt,					&CThread::PrintInt },
	{ &EV_ScriptThread_PrintFloat,					&CThread::PrintFloat },
	{ &EV_ScriptThread_PrintVector,					&CThread::PrintVector },
	{ &EV_ScriptThread_NewLine,						&CThread::NewLine },
	{ &EV_ScriptThread_Assert,						&CThread::Assert },
	{ &EV_ScriptThread_Break,						&CThread::Break },
	{ &EV_ScriptThread_Trigger,						&CThread::TriggerEvent },
	{ &EV_ScriptThread_TriggerEntity,				&CThread::TriggerEntityEvent },
	{ &EV_ScriptThread_StuffCommand,				&CThread::StuffCommand },
	{ &EV_ScriptThread_Spawn,						&CThread::Spawn },
	{ &EV_ScriptThread_Precache_Cache,				&CThread::CacheResourceEvent },
	{ &EV_ScriptThread_RegisterAlias,				&CThread::RegisterAlias },
	{ &EV_ScriptThread_RegisterAliasAndCache,		&CThread::RegisterAliasAndCache },
	{ &EV_ScriptThread_Map,							&CThread::MapEvent },
	{ &EV_ScriptThread_NoIntermission,				&CThread::noIntermission },
	{ &EV_ScriptThread_DontSaveOrientation,			&CThread::dontSaveOrientation },

	{ &EV_ScriptThread_PlayerDeathThread,			&CThread::setPlayerDeathThread },
	{ &EV_ScriptThread_EndPlayerDeathThread,		&CThread::endPlayerDeathThread },

	{ &EV_ScriptThread_GetCvar,						&CThread::GetCvarEvent },
	{ &EV_ScriptThread_GetCvarFloat,				&CThread::GetCvarFloatEvent },
	{ &EV_ScriptThread_GetCvarInt,					&CThread::GetCvarIntEvent },
	{ &EV_ScriptThread_SetCvar,						&CThread::SetCvarEvent },
	{ &EV_ScriptThread_CueCamera,					&CThread::CueCamera },
	{ &EV_ScriptThread_CuePlayer,					&CThread::CuePlayer },
	{ &EV_ScriptThread_FreezePlayer,				&CThread::FreezePlayer },
	{ &EV_ScriptThread_ReleasePlayer,				&CThread::ReleasePlayer },
	{ &EV_ScriptThread_FadeIn,						&CThread::FadeIn },
	{ &EV_ScriptThread_FadeOut,						&CThread::FadeOut },
	{ &EV_ScriptThread_FadeIsActive,				&CThread::FadeIsActive },
	{ &EV_ScriptThread_ClearFade,					&CThread::ClearFade },
	{ &EV_ScriptThread_Letterbox,					&CThread::Letterbox },
	{ &EV_ScriptThread_ClearLetterbox,				&CThread::ClearLetterbox },
	{ &EV_ScriptThread_MusicEvent,					&CThread::MusicEvent },
	{ &EV_ScriptThread_ForceMusicEvent,				&CThread::ForceMusicEvent },
	{ &EV_ScriptThread_MusicVolumeEvent,			&CThread::MusicVolumeEvent },
	{ &EV_ScriptThread_RestoreMusicVolumeEvent,		&CThread::RestoreMusicVolumeEvent },
	{ &EV_ScriptThread_AllowMusicDucking,			&CThread::allowMusicDucking },
	{ &EV_ScriptThread_AllowActionMusic,			&CThread::allowActionMusic },
	{ &EV_ScriptThread_SoundtrackEvent,				&CThread::SoundtrackEvent },
	{ &EV_ScriptThread_RestoreSoundtrackEvent,		&CThread::RestoreSoundtrackEvent },
	{ &EV_ScriptThread_SetCinematic,				&CThread::SetCinematic },
	{ &EV_ScriptThread_SetNonCinematic,				&CThread::SetNonCinematic },
	{ &EV_ScriptThread_SetLevelAI,					&CThread::SetLevelAI },
	{ &EV_ScriptThread_SetSkipThread,				&CThread::SetSkipThread },
	{ &EV_ScriptThread_KillEnt,						&CThread::KillEnt },
	{ &EV_ScriptThread_RemoveEnt,					&CThread::RemoveEnt },
	{ &EV_ScriptThread_KillClass,					&CThread::KillClass },
	{ &EV_ScriptThread_RemoveClass,					&CThread::RemoveClass },
	{ &EV_ScriptThread_RemoveActorsNamed,			&CThread::RemoveActorsNamed },
	{ &EV_ScriptThread_FakePlayer,					&CThread::FakePlayer },
	{ &EV_ScriptThread_RemoveFakePlayer,			&CThread::RemoveFakePlayer },
	{ &EV_AI_RecalcPaths,							&CThread::PassToPathmanager },
	{ &EV_AI_CalcPath,								&CThread::PassToPathmanager },
	{ &EV_ScriptThread_DoesEntityExist,				&CThread::doesEntityExist },
	{ &EV_ScriptThread_GetEntity,					&CThread::GetEntityEvent },
	{ &EV_ScriptThread_GetNextEntity,				&CThread::GetNextEntityEvent },
	{ &EV_ScriptThread_CosDegrees,					&CThread::CosDegrees },
	{ &EV_ScriptThread_SinDegrees,					&CThread::SinDegrees },
	{ &EV_ScriptThread_CosRadians,					&CThread::CosRadians },
	{ &EV_ScriptThread_SinRadians,					&CThread::SinRadians },
	{ &EV_ScriptThread_ArcTanDegrees,				&CThread::ArcTanDegrees },
	{ &EV_ScriptThread_Sqrt,						&CThread::ScriptSqrt },
	{ &EV_ScriptThread_Log,							&CThread::ScriptLog },
	{ &EV_ScriptThread_Exp,							&CThread::ScriptExp },
	{ &EV_ScriptThread_RandomFloat,					&CThread::RandomFloat },
	{ &EV_ScriptThread_RandomInteger,				&CThread::RandomInteger },
	{ &EV_ScriptThread_CameraCommand,				&CThread::CameraCommand },
	{ &EV_ScriptThread_SetLightStyle,				&CThread::SetLightStyle },
	{ &EV_ScriptThread_KillThreadEvent,				&CThread::KillThreadEvent },
	{ &EV_ScriptThread_SetThreadNameEvent,			&CThread::SetThreadName },

	{ &EV_SetFloatVar,								&CThread::SetFloatVar },
	{ &EV_SetVectorVar,								&CThread::SetVectorVar },
	{ &EV_SetStringVar,								&CThread::SetStringVar },

	{ &EV_DoesVarExist,								&CThread::doesVarExist },
	{ &EV_RemoveVariable,							&CThread::RemoveVariable },

	{ &EV_GetFloatVar,								&CThread::GetFloatVar },
	{ &EV_GetVectorVar,								&CThread::GetVectorVar },
	{ &EV_GetStringVar,								&CThread::GetStringVar },

	{ &EV_ScriptThread_CenterPrint,					&CThread::CenterPrint },
	{ &EV_ScriptThread_IsActorDead,					&CThread::isActorDead		},
	{ &EV_ScriptThread_IsActorAlive,				&CThread::isActorAlive		},
	{ &EV_ScriptThread_SendClientCommand,			&CThread::SendClientCommand		},
	{ &EV_ScriptThread_GetNumFreeReliableServerCommands, &CThread::GetNumFreeReliableServerCommands },
	{ &EV_ScriptThread_SendClientVar,				&CThread::SendClientVar		},
	{ &EV_ScriptThread_GetCurrentEntity,			&CThread::GetCurrentEntity     },
	{ &EV_ScriptThread_Mod,							&CThread::ModEvent },
	{ &EV_ScriptThread_Div,							&CThread::DivEvent },
	{ &EV_ScriptThread_VectorScale,					&CThread::VectorScaleEvent },
	{ &EV_ScriptThread_VectorDot,					&CThread::VectorDotEvent },
	{ &EV_ScriptThread_VectorCross,					&CThread::VectorCrossEvent },
	{ &EV_ScriptThread_VectorNormalize,				&CThread::VectorNormalizeEvent },
	{ &EV_ScriptThread_VectorLength,				&CThread::VectorLengthEvent },
	{ &EV_ScriptThread_VectorSetX,					&CThread::VectorSetXEvent },
	{ &EV_ScriptThread_VectorSetY,					&CThread::VectorSetYEvent },
	{ &EV_ScriptThread_VectorSetZ,					&CThread::VectorSetZEvent },
	{ &EV_ScriptThread_VectorGetX,					&CThread::VectorGetXEvent },
	{ &EV_ScriptThread_VectorGetY,					&CThread::VectorGetYEvent },
	{ &EV_ScriptThread_VectorGetZ,					&CThread::VectorGetZEvent },
	{ &EV_ScriptThread_VectorForward,				&CThread::VectorForwardEvent },
	{ &EV_ScriptThread_VectorLeft,					&CThread::VectorLeftEvent },
	{ &EV_ScriptThread_VectorUp,					&CThread::VectorUpEvent },
	{ &EV_ScriptThread_VectorToString,				&CThread::vectorToString },
	{ &EV_ScriptThread_TargetOf,					&CThread::TargetOfEvent },
	{ &EV_ScriptThread_Floor,						&CThread::FloorEvent },
	{ &EV_ScriptThread_Ceil,						&CThread::CeilEvent },
	{ &EV_ScriptThread_Round,						&CThread::RoundEvent },
	{ &EV_ScriptThread_GetGameplayFloat,			&CThread::GetGameplayFloat },
	{ &EV_ScriptThread_GetGameplayString,			&CThread::GetGameplayString },
	{ &EV_ScriptThread_SetGameplayFloat,			&CThread::SetGameplayFloat },
	{ &EV_ScriptThread_SetGameplayString,			&CThread::SetGameplayString },
	{ &EV_ScriptThread_GetIntegerFromString,		&CThread::GetIntegerFromString },
	{ &EV_ScriptThread_CreateCinematic,				&CThread::CreateCinematic },
	{ &EV_ScriptThread_GetTime,						&CThread::GetLevelTime },

	{ &EV_ScriptThread_ConnectPathnodes,			&CThread::connectPathnodes },
	{ &EV_ScriptThread_DisconnectPathnodes,			&CThread::disconnectPathnodes },

	{ NULL, NULL }
};

CThread::CThread()
{
	ClearWaitFor();
	currentEntity = NULL;
}

void CThread::ExecuteFunc( Event *ev )
{
	ClearWaitFor();
	
	Interpreter::Execute( ev );
}

void CThread::ClearWaitFor( void )
{
	waitUntil = 0;
	waitingFor = NULL;
	waitingNumObjects = 0;
	waitingForThread = NULL;
	waitingForDeath = "";
	waitingForPlayer = false;
}

qboolean CThread::WaitingFor( Entity *obj )
{
	assert( obj );
	
	return ( waitingFor && waitingFor->list.ObjectInList( obj ) );
}

void CThread::ObjectMoveDone( Event *ev )
{
	Entity *obj;
	
	obj = ev->GetEntity( 1 );
	assert( obj );
	
	if ( WaitingFor( obj ) )
	{
		waitingNumObjects--;
		if ( waitingNumObjects <= 0 )
		{
			ClearWaitFor();
			DelayedStart( 0.0f );
		}
	}
}

void CThread::CreateThread( Event *ev )
{
	CThread *pThread;
	
	pThread = Director.CreateThread( ev->GetToken( 1 ) );
	if ( pThread )
	{
		// start right away
		pThread->Start();
		
		ev->ReturnInteger( pThread->ThreadNum() );
	}
}

void CThread::TerminateThread( Event *ev )
{
	int threadnum;
	
	threadnum = ev->GetInteger( 1 );
	Director.GetThread( threadnum );
	Director.KillThread( threadnum );
}

void CThread::EventPause( Event *ev )
{
	DoMove();
	
	ClearWaitFor();
	
	doneProcessing = true;
}

void CThread::EventWait( Event *ev )
{
	DoMove();
	
	ClearWaitFor();
	
	waitUntil = ev->GetFloat( 1 ) + level.time;
	
	DelayedStart( ev->GetFloat( 1 ) );
	doneProcessing = true;
}

void CThread::EventWaitFor( Event *ev )
{
	Entity      *ent;
	const char  *tname;
	TargetList  *tlist;
	int         i;
	
	ClearWaitFor();
	doneProcessing = true;
	
	ent = ev->GetEntity( 1 );
	if ( ent )
	{
		tname = ent->TargetName();
		
		//
		// set the number of objects that belong to this targetname
		//
		tlist = world->GetTargetList( str( tname ) );
		waitingFor = tlist;
		waitingNumObjects = tlist->list.NumObjects();
		
		//
		// make sure all these objects are in the update list
		//
		for( i = 1; i <= waitingNumObjects; i++ )
		{
			ent = tlist->list.ObjectAt( i );
			
			// add the object to the update list to make sure we tell it to do a move
			if ( !updateList.ObjectInList( ent->entnum ) )
            {
				updateList.AddObject( ent->entnum );
            }
		}
		
		if ( waitingNumObjects <= 0 )
		{
			waitingNumObjects = 1;
			ev->Error( "no objects of targetname %s found.\n", tname );
		}
	}
	
	DoMove();
}

void CThread::EventWaitForThread( Event *ev )
{
	doneProcessing = true;
	
	ClearWaitFor();
	waitingForThread = Director.GetThread( ev->GetInteger( 1 ) );
	if ( !waitingForThread )
	{
		ev->Error( "Thread %d not running", ev->GetInteger( 1 ) );
		return;
	}
	
	DoMove();
}

void CThread::EventWaitForDeath( Event *ev )
{
	doneProcessing = true;
	
	ClearWaitFor();
	waitingForDeath = ev->GetString(1);
	if ( !waitingForDeath.length() )
	{
		ev->Error( "Null name" );
		return;
	}
	
	DoMove();
}

void CThread::EventWaitForSound( Event *ev )
{
	str sound;
	float delay;
	
	ClearWaitFor();
	
	DoMove();
	
	delay = 0.0f;
	sound = ev->GetString( 1 );
	
	delay = gi.SoundLength( sound.c_str() );
	
	if ( delay < 0.0f )
	{
		gi.WDPrintf( "Lip file not found for dialog %s\n", sound.c_str() );
	}
	
	if ( ev->NumArgs() > 1 )
	{
		delay += ev->GetFloat( 2 );
	}
	
	DelayedStart( delay );
	doneProcessing = true;
}

void CThread::EventWaitForDialog( Event *ev )
{
	float delay;
	Actor *act;
	Entity *ent;
	
	ent = ev->GetEntity( 1 );
	
	if ( !ent )
	{
		gi.WDPrintf( "WaitForDialog: Can't find entity\n" );
		return;
	}
	
	if ( ent->isSubclassOf( Actor ) )
	{
		act = ( Actor * )ent;
		
		delay = act->GetDialogRemainingTime( );
		if ( ev->NumArgs() > 1 )
		{
			delay += ev->GetFloat( 2 );
		}
		DelayedStart( delay );
		doneProcessing = true;
	}
}

void CThread::EventWaitDialogLength( Event *ev )
{
	str dialogName;
	float delay;
	char localizedDialogName[ MAX_QPATH ];
	
	// Get the localized dialog name

	dialogName = ev->GetString( 1 );

	gi.LocalizeFilePath( dialogName, localizedDialogName );

	// Figure out the delay time (length of dialog + extra specified delay)

	delay = gi.SoundLength( localizedDialogName );

	if ( ev->NumArgs() > 1 )
	{
		delay += ev->GetFloat( 2 );
	}

	// Delay the thread
	
	DelayedStart( delay );
	doneProcessing = true;
}

void CThread::EventWaitForAnimation( Event *ev )
{
	Entity *ent;
	str animName;
	int animNum;
	float totalTime;
	float extraTime;
	
	// Get the entity we are refering to
	
	ent = ev->GetEntity( 1 );
	
	if ( !ent )
		return;
	
	// Get the animation we are refering to
	
	animName = ev->GetString( 2 );
	
	// Get any extra time if any
	
	if ( ev->NumArgs() > 2 )
		extraTime = ev->GetFloat( 3 );
	else
		extraTime = 0.0f;
	
	// Get the anim number
	
	animNum = gi.Anim_NumForName( ent->edict->s.modelindex, animName.c_str() );
	
	if ( animNum < 0 )
	{
		gi.WDPrintf( "Waiting for animation %s, but %s doesn't have this anim!\n", animName.c_str(), ent->model.c_str() );
		return;
	}
	
	// Get the total time of this animation
	
	totalTime = gi.Anim_Time( ent->edict->s.modelindex, animNum );
	
	totalTime += extraTime;
	
	// Setup all of the wait for stuff
	
	ClearWaitFor();
	DoMove();
	DelayedStart( totalTime );
	
	doneProcessing = true;
}

void CThread::EventWaitForPlayer( Event *ev )
{
	if ( !Director.PlayerReady() )
	{
		doneProcessing = true;
		
		ClearWaitFor();
		waitingForPlayer = true;
		
		DoMove();
	}
}

void CThread::CPrint( Event *ev )
{
	//--------------------------------------------------------------
	// GAMEFIX - Changed: importance from normal to critical, to make sure the message is always shown - chrissstrahl
	// GAMEFIX - Fixed: message now shown to all players not just client 0 - chrissstrahl
	//--------------------------------------------------------------
	int         j;
	gentity_t* other;

	for (j = 0; j < game.maxclients; j++)
	{
		other = &g_entities[j];
		if (other->inuse && other->client)
		{
			gi.centerprintf(other, CENTERPRINT_IMPORTANCE_CRITICAL, ev->GetString(1));
		}
	}
}

void CThread::Print( Event *ev )
{
	int i;
	int n;
	
	n = ev->NumArgs();
	for( i = 1; i <= n; i++ )
	{
		//--------------------------------------------------------------
		// GAMEFIX - Changed: make sure to printed in windows and linux - chrissstrahl
		//--------------------------------------------------------------
		gi.Printf( "%s", ev->GetString( i ) );
	}
}

void CThread::PrintInt( Event *ev )
{
	//--------------------------------------------------------------
	// GAMEFIX - Changed: make sure to printed in windows and linux - chrissstrahl
	//--------------------------------------------------------------
	gi.Printf( "%d", ev->GetInteger( 1 ) );
}

void CThread::PrintFloat( Event *ev )
{
	//--------------------------------------------------------------
	// GAMEFIX - Changed: make sure to printed in windows and linux - chrissstrahl
	//--------------------------------------------------------------
	gi.Printf( "%.2f", ev->GetFloat( 1 ) );
}

void CThread::PrintVector( Event *ev )
{
	Vector vec;
	
	vec = ev->GetVector( 1 );
	//--------------------------------------------------------------
	// GAMEFIX - Changed: make sure to printed in windows and linux - chrissstrahl
	//--------------------------------------------------------------
	gi.Printf( "(%.2f %.2f %.2f)", vec.x, vec.y, vec.z );
}

void CThread::NewLine( Event *ev )
{
	//--------------------------------------------------------------
	// GAMEFIX - Changed: make sure to printed in windows and linux - chrissstrahl
	//--------------------------------------------------------------
	gi.Printf( "\n" );
}

void CThread::Assert( Event *ev )
{
	assert( ev->GetFloat( 1 ) );
}

void CThread::Break( Event *ev )
{
	// Break into the debugger
#ifdef _WIN32
	__asm int 3
#else
		assert( 0 );
#endif
}

void CThread::ScriptCallback( Event *ev )
{
	const char  *name;
	Entity	   *other;
	Entity	   *slave;
	
	if ( threadDying )
	{
		return;
	}
	
	
	slave = ev->GetEntity( 1 );
	name = ev->GetString( 2 );
	other = ev->GetEntity( 3 );
	currentEntity = slave;
	
	if ( !Goto( name ) )
	{
		ev->Error( "Label '%s' not found", name );
	}
	else
	{
		// kill any execute events (in case our last command was "wait")
		ClearWaitFor();
		
		// start right away
		Start();
	}
}

void CThread::ThreadCallback( Event *ev )
{
	CThread *thread;
	
	if ( threadDying )
	{
		return;
	}
	
	thread = ev->GetThread();
	if ( thread && ( thread == waitingForThread ) )
	{
		ClearWaitFor();
		Start();
	}
}

void CThread::DeathCallback( Event *ev )
{
	if ( threadDying )
	{
		return;
	}
	
	ClearWaitFor();
	DelayedStart( 0.0f );
}

void CThread::DoMove( void )
{
	int      entnum;
	Entity   *ent;
	Event    *event;
	int      count;
	int      i;
	
	count = updateList.NumObjects();
	
	for( i = 1; i <= count; i++ )
	{
		entnum = ( int )updateList.ObjectAt( i );
		ent = G_GetEntity( entnum );
		if ( ent && ( ent->ValidEvent( EV_ProcessCommands ) ) )
		{
			event = new Event( EV_ProcessCommands );
			event->SetThread( this );
			ent->PostEvent( event, 0.0f );
		}
		else
		{
			// try to remove this from the update list
			if ( waitingNumObjects > 0 )
            {
				waitingNumObjects--;
            }
		}
	}
	
	updateList.ClearObjectList();
}

void CThread::TriggerEvent( Event *ev )
{
	const char	*name;
	Event		   *event;
	Entity	   *ent;
	TargetList  *tlist;
	int         i;
	int         num;
	
	name = ev->GetString( 1 );
	
	if ( !name )
		return;
	
	// Check for object commands
	if ( name[ 0 ] == '$' )
	{
		tlist = world->GetTargetList( str( name + 1 ) );
		num = tlist->list.NumObjects();
		for ( i = 1; i <= num; i++ )
		{
			ent = tlist->list.ObjectAt( i );
			
			assert( ent );
			
			event = new Event( EV_Activate );
			event->SetSource( EV_FROM_SCRIPT );
			event->SetThread( this );
			event->SetLineNumber( ev->GetLineNumber() );
			event->AddEntity( world );
			ent->ProcessEvent( event );
		}
	}
	else if ( name[ 0 ] == '*' )   // Check for entnum commands
	{
		if ( !IsNumeric( &name[ 1 ] ) )
		{
			RunError( "trigger: Expecting numeric value for * command, but found '%s'\n", &name[ 1 ] );
		}
		else
		{
			ent = G_GetEntity( atoi( &name[ 1 ] ) );
			if ( ent )
            {
				event = new Event( EV_Activate );
				event->SetSource( EV_FROM_SCRIPT );
				event->SetThread( this );
				event->SetLineNumber( ev->GetLineNumber() );
				event->AddEntity( world );
				ent->ProcessEvent( event );
            }
			else
            {
				RunError( "trigger: Entity not found for * command\n" );
            }
		}
		return;
	}
	else
	{
		RunError( "trigger: Invalid entity reference '%s'.\n", name );
	}
}

//----------------------------------------------------------------
// Name:			TriggerEntityEvent
// Class:			CThread
//
// Description:		Triggers the specified entity
//
// Parameters:		Event *ev			- contains entity to trigger
//
// Returns:			none
//----------------------------------------------------------------

void CThread::TriggerEntityEvent( Event *ev )
{
	Event	   *event;

	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Crash if given entity did not exist - chrissstrahl
	//--------------------------------------------------------------
	Entity	   *ent = nullptr;
	
	// Get the entity we wish to trigger
	ent = ev->GetEntity( 1 );
	
	//--------------------------------------------------------------
	// GAMEFIX - Added: Print out on triggerEntity command failure for feedback - chrissstrahl
	//--------------------------------------------------------------
	if (!ent) {
		gi.Printf(_GFixEF2_WARN_EVENT_CTHREAD_triggerEntity_FAILED);
		return;
	}
	
	// Trigger the entity
	
	event = new Event( EV_Activate );
	
	event->SetSource( EV_FROM_SCRIPT );
	event->SetThread( this );
	event->SetLineNumber( ev->GetLineNumber() );
	event->AddEntity( world );
	
	ent->ProcessEvent( event );
}

void CThread::CacheResourceEvent( Event * ev )
{
	if ( !precache->integer )
	{
		return;
	}
	
	CacheResource( ev->GetString( 1 ), world );
}

void CThread::RegisterAlias( Event *ev )
{
	str parameters;
	int i;
	
	// Get the parameters for this alias command
	for( i = 3; i <= ev->NumArgs(); i++ )
	   {
		parameters += ev->GetString( i );
		parameters += " ";
	   }
	
	gi.GlobalAlias_Add( ev->GetString( 1 ), ev->GetString( 2 ),  parameters.c_str() );
}

void CThread::RegisterAliasAndCache( Event *ev )
{
	RegisterAlias( ev );
	
	if ( !precache->integer )
	{
		return;
	}
	
	CacheResource( ev->GetString( 2 ) );
}

void CThread::MapEvent( Event *ev )
{
	G_BeginIntermission( ev->GetString( 1 ) );
	doneProcessing = true;
}

void CThread::noIntermission( Event *ev )
{
	level._showIntermission = false;
}

void CThread::dontSaveOrientation( Event *ev )
{
	level._saveOrientation = false;
}

void CThread::setPlayerDeathThread( Event *ev )
{
	level.setPlayerDeathThread( ev->GetString( 1 ) );
}

void CThread::endPlayerDeathThread( Event *ev )
{
	G_FinishMissionFailed();
}

void CThread::GetCvarEvent( Event *ev )
{
	assert( ev );
	cvar_t *cvar = gi.cvar_get( ev->GetString( 1 ) );
	ev->ReturnString( cvar ? cvar->string : "" );
}

void CThread::GetCvarFloatEvent( Event *ev )
{
	assert( ev );
	cvar_t *cvar = gi.cvar_get( ev->GetString( 1 ) );
	ev->ReturnFloat( cvar ? cvar->value : 0.0f );
}

void CThread::GetCvarIntEvent( Event *ev )
{
	assert( ev );
	cvar_t *cvar = gi.cvar_get( ev->GetString( 1 ) );
	ev->ReturnFloat( cvar ? cvar->integer : 0.0f );
}

void CThread::SetCvarEvent( Event *ev )
{
	str name;
	
	name = ev->GetString( 1 );
	if ( name != "" )
	{
		gi.cvar_set( name.c_str(), ev->GetString( 2 ) );
	}
}

void CThread::CueCamera( Event *ev )
{
	float    switchTime;
	Entity   *ent;
	
	if ( ev->NumArgs() > 1 )
	{
		switchTime = ev->GetFloat( 2 );
	}
	else
	{
		switchTime = 0;
	}
	
	ent = ev->GetEntity( 1 );
	if ( ent )
	{
		SetCamera( ent, switchTime );
	}
	else
	{
		ev->Error( "Camera named %s not found", ev->GetString( 1 ) );
	}
}

void CThread::CuePlayer( Event *ev )
{
	float switchTime;
	
	if ( ev->NumArgs() > 0 )
	{
		switchTime = ev->GetFloat( 1 );
	}
	else
	{
		switchTime = 0;
	}


	//--------------------------------------------------------------
	// GAMEFIX - Added: Make player view from the current camera during cinematic, when just entering or switching around - chrissstrahl
	//--------------------------------------------------------------
	if (multiplayerManager.inMultiplayer()) {
		multiplayerManager.gamefixEF2_currentCamera = nullptr;
	}
	

	SetCamera( NULL, switchTime );
}

void CThread::FreezePlayer( Event *ev )
{
	level.playerfrozen = true;
}

void CThread::ReleasePlayer( Event *ev )
{
	level.playerfrozen = false;
}

void CThread::FakePlayer( Event *ev )
{
	//--------------------------------------------------------------
	// GAMEFIX - Fixed: crash when using fakeplayer script command in multiplayer  - chrissstrahl
	//--------------------------------------------------------------
	if (g_gametype->integer != GT_SINGLE_PLAYER) {
		gi.Printf(_GFixEF2_WARN_EVENT_CTHREAD_fakePlayer_ONLYSP);
		return;
	}

	Player *player;
	
	player = ( Player * )g_entities[ 0 ].entity;
	if ( player && player->edict->inuse && player->edict->client )
	{
		player->FakePlayer(false);
	}
}

void CThread::RemoveFakePlayer( Event *ev )
{
	//--------------------------------------------------------------
	// GAMEFIX - Fixed: crash when using fakeplayer script command in multiplayer - chrissstrahl
	//--------------------------------------------------------------
	if (g_gametype->integer != GT_SINGLE_PLAYER) {
		gi.Printf(_GFixEF2_WARN_EVENT_CTHREAD_fakePlayer_ONLYSP);
		return;
	}

	Player *player;
	
	player = ( Player * )g_entities[ 0 ].entity;
	if ( player && player->edict->inuse && player->edict->client )
	{
		player->RemoveFakePlayer();
	}
}

void CThread::Spawn( Event *ev )
{
	Entity		   *ent;
	Entity         *tent;
	const char	   *name;
	ClassDef		   *cls;
	int			   n;
	int			   i;
	const char     *targetname;
	const char	   *value;
	
	if ( ev->NumArgs() < 1 )
	{
		ev->Error( "Usage: spawn entityname [keyname] [value]..." );
		return;
	}
	
	// create a new entity
	SpawnArgs args;
	
	name = ev->GetString( 1 );
	
	if ( name )
	{
		cls = getClassForID( name );
		if ( !cls )
		{
			cls = getClass( name );
		}
		
		if ( !cls )
		{
			//--------------------------------------------------------------
			// GAMEFIX - Fixed: Warning C4456: Declaration of ? hides previous local declaration. - chrissstrahl
			//--------------------------------------------------------------
			str temp_n;
			

			temp_n = name;
			if ( !strstr( temp_n.c_str(), ".tik" ) )
            {
				temp_n += ".tik";
            }
			args.setArg( "model", temp_n.c_str() );
		}
		else
		{
			args.setArg( "classname", name );
		}
	}
	
	if ( ev->NumArgs() > 2 )
	{
		n = ev->NumArgs();
		for( i = 2; i <= n; i += 2 )
		{
			args.setArg( ev->GetString( i ), ev->GetString( i + 1 ) );
		}
	}
	
	cls = args.getClassDef();
	if ( !cls )
	{
		ev->Error( "'%s' is not a valid entity name", name );
		return;
	}
	
	// If there is a spawntarget set, then use that entity's origin and angles
	targetname = args.getArg( "spawntarget" );
	
	if ( targetname )
	{
		tent = G_FindTarget( NULL, targetname );
		if ( tent )
		{
			args.setArg( "origin", va( "%f %f %f", tent->origin[ 0 ], tent->origin[ 1 ], tent->origin[ 2 ] ) );
			args.setArg( "angle", va( "%f", tent->angles[1] ) );
		}
		else
		{
			ev->Error( "Can't find targetname %s", targetname );
		}
	}
	
	//
	// make sure to setup spawnflags properly
	//
	level.spawnflags = 0;
	value = args.getArg( "spawnflags" );
	if ( value )
	{
		level.spawnflags = atoi( value );
	}
	
	ent = args.Spawn();
	
	ev->ReturnEntity( ent );
}

//FIXME
//Move this to someplace Level class.
static float last_fraction = 1.0f/8.0f;

void CThread::Letterbox( Event *ev )
{
	float time;
	
	if ( ev->NumArgs() < 1 )
	{
		warning ("CThread::Letterbox", "time parameter required!\n");
		return;
	}
	
	last_fraction = 1.0f/8.0f;
	
	time = ev->GetFloat( 1 );
	
	if ( ev->NumArgs() > 1 )
	{
		last_fraction = ev->GetFloat( 2 );
	}
	
	// this is now handled server side
	//gi.SendServerCommand( NULL, va( "letterbox %0.2f 1 %0.2f",time,last_fraction ) );
	level.m_letterbox_time_start = time;
	level.m_letterbox_dir = letterbox_in;
	
	level.m_letterbox_time = time;
	level.m_letterbox_fraction = last_fraction;
}

void CThread::ClearLetterbox( Event *ev )
{
	float time;
	
	time = ev->GetFloat( 1 );
	
	// this is now handled server side
	//gi.SendServerCommand( NULL, va( "letterbox %0.2f -1 %0.2f",time,last_fraction ) );
	level.m_letterbox_time_start = time;
	level.m_letterbox_dir = letterbox_out;
	
	level.m_letterbox_time = time;
	level.m_letterbox_fraction = last_fraction;
}

void CThread::FadeIn( Event *ev )
{
	level.m_fade_time_start = ev->GetFloat( 1 );
	level.m_fade_time       = ev->GetFloat( 1 );
	level.m_fade_color[0]   = ev->GetFloat( 2 );
	level.m_fade_color[1]   = ev->GetFloat( 3 );
	level.m_fade_color[2]   = ev->GetFloat( 4 );
	level.m_fade_alpha      = ev->GetFloat( 5 );
	level.m_fade_type       = fadein;
	level.m_fade_style      = alphablend;
	
	if ( ev->NumArgs() > 5 )
	{
		level.m_fade_style   = (fadestyle_t)ev->GetInteger( 6 );
	}
}

void CThread::ClearFade( Event *ev )
{
	// this is now handled server side
	//gi.SendServerCommand( NULL, "clearfade" );
	level.m_fade_time = -1;
	level.m_fade_type = fadein;
}

void CThread::FadeOut( Event *ev )
{
	// Make sure we are not already faded or fading out

	if ( ( level.m_fade_type == fadeout ) && ( level.m_fade_time_start > 0.0f ) )
	{
		float alpha;

		alpha = 1.0f - ( level.m_fade_time / level.m_fade_time_start );

		if ( alpha > 0.0f )
			return;
	}

	level.m_fade_time_start = ev->GetFloat( 1 );
	level.m_fade_time       = ev->GetFloat( 1 );
	level.m_fade_color[0]   = ev->GetFloat( 2 );
	level.m_fade_color[1]   = ev->GetFloat( 3 );
	level.m_fade_color[2]   = ev->GetFloat( 4 );
	level.m_fade_alpha      = ev->GetFloat( 5 );
	level.m_fade_type       = fadeout;
	level.m_fade_style      = alphablend;
	
	if ( ev->NumArgs() > 5 )
	{
		level.m_fade_style   = (fadestyle_t)ev->GetInteger( 6 );
	}
}

void CThread::FadeIsActive( Event *ev )
{
	if ( level.m_fade_time > 0.0f )
		ev->ReturnInteger( true );
	else
		ev->ReturnInteger( false );
}

void CThread::MusicEvent( Event *ev )
{
	const char *current;
	const char *fallback;
	
	current = ev->GetString( 1 );
	
	fallback = NULL;
	if ( ev->NumArgs() > 1 )
	{
		fallback = ev->GetString( 2 );
	}
	
	ChangeMusic( current, fallback, false );
}

void CThread::MusicVolumeEvent( Event *ev )
{
	float volume;
	float fade_time;
	
	volume    = ev->GetFloat( 1 );
	fade_time = ev->GetFloat( 2 );
	
	ChangeMusicVolume( volume, fade_time );
}

void CThread::RestoreMusicVolumeEvent( Event *ev )
{
	float fade_time;
	
	fade_time = ev->GetFloat( 1 );
	
	RestoreMusicVolume( fade_time );
}

//----------------------------------------------------------------
// Name:			allowMusicDucking
// Class:			CThread
//
// Description:		Specifies whether or not music ducking is allowed
//
// Parameters:		Event *ev				- contains bool, specifies whether or not music ducking is allowed
//
// Returns:			none
//----------------------------------------------------------------

void CThread::allowMusicDucking( Event *ev )
{
	if ( ev->NumArgs() > 0 )
		G_AllowMusicDucking( ev->GetBoolean( 1 ) );
	else
		G_AllowMusicDucking( true );
}

//----------------------------------------------------------------
// Name:			allowActionMusic
// Class:			CThread
//
// Description:		Specifies whether or not action music is allowed
//
// Parameters:		Event *ev				- contains bool, specifies whether or not action music is allowed
//
// Returns:			none
//----------------------------------------------------------------

void CThread::allowActionMusic( Event *ev )
{
	if ( ev->NumArgs() > 0 )
		G_AllowActionMusic( ev->GetBoolean( 1 ) );
	else
		G_AllowActionMusic( true );
}

void CThread::ForceMusicEvent( Event *ev )
{
	const char *current;
	const char *fallback;
	
	current = ev->GetString( 1 );
	fallback = NULL;
	if ( ev->NumArgs() > 1 )
	{
		fallback = ev->GetString( 2 );
	}
	
	ChangeMusic( current, fallback, true );
}

void CThread::SoundtrackEvent( Event *ev )
{
	ChangeSoundtrack( ev->GetString( 1 ) );
}

void CThread::RestoreSoundtrackEvent( Event *ev )
{
	RestoreSoundtrack();
}

void CThread::SetCinematic( Event *ev )
{
	G_StartCinematic();
}

void CThread::SetNonCinematic( Event *ev )
{
	G_StopCinematic();
	//level.cinematic = false;
}

void CThread::SetLevelAI( Event *ev )
{
	qboolean ai = false;
	
	if ( ev->NumArgs() > 0 )
		ai = ev->GetBoolean( 1 );
	
	if ( ai )
		level.ai_on = true;
	else
		level.ai_on = false;
}

void CThread::SetSkipThread( Event *ev )
{
	world->skipthread = ev->GetString( 1 );
}

void CThread::PassToPathmanager( Event *ev )
{
	thePathManager.ProcessEvent( ev );
}

void CThread::StuffCommand( Event *ev )
{
	int i;
	str command;
	
	for( i = 1; i <= ev->NumArgs(); i++ )
	{
		command += ev->GetString( i );
		command += " ";
	}
	
	if ( command.length() )
	{
		gi.SendConsoleCommand( command.c_str() );
	}
}

void CThread::KillEnt( Event * ev )
{
	int num;
	Entity *ent;
	
	if ( ev->NumArgs() != 1 )
	{
		ev->Error( "No args passed in" );
		return;
	}
	
	num = ev->GetInteger( 1 );
	if ( ( num < 0 ) || ( num >= globals.max_entities ) )
	{
		ev->Error( "Value out of range.  Possible values range from 0 to %d.\n", globals.max_entities );
		return;
	}
	
	ent = G_GetEntity( num );
	ent->Damage( world, world, ent->max_health + 25.0f, vec_zero, vec_zero, vec_zero, 0, 0, 0 );
}

void CThread::RemoveEnt( Event * ev )
{
	int num;
	Entity *ent;
	
	if ( ev->NumArgs() != 1 )
	{
		ev->Error( "No args passed in" );
		return;
	}
	
	num = ev->GetInteger( 1 );
	if ( ( num < 0 ) || ( num >= globals.max_entities ) )
	{
		ev->Error( "Value out of range.  Possible values range from 0 to %d.\n", globals.max_entities );
		return;
	}
	
	ent = G_GetEntity( num );
	ent->PostEvent( Event( EV_Remove ), 0.0f );
}

void CThread::KillClass( Event * ev )
{
	int except;
	str classname;
	gentity_t * from;
	Entity *ent;
	
	if ( ev->NumArgs() < 1 )
	{
		ev->Error( "No args passed in" );
		return;
	}
	
	classname = ev->GetString( 1 );
	
	except = 0;
	if ( ev->NumArgs() == 2 )
	{
		except = ev->GetInteger( 1 );
	}
	
	for ( from = &g_entities[ game.maxclients ]; from < &g_entities[ globals.num_entities ]; from++ )
	{
		if ( !from->inuse )
		{
			continue;
		}
		
		assert( from->entity );
		
		ent = from->entity;
		
		if ( ent->entnum == except )
		{
			continue;
		}
		
		if ( ent->inheritsFrom( classname.c_str() ) )
		{
			ent->Damage( world, world, ent->max_health + 25.0f, vec_zero, vec_zero, vec_zero, 0, 0, 0 );
		}
	}
}

// RemoveActorsNamed - remove all actors in the game that have a matching name field
void CThread::RemoveActorsNamed( Event* ev )
{
	if ( ev->NumArgs() < 1 )
	{
		ev->Error( "No args passed in" );
		return;
	}
	
	str removename = ev->GetString( 1 );

	for( gentity_t* gent = &g_entities[ game.maxclients ]; 
		gent < &g_entities[ globals.num_entities ]; 
		gent++ )
	{
		if( !gent->inuse ) continue;
		assert( gent->entity );
		if( !gent->entity->inheritsFrom( "Actor" ) ) continue;

		Actor* act = static_cast< Actor* >( gent->entity );
		if( act->getName() == removename )
		{
			gent->entity->PostEvent( Event( EV_Remove ), 0.0f );
		}
	}
}

void CThread::RemoveClass( Event * ev )
{
	int except;
	str classname;
	gentity_t * from;
	Entity *ent;
	
	if ( ev->NumArgs() < 1 )
	{
		ev->Error( "No args passed in" );
		return;
	}
	
	classname = ev->GetString( 1 );
	
	except = 0;
	if ( ev->NumArgs() == 2 )
	{
		except = ev->GetInteger( 1 );
	}
	
	for ( from = &g_entities[ game.maxclients ]; from < &g_entities[ globals.num_entities ]; from++ )
	{
		if ( !from->inuse )
		{
			continue;
		}
		
		assert( from->entity );
		
		ent = from->entity;
		
		if ( ent->entnum == except )
			continue;
		
		if ( ent->inheritsFrom( classname.c_str() ) )
		{
			ent->PostEvent( Event( EV_Remove ), 0.0f );
		}
	}
}

//===============================================================
// Name:		doesEntityExist
// Class:		CThread
//
// Description: Determines if an entity with the specified name
//				exists.  The return value for the event is either
//				a 1 or a 0 ( true or false ).
// 
// Parameters:	Event* -- the event that triggered this call.
//
// Returns:		None
// 
//===============================================================

void CThread::doesEntityExist( Event *ev )
{
	if ( ev->IsEntityAt( 1 ) )
		ev->ReturnFloat( 1.0f );
	else
		ev->ReturnFloat( 0.0f );
}


void CThread::GetEntityEvent( Event *ev )
{
	TargetList *tlist;
	
	str name( ev->GetString( 1 ) );
	if ( ( name.length() > 0 ) && name[ 0 ] == '*' )
	{
		ev->ReturnEntity( G_GetEntity( atoi( &name[ 1 ] ) ) );
		return;
	}
	tlist = world->GetTargetList( name, false );
	
	//FIXME
	//ev->ReturnInteger( ( int )tlist );
	if ( tlist )
	{
		ev->ReturnEntity( tlist->GetNextEntity( NULL ) );
	}
	else
	{
		ev->ReturnEntity( NULL );
	}
}

void CThread::GetNextEntityEvent( Event *ev )
{
	TargetList *tlist;
	
	Entity *ent = 0;
	ent = ev->GetEntity( 1 );
	
	if ( !ent )
	{
		ev->ReturnEntity( NULL );
		return;
	}
	
	tlist = world->GetTargetList( ent->targetname, false );
	
	if ( tlist )
		ev->ReturnEntity( tlist->GetNextEntity( ent ) );
	else
		ev->ReturnEntity( NULL );
}

void CThread::CosDegrees( Event *ev )
{
	float degrees = ev->GetFloat( 1 );
	float radians = DEG2RAD( degrees );
	float result = cos( radians );
	
	ev->ReturnFloat( result );
}

void CThread::SinDegrees( Event *ev )
{
	float degrees = ev->GetFloat( 1 );
	float radians = DEG2RAD( degrees );
	float result = sin( radians );
	
	ev->ReturnFloat( result );
}

void CThread::CosRadians( Event *ev )
{
	float radians = ev->GetFloat( 1 );
	float result = cos( radians );
	
	ev->ReturnFloat( result );
}

void CThread::SinRadians( Event *ev )
{
	float radians = ev->GetFloat( 1 );
	float result = sin( radians );
	
	ev->ReturnFloat( result );
}

void CThread::ArcTanDegrees( Event *ev )
{
	float y = ev->GetFloat( 1 );
	float x = ev->GetFloat( 2 );
	float radians = atan2( y, x );
	float degrees = RAD2DEG( radians );
	
	ev->ReturnFloat( degrees );
}

void CThread::ScriptSqrt( Event *ev )
{
	/// Perform a sign-preserving square root ( i.e. sqrt( -16 ) = -4 )
	float x = ev->GetFloat( 1 );
	if( x < 0.0f )
	{
		x = static_cast<float>( -sqrt( -x ) );
	}
	else
	{
		x = static_cast<float>( sqrt( x ) );
	}
	ev->ReturnFloat( x );
}

void CThread::ScriptLog( Event *ev )
{
	float x = ev->GetFloat( 1 );

	ev->ReturnFloat( log( x ) );
}

void CThread::ScriptExp( Event *ev )
{
	float x = ev->GetFloat( 1 );

	ev->ReturnFloat( exp( x ) );
}

void CThread::RandomFloat( Event *ev )
{
	ev->ReturnFloat( G_Random( ev->GetFloat( 1 ) ) );
}

void CThread::RandomInteger( Event *ev )
{
	ev->ReturnFloat( (float)(int)G_Random( (float)ev->GetInteger( 1 ) ) );
}

void CThread::CameraCommand( Event * ev )
{
	Event *e;
	const char *cmd;
	int   i;
	int   n;
	
	if ( !ev->NumArgs() )
	{
		ev->Error( "Usage: cam [command] [arg 1]...[arg n]" );
		return;
	}
	
	cmd = ev->GetString( 1 );
	if ( Event::Exists( cmd ) )
	{
		e = new Event( cmd );
		e->SetSource( EV_FROM_SCRIPT );
		e->SetThread( this );
		e->SetLineNumber( ev->GetLineNumber() );
		
		n = ev->NumArgs();
		for( i = 2; i <= n; i++ )
		{
			e->AddToken( ev->GetToken( i ) );
		}
		
		CameraMan.ProcessEvent( e );
	}
	else
	{
		ev->Error( "Unknown camera command '%s'.\n", cmd );
	}
}

void CThread::SetLightStyle( Event *ev )
{
	gi.SetLightStyle( ev->GetInteger( 1 ), ev->GetString( 2 ) );
}

void CThread::KillThreadEvent( Event *ev )
{
	str threadToKill;

	threadToKill = ev->GetString( 1 );

	if ( threadToKill == threadName )
	{
		gi.WDPrintf( "Trying to kill the current thread %s, this is very bad!\n", threadName.c_str() );
		return;
	}

	if ( threadToKill.length() == 0 )
	{
		gi.WDPrintf( "Trying to kill a thread with no name\n" );
		return;
	}

	Director.KillThread( threadToKill );
}

void CThread::SetThreadName( Event *ev )
{
	threadName = ev->GetString( 1 );
}

void CThread::GetCurrentEntity( Event *ev )
{
	if ( currentEntity )
		ev->ReturnEntity( currentEntity );
	else
		ev->ReturnEntity ( NULL );
}

void CThread::SetCurrentEntity( Entity *ent )
{
	if ( ent )
		currentEntity = ent;
}

void CThread::Parse( const char *filename )
{
	str      token;
	Script   script;
	int      eventnum;
	Event    *ev;
	
	script.LoadFile( filename );
	while( script.TokenAvailable( true ) )
	{
		token = script.GetToken( true );
		if ( token == "end" )
		{
			break;
		}
		
		eventnum = Event::FindEvent( token );
		if ( !eventnum )
		{
			gi.WDPrintf( "Unknown command '%s' in %s\n", token.c_str(), filename );
			script.SkipToEOL();
		}
		else
		{
			ev = new Event( eventnum );
			while( script.TokenAvailable( false ) )
            {
				ev->AddToken( script.GetToken( false ) );
            }
			
			ProcessEvent( ev );
		}
	}
}

void CThread::SendCommandToSlaves( const char *name, Event *ev )
{
	Event		   *sendevent;
	Entity	   *ent;
	TargetList  *tlist;
	int         i;
	int         num;
	
	if ( name && name[ 0 ] )
	{
		updateList.ClearObjectList();
		
		tlist = world->GetTargetList( str( name + 1 ) );
		num = tlist->list.NumObjects();
		for ( i = 1; i <= num; i++ )
		{
			ent = tlist->list.ObjectAt( i );
			
			assert( ent );
			
			sendevent = new Event( *ev );
			
			if ( !updateList.ObjectInList( ent->entnum ) )
			{
				updateList.AddObject( ent->entnum );
				
				// Tell the object that we're about to send it some orders
				ent->ProcessEvent( EV_Script_NewOrders );
			}
			
			// Send the command
			ent->ProcessEvent( sendevent );
		}
		
		if ( !num )
		{
			warning( "SendCommandToSlaves", "Could not find target %s in world.\n", name );
		}
	}
	
	//
	// free up the event
	//
	delete ev;
}

void CThread::ProcessCommand( int argc, const char **argv )
{
	str		command;
	str		name;
	Event		*event;
	Entity	*ent;
	
	if ( argc < 1 )
	{
		return;
	}
	
	name = argv[ 0 ];
	if ( argc > 1 )
	{
		command = argv[ 1 ];
	}
	
	// Check for object commands
	if ( name[ 0 ] == '$' )
	{
		if ( Event::FindEvent( command.c_str() ) )
		{
			event = new Event( command );
			event->SetSource( EV_FROM_SCRIPT );
			event->SetThread( this );
			event->SetLineNumber( 0 );
			event->AddTokens( argc - 2, &argv[ 2 ] );
			SendCommandToSlaves( name.c_str(), event );
		}
		return;
	}
	
	// Check for entnum commands
	if ( name[ 0 ] == '*' )
	{
		if ( !IsNumeric( &name[ 1 ] ) )
		{
			gi.WPrintf( "Expecting numeric value for * command, but found '%s'\n", &name[ 1 ] );
		}
		else if ( Event::FindEvent( command.c_str() ) )
		{
			ent = G_GetEntity( atoi( &name[ 1 ] ) );
			if ( ent )
            {
				event = new Event( command );
				event->SetSource( EV_FROM_SCRIPT );
				event->SetThread( this );
				event->SetLineNumber( 0 );
				event->AddTokens( argc - 2, &argv[ 2 ] );
				ent->ProcessEvent( event );
            }
			else
            {
				gi.WPrintf( "Entity not found for * command\n" );
            }
		}
		return;
	}
	
	// Handle global commands
	if ( Event::FindEvent( name.c_str() ) )
	{
		event = new Event( name );
		event->SetSource( EV_FROM_SCRIPT );
		event->SetThread( this );
		event->SetLineNumber( 0 );
		event->AddTokens( argc - 1, &argv[ 1 ] );
		if ( !ProcessEvent( event ) )
		{
			gi.WPrintf( "Invalid global command '%s'\n", name.c_str() );
		}
	}
}

inline void CThread::Archive( Archiver &arc )
{
	int exists;
	
	Interpreter::Archive( arc );
	
	arc.ArchiveFloat( &waitUntil );
	
	// save waiting for thread
	
	if ( arc.Saving() )
	{
		if ( waitingForThread )
			exists = 1;
		else
			exists = 0;
	}
	
	arc.ArchiveInteger( &exists );
	
	if ( exists )
		arc.ArchiveObjectPointer( ( Class ** )&waitingForThread );
	else if ( arc.Loading() )
		waitingForThread = NULL;
	
	arc.ArchiveString( &waitingForDeath );
	arc.ArchiveBoolean( &waitingForPlayer );
	
	if ( arc.Saving() )
	{
		if ( waitingFor )
			exists = 1;
		else
			exists = 0;
	}
	
	arc.ArchiveInteger( &exists );
	
	if ( exists )
		arc.ArchiveObjectPointer( ( Class ** )&waitingFor );
	else if ( arc.Loading() )
		waitingFor = NULL;
	
	arc.ArchiveInteger( &waitingNumObjects );
	
	arc.ArchiveSafePointer( &currentEntity );
}

void CThread::SetFloatVar( Event *ev )
{
	str var_name;
	const char *real_var_name;
	float value;
	
	var_name = ev->GetString( 1 );
	value    = ev->GetFloat( 2 );
	
	if ( strncmp( var_name.c_str(), "level.", 6 ) == 0 )
	{
		real_var_name = var_name.c_str() + 6;
		levelVars.SetVariable( real_var_name, value );
	}
	else if ( strncmp( var_name.c_str(), "game.", 5 ) == 0 )
	{
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: using now +5 instead of +6 (the first letter got cut off) - chrissstrahl
		//--------------------------------------------------------------
		real_var_name = var_name.c_str() + 5;


		gameVars.SetVariable( real_var_name, value );
	}
}

void CThread::SetVectorVar( Event *ev )
{
	str var_name;
	const char *real_var_name;
	Vector value;
	
	var_name = ev->GetString( 1 );
	value    = ev->GetVector( 2 );
	
	if ( strncmp( var_name.c_str(), "level.", 6 ) == 0 )
	{
		real_var_name = var_name.c_str() + 6;
		levelVars.SetVariable( real_var_name, value );
	}
	else if ( strncmp( var_name.c_str(), "game.", 5 ) == 0 )
	{
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: using now +5 instead of +6 (the first letter got cut off) - chrissstrahl
		//--------------------------------------------------------------
		real_var_name = var_name.c_str() + 5;


		gameVars.SetVariable( real_var_name, value );
	}
}

void CThread::SetStringVar( Event *ev )
{
	str var_name;
	const char *real_var_name;
	str value;
	
	var_name = ev->GetString( 1 );
	value    = ev->GetString( 2 );
	
	if ( strncmp( var_name.c_str(), "level.", 6 ) == 0 )
	{
		real_var_name = var_name.c_str() + 6;
		levelVars.SetVariable( real_var_name, value );
	}
	else if ( strncmp( var_name.c_str(), "game.", 5 ) == 0 )
	{
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: using now +5 instead of +6 (the first letter got cut off) - chrissstrahl
		//--------------------------------------------------------------
		real_var_name = var_name.c_str() + 5;


		gameVars.SetVariable( real_var_name, value );
	}
}

void CThread::RemoveVariable( Event* ev )
{
	str var_name;
	const char* real_var_name;

	var_name = ev->GetString( 1 );

	if ( strncmp( var_name.c_str(), "level.", 6 ) == 0 )
	{
		real_var_name = var_name.c_str() + 6;
		levelVars.RemoveVariable( real_var_name );
	}
	else if ( strncmp( var_name.c_str(), "game.", 5 ) == 0 )
	{
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: using now +5 instead of +6 (the first letter got cut off) - chrissstrahl
		//--------------------------------------------------------------
		real_var_name = var_name.c_str() + 5;


		levelVars.RemoveVariable( real_var_name );
	}
}

void CThread::GetFloatVar( Event *ev )
{
	str var_name;
	const char *real_var_name;
	ScriptVariable *var = NULL;
	
	var_name = ev->GetString( 1 );
	
	if ( strncmp( var_name.c_str(), "level.", 6 ) == 0 )
	{
		real_var_name = var_name.c_str() + 6;
		var = levelVars.GetVariable( real_var_name );
	}
	else if ( strncmp( var_name.c_str(), "game.", 5 ) == 0 )
	{
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: using now +5 instead of +6 (the first letter got cut off) - chrissstrahl
		//--------------------------------------------------------------
		real_var_name = var_name.c_str() + 5;


		var = gameVars.GetVariable( real_var_name );
	}
	
	if ( var )
		ev->ReturnFloat( var->floatValue() );
	else
	{
		gi.WDPrintf( "%s variable not found\n", var_name.c_str() );
		ev->ReturnFloat( 0.0f );
	}
}

void CThread::GetVectorVar( Event *ev )
{
	str var_name;
	const char *real_var_name;
	ScriptVariable *var = NULL;
	
	var_name = ev->GetString( 1 );
	
	if ( strncmp( var_name.c_str(), "level.", 6 ) == 0 )
	{
		real_var_name = var_name.c_str() + 6;
		var = levelVars.GetVariable( real_var_name );
	}
	else if ( strncmp( var_name.c_str(), "game.", 5 ) == 0 )
	{
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: using now +5 instead of +6 (the first letter got cut off) - chrissstrahl
		//--------------------------------------------------------------
		real_var_name = var_name.c_str() + 5;


		var = gameVars.GetVariable( real_var_name );
	}
	
	if ( var )
		ev->ReturnVector( var->vectorValue() );
	else
	{
		gi.WDPrintf( "%s variable not found\n", var_name.c_str() );
		ev->ReturnVector( vec_zero );
	}
}

void CThread::GetStringVar( Event *ev )
{
	str var_name;
	const char *real_var_name;
	ScriptVariable *var = NULL;
	
	var_name = ev->GetString( 1 );
	
	if ( strncmp( var_name.c_str(), "level.", 6 ) == 0 )
	{
		real_var_name = var_name.c_str() + 6;
		var = levelVars.GetVariable( real_var_name );
	}
	else if ( strncmp( var_name.c_str(), "game.", 5 ) == 0 )
	{
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: using now +5 instead of +6 (the first letter got cut off) - chrissstrahl
		//--------------------------------------------------------------
		real_var_name = var_name.c_str() + 5;


		var = gameVars.GetVariable( real_var_name );
	}
	
	if ( var )
		ev->ReturnString( var->stringValue() );
	else
	{
		gi.WDPrintf( "%s variable not found\n", var_name.c_str() );
		ev->ReturnString( "\n" );
	}
}

void CThread::doesVarExist( Event *ev )
{
	str var_name;
	const char *real_var_name;
	ScriptVariable *var = NULL;
	
	var_name = ev->GetString( 1 );
	
	if ( strncmp( var_name.c_str(), "level.", 6 ) == 0 )
	{
		real_var_name = var_name.c_str() + 6;
		var = levelVars.GetVariable( real_var_name );
	}
	else if ( strncmp( var_name.c_str(), "game.", 5 ) == 0 )
	{
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: using now +5 instead of +6 (the first letter got cut off) - chrissstrahl
		//--------------------------------------------------------------
		real_var_name = var_name.c_str() + 5;


		var = gameVars.GetVariable( real_var_name );
	}
	
	if ( var )
		ev->ReturnFloat( 1.0f );
	else
		ev->ReturnFloat( 0.0f );
}

void CThread::CenterPrint( Event *ev )
{
	int         j;
	gentity_t   *other;
	
	for( j = 0; j < game.maxclients; j++ )
	{
		other = &g_entities[ j ];
		if ( other->inuse && other->client )
		{
			//--------------------------------------------------------------
			// GAMEFIX - Changed: importance from normal to critical, to make sure the message is always shown - chrissstrahl
			//--------------------------------------------------------------
			gi.centerprintf( other, CENTERPRINT_IMPORTANCE_CRITICAL, ev->GetString( 1 ) );
		}
	}
}

void CThread::isActorDead( Event *ev )
{
	ev->ReturnInteger( isActorDead( ev->GetString( 1 ) ) );
}

qboolean CThread::isActorDead( const str &actor_name )
{
	Actor *act;
	
	act = GetActor( actor_name );
	
	if ( !act || act->deadflag || ( act->health <= 0 ) )
		return true;
	
	return false;
}

void CThread::isActorAlive( Event *ev )
{
	ev->ReturnFloat( isActorAlive( ev->GetString( 1 ) ) );
}

qboolean CThread::isActorAlive( const str &actor_name )
{
	Actor *act;
	
	act = GetActor( actor_name );
	
	if ( !act || act->deadflag || ( act->health <= 0 ) )
		return false;
	
	return true;
}

void CThread::SendClientCommand( Event *ev )
{
	Entity	*entity;
	Player	*player;
	str		builtCommand;
	int		i;
	
	
	// Get the player
	
	entity = ev->GetEntity( 1 );

	//--------------------------------------------------------------
	// GAMEFIX - Fixed: crash if given entity does not exist - chrissstrahl
	//--------------------------------------------------------------
	if (!entity) {
		gi.Printf(_GFixEF2_WARN_EVENT_CTHREAD_sendClientCommand_FAILED);
		return;
	}
	
	if ( !entity->isSubclassOf( Player ) )
		return;
	
	player = (Player *)entity;
	
	// Build the command
	
	builtCommand += "stufftext";
	
	builtCommand += " \"";
	
	for ( i = 2 ; i <= ev->NumArgs() ; i++ )
	{
		builtCommand += " ";
		
		builtCommand += ev->GetString( i );
	}
	
	builtCommand += "\"\n";
	

	//--------------------------------------------------------------
	// GAMEFIX - Added: Script Code based client commands allowing delayed handle to prevent overflow - chrissstrahl
	//--------------------------------------------------------------
	gamefix_playerDelayedServerCommand(player->edict - g_entities, builtCommand.c_str());
}

void CThread::GetNumFreeReliableServerCommands( Event* ev )
{
	Entity* entity;
	Player* player;
	int i;

	entity = ev->GetEntity( 1 );

	//--------------------------------------------------------------
	// GAMEFIX - Fixed: crash if given entity does not exist - chrissstrahl
	//--------------------------------------------------------------
	if (!entity) {
		gi.Printf(_GFixEF2_WARN_EVENT_CTHREAD_getNumFreeRelSVCmds_FAILED);
		ev->ReturnFloat( 0.0 );
		return;
	}


	if( !entity->isSubclassOf( Player ) )
	{
		ev->ReturnFloat( 0.0 );
		return;
	}

	player = (Player*) entity;

	i = gi.GetNumFreeReliableServerCommands( player->edict - g_entities );
	ev->ReturnFloat( i );
}

void CThread::SendClientVar( Event *ev )
{
	Entity	*entity;
	Player	*player;
	str		builtCommand;
	str		varName;
	str		realVarName;
	ScriptVariable *var = NULL;
	str		levelVarPrefix = "level.";
	
	
	// Get the player
	
	entity = ev->GetEntity( 1 );
	
	if ( !entity->isSubclassOf( Player ) )
		return;
	
	player = (Player *)entity;
	
	// Build the command
	
	builtCommand += "stufftext";
	
	builtCommand += " \"SetClientVar ";
	
	// Add the variable name
	
	varName = ev->GetString( 2 );
	

	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Warning C4996 strnicmp: The POSIX name for this item is deprecated. Using Q_stricmpn instead. - chrissstrahl
	//--------------------------------------------------------------
	if ( Q_stricmpn( varName.c_str(), levelVarPrefix.c_str(), levelVarPrefix.length() ) != 0 )
	{
		gi.WDPrintf( "SendClientVar can only send level vars, %s not allowed\n", varName.c_str() );
		return;
	}
	
	realVarName = varName.c_str() + levelVarPrefix.length();
	
	builtCommand += realVarName;
	
	// Add the variable value
	
	builtCommand += " ";
	
	var = levelVars.GetVariable( realVarName );
	
	if ( var )
		builtCommand += var->stringValue();
	else
		return;
	
	builtCommand += "\"\n";
	
	// Send the command
	
	gi.SendServerCommand( player->edict - g_entities, builtCommand.c_str() );
}

//===============================================================
// Name:		ModEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_Mod event.
//				Gets the integer remainder of a division.
// 
// Parameters:	Event* -- Argument 1 is the nominator.
//						  Argument 2 is the denominator.
//						  Return cache is the integer remainder.
//
// Returns:		None
// 
//===============================================================
void CThread::ModEvent( Event *ev )
{
	assert( ev );
	if ( ev->NumArgs() != 2 )
	{
		ev->Error("Usage: variable = %s( nominator, denominator)\n", ev->getName());
		return ;
	}
	
	int	nominator	= ev->GetInteger( 1 );
	int	denominator = ev->GetInteger( 2 );
	int	remainder	= nominator % denominator ;
	ev->ReturnFloat( remainder );
}

//===============================================================
// Name:		DivEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_Div event.
//				Gets the integer result of a division.
// 
// Parameters:	Event* -- Argument 1 is the nominator
//						  Argument 2 is the denominator
//						  Return cache is the integer result
//
// Returns:		None
// 
//===============================================================
void CThread::DivEvent( Event *ev )
{
	assert( ev );
	if ( ev->NumArgs() != 2 )
	{
		ev->Error("Usage: variable = %s( nominator, denominator )\n", ev->getName());
		return ;
	}
	
	int	nominator	= ev->GetInteger( 1 );
	int	denominator	= ev->GetInteger( 2 );
	int	result		= nominator / denominator ;
	
	ev->ReturnFloat( result );
}

//===============================================================
// Name:		VectorScaleEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_VectorScaleEvent.
//				Scales a vector.
// 
// Parameters:	Event* -- Argument 1 is the vector to scale (vector)
//						  Argument 2 is the amount to scale (float)
//						  Return cache is the scaled vector.
//
// Returns:		None
// 
//===============================================================
void CThread::VectorScaleEvent( Event *ev )
{
	assert( ev );
	
	if ( ev->NumArgs() != 2 )
	{
		ev->Error("Usage: vectorvariable = %s( vector, scale_value )\n", ev->getName());
		return ;
	}
	
	Vector	v		= ev->GetVector( 1 );
	float	scaler	= ev->GetFloat( 2 );
	Vector returnVector = v * scaler ;
	
	ev->ReturnVector( returnVector );
}

//===============================================================
// Name:		VectorDotEvent
// Class:		CThread
//
// Description: Handles EV_ScriptThread_VectorDotEvent.
//				Gets the dot product of two vectors.
// 
// Parameters:	Event* -- Argument 1 is the first vector.
//						  Argument 2 is the second vector.
//						  Return cache is the dot product value.
//
// Returns:		None
// 
//===============================================================
void CThread::VectorDotEvent( Event *ev )
{
	assert( ev );
	
	if ( ev->NumArgs() != 2 )
	{
		ev->Error("Usage: floatvariable = %s( vector, vector )\n", ev->getName());
		return ;
	}
	
	Vector	v1		= ev->GetVector( 1 );
	Vector	v2		= ev->GetVector( 2 );
	float	result	= Vector::Dot( v1, v2 );
	
	ev->ReturnFloat( result );
}

//===============================================================
// Name:		VectorCrossEvent
// Class:		CThread
//
// Description: Handles EV_ScriptThread_VectorCrossEvent.
//				Gets the cross product of two vectors.
// 
// Parameters:	Event* -- Argument 1 is the first vector.
//						  Argument 2 is the second vector.
//						  Return cache is the cross product value.
//
// Returns:		None
// 
//===============================================================
void CThread::VectorCrossEvent( Event *ev )
{
	assert( ev );
	
	if ( ev->NumArgs() != 2 )
	{
		ev->Error("Usage: vectorvariable = %s( vector, vector )\n", ev->getName());
		return ;
	}
	
	Vector	v1		= ev->GetVector( 1 );
	Vector	v2		= ev->GetVector( 2 );
	Vector	result ;
	result.CrossProduct( v1, v2 );
	
	ev->ReturnVector( result );
}

//===============================================================
// Name:		VectorLengthEvent
// Class:		CThread
//
// Description: Handles EV_ScriptThread_VectorLengthEvent.
//				Gets the length of a vector.
// 
// Parameters:	Event* -- Argument 1 is the vector to get the length of.
//						  Return cache is the vector length.
//
// Returns:		None
// 
//===============================================================
void CThread::VectorLengthEvent( Event *ev )
{
	assert( ev );
	
	if ( ev->NumArgs() != 1 )
	{
		ev->Error( "Usage: floatvariable = %s( vector )\n", ev->getName());
		return ;
	}
	
	Vector v1 = ev->GetVector( 1 );
	ev->ReturnFloat( v1.length() );
}

//===============================================================
// Name:		VectorNormalizeEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_VectorNormalizeEvent.  
//				Normalizes a vector.
// 
// Parameters:	Event* -- Argument 1 is the vector to normalize.
//						  Return cache is the normalized vector.
//
// Returns:		None
// 
//===============================================================
void CThread::VectorNormalizeEvent( Event *ev )
{
	assert( ev );
	
	if ( ev->NumArgs() != 1 )
	{
		ev->Error( "Usage: vectorvariable = %s( vector )\n", ev->getName());
		return ;
	}
	
	Vector v1 = ev->GetVector( 1 );
	v1.normalize();
	ev->ReturnVector( v1 );
}

//===============================================================
// Name:		VectorGetXEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_VectorGetXEvent.
//				Gets the x value from a vector.
// 
// Parameters:	Event* -- Argument 1 is the vector to get x from.
//						  Return cache is the x value.
//
// Returns:		None
// 
//===============================================================
void CThread::VectorGetXEvent( Event *ev )
{
	assert( ev );
	
	if (ev->NumArgs() != 1 )
	{
		ev->Error( "Usage: floatvariable = %s( vector )\n", ev->getName());
		return ;
	}
	
	Vector v1 = ev->GetVector( 1 );
	ev->ReturnFloat( v1.x ) ;
}

//===============================================================
// Name:		VectorGetYEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_VectorGetXEvent.
//				Gets the y value from a vector.
// 
// Parameters:	Event* -- Argument 1 is the vector to get y from.
//						  Return cache is the y value.
//
// Returns:		None
// 
//===============================================================
void CThread::VectorGetYEvent( Event *ev )
{
	assert( ev );
	
	if (ev->NumArgs() != 1 )
	{
		ev->Error( "Usage: floatvariable = %s( vector )\n", ev->getName());
		return ;
	}
	
	Vector v1 = ev->GetVector( 1 );
	ev->ReturnFloat( v1.y ) ;
}

//===============================================================
// Name:		VectorGetZEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_VectorGetXEvent.
//				Gets the z value from a vector.
// 
// Parameters:	Event* -- Argument 1 is the vector to get z from.
//						  Return cache is the z value.
//
// Returns:		None
// 
//===============================================================
void CThread::VectorGetZEvent( Event *ev )
{
	assert( ev );
	
	if (ev->NumArgs() != 1 )
	{
		ev->Error( "Usage: floatvariable = %s( vector )\n", ev->getName());
		return ;
	}
	
	Vector v1 = ev->GetVector( 1 );
	ev->ReturnFloat( v1.z ) ;
}

//===============================================================
// Name:		VectorSetXEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_VectorSetXEvent.
//				Sets the x value for a vector.
// 
// Parameters:	Event* -- Argument 1 is the vector to set the x of.
//						  Argument 2 is the x value for the vector
//						  Return cache is the resulting vector.
//							
//
// Returns:		None
// 
//===============================================================
void CThread::VectorSetXEvent( Event *ev )
{
	assert( ev );
	
	if (ev->NumArgs() != 2 )
	{
		ev->Error( "Usage: vectorvariable = %s( vector, x_value )\n", ev->getName());
		return ;
	}
	
	Vector v1 = ev->GetVector( 1 );
	v1.x = ev->GetFloat( 2 );
	ev->ReturnVector( v1 );
}

//===============================================================
// Name:		VectorSetYEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_VectorSetYEvent.
//				Sets the y value for a vector.
// 
// Parameters:	Event* -- Argument 1 is the vector to set the y of.
//						  Argument 2 is the y value for the vector
//						  Return cache is the resulting vector.
//							
//
// Returns:		None
// 
//===============================================================
void CThread::VectorSetYEvent( Event *ev )
{
	assert( ev );
	
	if (ev->NumArgs() != 2 )
	{
		ev->Error( "Usage: vectorvariable = %s( vector, y_value )\n", ev->getName());
		return ;
	}
	
	Vector v1 = ev->GetVector( 1 );
	v1.y = ev->GetFloat( 2 );
	ev->ReturnVector( v1 );
}

//===============================================================
// Name:		VectorSetZEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_VectorSetZEvent.
//				Sets the z value for a vector.
// 
// Parameters:	Event* -- Argument 1 is the vector to set the z of
//						  Argument 2 is the z value for the vector
//						  Return cache is the resulting vector.
//							
// Returns:		None
// 
//===============================================================
void CThread::VectorSetZEvent( Event *ev )
{
	assert( ev );
	
	if (ev->NumArgs() != 2 )
	{
		ev->Error( "Usage: vectorvariable = %s( vector, z_value )\n", ev->getName());
		return ;
	}
	
	Vector v1 = ev->GetVector( 1 );
	v1.z = ev->GetFloat( 2 );
	ev->ReturnVector( v1 );
}

//===============================================================
// Name:		VectorForwardEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_VectorForwardEvent.
//				Gets the forward vector of a vector.
// 
// Parameters:	Event* -- Argument 1 is the vector to get the forward of.
//						  Return cache is the forward vector.
//
// Returns:		None
// 
//===============================================================
void CThread::VectorForwardEvent( Event *ev )
{
	assert( ev );
	
	if ( ev->NumArgs() != 1 )
	{
		ev->Error( "Usage: floatvariable = %s( vector )\n", ev->getName());
		return ;
	}
	
	Vector forward ;
	Vector v1 = ev->GetVector( 1 );
	v1.AngleVectors( &forward );
	ev->ReturnVector( forward );
}

//===============================================================
// Name:		VectorLeftEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_VectorLeftEvent.
//				Gets the left vector of a vector.
// 
// Parameters:	Event* -- Argument 1 is the vector to get the left of.
//						  Return cache is the left vector.
//
// Returns:		None
// 
//===============================================================
void CThread::VectorLeftEvent( Event *ev )
{
	assert( ev );
	
	if ( ev->NumArgs() != 1 )
	{
		ev->Error( "Usage: floatvariable = %s( vector )\n", ev->getName());
		return ;
	}
	
	Vector left ;
	Vector v1 = ev->GetVector( 1 );
	v1.AngleVectors( 0, &left );
	ev->ReturnVector( left );
}

//===============================================================
// Name:		VectorUpEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_VectorUpEvent.
//				Gets the up vector of a vector.
// 
// Parameters:	Event* -- Argument 1 is the vector to get the up of.
//						  Return cache is the up vector.
//
// Returns:		None
// 
//===============================================================
void CThread::VectorUpEvent( Event *ev )
{
	assert( ev );
	
	if ( ev->NumArgs() != 1 )
	{
		ev->Error( "Usage: floatvariable = %s( vector )\n", ev->getName());
		return ;
	}
	
	Vector up ;
	Vector v1 = ev->GetVector( 1 );
	v1.AngleVectors( 0, 0, &up );
	ev->ReturnVector( up );
}

//===============================================================
// Name:		vectorToString
// Class:		CThread
//
// Description: Converts a vector to a string
// 
// Parameters:	Event *ev				- contains a vector
//
// Returns:		None
// 
//===============================================================

void CThread::vectorToString( Event *ev )
{
	str stringResult;
	Vector vectorToConvert;
	
	vectorToConvert = ev->GetVector( 1 );
	
	stringResult = "\"";
	stringResult += vectorToConvert.x;
	stringResult += " ";
	stringResult += vectorToConvert.y;
	stringResult += " ";
	stringResult += vectorToConvert.z;
	stringResult += "\"";
	
	ev->ReturnString( stringResult );
}

//===============================================================
// Name:		TargetOfEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_TargetOfEvent.
//				Gets the target of an entity.
// 
// Parameters:	Event* -- Argument 1 is the entity whose target you want.
//						  Return cache is the name of the target,
//						  or the empty string if no target.
//
// Returns:		None
// 
//===============================================================
void CThread::TargetOfEvent( Event *ev )
{
	assert( ev );
	
	if ( ev->NumArgs() != 1 )
	{
		ev->Error( "Usage: stringvariable = %s( entity )\n", ev->getName());
		return ;
	}
	
	Entity *entity = ev->GetEntity( 1 );
	if ( !entity )
	{
		ev->Error( "First argument of %s is not an entity!\n", ev->getName());
		return ;
	}
	
	const char *target = entity->Target();
	ev->ReturnString( target ? target : "" );
}



//===============================================================
// Name:		FloorEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_FloorEvent,
//				Floors the specified floor.
// 
// Parameters:	Event* -- Argument 1 is the float you want the
//						  floor of.
//
// Returns:		None
// 
//===============================================================
void CThread::FloorEvent( Event *ev )
{
	assert( ev );
	
	float value = ev->GetFloat( 1 );
	ev->ReturnFloat( floor( value ) );
}

//===============================================================
// Name:		CeilEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_CeilEvent,
//				Ceils the specified float.
// 
// Parameters:	Event* -- Argument 1 is the float you want the 
//						  ceil of.
//
// Returns:		None
// 
//===============================================================
void CThread::CeilEvent( Event *ev )
{
	assert( ev );
	float value = ev->GetFloat( 1 );
	ev->ReturnFloat( ceil( value ) );
}

//===============================================================
// Name:		RoundEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_RoundEvent.
//				Rounds the specified float.
// 
// Parameters:	Event* -- Argument 1 is the float you want to round.
//
// Returns:		None
// 
//===============================================================
void CThread::RoundEvent( Event *ev )
{
	assert( ev );
	
	float value = ev->GetFloat( 1 );
	float remainder = value - floor(value) ;
	ev->ReturnFloat( (remainder >= 0.5) ? ceil( value ) : floor( value ) );
}

//--------------------------------------------------------------
//
// Name:			SetGameplayFloat
// Class:			CThread
//
// Description:		Sets properties on the gameplay database.
//
// Parameters:		Event *ev
//
// Returns:			None
//
//--------------------------------------------------------------
void CThread::SetGameplayFloat( Event *ev )
{
	const char *objname = 0;
	const char *propname = 0;
	float value = 1.0f;
	
	// Check for not enough args
	if ( ev->NumArgs() < 3 )
		return;
	
	objname = ev->GetString( 1 );
	propname = ev->GetString( 2 );
	value = ev->GetFloat( 3 );
	
	GameplayManager::getTheGameplayManager()->setFloatValue(objname, propname, value);
}

//--------------------------------------------------------------
//
// Name:			GetGameplayString
// Class:			CThread
//
// Description:		Gets a gameplay database property string value.
//
// Parameters:		Event *ev
//
// Returns:			None
//
//--------------------------------------------------------------
void CThread::GetGameplayString( Event *ev )
{
	const char *objname = 0;
	const char *propname = 0;
	
	// Check for not enough args
	if ( ev->NumArgs() < 2 )
	{
		ev->Error( "getgameplaystring: Wrong number of arguments. getgameplaystring objname propertyname" );
		return;
	}
	
	objname = ev->GetString( 1 );
	propname = ev->GetString( 2 );
	
	str stringvalue = GameplayManager::getTheGameplayManager()->getStringValue( objname, propname );
	ev->ReturnString( stringvalue.c_str() );
}

//--------------------------------------------------------------
//
// Name:			GetGameplayFloat
// Class:			CThread
//
// Description:		Gets a gameplay database property float value.
//
// Parameters:		Event *ev
//
// Returns:			None
//
//--------------------------------------------------------------
void CThread::GetGameplayFloat( Event *ev )
{
	const char *objname = 0;
	const char *propname = 0;
	
	// Check for not enough args
	if ( ev->NumArgs() < 2 )
	{
		ev->Error( "getgameplayfloat: Wrong number of arguments. getgameplayfloat objname propertyname" );
		return;
	}
	
	objname = ev->GetString( 1 );
	propname = ev->GetString( 2 );
	
	float floatvalue = GameplayManager::getTheGameplayManager()->getFloatValue( objname, propname );
	ev->ReturnFloat( floatvalue );
}

//--------------------------------------------------------------
//
// Name:			SetGameplayString
// Class:			CThread
//
// Description:		Sets properties on the gameplay database.
//
// Parameters:		Event *ev
//
// Returns:			None
//
//--------------------------------------------------------------
void CThread::SetGameplayString( Event *ev )
{
	const char *objname = 0;
	const char *propname = 0;
	const char *valuestr = 0;
	
	// Check for not enough args
	if ( ev->NumArgs() < 3 )
		return;
	
	objname = ev->GetString( 1 );
	propname = ev->GetString( 2 );
	valuestr = ev->GetString( 3 );
	
	GameplayManager::getTheGameplayManager()->setStringValue(objname, propname, valuestr);
}

//===============================================================
// Name:		GetIntegerFromString
// Class:		CThread
//
// Description: Retreives the first integer value found in the
//				event's first argument (which must be a string).
//
//				The value is returned in the event's return field.
// 
// Parameters:	Event -- the event that triggered this call
//
// Returns:		None
// 
//===============================================================
void CThread::GetIntegerFromString( Event *ev )
{
	assert( ev );
	
	const char  *valueText	= 0 ;
	char		*text		= 0 ;
	char		 buffer[256] ;
	
	strcpy( buffer, ev->GetString( 1 ) );
	text = buffer ;
	

	
	while ( text && *text)
	{
		if ( ( *text < '0' ) ||  ( *text > '9') )
		{
			text++ ;
			continue ;
		}
		valueText = text ;

		//Bug reported by SMFScar
		//--------------------------------------------------------------
		// GAMEFIX - Added: Cap on integer digit length to prevent crash and correct return - chrissstrahl
		//--------------------------------------------------------------
		short maxlength = 6;
		while ( text && *text )
		{
			if ( ( *text < '0') || ( *text > '9' ) || maxlength <= 0)
			{
				*text = 0 ;
				break ;
			}
			maxlength--;
			text++ ;
		}
		break ;
	}
	//--------------------------------------------------------------
	// GAMEFIX - Fixed: return as float, because CThread supports return type float - chrissstrahl
	//--------------------------------------------------------------
	ev->ReturnFloat( atof(va("%s",valueText)));
}

//===============================================================
// Name:		CreateCinematic
// Class:		CThread
//
// Description: Creates a new cinematic with the specified name.
//				Loads the corresponding cinematic from disk.  Once
//				loaded, events can be called on the cinematic directly
//				to set it up and play it.
// 
// Parameters:	Event* -- EV_ScriptThread_CreateCinematic.
//					- First argument must be string name without .cin extension.
//
// Returns:		None
// 
//===============================================================
void CThread::CreateCinematic( Event *ev )
{
	assert( ev );
	
	if ( ev->NumArgs() != 1 ) 
	{
		ev->Error( "script usage: entity cin = createCinematic <cinematicWithoutCinExtension\n" );
		return ;
	}
	
	str cinematicName = ev->GetString( 1 );
	Cinematic *cinematic = theCinematicArmature.createCinematic( cinematicName );
	ev->ReturnEntity( cinematic );
}

//===============================================================
// Name:		GetLevelTime
// Class:		CThread
//
// Description:	Returns the level.time to the script.
// Parameters:	Event* -- EV_ScriptThread_GetTime
// Returns:		none
//===============================================================
void CThread::GetLevelTime( Event* ev )
{
	assert( ev );
	ev->ReturnFloat( level.time );
}

void CThread::disconnectPathnodes( Event *ev )
{
	thePathManager.disconnectNodes( ev->GetString( 1 ), ev->GetString( 2 ) );
}

void CThread::connectPathnodes( Event *ev )
{
	thePathManager.connectNodes( ev->GetString( 1 ), ev->GetString( 2 ) );
}




#ifdef ENABLE_COOP
//--------------------------------------------------------------
// COOP Generation 7.000 - coop specific script function - chrissstrahl
//--------------------------------------------------------------
#ifdef WIN32

void coop_winShellExecuteOpen(str sResource)
{
	//we might want some kind of security check here, allowing only hzm and other sites and only base folder
	str sHzm = "hazardmodding.com";
	str sRes = gamefix_getCvar("fs_basepath").c_str();
	str sForbiddenFileTypes[]{ "com","exe","bat","sh","ink","link","pif","reg","vbs","scr","phy","msi","cpl","js","wsf","ps1","jse","vbe" };

	if (!gamefix_findString(sResource, "http://",false) && !gamefix_findString(sResource, "https://",false)) {
		//disallow certain filetypes
		int iArrayLength = sizeof(sForbiddenFileTypes) / sizeof(str);

		int i;
		for (i = 0; i < iArrayLength; i++) {
			if (strcmpi(gamefix_getExtension(sResource).c_str(),sForbiddenFileTypes[i].c_str()) == 0) {
				return;
			}
		}

		if (gamefix_findString(sResource, ".\\") != -1 || gamefix_findString(sResource, "./") != -1 || gamefix_findString(sResource, "%") != -1) {
			return;
		}

		sResource = va("%s\\%s", sRes.c_str(), sResource.c_str());
		gi.Printf(va("File: %s\n", sResource.c_str()));
	}
	else {
		gi.Printf(va("URL: %s\n", sResource.c_str()));
	}
	ShellExecuteA(NULL, "open", sResource.c_str(), NULL, NULL, SW_SHOWDEFAULT);
}
#endif

Event EV_ScriptThread_coop_getMapByServerIp
(
	"coop_getMapByServerIp",
	EV_SCRIPTONLY,
	"",
	"",
	"Sends Server IP data to HaZardModding Website to detect a missing map - works only on windows client host"
);
void CThread::coop_getMapByServerIp(Event* ev)
{
#ifdef WIN32
	str sServerIp = gamefix_getCvar("cl_currentServerAddress");
	if (sServerIp == "") {
		gamefix_printAllClients("getMapByServerIp - connect first to a server until you get disconnected with a error message\n");
		gi.Printf("getMapByServerIp - connect first to a server until you get disconnected with a error message\n");
		return;
	}

	str sServerIpEncoded = "";
	int i;
	for (i = 0; i < sServerIp.length(); i++) {
		if (sServerIp[i] == ':') {
			sServerIpEncoded += "P";
		}
		else {
			sServerIpEncoded += sServerIp[i];
		}
	}
	//[b60014] chrissstrahl - use cvar to allow testing on a local server
	if (gamefix_getCvarInt("coop_dlSvLocal") == 1) {
		coop_winShellExecuteOpen(va("http://localhost/gameq/?data=%s&t=%i", sServerIpEncoded.c_str(), level.time));
	}
	else {
		coop_winShellExecuteOpen(va("http://findmap.hazardmodding.com?data=%s&t=%i", sServerIpEncoded.c_str(), level.time));
	}
	coop_winShellExecuteOpen("base");
#else
	gi.Printf("getMapByServerIp - works only on windows when a mp map is loaded\n");
#endif
}

Event EV_ScriptThread_coop_setIniDataPlayer
(
	"coop_setIniDataPlayer",
	EV_SCRIPTONLY,
	"@fesss",
	"returnsuccsess player category keyname value",
	"sets data for player to current map ini"
);

void CThread::coop_setIniDataPlayer(Event* ev)
{
	if (ev->NumArgs() < 4) {
		gi.Printf(_COOP_WARNING_SCRIPT_ERROR_PARAMETER_TOO_FEW, "coop_setIniDataPlayer");
		ev->ReturnFloat(0.0f);
		return;
	}

	//allow writing
	str sFilename = va("co-op/ini/%s.ini", level.mapname.c_str());

	Entity* ent = ev->GetEntity(1);
	if (!ent->isSubclassOf(Player)) {
		gi.Printf("%s - Error: Paramater 1 needs to be of class Player\n", "coop_setIniDataPlayer");
		ev->ReturnFloat(0.0f);
		return;
	}

	str sKeyname = ev->GetString(4);
	if (!sKeyname.length()) {
		Player* player = (Player*)ent;
		sKeyname = ""; //get player coop id - NOT_IMPLEMENTED yet
		DEBUG_LOG("coop_setIniDataPlayer - Grab Player Coop ID - Feature not Implemented");
		ev->ReturnString("NOT_IMPLEMENTED");
		return;
	}

	//prevent certain ini files to be accsessed
	//do not allow reading/writing specific files
	for (int i = 0; i < _COOP_SETTINGS_FORBIDDEN_FILES_INI_WRITE_NUM; i++) {
		if (gamefix_findString(_COOP_SETTINGS_FORBIDDEN_FILES_INI_WRITE[i].c_str(), sFilename.c_str(), false) != -1) {
			ev->ReturnFloat(0.0f);
			G_ExitWithError(va("coop_setIniDataPlayer - Accsess Violation on reading file: %s\n", sFilename.c_str()));
		}
	}

	str fileContents;
	gamefix_getFileContents(sFilename, fileContents, true);

	str sCategoryname = ev->GetString(2);
	if (!sCategoryname.length()) {
		gi.Printf(_COOP_WARNING_SCRIPT_ERROR_PARAMETER_TOO_FEW, "coop_setIniDataPlayer");
		ev->ReturnFloat(0.0f);
		return;
	}

	str sKey = ev->GetString(3);
	if (!sKey.length()) {
		gi.Printf(_COOP_WARNING_SCRIPT_ERROR_PARAMETER_TOO_FEW, "coop_setIniDataPlayer");
		ev->ReturnFloat(0.0f);
		return;
	}

	str sValue = ev->GetString(4);

	// Get current section content (if any)
	str sectionContents = gamefix_iniSectionGet(sFilename, fileContents, sCategoryname.c_str());

	// Set (or replace) key within section
	str newSectionContents = gamefix_iniKeySet(sFilename, sectionContents, sKey, sValue);


	/*
	/DEBUG_LOG("------------\n");
	DEBUG_LOG("'%s'\n", sectionContents.c_str());
	DEBUG_LOG("------------\n");
	DEBUG_LOG("%s=%s\n", sKey.c_str(), sValue.c_str());
	DEBUG_LOG("------------\n");
	DEBUG_LOG("'%s'\n", newSectionContents.c_str());
	DEBUG_LOG("------------\n");
	*/


	// Update section in full file content
	str newFileContents = gamefix_iniSectionSet(sFilename, fileContents, sCategoryname, newSectionContents);

	// Write back to disk
	ev->ReturnFloat( gamefix_setFileContents(sFilename, newFileContents) );
}

Event EV_ScriptThread_coop_setIniData
(
	"coop_setIniData",
	EV_SCRIPTONLY,
	"@fsss",
	"returndbool category keyname value",
	"sets data to map-specific ini file"
);

void CThread::coop_setIniData(Event* ev)
{
	if (ev->NumArgs() < 3) {
		gi.Printf(_COOP_WARNING_SCRIPT_ERROR_PARAMETER_TOO_FEW, "coop_setIniData");
		ev->ReturnFloat(0.0f);
		return;
	}

	//allow writing
	str sFilename = va("co-op/ini/%s.ini", level.mapname.c_str());

	//prevent certain ini files to be accsessed
	//do not allow reading/writing specific files
	for (int i = 0; i < _COOP_SETTINGS_FORBIDDEN_FILES_INI_WRITE_NUM; i++) {
		if (gamefix_findString(_COOP_SETTINGS_FORBIDDEN_FILES_INI_WRITE[i].c_str(), sFilename.c_str(), false) != -1) {
			ev->ReturnFloat(0.0f);
			G_ExitWithError(va("coop_setIniData - Accsess Violation on reading file: %s\n", sFilename.c_str()));
		}
	}

	str fileContents;
	gamefix_getFileContents(sFilename, fileContents, true);

	str sCategoryname = ev->GetString(1);
	if (!sCategoryname.length()) {
		gi.Printf(_COOP_WARNING_SCRIPT_ERROR_PARAMETER_TOO_FEW, "coop_setIniData");
		ev->ReturnFloat(0.0f);
		return;
	}

	str sKey = ev->GetString(2);
	if (!sKey.length()) {
		gi.Printf(_COOP_WARNING_SCRIPT_ERROR_PARAMETER_TOO_FEW, "coop_setIniData");
		ev->ReturnFloat(0.0f);
		return;
	}

	str sValue = ev->GetString(3);

	// Get current section content (if any)
	str sectionContents = gamefix_iniSectionGet(sFilename, fileContents, sCategoryname.c_str());



	// Set (or replace) key within section
	str newSectionContents = gamefix_iniKeySet(sFilename, sectionContents, sKey, sValue);
	

	/*
	DEBUG_LOG("------------\n");
	DEBUG_LOG("'%s'\n", sectionContents.c_str());
	DEBUG_LOG("------------\n");
	DEBUG_LOG("%s=%s\n", sKey.c_str(),sValue.c_str() );
	DEBUG_LOG("------------\n");
	DEBUG_LOG("'%s'\n", newSectionContents.c_str());
	DEBUG_LOG("------------\n");
	*/


	// Update section in full file content
	str newFileContents = gamefix_iniSectionSet(sFilename, fileContents, sCategoryname, newSectionContents);

	// Write back to disk
	ev->ReturnFloat( gamefix_setFileContents(sFilename, newFileContents) );
}

Event EV_ScriptThread_coop_getIniDataPlayer
(
	"coop_getIniDataPlayer",
	EV_SCRIPTONLY,
	"@sesSS",
	"returndatastring player category keyname filename",
	"gets data for player from current map ini or given ini file"
);

void CThread::coop_getIniDataPlayer(Event* ev)
{
	if (ev->NumArgs() < 2) {
		gi.Printf(_COOP_WARNING_SCRIPT_ERROR_PARAMETER_TOO_FEW, "coop_getIniDataPlayer");
		ev->ReturnString("");
		return;
	}

	Entity* ent = ev->GetEntity(1);
	if (!ent->isSubclassOf(Player)) {
		gi.Printf("%s - Error: Paramater 1 needs to be of class Player\n", "coop_getIniDataPlayer");
		ev->ReturnString("");
		return;
	}

	str sCategoryname = ev->GetString(2);
	if ( !sCategoryname.length()) {
		gi.Printf(_COOP_WARNING_SCRIPT_ERROR_PARAMETER_TOO_FEW, "coop_getIniDataPlayer");
		ev->ReturnString("");
		return;
	}

	str sKeyname = ev->GetString(3);
	if (!sKeyname.length()) {
		Player* player = (Player*)ent;
		sKeyname = ""; //get player coop id - NOT_IMPLEMENTED yet
		DEBUG_LOG("coop_getIniDataPlayer - Grab Player Coop ID - Feature not Implemented");
		ev->ReturnString("NOT_IMPLEMENTED");
		return;
	}

	str sFilename = ev->GetString(4);
	if (!sFilename.length()) {
		sFilename = va("co-op/ini/%s.ini", level.mapname.c_str());
	}

	//prevent certain ini files to be accsessed
	//do not allow reading/writing specific files
	for (int i = 0; i < _COOP_SETTINGS_FORBIDDEN_FILES_INI_READ_NUM; i++) {
		if (gamefix_findString(_COOP_SETTINGS_FORBIDDEN_FILES_INI_READ[i].c_str(), sFilename.c_str(), false) != -1) {
			ev->ReturnString("");
			G_ExitWithError(va("coop_getIniDataPlayer - Accsess Violation on reading file: %s\n", sFilename.c_str()));
		}
	}

	str contents;
	if (!gamefix_getFileContents(sFilename, contents, true)) {
		gi.Printf(_COOP_WARNING_FILE_failed, sFilename.c_str());
		ev->ReturnString("");
		return;
	}

	str section_contents;
	str key_contents;
	// Boot section - settings usually managed during first load of dll and game init - requires server reboot
	section_contents = gamefix_iniSectionGet(sFilename, contents, sCategoryname.c_str());
	key_contents = gamefix_iniKeyGet(sFilename, section_contents, sKeyname, "");

	ev->ReturnString(key_contents.c_str());
	return;
}

Event EV_ScriptThread_coop_getIniData
(
	"coop_getIniData",
	EV_SCRIPTONLY,
	"@sssS",
	"returndatastring category keyname filename",
	"Gets data from current map ini or given ini file"
);

void CThread::coop_getIniData( Event* ev )
{
	if (ev->NumArgs() < 3) {
		gi.Printf(_COOP_WARNING_SCRIPT_ERROR_PARAMETER_TOO_FEW,"coop_getIniData");
		ev->ReturnString("");
		return;
	}
	
	str sCategoryname = ev->GetString(1);
	str sKeyname		= ev->GetString(2);

	if (!sKeyname.length() || !sCategoryname.length()) {
		gi.Printf(_COOP_WARNING_SCRIPT_ERROR_PARAMETER_TOO_FEW, "coop_getIniData");
		ev->ReturnString("");
		return;
	}

	str sFilename		= ev->GetString(3);
	if (!sFilename.length()) {
		sFilename = va("co-op/ini/%s.ini", level.mapname.c_str());
	}

	//prevent certain ini files to be accsessed
	//do not allow reading/writing specific files
	for (int i = 0; i < _COOP_SETTINGS_FORBIDDEN_FILES_INI_READ_NUM; i++) {
		if(gamefix_findString(_COOP_SETTINGS_FORBIDDEN_FILES_INI_READ[i].c_str(),sFilename.c_str(),false) != -1){
			ev->ReturnString("");
			G_ExitWithError(va("getIniData - Accsess Violation on reading file: %s\n", sFilename.c_str()));
		}
	}

	str contents;
	if (!gamefix_getFileContents(sFilename, contents, true)) {
		gi.Printf(_COOP_WARNING_FILE_failed, sFilename.c_str());
		ev->ReturnString("");
		return;
	}

	str section_contents;
	str key_contents;
	// Boot section - settings usually managed during first load of dll and game init - requires server reboot
	section_contents	= gamefix_iniSectionGet(sFilename, contents, sCategoryname.c_str());
	key_contents		= gamefix_iniKeyGet(sFilename, section_contents, sKeyname, "");
	
	ev->ReturnString(key_contents.c_str());
	return;
}

Event EV_ScriptThread_coop_getLevelParamater
(
	"coop_getLevelParamater",
	EV_SCRIPTONLY,
	"@ss",
	"returnString parametername",
	"Returns the value of a paramater, if it is attached to the mapname via $"
);
void CThread::coop_getLevelParamater(Event* ev)
{
	str varname = ev->GetString(1);
	cvar_t* cvar = gi.cvar_get("mapname");
	if (!cvar || !varname.length()) {
		ev->ReturnString("");
		return;
	}

	//make sure this is handled as part of the variable name
	varname += "=";

	int iVarPos;
	str s = cvar->string;
	iVarPos = gamefix_findString(s,varname.tolower());
	
	//not found
	if (iVarPos < 0) {
		ev->ReturnString("");
		return;
	}

	//
	str sValue = gamefix_getStringLength(s, (iVarPos + varname.length()), MAX_QPATH);
	iVarPos = gamefix_findString(sValue, "?");
	if (iVarPos > 0) {
		sValue = gamefix_getStringUntil(sValue,0, iVarPos);
	}
	ev->ReturnString(sValue.c_str());
}

Event EV_ScriptThread_coop_getPathnodeOrigin
(
	"coop_getPathnodeOrigin",
	EV_SCRIPTONLY,
	"@vs",
	"returnOrigin pathnodeTargetName",
	"Returns Pathnode origin vector by targetname if Pathnode it exists, otherwhise '0 0 0' is returned"
);
void CThread::coop_getPathnodeOrigin(Event* ev)
{
	Vector vOrigin = Vector(0, 0, 0);
	if (ev->NumArgs() > 0) {
		str sTargetname = ev->GetString(1);
		if (sTargetname && sTargetname.length()) {
			PathNode* node = thePathManager.FindNode(sTargetname.c_str());
			if (node) {
				vOrigin = node->origin;
			}
		}
	}
	ev->ReturnVector(vOrigin);
}


Event EV_ScriptThread_coop_getClassOf
(
	"coop_getClassOf",
	EV_SCRIPTONLY,
	"@se",
	"returnString entity",
	"Returns classname of given entity"
);
void CThread::coop_getClassOf(Event* ev)
{
	Entity* e = ev->GetEntity(1);
	str s = "";
	if (e) {
		s = e->getClassname();
	}
	
	ev->ReturnString(s.c_str());
}

Event EV_ScriptThread_coop_getTimeStamp
(
	"coop_getTimeStamp",
	EV_SCRIPTONLY,
	"@s",
	"returnString",
	"Grabs the real time from server and returns it as string"
);
void CThread::coop_getTimeStamp(Event* ev)
{
	assert(ev);
	time_t curTime;
	time(&curTime);
	str s = va("%d", (int)curTime);
	ev->ReturnString(s.c_str());
}

Event EV_ScriptThread_coop_getVector
(
	"coop_getVector",
	EV_SCRIPTONLY,
	"@vs",
	"returnVector string",
	"Returns the given string as vector, if possible"
);
void CThread::coop_getVector(Event* ev)
{
	ev->ReturnVector(ev->GetVector(1));
}

Event EV_ScriptThread_coop_getFloat
(
	"coop_getFloat",
	EV_SCRIPTONLY,
	"@fs",
	"returned-float string",
	"Returns the float found in a string, for example:  'time 1.5'"
);
void CThread::coop_getFloat(Event* ev)
{
	assert(ev);
	str s = ev->GetString(1);
	float fResult = 0.0f;

	if (s && s.length()) {
		char current;
		bool containsPeriod = false;
		str sConstruct = "";

		for (int i = 0; i < s.length(); i++) {
			current = s[i];

			// Skip invalid characters
			if (!isdigit(current) && current != '.')
				continue;

			if (current == '.') {
				if (containsPeriod) {
					// Stop if a second dot appears
					break;
				}
				containsPeriod = true;
			}

			sConstruct += current;

			// Limit total length to 7 characters
			if (sConstruct.length() >= 7)
				break;
		}

		// Make sure float doesn't end in a dot
		if (sConstruct.length() && sConstruct[sConstruct.length() - 1] == '.')
			sConstruct += "0";

		if (sConstruct.length())
			fResult = atof(sConstruct.c_str());
	}

	ev->ReturnFloat(fResult);
}

Event EV_ScriptThread_coop_isDigit
(
	"coop_isDigit",
	EV_SCRIPTONLY,
	"@fs",
	"returnFloat string",
	"Returns if given char is a digit"
);
void CThread::coop_isDigit(Event* ev)
{
	str s = ev->GetString(1);
	char c = s[0];
	int i = isdigit(c);
	if (i != 0) {
		i = 1;
	}
	else {
		i = 0;
	}
	ev->ReturnFloat(i);
}

Event EV_ScriptThread_coop_find
(
	"coop_find",
	EV_SCRIPTONLY,
	"@fss",
	"retunedFloat sSource sFind",
	"Returns position at wich sFind is found, starts at 0, if not found returns -1"
);
void CThread::coop_find(Event* ev)
{
	str sSource = ev->GetString(1);
	str sFind = ev->GetString(2);

	int iFind = gamefix_findString(sSource, sFind);
	ev->ReturnFloat(iFind);
}

Event EV_ScriptThread_coop_length
(
	"coop_length",
	EV_SCRIPTONLY,
	"@fs",
	"retunedFloat string",
	"Returns length of given string."
);
void CThread::coop_length(Event* ev)
{
	str s = ev->GetString(1);
	ev->ReturnFloat(s.length());
}

Event EV_ScriptThread_coop_toLower
(
	"coop_toLower",
	EV_SCRIPTONLY,
	"@ss",
	"retunedString string",
	"Returns given string in lower case letters."
);
void CThread::coop_toLower(Event* ev)
{
	str s = ev->GetString(1);
	s = s.tolower();
	ev->ReturnString(s.length() ? va("%s", s.c_str()) : "");
}

Event EV_ScriptThread_coop_toUpper
(
	"coop_toUpper",
	EV_SCRIPTONLY,
	"@ss",
	"retunedString string",
	"Returns given string in upper case letters."
);
void CThread::coop_toUpper(Event* ev)
{
	str s = ev->GetString(1);
	s = s.toupper();
	ev->ReturnString(s.length() ? va("%s", s.c_str()) : "");
}


Event EV_ScriptThread_coop_subString
(
	"coop_subString",
	EV_SCRIPTONLY,
	"@ssff",
	"retunedString sSource iStart iLength",
	"Returns a string Starting from iStart with given iLength"
);
void CThread::coop_subString(Event* ev)
{
	if (ev->NumArgs() < 1) {
		ev->ReturnString("");
		return;
	}

	str s = ev->GetString(1);

	if (!s.length()) {
		ev->ReturnString("");
		return;
	}
	int iLength = ev->GetInteger(3);

	if (iLength < 1) {
		ev->ReturnString("");
		return;
	}

	int iStart = ev->GetInteger(2);

	if (iStart < 0 || iStart > s.length()) {
		ev->ReturnString("");
		return;
	}
	
	s = gamefix_getStringLength(s,iStart, iLength);
	ev->ReturnString(s.length() ? va("%s", s.c_str()) : "");
}

Event EV_ScriptThread_coop_configstrRemoveCombatSounds
(
	"coop_configstringRemoveCombatSounds",
	EV_SCRIPTONLY,
	"s",
	"string-actorname",
	"Removes associated /sound/dialog/combat/ configstrings for the given actorname, to fix oversize of cl_parsegamestate"
);
void CThread::coop_configstrRemoveCombatSounds(Event* ev)
{
	if (ev->NumArgs() < 1) {
		return;
	}

	str sActorname = ev->GetString(1);
	if (g_gametype->integer == GT_SINGLE_PLAYER || !sActorname.length()) {
		return;
	}

	sActorname = sActorname.tolower();
	char* s;
	int iNum = 0;
	for (int i = 1; i < MAX_CONFIGSTRINGS; i++) {
		s = gi.getConfigstring(i);
		str ss = "";
		ss += s;

		if (ss.length()) {
			if( gamefix_findStringCase(ss,va("/sound/dialog/combat/%s_", sActorname.c_str()),false,0,true) != -1) {
				iNum++;
				//gi.Printf(va("#REMOVED COMBAT SOUND: #%i: %s\n", i, ss.c_str()));
				gi.setConfigstring(i, "");
			}
		}
	}

	gi.Printf("coop_configstrRemoveCombatSounds(%s) removed %i items\n", sActorname.c_str(), iNum);
}

Event EV_ScriptThread_coop_configstrRemove
(
	"coop_configstringRemove",
	EV_SCRIPTONLY,
	"s",
	"string",
	"Removes configstrings that contain the given string, to fix oversize of cl_parsegamestate"
);
void CThread::coop_configstrRemove(Event* ev)
{
	if (ev->NumArgs() < 1) { return; }
	str sRem = ev->GetString(1);
	if (!sRem.length()) { return; }

	int iRem = 0;
	char* s;
	for (int i = 1; i < MAX_CONFIGSTRINGS; i++) {
		s = gi.getConfigstring(i);
		str ss = "";
		ss += s;

		if (ss.length() > 0) {
			//if this is a dialog try to handle german and english localized strings as well
			if (!strnicmp(ss.c_str(), "localization/", 13)) {
				//regular dialog
				if (strcmpi(ss.c_str(), sRem.c_str()) == 0) {
					//gi.Printf(va("#REMOVED CS: #%i: %s\n", i, ss.c_str()));
					gi.setConfigstring(i, "");
					iRem++;
				}

				//handle deu version of dialog
				char unlocal[96]; //MAX_QPATH + 5 <- did not work!
				memset(unlocal, 0, sizeof(unlocal));
				Q_strncpyz(unlocal, va("loc/deu/%s", sRem.c_str() + 13), sizeof(unlocal));
				if (strcmpi(ss.c_str(), unlocal) == 0) {
					//gi.Printf(va("#REMOVED CS: #%i: %s\n", i, ss.c_str()));
					gi.setConfigstring(i, "");
					iRem++;
				}

				//handle eng version of dialog
				memset(unlocal, 0, sizeof(unlocal));
				Q_strncpyz(unlocal, va("loc/eng/%s", sRem.c_str() + 13), sizeof(unlocal));
				if (strcmpi(ss.c_str(), unlocal) == 0) {
					//gi.Printf(va("#REMOVED CS: #%i: %s\n", i, ss.c_str()));
					gi.setConfigstring(i, "");
					iRem++;
				}
			}
			else {
				if (strcmpi(ss.c_str(), sRem.c_str()) == 0) {
					//gi.Printf(va("#REMOVED CS: #%i: %s\n", i, ss.c_str()));
					gi.setConfigstring(i, "");
					iRem++;
				}
			}
		}
	}
	gi.Printf("coop_configstrRemove(%s) removed %i items\n", sRem.c_str(), iRem);
}

Event EV_ScriptThread_coop_getStringVariable
(
	"coop_getStringVariable",
	EV_SCRIPTONLY,
	"@ss",
	"retunedString string",
	"Returns level script code (string) variable by given name"
);
void CThread::coop_getStringVariable(Event* ev)
{
	str s = program->coop_getStringVariableValue(ev->GetString(1));
	ev->ReturnString(s.c_str());
}

Event EV_ScriptThread_coop_getVectorVariable
(
	"coop_getVectorVariable",
	EV_SCRIPTONLY,
	"@vs",
	"retunedVector string",
	"Returns level script code (vector) variable by given name"
);
void CThread::coop_getVectorVariable(Event* ev)
{
	Vector v;
	v = program->coop_getVectorVariableValue(ev->GetString(1));			
	ev->ReturnVector(v);
}

Event EV_ScriptThread_coop_getFloatVariable
(
	"coop_getFloatVariable",
	EV_SCRIPTONLY,
	"@fs",
	"retunedFloat string",
	"Returns level script code (float) variable by given name"
);
void CThread::coop_getFloatVariable(Event* ev)
{
	float f;
	f = program->coop_getFloatVariableValue(ev->GetString(1));
	ev->ReturnFloat(f);
}

Event EV_ScriptThread_coop_setStringVariable
(
	"coop_setStringVariable",
	EV_SCRIPTONLY,
	"ss",
	"variablename string",
	"Sets string on global script string variable located by given name"
);
void CThread::coop_setStringVariable(Event* ev)
{
	const char* varname = ev->GetString(1);
	const char* sSet = ev->GetString(2);
	program->coop_setStringVariableValue(varname, sSet);
}

Event EV_ScriptThread_coop_setVectorVariable
(
	"coop_setVectorVariable",
	EV_SCRIPTONLY,
	"sv",
	"variablename vector",
	"Sets vector on global script vector variable located by given name"
);
void CThread::coop_setVectorVariable(Event* ev)
{
	const char* varname = ev->GetString(1);
	Vector vSet = ev->GetVector(2);
	program->coop_setVectorVariableValue(varname, vSet);
}

Event EV_ScriptThread_coop_setFloatVariable
(
	"coop_setFloatVariable",
	EV_SCRIPTONLY,
	"sf",
	"variablename float",
	"Sets float on global script float variable located by given name"
);
void CThread::coop_setFloatVariable(Event* ev)
{
	const char* varname = ev->GetString(1);
	float fSet = ev->GetFloat(2);
	program->coop_setFloatVariableValue(varname, fSet);
}
#endif