//--------------------------------------------------------------
// GAMEFIX - GENERAL CODE USED FOR GAME-FIXES
// 
// Elite Force 2 Specific Functions and classes go to: api_stef2.cpp
//--------------------------------------------------------------


#pragma once
#include "_pch_cpp.h"
#include "api_stef2.hpp"

//WATER DROWN AND DAMAGE VALUES
constexpr auto _GFix_PLAYER_air_finished = 20.0f; //was 20
constexpr auto _GFix_PLAYER_drown_damage = 2.0f; //was 2.0f
constexpr auto _GFix_PLAYER_drown_damage_max = 15.0f; //was 15.0f
constexpr auto _GFix_PLAYER_increase = 1.0f; //was 2.0f
constexpr auto _GFix_PLAYER_next_drown_time_delay = 3.0f; //was 2.0f
constexpr auto _GFix_PLAYER_next_painsound_time = 3.0f; //was 3.0f

//--------------------------------------------------------------
// GAMEFIX - Added: Support for ini-files - chrissstrahl
//--------------------------------------------------------------
#define MAX_LINES 1024
#define MAX_LINE_LENGTH 1024
#define MAX_SECTIONS 150

typedef struct {
	char section[MAX_LINE_LENGTH];
	char lines[MAX_LINES][MAX_LINE_LENGTH];
	int line_count;
} gamefix_iniFileSection;

//--------------------------------------------------------------
// GAMEFIX - Added: Function to read contents of a file into a container, each line will be one object
//--------------------------------------------------------------
extern Container<str> gamefix_fileContentTokenized;

//--------------------------------------------------------------
// GAMEFIX - Added: For delayed server commands - daggolin
//--------------------------------------------------------------
typedef struct gamefix_pendingServerCommand_s
{
	char* command;
	gamefix_pendingServerCommand_s* next;
} gamefix_pendingServerCommand;
extern gamefix_pendingServerCommand* pendingServerCommandList[MAX_CLIENTS];

//--------------------------------------------------------------
// GAMEFIX - Added: Information list for all standard levels - chrissstrahl
//--------------------------------------------------------------
struct gamefix_defaultMaps_s
{
	str				mapname = "";
	str				gametypes = "";
	str				gamemodes = "";
};
extern gamefix_defaultMaps_s gamefix_defaultMaps_t[gamefix_defaultMapsSize];

//--------------------------------------------------------------
// GAMEFIX - Added: Information we want to persist over level changes and restarts - chrissstrahl
//--------------------------------------------------------------
struct gamefix_client_persistant_s
{
	bool			isBot = false;
	str				language = "Eng";
	bool			admin = false;
	str				commandsStalled = "";
	int				commands = 0;
	float			commandsLast = 0.0f;
	int				chats = 0;
	float			chatsLast = 0.0f;
	str				currentModel = "models/char/munro.tik";
	str				currentTeam = "none";


	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Phaser shots and hits being count on a per bullet rather as per beam basis - chrissstrahl
	//--------------------------------------------------------------
	EntityPtr		heuristicsWeap = nullptr;
	long int		heuristicsShots = 0;
	bool			heuristicsHit = false;
};
extern gamefix_client_persistant_s gamefix_client_persistant_t[MAX_CLIENTS];

//--------------------------------------------------------------
// GAMEFIX - Added:  - chrissstrahl
//--------------------------------------------------------------
struct gamefix_entity_extraData_s
{
	EntityPtr		activator = nullptr;
	float			lastActivated = -9999.0f;
};
extern gamefix_entity_extraData_s gamefix_entity_extraData_t[MAX_GENTITIES];


Entity*				gamefix_returnInfoPlayerStart(str info);

Entity*				gamefix_getEntity(str& name);
Player*				gamefix_getPlayer(int index);
int					gamefix_getPlayers(bool state);
bool				gamefix_EntityValid(Entity* entity);
bool				gamefix_PlayerValid();
bool				gamefix_checkNotarget(Entity* entity);

Player*				gamefix_getPlayerInsideOfEntity(Entity* eTheBox);
bool				gamefix_checkEntityInsideOfEntity(Entity* eCheck, Entity* eTheBox);

Player*				gamefix_getPlayerByTargetname(const str& targetname);
bool				gamefix_targetedByOtherPlayer(Player* player, Entity* entity);



Player*				gamefix_getClosestPlayer(Entity* entity);
Player*				gamefix_getClosestPlayerSamePlane(Entity* entity);
Player*				gamefix_getClosestPlayer(Entity* entity, bool noSpectator, bool noDead, bool samePlane, int planeMaxVertDiff, int planeMaxRange);
Player*				gamefix_getClosestPlayerToFollow(Actor* actor);
Player*				gamefix_getClosestPlayerActorCanSee(Actor* actor, qboolean useFOV);
Player*				gamefix_getClosestPlayerCanseeIfNoCurEnemy(Actor* actor);
Player*				gamefix_getAnyPlayerPreferably();
Player*				gamefix_getAnyPlayerPreferably(bool noDead, bool noSpectator);
void				gamefix_playerSpectator(Player* player);
void				gamefix_playerChangedTeam(Player* player,const str &realTeamName);
void				gamefix_playerKilled(Player* player);
void				gamefix_playerEntered(Player* player);
void				gamefix_playerSpawn(Player* player);
void				gamefix_playerModelChanged(Player* player);
void				gamefix_playerUseItem(Player* player, const char* name);
void				gamefix_playerScore(Player* player);
void				gamefix_playerClientBegin(gentity_t* ent);
void				gamefix_dialogSetupPlayers(Actor* speaker, char *localizedDialogName, bool headDisplay);
void				gamefix_playerDelayedServerCommand(int entNum, const char* commandText);
void				gamefix_playerHandleDelayedServerCommand(void);
void				gamefix_playerClearDelayedServerCommand(int entNum);

