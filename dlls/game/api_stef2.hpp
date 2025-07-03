//--------------------------------------------------------------
// GAMEFIX - ELITE FORCE 2 - API
// 
// Elite Force 2 Specific Functions and classes are made available
// here in a fashion that the regular gamefix code can be ported
// to fakk2 and others, with very little effort
//--------------------------------------------------------------


#pragma once

constexpr auto gamefix_defaultMapsSize = 88;
constexpr auto GAMEFIX_API_CHAT_MAX = 3;
constexpr auto GAMEFIX_API_CHAT_CYCLE = 3.0f;
constexpr auto GAMEFIX_API_COMMANDS_MAX = 3;
constexpr auto GAMEFIX_API_COMMANDS_CYCLE = 1.0f;

#define GAME_STAR_TREK_ELITE_FORCE_2
//#define GAME_HEAVY_METAL_FAKK_2
//#define GAME_MEDAL_OF_HONOR_ALLIED_ASSAULT


#include "_pch_cpp.h"
#include "api_stef2_strings.hpp"
#include "gamefix_strings.hpp"
#include "gamefix.hpp"

//--------------------------------------------------------------
// GAMEFIX - Added: Information list for all standard levels - chrissstrahl
//--------------------------------------------------------------
extern Container<str> gameFixAPI_maplistContainer;

bool gameFixAPI_inSingleplayer();
bool gameFixAPI_inMultiplayer();
bool gameFixAPI_isSpectator_stef2(Entity* ent);
bool gameFixAPI_isDead(Entity* ent);
bool gameFixAPI_isDedicatedServer();
bool gameFixAPI_isListenServer();
bool gameFixAPI_isWindowsServer();
bool gameFixAPI_isLinuxServer();
bool gameFixAPI_isHost(Player* player);
void gameFixAPI_hudPrint(Player* player, str sText);
int gameFixAPI_maxClients();
bool gameFixAPI_isBot(gentity_t* ent);
bool gameFixAPI_isBot(Player* player);

void gameFixAPI_playerEntered(Player* player);
void gameFixAPI_playerSpawn(Player* player);
void gameFixAPI_playerUseItem(Player* player, const char* name);
void gameFixAPI_playerKilled(Player* player);
void gameFixAPI_playerChangeTeam(Player* player,const str &realTeamName);
void gameFixAPI_playerSpectator(Player* player);
void gameFixAPI_playerModelChanged(Player* player);
void gameFixAPI_playerScore(Player* player);
void gameFixAPI_playerSetupUi(Player* player);
void gameFixAPI_playerClientThink(Player* player);

void gameFixAPI_clearArchetypeInfoDisplay(Player* player, Entity* entity);
Entity* gameFixAPI_getTargetedEntity(Player* player);
Player* gameFixAPI_getClosestPlayerInCallvolume(Entity* entity);

void gameFixAPI_initPersistant(int clientNum, bool isBot);
bool gamefixAPI_commandsUpdate(int clientNum, const str &cmd);
bool gamefixAPI_chatUpdate(int clientNum, const str &text);

qboolean gameFixAPI_languageEng(const gentity_t* ent);
qboolean gameFixAPI_languageDeu(const gentity_t* ent);
str gameFixAPI_getServerLanguage();
str gameFixAPI_getLanguage(Player* player);
Entity* gameFixAPI_getActorFollowTargetEntity(Actor* actor);
bool gameFixAPI_actorCanSee(Actor* actor, Entity* entity, bool useFOV, bool useVisionDistance);
Entity* gameFixAPI_actorGetCurrentEnemy(Actor* actor);
bool gameFixAPI_actorHates(Actor* actor,Sentient *sentient);
bool gameFixAPI_checkPlayerUsingWeaponNamed(Player* player, const str& weaponNameOfPlayer);
int gameFixAPI_getPlayers(bool state);
void gameFixAPI_hudPrintAllClients(const str text);
Entity* gameFixAPI_getEntity(str& name);
void gameFixAPI_levelFixes();
void gameFixAPI_levelfix_m11l3a_drull_ruins3_boss();
void gameFixAPI_levelfix_swsglobe();
void gameFixAPI_maxLevelitems_ctf_grey();
void gameFixAPI_spawnlocations_dm_ctf_voy1();
void gameFixAPI_setLastInflictor(Entity* entity, Entity* eInflictor);
Entity* gameFixAPI_getLastInflictor(Entity* entity);
float gameFixAPI_getActivationTime(Entity* entity);
Player* gameFixAPI_getActivator(Entity* puzzle);
void gameFixAPI_setActivator(Entity* entity, Entity* activator);
str gameFixAPI_mapListGetAtPos(int currentPos);
int gameFixAPI_mapListCount();
void gameFixAPI_mapList();
int gameFixAPI_mapListGetCurrentPos();
str gameFixAPI_mapListUp();
str gameFixAPI_mapListDown();
void gameFixAPI_dialogSetupPlayers(Actor* speaker, char *localizedDialogName, bool headDisplay);
void gameFixAPI_addMap(const str& name, str gametypes, const str& gamemodes);
static void gameFixAPI_addDefaultMaps();
bool gameFixAPI_mapIsStock(const str& name);
bool gameFixAPI_mapForSingleplayer(const str& name);
bool gameFixAPI_mapForMultiplayer(const str& name);
bool gameFixAPI_mapHasGameMode(const str& name,const str& gamemode);
bool gameFixAPI_callvoteMapCheckSingleplayer(Player* player, str mapname);
bool gameFixAPI_callvoteMap(Player* player, str command, str arg);
void gameFixAPI_callvoteIniGet(const str& file, const str& sectionName, str& iniContents, str& iniContentsSection, Container<str>& iniSectionNames);
void gameFixAPI_callvoteIniHudPrintSectionNames(Player* player, Container<str>& iniSectionNames);
bool gameFixAPI_callvoteIniHandle(Player* player, const str& command, const str& arg, const str& file, str& voteCommand, str& contentsSections);
void gameFixAPI_shutdownGame();
void gameFixAPI_initGame();
void gameFixAPI_cleanupGame(qboolean restart);
