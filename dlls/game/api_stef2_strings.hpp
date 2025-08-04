//--------------------------------------------------------------
// GAMEFIX - ELITE FORCE 2 - API - STRINGS
// 
// Elite Force 2 Specific STRINGS USED FOR TEXT THAT IS SPECIFIC TO ELITE FORCE 2
//--------------------------------------------------------------

#pragma once

//--- GAMEFIX ---
//-> Strings related to gamefix.cpp/gamefix.hpp are in gamefix_strings.hpp
//---------------


//--- API --- api_stef2.cpp/api_stef2.hpp SPECIFIC
constexpr auto _GFixAPI_YOUR_LANG_WAS_SET_TO_ENG = "Your Language was set to English\n";
constexpr auto _GFixAPI_YOUR_LANG_WAS_SET_TO_DEU = "Ihre Sprache wurde auf deutsch festgelegt\n";


//--- GAME CODE --- regular gamecode files SPECIFIC
constexpr auto _GFixEF2_INFO_FUNC_ClientBegin = "level_ai - ON - Server no longer empty!\n";
constexpr auto _GFixEF2_INFO_FUNC_ClientDisconnect = "level_ai - OFF - Server is now empty!\n";

constexpr auto _GFixEF2_INFO_GAMEFIX_spawnlocations_dm_ctf_voy1 = "Badly placed info_player_deathmatch - dm_ctf_voy1\n";
constexpr auto _GFixEF2_INFO_GAMEFIX_spawnlocations_HoldableTransporter = "Missing currently valid info_player_deathmatch - HoldableItemTransporter::use\n";
constexpr auto _GFixEF2_INFO_GAMEFIX_spawnlocations_TeamBaseRespawnPlayer = "Missing currently valid info_player_deathmatch - ModeTeamBase::respawnPlayer\n";
constexpr auto _GFixEF2_INFO_GAMEFIX_spawnlocations_TeamBaseAddPlayerToTeam = "Missing currently valid info_player_deathmatch - ModeTeamBase::addPlayerToTeam\n";
constexpr auto _GFixEF2_INFO_GAMEFIX_spawnlocations_DeathmatchAddPlayer = "Missing currently valid info_player_deathmatch - ModeDeathmatch::AddPlayer\n";
constexpr auto _GFixEF2_INFO_GAMEFIX_spawnlocations_DeathmatchRespawnPlayer = "Missing currently valid info_player_deathmatch - ModeDeathmatch::respawnPlayer\n";
constexpr auto _GFixEF2_INFO_GAMEFIX_maxLevelitems_ctf_grey = "Cvar max_levelitems set to 384, to many info_helpernodes - ctf_grey\n";
constexpr auto _GFixEF2_INFO_GAMEFIX_mapCheckFromMaplist = "%s: %s not found in maplist, false returned!\n";
constexpr auto _GFixEF2_INFO_GAMEFIX_couldNotBeFoundOnServer = "%s could not be found on the server\n";
constexpr auto _GFixEF2_INFO_GAMEFIX_stalledCommandsFor = "Gamefix Stalled Commands (%s) for %s\n";

constexpr auto _GFixEF2_INFO_GAMEFIX_mpMapListDoesNotExist = "mp_mapList: maps/%s.bsp does not exist on server\n";

constexpr auto _GFixEF2_MSG_FUNC_FinishMissionFailed = "^1=/\\=^3 Mission Failed ^1=/\\=\n";
constexpr auto _GFixEF2_MSG_FUNC_callvote_singleplayer_not_allowed = "^3Singleplayer^8 levels ^3not allowed^8 on the server!^8 'seta gfix_allowSpMaps 1' to allow.\n";
constexpr auto _GFixEF2_MSG_FUNC_callvote_mpMaplist = "Cvar mp_useMapList needs to be 1 and mp_mapList needs to be populated\n";
constexpr auto _GFixEF2_MSG_FUNC_callVote_specificVotes = "Server Specific: %s.\n";
constexpr auto _GFixEF2_MSG_FUNC_callVote_ini_err_cmdEmpty = ".ini error (callvote) key 'command' has no value!\n";
constexpr auto _GFixEF2_MSG_FUNC_callVote_cmd_req_arg_range = "$$InvalidVote$$ %s requires argument (%s) range: %s.\n";
constexpr auto _GFixEF2_MSG_FUNC_callVote_cmd_req_arg_length = "$$InvalidVote$$ %s requires argument (%s) max length: %s. But was empty\n";
constexpr auto _GFixEF2_MSG_FUNC_callVote_cmd_req_cvar_be = "Vote %s requires Cvar^3 %s ^8to be^3 %s^8.\n";
constexpr auto _GFixEF2_MSG_FUNC_callVote_arg_invalid = "$$InvalidVote$$ Vote Argument for command: %s!\n";
constexpr auto _GFixEF2_MSG_FUNC_callVote_arg_valid = "-> Valid: %s.\n";
constexpr auto _GFixEF2_MSG_FUNC_callVote_willForceReload = "^6WARNING:^3Vote %s ^3will force a level reload!\n";
constexpr auto _GFixEF2_MSG_FUNC_callVote_changeTakeEffect = "^3Vote changes will take effect after level change.\n";
constexpr auto _GFixEF2_MSG_FUNC_callVote_exceeded_length = "$$InvalidVote$$ exceeded given length limit of: %d\n";
constexpr auto _GFixEF2_MSG_FUNC_callVote_cvarName_exceeds = "Cvar-Name for command %s in callvote.ini exceeded length of: 32\n";

constexpr auto _GFixEF2_WARN_EVENT_ACTOR_attack_FAILED = "Actor::AttackEntity - $%s.attack($entity) failed, entity does not exist\n";
constexpr auto _GFixEF2_WARN_EVENT_CTHREAD_triggerEntity_FAILED = "triggerEntity($...) - given entity does not exist\n";
constexpr auto _GFixEF2_WARN_EVENT_CTHREAD_fakePlayer_ONLYSP = "WARNING: fakeplayer script command can only be used in Singleplayer\n";
constexpr auto _GFixEF2_WARN_EVENT_CTHREAD_sendClientCommand_FAILED = "SendClientCommand: Given Entity does not exist!\n";
constexpr auto _GFixEF2_WARN_EVENT_CTHREAD_getNumFreeRelSVCmds_FAILED = "GetNumFreeReliableServerCommands: Given Entity does not exist!\n";
constexpr auto _GFixEF2_WARN_CALLVOTE_map_NOTSTOCK = "^3WARNING^8: ^3Some players might not have map:^5 %s ^3-> They will get dropped!\n";

constexpr auto _GFixEF2_ERR_FUNC_SensoryInFov_act_NULL = "SensoryPerception::InFOV -- actor is NULL\n";
constexpr auto _GFixEF2_ERR_LEVEL_InfoPlayerDeathmatch_AT_ZERO = "info_player_deathmatch ($%s) is at (0 0 0)\n";

constexpr auto _GFixEF2_ERR_gameFixAPI_initPersistantClNum = "gameFixAPI_initPersistant - clientnum out of range, was %d\n";
constexpr auto _GFixEF2_ERR_gamefixAPI_commandsUpdateClNum = "gamefixAPI_commandsUpdate - clientnum out of range, was %d\n";
constexpr auto _GFixEF2_ERR_gamefixAPI_chatUpdateClNum = "gamefixAPI_chatUpdate - clientnum out of range, was %d\n";