void				gamefix_aiTurnOff();
void				gamefix_aiTurnOn();
Entity*				gamefix_getActorFollowTarget(Actor* actor);
bool				gamefix_actorCanSee(Actor* actor, Entity* entity, qboolean useFOV, qboolean useVisionDistance);
Entity*				gamefix_actorGetCurrentEnemy(Actor* actor);
bool				gamefix_actorHates(Actor* actor, Entity* entity);
bool				gamefix_checkPlayerRanged(Actor* actor, Player* player);
bool				gamefix_checkPlayerRanged(Actor* actor, bool closestOnly);
bool				gamefix_checkPlayerUsingWeaponNamed(Player* player, const str& weaponNameOfPlayer);
Player*				gamefix_getActivatorOrClosestPlayer(Entity* entity);
Player*				gamefix_getActivatorOrClosestPlayerCansee(Actor* actor);

str					gamefix_getServerLanguage();
str					gamefix_getLanguage(Player* player);
qboolean			gamefix_languageEng(const gentity_t* ent);
qboolean			gamefix_languageDeu(const gentity_t* ent);
void				gamefix_vstrLocalLanguage(gentity_t* ent);
str					gamefix_getLocalizedString(Player* player, const str sEnglish,const str sGerman);
str					gamefix_localizeStringForPlayer(Player* player, char unlocal[MAX_QPATH]);
void				gamefix_printAllClients(const str text);

int					gamefix_getCvarInt(str cvarName);
float				gamefix_getCvarFloat(str cvarName);
str					gamefix_getCvar(str cvarName);

Entity*				gamefix_spawn(char const* className, char const* model, char const* origin, char const* targetname, const int spawnflags);
void				gamefix_svFloodProtectDisable();
void				gamefix_kickBots();
void				gamefix_levelFixes();

bool				gamefix_getFileContents(str sFile, str& contents, bool tokenize);
bool				gamefix_setFileContents(str sFile, str& contents);

gamefix_iniFileSection* gamefix_iniSectionsParse(const str& file, const char* data, int* section_count);
void				gamefix_iniSectionsNamesGet(const str& file, Container<str>& sectionList, const str& contents);
str					gamefix_iniSectionGet(const str& file, const str& data, const char* section_name);
str					gamefix_iniSectionSet(const str& file, const str& file_contents, const str& section_name, const str& new_section_contents);
str					gamefix_iniSectionDelete(const str& file, const str& file_contents, const str& section_name);
str					gamefix_iniKeyGet(const str& file, const str& section_contents, const str& key, const str& altVal);
str					gamefix_iniKeySet(const str& file, const str& section_contents, const str& key, const str& value);
str					gamefix_iniKeyDelete(const str& file, const str& section_contents, const str& key);
str					gamefix_iniFindSectionByKey(const str& file, const str& file_contents, const str& key);
str					gamefix_iniFindSectionByValue(const str& file, const str& file_contents, const str& value);

bool				gamefix_containsNonANSI(const unsigned char* buffer, size_t length);
char*				gamefix_convertUtf8UmlautsToAnsi(const char* utf8_str);

str					gamefix_getExtension(const str& in);
void				gamefix_extractIntegerRange(const str& input, int& first, int& second);
void				gamefix_extractFloatRange(const str input, float& first, float& second);
float				gamefix_dialogGetSoundlength(char sound[MAX_QPATH]);
void				gamefix_listSeperatedItems(Container<str>& container, const str& src, const str& seperator);

int					gamefix_findChar(const char* str, const char find);
int					gamefix_findChars(const char* str, const char* find);
int					gamefix_findCharsReverse(const char* str, const char* find);
int					gamefix_findCharsReverse(const char* str, const char* find, unsigned int endAt, unsigned int startAt);
int					gamefix_findString(const char* str, const char* find);
int					gamefix_findStringCase(const str& latinumstack, const str& find, bool wholeWord, unsigned int startPos, bool sameLine);
int					gamefix_countCharOccurrences(const char* str, const char& ch);

char*				gamefix_duplicateString(const char* source);
void				gamefix_replaceSubstring(char* str, const char* find, const char* replace);
str					gamefix_getStringUntilChar(const str& source, const char& delimiter, unsigned int startPos);
char*				gamefix_getStringUntilChar(const char* source, const char& delimiter, unsigned int startPos);
str					gamefix_getStringUntil(const str& sString, const unsigned int iStart, const unsigned int iEnd);
str					gamefix_getLine(const str& sString, const unsigned int iStart);

char*				gamefix_trimWhitespace(char* input, bool dontTrimNewLine);
str					gamefix_trimWhitespace(const str& input, bool dontTrimNewLine);
str					gamefix_cleanMapName(const str& mapname);
str					gamefix_cleanMapKeepParameters(const str& mapname);
str					gamefix_filterChars(const str filthy, str illegal);
str					gamefix_stripDoubleChar(const str filthy, str illegal);

str					gamefix_debugTextErrorMarker(unsigned int pos);

void				gamefix_runFrame(int levelTime, int frameTime);
void				gamefix_shutdownGame();
void				gamefix_initGame();
void				gamefix_cleanupGame(qboolean restart);

