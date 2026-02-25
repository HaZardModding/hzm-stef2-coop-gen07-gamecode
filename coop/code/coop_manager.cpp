#ifdef ENABLE_COOP

#include "../../dlls/game/gamefix.hpp"
#include "../../dlls/game/level.h"
#include "../../dlls/game/mp_manager.hpp"
#include "coop_objectives.hpp"
#include "coop_armory.hpp"
#include "coop_radar.hpp"
#include "coop_manager.hpp"
#include "coop_class.hpp"

extern qboolean G_SetWidgetTextOfPlayer(const gentity_t* ent, const char* widgetName, const char* widgetText);

coopManager_client_persistant_s coopManager_client_persistant_t[MAX_CLIENTS];
coopManager_mapSettings_s coopManager_mapSettings_t;

CoopManager& CoopManager::Get() {
    static CoopManager instance;
    return instance;
}


bool CoopManager::IsCoopEnabled() const {
    return coopEnabled;
}

void CoopManager::DisableCoop() {
    coopEnabled = false;
}

bool CoopManager::IsCoopLevel() {
    return mapFlags.coopMap;
}

bool CoopManager::IsRpgEnabled() const {
    return rpgEnabled;
}

bool CoopManager::IsSameEnviroment(str levelCurrent, str levelOther) {
    if (gi.areSublevels(gamefix_cleanMapName(levelCurrent).c_str(), gamefix_cleanMapName(levelOther).c_str()) || strcmp(gamefix_cleanMapName(levelCurrent).c_str(), gamefix_cleanMapName(levelOther).c_str()) == 0) {
        return true;
    }
       
    return false;
}

bool CoopManager::getSkippingCinematics() {
    return skippingCinematics;
}
void CoopManager::setSkippingCinematics(bool skipping) {
    skippingCinematics = skipping;
}

float CoopManager::getskippingCinematicsLast() {
    return skippingCinematicsLast;
}
void CoopManager::setskippingCinematicsLast(float timeLast) {
    skippingCinematicsLast = timeLast;
}
void CoopManager::setCommunicatorTransporterUiUpdate() {
    communicatorTransporterUiUpdateCheck = true;
}



//check if specific coop files are included
//this is required because certain global_scripts
//need to be included differehtly then
str CoopManager::IncludeScriptReplace(str sLex) {
   
    //check for known coop script files
    IncludeScriptCheck(sLex);
    
    if (IncludedScriptCoop()) {
        if (gamefix_findString(sLex.c_str(), "maps/global_scripts/", false) != -1) {
            str sFile = gamefix_getFileName(sLex);
            str sReplace = "co-op/maps/global_scripts/";
            sReplace += sFile;

            if (gi.FS_ReadFile(sReplace.c_str(), NULL, true) != -1) {
                //DEBUG_LOG(va("%s used from /co-op/ folder\n", sFile.c_str()));
                gi.Printf(va("%s used from /co-op/ folder\n", sFile.c_str()));
                return sReplace;
            }
        }
    }
    return "";
}

void CoopManager::IncludeScriptCheck(str &sLex) {
    if (Q_stricmp(sLex,_COOP_FILE_main) == 0) {
        mapFlags.scriptIncludedCoopMain = true;
        return;
    }
    if (Q_stricmp(sLex, _COOP_FILE_noscript) == 0) {
        mapFlags.scriptIncludedCoopNoscript = true;
        return;
    }
    if (Q_stricmp(sLex, _COOP_FILE_multioptions) == 0) {
        mapFlags.scriptIncludedCoopMom = true;
        return;
    }
    if (Q_stricmp(sLex, _COOP_FILE_multioptions4) == 0) {
        mapFlags.scriptIncludedCoopMom4 = true;
        return;
    }
    if (Q_stricmp(sLex, _COOP_FILE_mrm) == 0) {
        mapFlags.scriptIncludedCoopMrm = true;
        return;
    }
}

bool CoopManager::IncludedScriptCoop() {
    return mapFlags.scriptIncludedCoopMain;
}

str CoopManager::playerScriptCallValidateThreadname(str threadName)
{
    str threadToCall = "";
    for (unsigned int i = 0; i < strlen(threadName); i++) {
        if (threadName[i] == '"' ||
            threadName[i] == '%' ||
            threadName[i] == ';' ||
            threadName[i] == ' ')
        {
            break;
        }
        threadToCall += threadName[i];
    }
    return threadToCall;
}

//checks if this player is allowed to call this thread at this time and circumstance - security measurs
bool CoopManager::playerScriptCallExecute(Entity* entPlayer, str commandName, str threadName, Entity* entUsed)
{
    if (g_gametype->integer == GT_SINGLE_PLAYER) {
        return true;
    }

    if (!entPlayer || !multiplayerManager.inMultiplayer() || !CoopManager::Get().IsCoopEnabled() || !entPlayer->isSubclassOf(Player)) {
        return false;
    }


    threadName = playerScriptCallValidateThreadname(threadName);
    

    Player* player = (Player*)entPlayer;
    /*Deactivated for testing
    if (player->coop_isAdmin()) {
        ExecuteThread(threadName, true,entPlayer);
        return true;
    }*/

    for (int i = CoopSettings_playerScriptThreadsAllowList.NumObjects(); i > 0; i--) {
        CoopSettings_clientThreads_s threadListTemp;
        threadListTemp = CoopSettings_playerScriptThreadsAllowList.ObjectAt(i);



        //command match
        if (Q_stricmp(commandName.c_str(), threadListTemp.command.c_str()) == 0) {
            //dialog active check
            if (entUsed && entUsed->isSubclassOf(Actor) && Q_stricmpn(threadListTemp.command.c_str(), "dialogrunthread", strlen(threadListTemp.command.c_str()))) {
                Actor* actor = (Actor*)entUsed;
                str sDialogName = actor->coop_getBranchDialogName();
                if (!strlen(sDialogName)) {
                    return false;
                }
            }
            //thread match
            if (strlen(threadListTemp.thread) && Q_stricmpn(threadName.c_str(), threadListTemp.thread.c_str(), strlen(threadListTemp.thread.c_str())) == 0) {
                //item check
                if (strlen(threadListTemp.item) && threadListTemp.item != "None") {
                    str itemName = "";
                    weaponhand_t	hand = WEAPON_ANY;
                    player->getActiveWeaponName(hand, itemName);
                    //mismatch - just skip
                    if (Q_stricmpn(threadListTemp.item.c_str(), threadListTemp.item.c_str(), strlen(threadListTemp.item.c_str())) != 0) {
                        return false;
                    }
                }

                //check if admin is required
                if (threadListTemp.adminRequired && !player->coop_isAdmin()) {
                    return true;
                }

                ExecuteThread(threadName, true, entPlayer);
                return true;
            }
        }
    }
    return false;
}

bool CoopManager::entityUservarGetKillMessage(Entity* inflictor, str& killmessageEng, str& killmessageDeu)
{
    int uservarsNumHandeled = 10;

    if (!inflictor) {
        killmessageEng = "Inflictor-Missing-???";
        killmessageDeu = "Inflictor-Fehlt-???";
        return true;
    }
    killmessageEng = "";
    killmessageDeu = "";

    str entityValue="";
    ScriptVariable* entityData = nullptr;
    for (int i = 1; i <= uservarsNumHandeled; i++) {
        entityData = inflictor->entityVars.GetVariable(va("uservar%d", i));       
        if (!entityData) {
            continue;
        }

        entityValue = entityData->stringValue();
        if (Q_stricmpn("killmessage ", entityValue.c_str(), 12) == 0) {
            //"uservar1" "killmessage AbC"
            if (entityValue.length() > 12) {
                killmessageEng = "";
                for (int j = 12; j < entityValue.length(); j++) {
                    killmessageEng += entityValue[j];
                }
            }
        }

        entityValue = entityData->stringValue();
        if (Q_stricmpn("killmessage_de ", entityValue.c_str(), 15) == 0) {
            //"uservar1" "killmessage_de AbC"
            if (entityValue.length() > 15) {
                killmessageDeu = "";
                for (int j = 15; j < entityValue.length(); j++) {
                    killmessageDeu += entityValue[j];
                }
            }
        }
    }
    if (!killmessageDeu.length() && killmessageEng.length()) {
        killmessageDeu = killmessageEng;
    }

    if (!killmessageEng.length() && killmessageDeu.length()) {
        killmessageEng = killmessageDeu;
    }

    if (killmessageEng.length()) {
        return true;
    }
    return false;
}

bool CoopManager::entityUservarGetName(Entity* inflictor, str& killmessageEng, str& killmessageDeu)
{
    int uservarsNumHandeled = 10;

    if (!inflictor) {
        killmessageEng = "Inflictor-Missing-???";
        killmessageDeu = "Inflictor-Fehlt-???";
        return true;
    }
    killmessageEng = "";
    killmessageDeu = "";

    str entityValue = "";
    ScriptVariable* entityData = nullptr;
    for (int i = 1; i <= uservarsNumHandeled; i++) {
        entityData = inflictor->entityVars.GetVariable(va("uservar%d", i));
        if (!entityData) {
            continue;
        }

        entityValue = entityData->stringValue();
        if (Q_stricmpn("name", entityValue.c_str(), 4) == 0) {
            //"uservar1" "killmessage AbC"
            if (entityValue.length() > 5) {
                killmessageEng = "";
                for (int j = 6; j < entityValue.length(); j++) {
                    killmessageEng += entityValue[j];
                }
            }
        }

        if (!killmessageDeu.length() && killmessageEng.length()) {
            killmessageDeu = killmessageEng;
        }

        if (killmessageEng.length()) {
            return true;
        }
    }
    return false;
}

int CoopManager::entityUservarContains(Entity* ent, const str &find)
{
    int uservarsNumHandeled = 10;

    str entityValue = "";
    ScriptVariable* entityData = nullptr;
    for (int i = 1; i <= uservarsNumHandeled; i++) {
        entityData = ent->entityVars.GetVariable(va("uservar%d", i));
        if (!entityData) {
            continue;
        }

        entityValue = entityData->stringValue();
        if (gamefix_findString(entityValue.c_str(), find.c_str(), false)) {
            return i;
        }
    }

    //return value 1-10 is true
    return 0;
}

void CoopManager::ActorThink(Actor *actor) {
    if (!CoopManager::Get().IsCoopEnabled()) {
        return;
    }
}

void CoopManager::ClientThink(Player *player) {
    playerSetup(player);
    playerAdminThink(player);
    coop_objectivesUpdatePlayer(player);
    coop_radarUpdate(player);
    communicatorUpdateUi();

    coopClass.coop_classCheckUpdateStat(player);
    coopClass.coop_classRegenerate(player);

	coopCircleMenu.circleMenuThink(player);
}

bool CoopManager::callvoteManager(const str& _voteString) { 
    if (!_voteString.length()) {
        return false;
    }
    Container<str> voteStringList;
    gamefix_listSeperatedItems(voteStringList, _voteString," ");

    if (Q_stricmp(voteStringList.ObjectAt(1).c_str(), "skipcinematic") == 0) {
        if (level.cinematic && world->skipthread && (world->skipthread.length() > 0) && Q_stricmp(world->skipthread.c_str(), "null") != 0) {
            G_ClearFade();
            str skipthread = world->skipthread;
            world->skipthread = "";
            ExecuteThread(skipthread);
            setSkippingCinematics(false);
            setskippingCinematicsLast(level.time);
            //return true;
        }
        return true;
    }

    if (Q_stricmp(voteStringList.ObjectAt(1).c_str(), "coop_flushtikis") == 0) {
        CoopManager::Get().flushTikis();
        return true;
    }

    if (Q_stricmp(voteStringList.ObjectAt(1).c_str(), "coop_quit") == 0) {
        gi.SendConsoleCommand("quit");
        return true;
    }

    if (Q_stricmp(voteStringList.ObjectAt(1).c_str(), "coop_end") == 0) {
        CThread* thread = ExecuteThread("coop_endLevel", true, (Entity*)world);
        if (thread == NULL) {
            gamefix_printAllClients(_COOP_INFO_usedCommand_levelend1);
        }
        else {
            gamefix_printAllClients(_COOP_INFO_usedCommand_levelend2);
        }
        return true;
    }

    if (Q_stricmp(voteStringList.ObjectAt(1).c_str(), "coop_next") == 0 || Q_stricmp(voteStringList.ObjectAt(1).c_str(), "coop_prev") == 0) {
        str prevVal, nextVal, mapName = "";
        cvar_t* cvar = gi.cvar_get("mapname");
        if (cvar) {
			mapName = cvar->string;
            mapName = gamefix_getStringUntilChar(mapName,'$', 0);
        }

        str categoryName = gamefix_iniFindSectionByValue(_COOP_FILE_maplist, coopManager_maplist_contents, mapName);

        if (!gamefix_iniFindPrevNextValueWrap_maplist(
            _COOP_FILE_maplist,
            coopManager_maplist_contents,
            categoryName,
            mapName, //gamefix_cleanMapName(level.mapname)
            &prevVal,
            &nextVal
        )) {
			gi.Printf("CoopManager::callvoteManager: Failed to find previous or next map for %s.\n", level.mapname.c_str());
            return true;
        }

        if (Q_stricmp(voteStringList.ObjectAt(1).c_str(), "coop_next") == 0) {
            if (nextVal.length()) {
                gi.SendConsoleCommand(va("map %s", nextVal.c_str()));
            }
        }
        else { // coop_prev
            if (prevVal.length()) {
                gi.SendConsoleCommand(va("map %s", prevVal.c_str()));
            }
        }
        return true;
    }

    if (voteStringList.NumObjects() >= 2) {
        if (Q_stricmp(voteStringList.ObjectAt(1).c_str(), "coop_ff") == 0) {
            float firedlyFireVal = atof(voteStringList.ObjectAt(2));
            coopSettings.setSetting_friendlyFireMultiplicator(firedlyFireVal);
            callvoteUpdateUi("Friendly Fire",va("%.2f", firedlyFireVal), "coopGpoFF");
            return true;
        }
        if (Q_stricmp(voteStringList.ObjectAt(1).c_str(), "coop_airaccelerate") == 0) {
            unsigned int airaccelerate = atoi(voteStringList.ObjectAt(2));
            sv_airaccelerate->integer = coopSettings.setSetting_airaccelerate(airaccelerate);;
            callvoteUpdateUi("Air Accelerate",va("%d", airaccelerate), "coopGpoAa");
            return true;
        }
        if (Q_stricmp(voteStringList.ObjectAt(1).c_str(), "coop_maxspeed") == 0) {
            unsigned int maxSpeed = atoi(voteStringList.ObjectAt(2));
            callvoteUpdateUi("Movement Speed",va("%d", maxSpeed), "coopGpoMvSpd");
            world->setPhysicsVar("maxspeed", coopSettings.setSetting_maxSpeed(maxSpeed));
            return true;
        }
        if (Q_stricmp(voteStringList.ObjectAt(1).c_str(), "coop_skill") == 0) {
            unsigned int skillValue = atoi(voteStringList.ObjectAt(2));
            callvoteUpdateUi("Difficulty",va("%d", skillValue), "coopGpoSkill");
            skill->integer = coopSettings.setSetting_difficulty(skillValue);
            return true;
        }
        if (Q_stricmp(voteStringList.ObjectAt(1).c_str(), "coop_awards") == 0) {
            bool awards = (bool)atoi(voteStringList.ObjectAt(2));
            callvoteUpdateUi("Coop Awards",va("%d", awards), "coopGpoAw");
            coopSettings.setSetting_awards(awards);
            return true;
        }
    }

    return false;
}

void CoopManager::callvoteUpdateUiPlayer(Player *player, str sValue, str sWidget)
{
    if (!player) {
        return;
    }
    if (player->coop_hasCoopInstalled()) {
        gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand %s title %s", sWidget.c_str(), sValue.c_str()));
    }
}

void CoopManager::callvoteUpdateUi(str sText, str sValue, str sWidget)
{
    if (!CoopManager::Get().IsCoopEnabled()) {
        return;
    }

    Player* player = nullptr;
    for (int i = 0; i < gameFixAPI_maxClients(); i++) {
        player = GetPlayer(i);
        if (player) {
            gameFixAPI_hudPrint(player, va("^5INFO^8: %s set to^5 %s\n", sText.c_str(), sValue.c_str()));
            if (player->coop_hasCoopInstalled()) {
                callvoteUpdateUiPlayer(player, sValue, sWidget);
            }
        }
    }
}

bool CoopManager::callvoteSkipCinematicPlayer(Player* player)
{
    if (!player || !level.cinematic) {
        return false;
    }
    if (g_gametype->integer == GT_SINGLE_PLAYER) {
        return true;
    }

    //don't execute skipcinematic to fast
    //this pervents that another vote will be started while the current one is being skipped
    if ((getskippingCinematicsLast() + 0.5f) > level.time) {
        return false;
    }

    //player presses long or repeatedly ESC
    if ((getPlayerData_cinematicEscapePressLastTime(player) + 0.25) > level.time && !getSkippingCinematics() && world->skipthread.length() > 0) {
        setskippingCinematicsLast(level.time);
        setSkippingCinematics(true);
        setPlayerData_cinematicEscapePressLastTime(player,level.time);
        multiplayerManager.callVote(player, "skipcinematic", "");
        return false;
    }
    //player presses long ESC and a cinematic skip vote is active
    else {
        //check if player has voted, if not make him vote yes for skip, if a vote is active, then exit
        if ((getPlayerData_cinematicEscapePressLastTime(player) + 0.25) > level.time && getSkippingCinematics())
        {
            if (level.cinematic && world->skipthread.length() > 0 && !multiplayerManager.gameFixAPI_getPlayerHasVoted(player)) {
                multiplayerManager.vote(player, "y");
            }
            setPlayerData_cinematicEscapePressLastTime(player,level.time);
            return false;
        }

        //if ESC is pressed only shortly or if no skip is active, show/hide menu
        setPlayerData_cinematicEscapePressLastTime(player,level.time);
		gamefix_playerDelayedServerCommand(player->entnum, "pushmenu ingame_multiplayer");
        return false;
    }
}

void CoopManager::puzzleObjectUsedstartthread(PuzzleObject *puzzle)
{
    if (CoopManager::Get().IsCoopEnabled()) {
        ScriptVariable* usedThread = puzzle->entityVars.GetVariable("coop_usedStartThread");
        if (usedThread) {
            str threadName = usedThread->stringValue();
            if (threadName.length()) {
                ExecuteThread(threadName, true, puzzle);
            }
        }
    }
}

CoopManager::MapFlags CoopManager::getMapFlags()
{
    return mapFlags;
}

float CoopManager::getSkillBasedDamage(float currentDamage) {
    GameplayManager* gpm = nullptr;
    float damageMultiplier;
    int skillLevel;

    skillLevel = skill->integer;

    gpm = GameplayManager::getTheGameplayManager();

    if (gpm->hasObject("SkillLevel-PlayerDamage"))
    {
        if (skillLevel == 0)
            damageMultiplier = gpm->getFloatValue("SkillLevel-PlayerDamage", "Easy");
        else if (skillLevel == 1)
            damageMultiplier = gpm->getFloatValue("SkillLevel-PlayerDamage", "Normal");
        else if (skillLevel == 2)
            damageMultiplier = gpm->getFloatValue("SkillLevel-PlayerDamage", "Hard");
        else
            damageMultiplier = gpm->getFloatValue("SkillLevel-PlayerDamage", "VeryHard");


        currentDamage *= damageMultiplier;
    }
    return currentDamage;
}

//return player quanity based on certain parameters
int CoopManager::getNumberOfPlayers(bool noDead, bool noSpectator) {
    int iNumberOfPlayer = 0;
    Player* player = nullptr;
    for (int i = 0; i < gameFixAPI_maxClients(); i++) {
        player = gamefix_getPlayer(i);

        if (!player) {
            continue;
        }

        if (noDead && gameFixAPI_isDead((Entity*)player) || noSpectator && gameFixAPI_isSpectator_stef2((Entity*)player)) {
            continue;
        }

        if (gamefix_checkNotarget((Entity*)player)) {
            continue;
        }
        iNumberOfPlayer++;
    }
    return iNumberOfPlayer;
}

//executed once, only on game server start/load
//executed before world and other entities exist
void CoopManager::Init() {
    try {
        gi.Printf(_COOP_INFO_INIT_game);
        DEBUG_LOG(_COOP_INFO_INIT_game);

        LoadMapListFromINI();
        LoadPlayerModelsFromINI();
        CreateConfigstringList();

        gi.Printf(_COOP_INFO_INIT_gamedone);
    }
    catch (const char* error) {
        gi.Printf(_COOP_ERROR_fatal, error);
        G_ExitWithError(error);
    }
}

//executed once, only on game server start/load
//creates a list of all possible player models
//- will allow to remove all unused from configstrings
void CoopManager::CreateConfigstringList() {
    str contents = "";
    str section_contents = "";
    if (!gamefix_getFileContents(_COOP_FILE_configstring, contents, true)) {
        gi.Printf(_COOP_WARNING_FILE_failed, _COOP_FILE_configstring);
        return;
    }

    section_contents = gamefix_iniSectionGet(_COOP_FILE_configstring, contents, "remove");
    gamefix_listSeperatedItems(CoopManager_configstringList, section_contents, "\n");
    section_contents = "";
    contents = "";

    str skinFile = "";
    const int limitofChars = 131072;
    char filename[128];
    int numdirs;
    char dirlist[limitofChars];
    char* dirptr;
    int i;
    int dirlen;
    bool validPlayerModel = false;

    numdirs = gi.FS_GetFileList("models/char", ".tik", dirlist, limitofChars);
    dirptr = dirlist;

    for (i = 0; i < numdirs; i++, dirptr += dirlen + 1) {
        validPlayerModel = false;
        dirlen = strlen(dirptr);
        strcpy(filename, dirptr);
        char* data = NULL;
        int len = gi.FS_ReadFile(va("models/char/%s", filename), (void**)&data, qtrue);

        //TIKI
        //$include models/char/playerdata.tik
        //init { server { ... validPlayerModel
        if (len > 0 && data) {
            const char* parse = data;
            const char* token;

            bool token_tiki_found = false;
            bool token_include_open = false;
            bool token_init_open = false;
            bool token_server_open = false;

            while ((token = COM_Parse(&parse)) && token[0]) {
                //gi.Printf("TOKEN: %s\n", token);

                //check tiki
                if (!token_tiki_found) {
                    if (Q_stricmp(token, va("tiki")) == 0) {
                        token_tiki_found = true;
                        //gi.Printf(" - FOUND: %s\n", token);
                    }
                    continue;
                }

                //check include
                if (!token_include_open) {
                    if (Q_stricmp(token, va("$include")) == 0) {
                        token_include_open = true;
                        //gi.Printf(" - FOUND: %s\n", token);
                        continue;
                    }
                }
                else {
                    if (Q_stricmp(token, va("models/char/playerdata.tik")) == 0) {
                        validPlayerModel = true;
                        token_include_open = false;
                        //gi.Printf(" - FOUND: %s\n", token);
                        break;
                    }

                    //unexpected token - end here
                    token_include_open = false;
                    continue;
                }

                //check valid player model string
                if (!token_init_open) {
                    if (Q_stricmp(token, "init") == 0) {
                        token_init_open = true;
                        //gi.Printf(" - FOUND: %s\n", token);
                        continue;
                    }
                }
                else {
                    //skip open brakets
                    if (Q_stricmp(token, "{") == 0) {
                        continue;
                    }

                    //exepecting server
                    if (!token_server_open) {
                        //found server
                        if (Q_stricmp(token, "server") == 0) {
                            token_server_open = true;
                            //gi.Printf(" - FOUND: %s\n", token);
                            continue;
                        }

                        //unexpected token - end here
                        token_init_open = false;
                        token_server_open = false;
                        continue;
                    }
                    //expecting validPlayerModel
                    else {
                        //section ends
                        if (Q_stricmp(token, "}") == 0) {
                            token_server_open = false;
                            token_init_open = false;
                            validPlayerModel = false;
                            break;
                        }

                        //found validPlayerModel
                        if (Q_stricmp(token, "validplayermodel") == 0) {
                            token_server_open = false;
                            token_init_open = false;
                            validPlayerModel = true;
                            //gi.Printf(" - FOUND: %s\n", token);
                            break;
                        }
                    }
                }
            }
        }
        if (data) {
            gi.FS_FreeFile(data);
        }

        if (validPlayerModel) {
            CoopManager_configstringList.AddUniqueObject(va("models/char/%s", filename));
        }
    }
}

//executed once, only on game server start/load
//loads coop valid model list for players from ini
void CoopManager::LoadPlayerModelsFromINI() {
    str contents;
    str section_contents;
    if (!gamefix_getFileContents(_COOP_FILE_validPlayerModels, contents, true)) {
        gi.Printf(_COOP_WARNING_FILE_failed, _COOP_FILE_validPlayerModels);
        //default fallback hardcoded model - if coop gamefiles are missing
        coopManager_validPlayerModels.AddUniqueObject("models/char/munro.tik");
        return;
    }

    section_contents = gamefix_iniSectionGet(_COOP_FILE_validPlayerModels, contents, "coopSkins");
	section_contents = section_contents.tolower();
    gamefix_listSeperatedItems(coopManager_validPlayerModels, section_contents, "\n");
    coopManager_validPlayerModels.AddUniqueObject("models/char/munro.tik");
}

//executed once, only on game server start/load
//loads coop settings from ini
void CoopManager::LoadMapListFromINI() {
    if (!gamefix_getFileContents(_COOP_FILE_maplist, coopManager_maplist_contents, true)) {
        //Print maplist file warning
        ///////////////////////////////////////////////////
        gi.Printf(va(_COOP_WARNING_FILE_failed, _COOP_FILE_maplist));
        return;
    }
}

//executed each level load - always
//requires the world to be already spawned
//Other: CoopManager::LevelStart()
void CoopManager::InitWorld() {
    try {
        gi.Printf(_COOP_INFO_INIT_world, level.mapname.c_str());
        DEBUG_LOG(_COOP_INFO_INIT_world, level.mapname.c_str());

        coopSettings.serverConfigCheck();

        skippingCinematics = false;

        SetMapType();

        str coopStatus = "inactive";
        str sAllowSpMaps = "0";

        //this early we can't use API - multiplayer manager has not been started yet
        if (getMapFlags().coopMap && g_gametype->integer == GT_MULTIPLAYER) {
            sAllowSpMaps = "1";
            coopStatus = "ACTIVE";
            coopEnabled = true;
        }
        gi.cvar_set("gfix_allowSpMaps", sAllowSpMaps);

        //notify scripts - is a coop level, is a coop server
        levelVars.SetVariable("isCoopLevel",float(int(coopEnabled)));
        levelVars.SetVariable("isCoopServer", float(int(coopEnabled)));


        if (coopEnabled) {
            if (world) {
                world->setPhysicsVar("maxSpeed",coopSettings.getSetting_maxSpeed());
            }

			CoopManager::Get().loadClientIniData();

            coopSettings.loadSettings();
            coopSettings.playerCommandsAllow();
            coopSettings.playerScriptThreadsAllow();
            coopSettings.loadScoreList();
            coopSettings.loadDeathList();
            coopPlaydialog.readDialogFile("loc/Eng/dialog/", gamefix_cleanMapName(level.mapname), CoopPlaydialog_dialogListContainer_eng);
            coopPlaydialog.readDialogFile("loc/Deu/dialog/", gamefix_cleanMapName(level.mapname), CoopPlaydialog_dialogListContainer_deu);

            gi.Printf(_COOP_INFO_INIT_status, coopStatus.c_str(), level.mapname.c_str());
        }
    }
    catch (const char* error) {
        gi.Printf(_COOP_ERROR_fatal, error);
        G_ExitWithError(error);
    }
}

//detects what kind of gametype the level
//support and sets related parameters
coopManager_mapSettings_s CoopManager::DetectMapType(str mapName) {

    str mapNameClean = gamefix_cleanMapName(mapName);

    coopManager_mapSettings_s struct_currentMap;
    struct_currentMap.cleanName = mapNameClean;

    //remember if it is a standard map - do this always
    //check if it is a coop or rpg map
    if (gameFixAPI_mapIsStock(mapNameClean))         struct_currentMap.stockMap = true;
    if (mapNameClean.icmpn("coop_", 5) == 0)         struct_currentMap.coopMap = true;
    else if (mapNameClean.icmpn("rpg_", 4) == 0)     struct_currentMap.rpgMap = true;

    struct_currentMap.checkPoint = "";

    if (!coopManager_maplist_contents.length()) {
        return struct_currentMap;
    }
    const str fileContents = coopManager_maplist_contents;


    //Get name of the section the map is listed in
    str sectionOfCurrentMap = gamefix_iniFindSectionByValue(_COOP_FILE_maplist, fileContents, mapNameClean.c_str());
    if (sectionOfCurrentMap == _COOP_MAPLIST_CAT_included) { struct_currentMap.coopIncluded = true; struct_currentMap.coopMap = true; }
    if (sectionOfCurrentMap == _COOP_MAPLIST_CAT_coopSpMission) { struct_currentMap.coopSpMission = true; struct_currentMap.coopMap = true; }
    if (sectionOfCurrentMap == _COOP_MAPLIST_CAT_coopSpIgm) { struct_currentMap.coopSpIgm = true; struct_currentMap.coopMap = true; }
    if (sectionOfCurrentMap == _COOP_MAPLIST_CAT_coopSpSecret) { struct_currentMap.coopSpSecret = true; struct_currentMap.coopMap = true; }
    if (sectionOfCurrentMap == _COOP_MAPLIST_CAT_multiplayerOnly) { struct_currentMap.multiplayerOnly = true; }
    if (sectionOfCurrentMap == _COOP_MAPLIST_CAT_singleplayerOnly) { struct_currentMap.singleplayerOnly = true; }

    if (struct_currentMap.coopMap == false && struct_currentMap.multiplayerOnly == false && struct_currentMap.singleplayerOnly == false) {
        str sectionContents = gamefix_iniSectionGet(_COOP_FILE_maplist, fileContents, _COOP_MAPLIST_CAT_coopcompatible);
        str CoopCompatible = gamefix_iniKeyGet(_COOP_FILE_maplist, sectionContents, sectionOfCurrentMap, "");

        if (CoopCompatible.length() && CoopCompatible == "true") {
            struct_currentMap.coopMap = true;
        }
    }
    return struct_currentMap;
}

//detects what kind of gametype the level
//support and sets related parameters
void CoopManager::SetMapType() {
    coopManager_mapSettings_s struct_currentMap = DetectMapType(level.mapname);

    mapFlags.rpgMap = struct_currentMap.rpgMap;
    mapFlags.coopMap = struct_currentMap.coopMap;
    mapFlags.coopIncluded = struct_currentMap.coopIncluded;
    mapFlags.coopSpMission = struct_currentMap.coopSpMission;
    mapFlags.coopSpIgm = struct_currentMap.coopSpIgm;
    mapFlags.coopSpSecret = struct_currentMap.coopSpSecret;
    mapFlags.multiplayerOnly = struct_currentMap.multiplayerOnly;
    mapFlags.singleplayerOnly = struct_currentMap.singleplayerOnly;
    mapFlags.stockMap = struct_currentMap.stockMap;
	mapFlags.cleanName = struct_currentMap.cleanName;
    mapFlags.scriptIncludedCoopMain = false;
    mapFlags.scriptIncludedCoopNoscript = false;
    mapFlags.scriptIncludedCoopMom = false;
    mapFlags.scriptIncludedCoopMom4 = false;
    mapFlags.scriptIncludedCoopMrm = false;
}

//Executes the level script
//prefers coop/maps/ over maps/ , prefers .script over .scr
void CoopManager::LoadLevelScript(str mapname) {
    mapname = gamefix_cleanMapName(mapname);
    
    Container<str> scriptFileExt;
    scriptFileExt.AddObject("script");
    scriptFileExt.AddObject("scr");
    int scriptFileExtensions = scriptFileExt.NumObjects();

    Container<str> scriptFileDir;
    scriptFileDir.AddObject("co-op/maps/");
    scriptFileDir.AddObject("maps/");
    int scriptFileDirectories = scriptFileDir.NumObjects();
    
    str dir,ext;
    //multiplayer prioritise files from co-op folder and .script files
    if (gameFixAPI_inMultiplayer()) {
        for (int j = 1; j <= scriptFileDirectories; j++) {
            dir = scriptFileDir.ObjectAt(j);
            for (int i = 1; i <= scriptFileExtensions; i++) {
                ext = scriptFileExt.ObjectAt(i);
                //construct directory mapname and file extension
                str sFile = va("%s%s.%s", dir.c_str(), mapname.c_str(), ext.c_str());
                if (gi.FS_ReadFile(sFile.c_str(), NULL, true) != -1) {
                    gi.Printf("Adding script: '%s'\n", sFile.c_str());
                    // just set the script, we will start it in G_Spawn
                    level.SetGameScript(sFile.c_str());
                    return;
                }
            }
        }
    }
    //singleplayer prioritise files from maps folder and .scr files
    else {
        for (int j = scriptFileDirectories; j > 0; j--) {
            dir = scriptFileDir.ObjectAt(j);
            for (int i = scriptFileExtensions; i > 0; i--) {
                ext = scriptFileExt.ObjectAt(i);
                //construct directory mapname and file extension
                str sFile = va("%s%s.%s", dir.c_str(), mapname.c_str(), ext.c_str());
                if (gi.FS_ReadFile(sFile.c_str(), NULL, true) != -1) {
                    gi.Printf("Adding script: '%s'\n", sFile.c_str());
                    // just set the script, we will start it in G_Spawn
                    level.SetGameScript(sFile.c_str());
                    return;
                }
            }
        }
    }

    gi.Printf("Level *.scr or *.script file not found in co-op/maps or maps: '%s'\n", mapname.c_str());
    level.SetGameScript("");
}

//Executed if game is shut down
//Cleans up stuff while world and entities still exist
//Executed ONLY on game shutdown
void CoopManager::Shutdown() {

    if (coopEnabled) {
        CoopSettings_playerScriptThreadsAllowList.FreeObjectList();
    }

    coopManager_validPlayerModels.FreeObjectList();
}

//Executed each time a level is starting - scripts are started
//Executed from: Level::Start - always
//Other: CoopManager::InitWorld
void CoopManager::LevelStart(CThread* gamescript) {
    //start coop main function automatically, will auto-handle SP/MP
    if (gamescript && gamescript->labelExists(_COOP_SCRIPT_main)) {
        CThread* coopMain = Director.CreateThread(_COOP_SCRIPT_main);
        if (coopMain) {
            coopMain->DelayedStart(0.0f);
        }
    }
}

//Executed if level is exited/changed/restarted - but not on first load/game start
//Cleans up stuff while world and entities still exist - Not executed if game server is quit
void CoopManager::LevelEndCleanup(qboolean temp_restart) {
    coop_objectives_reset();

    setSkippingCinematics(false);
    setskippingCinematicsLast(-99.1f);

    Player* player = nullptr;
    for (int i = 0; i < gameFixAPI_maxClients(); i++) {
        player = gamefix_getPlayer(i);
        if (player) {
            setPlayerData_spawnLocationSpawnForced(player, true);
            setPlayerData_respawnLocationSpawn(player, false);
            setPlayerData_lastValidLocation(player, Vector(0.0f, 0.0f, 0.0f));
            setPlayerData_objectives_reset(player);
        }
    }

    if (coopEnabled) {
        if (coopSettings.getSetting_strafeJumpingAllowedWasOn()) {
            sv_strafeJumpingAllowed->integer = 1;
        }
        
        //make sure to save client data to ini file
        saveClientIniData();

        CoopSettings_deathList.FreeObjectList();
        CoopSettings_scoreKillList.FreeObjectList();
        CoopSettings_playerScriptThreadsAllowList.FreeObjectList();
		CoopPlaydialog_dialogListContainer_eng.FreeObjectList();
		CoopPlaydialog_dialogListContainer_deu.FreeObjectList();

        //save settings to ini
        coopSettings.saveSettings();

        //make sure the model cache is cleared, so that removed models and animations are not still in memory
        CoopManager::Get().flushTikis();
    }
}

void CoopManager::MissionFailed(const str & reason) {
    if (g_gametype->integer == GT_SINGLE_PLAYER) { return; }

    str sReason = reason;
    if (reason.length() == 0) {
        sReason = "DefaultFailure";
    }

    //-- Intentionally no $$ wanted
    if (sReason[0] == '#') {
        sReason = gamefix_getStringUntil(sReason, 1, 120);
    }
    //-- If it is just a single word and has no $$ it is extreemly likley a local string, so add $$
    else if (gamefix_findString(sReason.c_str(), "$$") == -1 && gamefix_findString(sReason.c_str(), " ") == -1) {
        sReason = va("$$%s$$", sReason.c_str());
    }

    gentity_t* other;
    for (int j = 0; j < game.maxclients; j++) {
        other = &g_entities[j];
        if (other->inuse && other->client && other->entity) {
            Player* player = (Player*)other->entity;
            //show hud for coop clients and text for others
            if (player->coop_hasCoopInstalled()) {
                gamefix_playerDelayedServerCommand(player->entnum, va("set ui_failureReason %s\n", sReason.c_str()));
                gamefix_playerDelayedServerCommand(player->entnum, "pushmenu coop_failure");
            }
            else {
                gamefix_playerDelayedServerCommand(player->entnum, "hudprint ^1=============^3$$MISSIONFAILED$$^1=============\n");
                gamefix_playerDelayedServerCommand(player->entnum, va("hudprint ^3%s\n", sReason.c_str()));
            }
        }
    }
}

str CoopManager::MissionFailureConfigString(const str& reason)
{
    str newReason = reason;
    //if # is used it means set string without $$
    if (reason[0] == '#') {
        newReason = gamefix_getStringUntil(reason, 1, 120);
        newReason = va("\n\n%s^0", newReason.c_str());
    }
    return newReason;
}

void CoopManager::MissionFailureLoadMap()
{
    if (!multiplayerManager.inMultiplayer()) { return; }

    Event* ev_loadMap = new Event(EV_World_coop_loadMap);
    str sParameters = "";

    //Checkpoints - do this only if we know we have a valid scriptfile for this map, cuz scriptmaster keeps values of last script file - chrissstrahl
    if (level.game_script != "") {
        sParameters = program.coop_getStringVariableValue("coop_string_checkpointParameters");
    }

    if (sParameters != "") {
        ev_loadMap->AddString(va("%s$%s", level.mapname.c_str(), sParameters.c_str()));
    } else {
        ev_loadMap->AddString(level.mapname.c_str());
    }

    world->PostEvent(ev_loadMap, 6.0f);
}

//called by ClientThink
void CoopManager::playerSetup(Player* player) {
    if (!player) {
        return;
    }

    if (getPlayerData_coopSetupDone(player) && getPlayerData_coopClientIdDone(player)) {
        return;
    }

    if (getPlayerData_coopSetupNextCheckTime(player) >= level.time) {
        return;
    }

    setPlayerData_coopSetupNextCheckTime_update(player);
    setPlayerData_coopSetupTries_update(player);

    if (!getPlayerData_coopSetupStarted(player)) {
        //headhudtext widget hide in multiplayer, because it does not work right (flickering) - also coop mod has its own
        gamefix_playerDelayedServerCommand(player->entnum, "globalwidgetcommand DialogConsole rect -10000 0 0 0");

        //Client ID
        gamefix_playerDelayedServerCommand(player->entnum, "vstr coop_cId");
        //Coop Version
        gamefix_playerDelayedServerCommand(player->entnum, "exec co-op/cfg/detect.cfg");
        setPlayerData_coopSetupStarted(player, true);
    }

    //handle client id
    playerSetupClId(player);
    playerSetupCoop(player);
}

void CoopManager::playerSetupClId(Player* player) {
    if (getPlayerData_coopClientIdDone(player)) {
        return;
    }
    
    if (getPlayerData_coopSetupTries(player) > _COOP_SETTINGS_SETUP_TRIES) {
        playerClIdSet(player);
        setPlayerData_coopClientIdDone(player, true);

        DEBUG_LOG("# playerSetupClId Failed\n");
        return;
    }
}

void CoopManager::playerSetupCoop(Player* player) {
    if (getPlayerData_coopSetupDone(player)) {
        return;
    }
    
    if (getPlayerData_coopSetupTries(player) > _COOP_SETTINGS_SETUP_TRIES) {
        coopManager_client_persistant_t[player->entnum].coopVersion = 0;

        setPlayerData_coopSetupDone(player, true);
        setPlayerData_objectives_setupDone(player);

        DEBUG_LOG("# playerSetupCoop Failed\n");
        return;
    }
}

void CoopManager::playerSetupCoopUi(Player* player) {
    if (!player || !player->coop_hasCoopInstalled()) {
        return;
    }

    if (gameFixAPI_inSingleplayer()) {
        return;
    }

    //populate ui
    gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand coopGpoSkill title %d", skill->integer));
    gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand coopGpoAw title %d", coopSettings.getSetting_awards()));
    gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand coopGpoMvSpd title %d", coopSettings.getSetting_maxSpeed()));
    gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand coopGpoFF title %.2f", coopSettings.getSetting_friendlyFireMultiplicator()));
    gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand coopGpoRspwt title %s", "NOT_IMPL"));
    gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand coopGpoLms title %s", "NOT_IMPL"));
    gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand coopGpoDb title %s", "NOT_IMPL"));
    gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand coopGpoSt title %s", "NOT_IMPL"));
    gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand coopGpoAa title %d", sv_airaccelerate->integer));
}

//called by console command "coopinstalled"
void CoopManager::playerCoopDetected(const gentity_t* ent, const char* coopVer) {
    if (!ent || !ent->entity || !ent->client || !coopVer || !strlen(coopVer)) {
        return;
    }
    Player* player = (Player*)ent->entity;

    if (getPlayerData_coopSetupDone(player)) {
        return;
    }   
    
    int iVer =  atoi(coopVer);
    coopCircleMenu.circleMenuSetup(player);
    setPlayerData_coopVersion(player, iVer);
    setPlayerData_coopSetupDone(player, true);
    setPlayerData_coopUpdateNoticeSend(player, false);
    gamefix_playerDelayedServerCommand(player->entnum, "vstr coop_class");
    setCommunicatorTransporterUiUpdate();

    if (iVer < _COOP_CLIENT_MINIMUM_COMPATIBELE_VERSION) {
        //let player know that there is a newer version of the coop mod
        CoopManager::Get().playerUpdateNoticeUi(player);

        DEBUG_LOG("# COOP OUTDATED: %d for %s (WAITED: %d)\n", iVer, player->client->pers.netname, getPlayerData_coopSetupTries(player));
        return;
    }

    //run coop setup
    DEBUG_LOG("# COOP DETECTED: %d for %s (WAITED: %d)\n", iVer, player->client->pers.netname, getPlayerData_coopSetupTries(player));
}

//called by console command "coopcid"
void CoopManager::playerClIdDetected(const gentity_t* ent, const char* clientId) {
    if (!ent || !ent->entity || !ent->client || !clientId || !strlen(clientId)) {
        return;
    }
    Player* player = (Player*)ent->entity;

    const char* cCClientId = gi.argv(1);
    str sId = va("%s", cCClientId);

    if (getPlayerData_coopClientIdDone(player)) {
        player->hudPrint("COOPDEBUG: coop_cId - Timed Out: Rejected!\n");
        gi.Printf(va("COOPDEBUG: coop_cId - Timed Out: Rejected! For: %s\n", player->client->pers.netname));
        DEBUG_LOG("# CId TIMED OUT: %s\n", sId.c_str());
        return;
    }

    if (!sId.length()) {
        gi.Printf(va("COOPDEBUG: coop_cId - Bad or Empty: Rejected! For: %s\n", player->client->pers.netname));
        DEBUG_LOG("COOPDEBUG: coop_cId - Bad or Empty: Rejected! For: %s\n", player->client->pers.netname);
        return;
    }

    str sClientId = "";
    sId = gamefix_filterChars(sId, ";[]=%");
    sId = gamefix_trimWhitespace(sId,false);

    sClientId = sId;
    if (sClientId.length()) {
        //client id will be saved to CoopManager_clientIniData if the level changes or the player disconnects
        
        //prevent players from cheating lms by reconnecting
        //player->coop_lmsCheckReconnectHack();
        setPlayerData_coopClientIdDone(player,true);
        setPlayerData_coopClientId(player, sId);
        playerDataRestore(player);
    }
}

void CoopManager::playerClIdSet(Player* player)
{
    if (getPlayerData_coopClientIdDone(player)) {
        return;
    }

    //that should create a pretty uniqe player id
    str sId = va("%d%d", (int)gamefix_getTimeStamp(), player->entnum);
    setPlayerData_coopClientId(player, sId);
    gi.SendServerCommand(player->entnum, va("stufftext \"seta coop_cId 0;set coop_cId coopcid %s\"\n", sId.c_str()));
    DEBUG_LOG("# CId NEW CREATED: %s\n", sId.c_str());
}

void CoopManager::playerAddMissionHuds(Player* player)
{
    str hudName = "";
    str hudCommand = "";
    for (int iHuds = 0; iHuds < _COOP_SETTINGS_MISSION_HUDS_MAX; iHuds++) {
        hudName = gamefix_getEntityVarString(world, va("coop_registredHud%d", iHuds));
        if (!hudName.length()) {
            continue;
        }
        gamefix_playerDelayedServerCommand(player->entnum,va("ui_addhud %s", hudName.c_str()));

        //execute command if one is attached - usually to exec a cfg or to handle globalwidgetcommands for ui elements
        hudCommand = gamefix_getEntityVarString(world, va("coop_registredHud%d_command", iHuds));
        if (hudCommand.length()) {
            gamefix_playerDelayedServerCommand(player->entnum, va("%s", hudCommand.c_str()));
        }
    }
}

void CoopManager::playerRemoveMissionHuds(Player* player)
{
    if (!player) {
        return;
    }

    str hudName = "";
    str hudCommand = "";
    for (int iHuds = 0; iHuds < _COOP_SETTINGS_MISSION_HUDS_MAX; iHuds++) {
        hudName = gamefix_getEntityVarString(world, va("coop_registredHud%d", iHuds));
        if (!hudName.length()) {
            continue;
        }
        gamefix_playerDelayedServerCommand(player->entnum,va("ui_removehud %s", hudName.c_str()));
    }
}

void CoopManager::playerUpdateNoticeUi(Player* player)
{
    if( !player || player->coop_getCoopVersion() >= _COOP_THIS_VERSION || player->coop_getCoopVersion() == 0 || getPlayerData_coopUpdateNoticeSend(player)) {
        return;
	}

    setPlayerData_coopUpdateNoticeSend(player, true);
    gamefix_playerDelayedServerCommand(player->entnum,"pushmenu okDialog");
    
    const char* infoText;
    if (player->coop_hasLanguageGerman()) {
        infoText = _COOP_INFO_VERSION_pleaseUpdate_deu;
    }
    else {
        infoText = _COOP_INFO_VERSION_pleaseUpdate;
    }

    gamefix_playerDelayedServerCommand(player->entnum, va("globalWidgetCommand OkDialogTitle labeltext %s~Server:^%d~Client:^%d", gamefix_replaceForLabelText(infoText).c_str(), _COOP_THIS_VERSION, player->coop_getCoopVersion()));
}

void CoopManager::playerTricorderScanUi(Player* player, bool add)
{
    if(!player || player->coop_getCoopVersion() == 0) {
        return;
    }

    if (getPlayerData_coopTricorderScanHudOn(player) && !add) {
        gamefix_playerDelayedServerCommand(player->entnum, "ui_removehud coop_scan");
        setPlayerData_coopTricorderScanHudOn(player, false);
    }
    else if(!getPlayerData_coopTricorderScanHudOn(player) && add) {
        gamefix_playerDelayedServerCommand(player->entnum, "ui_addhud coop_scan");
        setPlayerData_coopTricorderScanHudOn(player, true);
    }
}

void CoopManager::playerTricorderScanUiHandle(Player* player, Entity* ent)
//sets menu text
{
    if(!player || player->coop_getCoopVersion() == 0) {
        return;
    }

    bool isScanning = false;
    Equipment* equipment = nullptr;
    Weapon* weapon = player->GetActiveWeapon(WEAPON_DUAL);
    if (weapon && weapon->isSubclassOf(Equipment)) {
        equipment = (Equipment*)weapon;
        isScanning = equipment->isScanning();
    }

    if (!isScanning || playerGetDoingPuzzle(player)) {
        playerTricorderScanUi(player, false);
        return;
    }

    if (!player->GetTargetedEntity()) {
        return;
    }

    if ((coopManager_client_persistant_t[player->entnum].coopTricorderScanLastSend + 0.5) >= level.time) {
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopTricorderScanLastSend = level.time;

    GameplayManager* gpm = GameplayManager::getTheGameplayManager();
    str type = player->GetTargetedEntity()->getArchetype();
    str descr1 = gpm->getStringValue(type, "Description1");
    str descr2 = gpm->getStringValue(type, "Description2");
    str descr3 = gpm->getStringValue(type, "Description3");

    if (!descr1.length() && !descr2.length() && !descr3.length()) {
        return;
    }

    playerTricorderScanUi(player, true);

    str sInteractiveType = gpm->getStringValue(type, "InteractiveType");
    str sRed = "1";
    str sBlue = "1";
    str sGreen = "1";

    if (sInteractiveType.length()) {
        sRed = gamefix_getStringLength(gpm->getFloatValue(sInteractiveType, "Red"), 0, 4);
        sBlue = gamefix_getStringLength(gpm->getFloatValue(sInteractiveType, "Blue"), 0, 4);
        sGreen = gamefix_getStringLength(gpm->getFloatValue(sInteractiveType, "Green"), 0, 4);
    }

    if (descr1 != coopManager_client_persistant_t[player->entnum].coopTricorderScanData1) {
        coopManager_client_persistant_t[player->entnum].coopTricorderScanData1 = descr1;
        if (descr1 == "") {
            gamefix_playerDelayedServerCommand(player->entnum, "set coop_scan0 $$TriPuz_Default$$");
        }
        else {
            gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand Descr1 fgcolor %s %s %s 1", sRed.c_str(), sGreen.c_str(), sBlue.c_str()));
            gamefix_playerDelayedServerCommand(player->entnum, va("set coop_scan0 $$%s$$", descr1.c_str()));
        }
    }
    if (descr2 != coopManager_client_persistant_t[player->entnum].coopTricorderScanData2) {
        coopManager_client_persistant_t[player->entnum].coopTricorderScanData2 = descr2;
        if (descr2 == "") {
            gamefix_playerDelayedServerCommand(player->entnum, "set coop_scan1 $$TriPuz_Default$$");
        }
        else {
            gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand Descr2 fgcolor %s %s %s 1", sRed.c_str(), sGreen.c_str(), sBlue.c_str()));
            gamefix_playerDelayedServerCommand(player->entnum, va("set coop_scan1 $$%s$$", descr2.c_str()));
        }
    }
    if (descr3 != coopManager_client_persistant_t[player->entnum].coopTricorderScanData3) {
        coopManager_client_persistant_t[player->entnum].coopTricorderScanData3 = descr3;
        if (descr3 == "") {
            gamefix_playerDelayedServerCommand(player->entnum, "set coop_scan2 $$TriPuz_Default$$");
        }
        else {
            gamefix_playerDelayedServerCommand(player->entnum, va("globalwidgetcommand Descr3 fgcolor %s %s %s 1", sRed.c_str(), sGreen.c_str(), sBlue.c_str()));
            gamefix_playerDelayedServerCommand(player->entnum, va("set coop_scan2 $$%s$$", descr3.c_str()));
        }
    }
}

bool CoopManager::playerDataReset(Player* player) {

    if (!player ||
        gameFixAPI_isBot(player)) {
        return false;
    }

	//grab data of player and update or create data
	int clientContainerPos = -1;
    for (int i = 1; i <= CoopManager_clientIniData.NumObjects(); i++) {
        if (CoopManager_clientIniData.ObjectAt(i).clientId == getPlayerData_coopClientId(player)) {
            clientContainerPos = i;
            break;
        }
    }
	
    //get data into the right format
    coopManager_clientIniData_s clientIniData;
	clientIniData.clientId = getPlayerData_coopClientId(player);
    clientIniData.health = 0;
    clientIniData.armor = 0;
    clientIniData.phaser = 0;
	clientIniData.plasma = 0;
	clientIniData.fed = 0;
	clientIniData.idryll = 0;
	clientIniData.deathTime = 0; //TODO: implement lms deathTime <- they should be updated anyway
    clientIniData.mapName = "";
	clientIniData.lmsDeaths = 0; //TODO: implement lms deaths <- they should be updated anyway

	DEBUG_LOG("CoopManager::playerDataSave - Saving data for %s\n", player->client->pers.netname);

    //if data found, update
    if (clientContainerPos > 0) {
        CoopManager_clientIniData.SetObjectAt(clientContainerPos, clientIniData);
        return true;
    }

    //if no data found, create new
    CoopManager_clientIniData.AddObject(clientIniData);
    return true;
}

bool CoopManager::playerDataSave(Player* player) {

    if (!player ||
        gameFixAPI_isSpectator_stef2((Entity*)player) ||
        gameFixAPI_isDead((Entity*)player) ||
        gameFixAPI_isBot(player)) {
        return false;
    }

    //not a mission or custom coop map do not restore
    if (!IsCoopLevel()) {
        return false;
    }
    if (!getMapFlags().coopSpMission &&
        !getMapFlags().coopMap &&
        !getMapFlags().rpgMap) {
        return false;
    }

	//grab data of player and update or create data
	int clientContainerPos = -1;
    for (int i = 1; i <= CoopManager_clientIniData.NumObjects(); i++) {
		//no id, don't save, the player is probably already disconnected
        if (!getPlayerData_coopClientId(player).length()) {
			return false;
        }

        if (CoopManager_clientIniData.ObjectAt(i).clientId == getPlayerData_coopClientId(player)) {
            clientContainerPos = i;
            break;
        }
    }
	
    //get data into the right format
    Sentient* sentPlayer = (Sentient*)player;
    coopManager_clientIniData_s clientIniData;
	clientIniData.clientId = getPlayerData_coopClientId(player);
    clientIniData.health = sentPlayer->health;
    clientIniData.armor = sentPlayer->GetArmorValue();
    clientIniData.phaser = sentPlayer->AmmoCount("Phaser");
	clientIniData.plasma = sentPlayer->AmmoCount("Plasma");
	clientIniData.fed = sentPlayer->AmmoCount("Fed");
	clientIniData.idryll = sentPlayer->AmmoCount("Idryll");
	clientIniData.deathTime = 0; //TODO: implement lms deathTime
	clientIniData.mapName = level.mapname;
	clientIniData.lmsDeaths = 0; //TODO: implement lms deaths

	DEBUG_LOG("CoopManager::playerDataSave - Saving data for %s\n", player->client->pers.netname);

	//if data found, update
    if (clientContainerPos > 0) {
        CoopManager_clientIniData.RemoveObjectAt(clientContainerPos);
    }
    
    //if no data found, create new
    CoopManager_clientIniData.AddObject(clientIniData);
    return true;
}

bool CoopManager::playerDataRestore(Player* player) {

    if (!player ||
        gameFixAPI_isSpectator_stef2((Entity*)player) ||
        gameFixAPI_isDead((Entity*)player) ||
        gameFixAPI_isBot(player)) {
        return false;
    }

    //not a mission or custom coop map do not restore
    if (!IsCoopLevel()) {
        return false;
    }
    if (!getMapFlags().coopSpMission &&
        !getMapFlags().coopMap &&
        !getMapFlags().rpgMap) {
        return false;
    }

    if (!getPlayerData_coopClientId(player).length()) {
		return false;
    }

    //grab data of player and applay to player
    coopManager_clientIniData_s clientIniData;
    for (int i = 1; i <= CoopManager_clientIniData.NumObjects(); i++) {
        clientIniData = CoopManager_clientIniData.ObjectAt(i);
        if (getPlayerData_coopClientId(player) == CoopManager_clientIniData.ObjectAt(i).clientId) {

			//different map, do not restore
            if (!IsSameEnviroment(clientIniData.mapName,level.mapname)) {
                return false;
            }

            //apply data to player
			Sentient* sentPlayer = (Sentient*)player;
            sentPlayer->SetHealth(clientIniData.health);
            
            float curArmor = sentPlayer->GetArmorValue();
            Event* armorEvent;
            armorEvent = new Event(EV_Sentient_GiveArmor);
            armorEvent->AddString("BasicArmor");
            armorEvent->AddInteger(clientIniData.armor - curArmor);
            player->ProcessEvent(armorEvent);
            //sentPlayer->SetMyArmorAmount();

gi.Printf("CoopManager::playerDataRestore Add BasicArmor: %f %s\n",float(clientIniData.armor - curArmor), player->client->pers.netname);

            sentPlayer->GiveAmmo("Phaser", ((clientIniData.phaser) - player->AmmoCount("Phaser")), false, -1);
            sentPlayer->GiveAmmo("Plasma", ((clientIniData.plasma) - player->AmmoCount("Plasma")), false, -1);
            sentPlayer->GiveAmmo("Fed", ((clientIniData.fed) - player->AmmoCount("Fed")), false, -1);
            sentPlayer->GiveAmmo("Idryll", ((clientIniData.idryll) - player->AmmoCount("Idryll")), false, -1);
            //sentPlayer-> (clientIniData.deathTime);
            //sentPlayer-> (clientIniData.lmsDeaths);

            DEBUG_LOG("# DATA RESTORED for %s (WAITED: %d)\n",player->client->pers.netname, getPlayerData_coopSetupTries(player));
			return true;
        }
    }
	//gi.Printf("CoopManager::playerDataRestore - No data found for player %s\n", player->client->pers.netname);
	DEBUG_LOG("CoopManager::playerDataRestore - No data found for player %s\n", player->client->pers.netname);
	return false;
}

bool CoopManager::playerItemPickup(Entity* player, Item* item)
{
    //allow to pick up item if all other players have that item
    if (!player || !item || !player->isSubclassOf(Player)) {
        return false;
    }

    if (!IsCoopEnabled()) {
        return true;
    }

    // fire off any pickup_thread's
    str pickupThread = item->GetPickupThread();
    
    if (pickupThread.length()) {
        ExecuteThread(pickupThread, qtrue, (Entity*)player);
    }

    if (!item->isSubclassOf(Weapon)) {
        return true;
    }

    Sentient* sent = (Sentient*)player;
    if (!sent->coop_hasItem(item->model)) {
        return true;
    }

    Player* other = nullptr;
    for (int i = 0; i < gameFixAPI_maxClients(); i++) {
        other = gamefix_getPlayer(i);

        if (!other || gameFixAPI_isSpectator_stef2((Entity*)other) || gameFixAPI_isDead((Entity*)other)) {
            continue;
        }
        
        if (!(Sentient*)other->coop_hasItem(item->model)) {
            return false;
        }
    }

    return true;
}

void CoopManager::playerAdminThink(Player* player) {
    if (getPlayerData_coopAdmin(player)) {
        return;
    }

    if (!getPlayerData_coopAdminAuthStarted(player)) {
        return;
    }

    if (!getPlayerData_coopAdminAuthString_changed(player)) {
        return;
    }

    str authStringActual = gamefix_getCvar("coop_admin");
    if (strlen(authStringActual) < _COOP_SETTINGS_ADMIN_LOGIN_AUTH_STRING_LENGTH_MIN){
        setPayerData_coopAdminAuthStarted(player,false);
        gamefix_playerDelayedServerCommand(player->entnum, va(_COOP_INFO_adminLogin_auth, gamefix_replaceForLabelText(_COOP_INFO_adminLogin_authCvarToShort).c_str()));
        return;
    }

    if (strlen(authStringActual) > _COOP_SETTINGS_ADMIN_LOGIN_AUTH_STRING_LENGTH_MAX){
        setPayerData_coopAdminAuthStarted(player,false);
        gamefix_playerDelayedServerCommand(player->entnum, va(_COOP_INFO_adminLogin_auth, gamefix_replaceForLabelText(_COOP_INFO_adminLogin_authCvarToLong).c_str()));
        return;
    }

    str authStringPlayer = getPlayerData_coopAdminAuthString(player);
    int authStringLengthNew = strlen(authStringPlayer);
    setPlayerData_coopAdminAuthStringLengthLast(player, authStringLengthNew);

    //total failure
    if (authStringLengthNew > _COOP_SETTINGS_ADMIN_LOGIN_AUTH_STRING_LENGTH_MAX || getPlayerData_coopAdminAuthAttemps(player) > _COOP_SETTINGS_ADMIN_LOGIN_TRIES) {
        setPayerData_coopAdminAuthAttemps_update(player);
        setPayerData_coopAdminAuthStarted(player, false);
        setPlayerData_coopAdminAuthString(player,"");
        if (getPlayerData_coopAdminAuthAttemps(player) > _COOP_SETTINGS_ADMIN_LOGIN_TRIES) {
            gi.SendConsoleCommand(va("kick %d\n", player->entnum));
            return;
        }
        gamefix_playerDelayedServerCommand(player->entnum, "globalwidgetcommand coop_comCmdLoginCode title ''");
        gamefix_playerDelayedServerCommand(player->entnum, va(_COOP_INFO_adminLogin_auth, gamefix_replaceForLabelText(_COOP_INFO_adminLogin_authFailure).c_str()));
        return;
    }

    //success - match
    if (authStringPlayer == authStringActual) {
        setPlayerData_coopAdmin(player, true);
        setPayerData_coopAdminAuthStarted(player, false);
        setPayerData_coopAdminAuthAttemps_reset(player);
        gamefix_playerDelayedServerCommand(player->entnum, va(_COOP_INFO_adminLogin_auth, gamefix_replaceForLabelText(_COOP_INFO_adminLogin_authSuccess).c_str()));
        return;
    }
}

void CoopManager::communicatorUpdateUi()
{
    if (!communicatorTransporterUiUpdateCheck) {
        return;
    }
    communicatorTransporterUiUpdateCheck = false;

    str emptyName = "$$Empty$$";
    Player* player = nullptr;

    for (int i = 0; i < gameFixAPI_maxClients(); i++) {

        player = GetPlayer(i);
        if (!player || gameFixAPI_isBot(player) || player->coop_getCoopVersion() == 0) {
            continue;
        }

        if (i >= _COOP_SETTINGS_PLAYER_SUPPORT_MAX) {
            break;
        }

        Player* otherPlayer = nullptr;
        for (int j = 0; j < _COOP_SETTINGS_PLAYER_SUPPORT_MAX; j++) {
            otherPlayer = GetPlayer(j);

            //set empty for name if player not valid or disconnecting - clear old name
            if (!otherPlayer || !otherPlayer->client || getPlayerData_disconnecting(otherPlayer->entnum)) {
                if (coopManager_client_persistant_t[player->entnum].communicatorSendNames[j] != emptyName) {
                    coopManager_client_persistant_t[player->entnum].communicatorSendNames[j] = emptyName;
                    gamefix_playerDelayedServerCommand(player->entnum, va("globalWidgetCommand coop_comTran%d title %s", j, emptyName.c_str()));
                }
                continue;
            }

            //set name if player valid and name changed or not yet send to user
            if (coopManager_client_persistant_t[player->entnum].communicatorSendNames[j] != otherPlayer->client->pers.netname ) { //|| coopManager_client_persistant_t[player->entnum].communicatorSendNames[j] != ""
               
                str sendPlayerName = otherPlayer->client->pers.netname;
                coopManager_client_persistant_t[player->entnum].communicatorSendNames[j] = sendPlayerName;
                char* name = (char*)sendPlayerName.c_str();

                Q_CleanStr(name);
                gamefix_replaceSubstring(name, " ", "_");
                gamefix_playerDelayedServerCommand(player->entnum, va("globalWidgetCommand coop_comTran%d title %s", j, sendPlayerName.c_str()));
            }
        }
    }
}

void CoopManager::playerMoveToSpawn(Player* player) {
    Entity* spawnPoint = multiplayerManager.gameFixAPI_getMultiplayerGame()->getSpawnPoint(player);
    if (!spawnPoint) {
        spawnPoint = gamefix_returnInfoPlayerStart(_GFixEF2_INFO_GAMEFIX_spawnlocations_TeamBaseAddPlayerToTeam);
    }
    if (spawnPoint) {
        player->WarpToPoint(spawnPoint);
    }
}

Entity* CoopManager::getSpawnSpecific(int spotNumber){
    Entity* ent = nullptr;

    if (spotNumber > _COOP_SETTINGS_PLAYER_SPAWNSPOT_MAX || spotNumber < 1) {
        gi.Error(ERR_FATAL, va("CoopManager::getSpawnSpecific() spotNumber '%d' out of range\n", spotNumber));
        return ent;
    }

    //Script based spawnspot
    Vector vAngle = Vector(0.0f, 0.0f, 0.0f);
    Vector vSpawn = program.coop_getVectorVariableValue(va("coop_vector_spawnOrigin%i", spotNumber));
    vAngle[1] = program.coop_getFloatVariableValue(va("coop_float_spawnAngle%d", spotNumber));
    if (vAngle[1] == 0.0f) {
        vAngle[1] = program.coop_getFloatVariableValue("coop_float_spawnAngle0");
    }

    if (vSpawn.length() > 0) {
        ent = G_FindClass(NULL, "info_player_start");
        Vector vOld = ent->origin;
        if (ent) {
            ent->setAngles(vAngle);
            ent->setOrigin(vSpawn);
            ent->NoLerpThisFrame();
            return ent;
        }
    }
    
    //info_player_deathmatch based spawnspot with targetname
    TargetList* tlist;
    tlist = world->GetTargetList(va("ipd%d", spotNumber), false);
    if (tlist) {
        ent = tlist->GetNextEntity(NULL);
        if (ent) {
            return ent;
        }
    }

    gi.Printf(va(_COOP_INFO_spawnspotNumberedNotFound,spotNumber));

    return ent;
}

void CoopManager::ActorDamage(Actor* actor, Entity* enemy, float& damage)
{
    //Scale Damage Actors recieve from players
    if (!actor || !enemy || !IsCoopEnabled()) {
        return;
    }

    /* //check against (Team) ai
    if (enemy->isSubclassOf(Actor)) {
        Actor* enemyActor = (Actor*)enemy;
        if (enemyActor->actortype == actor->actortype) {
            return;
        }
    } else */

    if(!enemy->isSubclassOf(Player)) {
        return;
    }

    if (damage < actor->max_health) {//check if enemy can't be killed with one hit - like small bugs and such
        int iPlayerNum = gamefix_getPlayers(true);

        float fMultiplicator = 1;

        //this is a boss - scale damage with the number of players
        if (actor->GetActorFlag(ACTOR_FLAG_UPDATE_BOSS_HEALTH)) {
            if (iPlayerNum <= 1) fMultiplicator = 1.0f;
            else if (iPlayerNum == 2)fMultiplicator = 0.70f;
            else if (iPlayerNum == 3)fMultiplicator = 0.60f;
            else if (iPlayerNum == 4)fMultiplicator = 0.50f;
            else if (iPlayerNum == 5)fMultiplicator = 0.40f;
            else if (iPlayerNum == 6)fMultiplicator = 0.30f;
            else fMultiplicator = 0.25f;
        }
        //regular enemies - make enemies take slightly less damage
        else {
            if (iPlayerNum <= 1) fMultiplicator = 1.0f;
            else if (iPlayerNum == 2)fMultiplicator = 0.90f;
            else if (iPlayerNum == 3)fMultiplicator = 0.85f;
            else if (iPlayerNum == 4)fMultiplicator = 0.80f;
            else if (iPlayerNum == 5)fMultiplicator = 0.75f;
            else if (iPlayerNum == 6)fMultiplicator = 0.70f;
            else fMultiplicator = 0.65f;
        }

        damage = (damage * fMultiplicator);
    }
}

bool CoopManager::playerMoveToSpawnSpecific(Player* player, int spotNumber)
{
    if (!player) {
        return false;
    }
    if (spotNumber > _COOP_SETTINGS_PLAYER_SPAWNSPOT_MAX || spotNumber < 1) {
        gi.Error(ERR_FATAL, va("CoopManager::playerMoveToSpawnSpecific() spotNumber '%d' out of range\n", spotNumber));
        return false;
    }
    Entity* ent = getSpawnSpecific(spotNumber);
    if (ent) {
        player->WarpToPoint(ent);
        return true;
    }
    return false;
}

void CoopManager::playerTargetnames(Player *player, Entity* viewTrace)
{
    if (!CoopManager::Get().IsCoopEnabled()) {
        return;
    }

    if (!player || player->getHealth() <= 0 || !viewTrace || viewTrace->entnum == world->entnum) {
        return;
    }

    if (multiplayerManager.inMultiplayer() && getPlayerData_targetedShow(player)) {
        Vector vData;
        ScriptVariable* scriptVar = NULL;
        scriptVar = player->entityVars.GetVariable("!targeted");
        if (scriptVar != NULL) {
            vData = scriptVar->vectorValue();
        }
        else {
            vData = Vector(0, 0, 0);
        }
        //0=time,1=entitynum,2=?
        if ((vData[0] + 3) < level.time || (int)vData[1] != viewTrace->entnum) {
            vData[0] = level.time;
            vData[1] = viewTrace->entnum;
            player->entityVars.SetVariable("!targeted", vData);
            player->hudPrint(va("^5Object:^3 $%s, ^5Class:^3 %s\n", viewTrace->targetname.c_str(), viewTrace->getClassname()));
        }
    }
}

//Executed every level restart/reload or when player disconnects
void CoopManager::playerReset(Player* player) {
    if (!player) {
        return;
    }
    
    setPlayerData_targetedShow(player, false);
    setPlayerData_respawnMe(player,false);
    setPlayerData_spawnLocationSpawnForced(player,true);
    setPlayerData_respawnLocationSpawn(player,false);
    setPlayerData_lastValidLocation(player, Vector(0.0f, 0.0f, 0.0f));
    setPlayerData_lastValidViewAngle(player, Vector(0.0f, 0.0f, 0.0f));
    setPlayerData_lastSpawned(player, -1.0f);
    setPlayerData_objectives_reset(player);
    setPlayerData_cinematicEscapePressLastTime(player,0.0f);

    setPlayerData_coopTricorderPuzzleing(player, false);
    setPlayerData_coopClassLocked(player, false);
    setPlayerData_coopClasslastTimeUpdatedStat(player, -1.0f);

    setPlayerData_coopClassLastTimeChanged(player, -999.8f);
    setPlayerData_coopClassLastTimeApplied(player, -998.7f);
    setPlayerData_coopClassLastTimeUsedMsg(player,0.0f);
    setPlayerData_revivedStepLasttime(player,-997.6);
    setPlayerData_revivedStepCounter(player,0);

    coopCircleMenu.circleMenuReset(player);
    setPlayerData_communicatorSendNames_reset(player);
    setPlayerData_entityTargetedSince_reset(player);
    setPlayerData_coopTricorderScanData_reset(player);

    setPlayerData_coopSetupUiSpawnedDone(player, false);

    //see also will cleanup in: playerLeft
}

//Executed only when player connects first time
void CoopManager::playerConnect(int clientNum) {
    if (clientNum < 0 || clientNum >= MAX_CLIENTS) {
        return;
    }

    if (gameFixAPI_inMultiplayer()) {
        //used to minimize the usage of configstrings due to cl_parsegamestate issue
        //if a new player connects mid game
        if (level.time > 30.0f) {
            configstringCleanup();
        }

        setPlayerData_disconnecting(clientNum, false);
        setCommunicatorTransporterUiUpdate();
    }
    

    DEBUG_LOG("# playerConnect\n");
}

//Executed when player disconnects - only upon actual disconnect not on mapchange or relaod
void CoopManager::playerDisconnect(Player* player) {
    if (!player) {
        return;
    }
    DEBUG_LOG("# playerDisconnect\n");

    setPlayerData_disconnecting(player->entnum, true);

    //update player data, so that it can be written to ini
    playerDataSave(player);

    coop_radarReset(player);

    setPlayerData_coopUpdateNoticeSend(player,false);
    setPlayerData_coopClientIdDone(player, false);
    setPlayerData_coopAdmin(player,false);
    setPayerData_coopAdminAuthAttemps_reset(player);
    setPlayerData_coopClientId(player,"");
    setPlayerData_coopSetupStarted(player, false);
    setPlayerData_coopSetupTries_reset(player);
    setPlayerData_coopSetupNextCheckTime_reset(player);
    setPlayerData_coopSetupDone(player, false);
    setPlayerData_coopVersion(player, -1);
    setPlayerData_coopClass(player, "");
    
    //rest will be cleaned up in: playerLeft

    //update stats for all other players
    setCommunicatorTransporterUiUpdate();
    coopClass.coop_classUpdateClassStats();
}

//Executed when player object is detroyed, every map reload or exit or disconnect - Always (Multiplayer + Singleplayer)
void CoopManager::playerLeft(Player* player) {

    if (player) {
        ExecuteThread("coop_justLeft", true, player);
    }

	//update player data, so that it can be written to ini
    playerDataSave(player);

    //reset player data
    playerReset(player);

    DEBUG_LOG("# playerLeft\n");
}

void CoopManager::playerJoined(Player* player) {
    if (!player) {
        return;
    }

    DEBUG_LOG("# playerJoined\n");
}

//Executed upon entering server - Always (Multiplayer + Singleplayer)
void CoopManager::playerEntered(gentity_t* ent) {
    if (ent && ent->entity) {
        ExecuteThread("coop_justEntered", true, ent->entity);

        setCommunicatorTransporterUiUpdate();
    }
    DEBUG_LOG("# playerEntered\n");
}

//Executed on death - Always (Multiplayer + Singleplayer)
void CoopManager::playerDied(Player *player) {
    if (!player) {
        return;
    }

	//reset player data
    playerDataReset(player);

    playerRemoveMissionHuds(player);
    ExecuteThread("coop_justDied", true, player);

    //entity archetype info reset
    setPlayerData_entityTargetedSince_reset(player);

    playerTricorderScanUi(player, false);
}
//Executed when player gets transported - Always (Multiplayer + Singleplayer)
void CoopManager::playerTransported(Entity *entity){
    if (!entity)
        return;

    if (!entity->isSubclassOf(Sentient))
        return;

    gamefix_setMakeSolidAsap(entity, true, level.time + FRAMETIME);
    
    if (entity->isSubclassOf(Player)) {
        //entity archetype info reset
        setPlayerData_entityTargetedSince_reset((Player*)entity);

        ExecuteThread("coop_justTransported", true, (Player*)entity);
    }
}
//Executed on spawn - Multiplayer
void CoopManager::playerSpawned(Player* player) {
    if (!player)
        return;

    if (!gameFixAPI_isSpectator_stef2(player)) {
        coop_armoryEquipPlayer(player);

        //entity archetype info reset
        setPlayerData_entityTargetedSince_reset(player);

        playerDataRestore(player);

        coop_radarReset(player);
        gamefix_playerDelayedServerCommand(player->entnum, "exec co-op/cfg/ea.cfg");
        playerAddMissionHuds(player);

        setPlayerData_coopSetupUiSpawnedDone(player, true);

        coopClass.coop_classApplayAttributes(player, true);
    }

    //make sure ability can be used right after every spawn
    player->entityVars.SetVariable("!ability", -9999.0f);
    player->entityVars.SetVariable("!abilityPrintout", -9999.0f);

    gamefix_setMakeSolidAsap((Entity*)player, true, level.time + FRAMETIME);
    ExecuteThread("coop_justSpawned", true, player);
}
//Executed on death - Multiplayer
void CoopManager::playerBecameSpectator(Player *player){
    if (player) {
        coopManager_client_persistant_t[player->entnum].spawnLocationSpawnForced = true;
        coopManager_client_persistant_t[player->entnum].respawnLocationSpawn = true;

        //entity archetype info reset
        setPlayerData_entityTargetedSince_reset(player);

        playerTricorderScanUi(player, false);

        ExecuteThread("coop_justBecameSpectator", true, player);
    }
}
//Executed on death - Multiplayer
void CoopManager::playerChangedModel(Player *player){
    if (player) {
        //not a valid model, handle
        str sModel = player->model.c_str();
        if (coopManager_validPlayerModels.IndexOfObject(sModel.tolower()) == 0) {
            //DEBUG_LOG("NOT ALLOWED: %s\n", player->model.c_str());
            sModel = multiplayerManager.getDefaultPlayerModel(player);
            multiplayerManager.changePlayerModel(player, va("%s", sModel.c_str()), true);

            //show visual feedback in menu and reset model with some delay
            gamefix_playerDelayedServerCommand(player->entnum, "globalwidgetcommand playersetup_coopWarn enable");
            gamefix_playerDelayedServerCommand(player->entnum,va("set mp_playermodel %s", sModel.c_str()),1.0f);
            return;
        }
        ExecuteThread("coop_justChangedModel", true, player);
    }
}
//Executed on death - Multiplayer
void CoopManager::playerChangedClass(Player* player) {
    if (player) {
        ExecuteThread("coop_justChangedClass", true, player);
    }
}

void CoopManager::playerKilledActor(Player* player, Actor* actor) {
    if (!player || !actor || !player->isSubclassOf(Player) || !gameFixAPI_inMultiplayer()) {
        return;
    }

    //HACK, set attacker as enemy, so we can retrive it
    actor->enemyManager->SetCurrentEnemy((Entity*)player);
    
    //change player statistics
    multiplayerManager.addKill(player->entnum, 1);

    if (actor->actortype == IS_CIVILIAN || actor->actortype == IS_FRIEND || actor->actortype == IS_TEAMMATE) {
        multiplayerManager.addPoints(player->entnum, -_COOP_SETTINGS_PLAYER_PENALTY_BADKILL);
    }
    else {
        //use - scorelist with actor names
        int pointsEarned = 1;

        str actorArcheType = actor->getArchetype();
        str actorTargetName = actor->targetname;
        if(actorTargetName.length()) {
            for (int i = 1; i <= CoopSettings_scoreKillList.NumObjects(); i++) {
                CoopSettings_killScore_s addKillScore;
                addKillScore = CoopSettings_scoreKillList.ObjectAt(i);
                if (addKillScore.name == actorTargetName && addKillScore.type == "targetnames" || addKillScore.name == actorArcheType && addKillScore.type == "actornames") {
                    pointsEarned = addKillScore.points;
                    break;
                }
            }
        }
        multiplayerManager.addPoints(player->entnum, pointsEarned);

        if (coopSettings.getSetting_awards()) {
            multiplayerManager.gameFixAPI_getMultiplayerAwardSystem()->coop_awardEnemyKilled(player,(Entity*)actor);
        }
    }
}

bool CoopManager::playerDamagedCoop(Player* damagedPlayer, Damage& damage) {
    if (!gameFixAPI_inMultiplayer() || !IsCoopEnabled() || !IsCoopLevel()) {
        return false;
    }

    float oldHealth = damagedPlayer->health;
    Entity* actualAttacker = nullptr;
    Player* attackingPlayer = nullptr;
    float finalDamage = damage.damage;
    int finalKnockBack = damage.knockback;

    /*if (damagedPlayer->coop_getPowerup())
        damage.damage = damagedPlayer->coop_getPowerup()->getDamageTaken(damage.attacker, damage.damage, damage.meansofdeath);

    if (damagedPlayer->coop_getRune())
        damage.damage = damagedPlayer->coop_getRune()->getDamageTaken(damage.attacker, damage.damage, damage.meansofdeath);
    */

    if (damage.attacker && damage.attacker != world) {
        actualAttacker = damage.attacker;
        if (actualAttacker->isSubclassOf(Player)) {
            attackingPlayer = (Player*)actualAttacker;

            // Change damage based on the mode
            finalDamage = multiplayerManager.gameFixAPI_getMultiplayerGame()->playerDamaged(damagedPlayer, attackingPlayer, damage.damage, damage.meansofdeath);

            //damage.damage = multiplayerManager.playerDamaged(this, (Player*)damage.attacker, damage.damage, damage.meansofdeath);
            //finalKnockBack = (int)multiplayerManager.getModifiedKnockback(damagedPlayer, (Player*)damage.attacker, damage.knockback);

			finalKnockBack *= _COOP_SETTINGS_KNOCKBACK_PLAYER; //default knockback for player vs player
        }
        else {
            //adjust damage for NPC and SCRIPT attacks
            finalDamage = CoopManager::Get().getSkillBasedDamage(damage.damage);
            if (damage.inflictor && damage.inflictor->isSubclassOf(Projectile)) {
                finalKnockBack *= _COOP_SETTINGS_KNOCKBACK_PROJECTILE; //reduce knockback for NPC and SCRIPT attacks
            }
            else {
                finalKnockBack *= _COOP_SETTINGS_KNOCKBACK_NPC_OBJECTS;
            }
        }
    }

    damage.damage = finalDamage;
    damage.knockback = finalKnockBack;

    // Inform the award system that someone was damaged
    multiplayerManager.gameFixAPI_getMultiplayerAwardSystem()->playerDamaged(damagedPlayer, attackingPlayer, finalDamage, damage.meansofdeath);

    Sentient* dammagedSent = (Sentient*)damagedPlayer;
    dammagedSent->ArmorDamage(damage);

    if (multiplayerManager.inMultiplayer()) {
        float damageTaken;
        damageTaken = oldHealth - damagedPlayer->getHealth();
        if (damageTaken > 0.0f) {
            // Increase victim's action level
            if (damage.meansofdeath > MOD_LAST_SELF_INFLICTED) {
                damagedPlayer->IncreaseActionLevel(damageTaken);
            }

            if (attackingPlayer && damage.attacker->isSubclassOf(Player)) {
                // Tell the multiplayer system that the player took damage
                multiplayerManager.playerTookDamage(damagedPlayer, attackingPlayer, damageTaken, damage.meansofdeath);

                // Increase attacker's action level
                if (attackingPlayer && attackingPlayer != damagedPlayer) {
                    attackingPlayer->IncreaseActionLevel(damageTaken);
                }
            }
        }
    }
    return true;
}

void CoopManager::playerSharePickedUpAmmo(const Player* player, const str& itemName, const int& amount, int& amountUsed)
{
    if (!gameFixAPI_inMultiplayer() || !IsCoopEnabled() || !IsCoopLevel()) {
        return;
    }

    Player* coopPlayer = nullptr;
    for (int i = 0; i < gameFixAPI_maxClients(); i++) {
        coopPlayer = GetPlayer(i);

       
        if (!coopPlayer || gameFixAPI_isBot(coopPlayer) || gameFixAPI_isSpectator_stef2(coopPlayer)) {
            continue;
        }

        //skip the player who picked the ammo up
        if (coopPlayer == player) {
            continue;
        }

        amountUsed += coopPlayer->GiveAmmo(itemName, (int)amount, true);
    }
}

bool CoopManager::playerOtherThanTargetingEntity(const Player* player, const Entity* entityTarget)
{
    if (!player || !entityTarget || g_gametype->integer == GT_SINGLE_PLAYER) {
        return false;
    }

    Player* playerOther = nullptr;
    for (int i = 0; i < maxclients->integer; i++) {
        playerOther = gamefix_getPlayer(i);
        
        //nullptr, same player, dead, spec, bot or not targeting anything (world)
        if (!playerOther || playerOther->entnum == player->entnum || playerOther->getHealth() <= 0 || gameFixAPI_isSpectator_stef2(playerOther) || (playerOther->edict->svflags & SVF_BOT) || !playerOther->GetTargetedEntity()) {
            continue;
        }

        if (playerOther->GetTargetedEntity()->entnum == entityTarget->entnum) {
            return true;
        }
    }
    return false;
}

bool  CoopManager::playerHaveArchetypeEntityRespond(Player* player, Entity* entityTarget)
{
    if (!player || !entityTarget) {
        return false;
    }
    if (g_gametype->integer == GT_SINGLE_PLAYER) {
        return true;
    }

	float timePlayerTargeting = getPlayerData_entityTargetedSince(player, entityTarget);

    Player* playerOther = nullptr;
    for (int i = 0; i < maxclients->integer; i++) {
        playerOther = gamefix_getPlayer(i);

        //nullptr, same player, dead, spec, bot or not targeting anything (world)
        if (!playerOther || playerOther->entnum == player->entnum || playerOther->getHealth() <= 0 || gameFixAPI_isSpectator_stef2(playerOther) || (playerOther->edict->svflags & SVF_BOT) || !playerOther->GetTargetedEntity()) {
            continue;
        }

        if (playerOther->GetTargetedEntity()->entnum == entityTarget->entnum) {
            if (getPlayerData_entityTargetedSince(playerOther, playerOther->GetTargetedEntity()) > timePlayerTargeting) {
                return false;
            }
        }
    }
    return true;
}

bool CoopManager::playerGetDoingPuzzle(Player* player)
{
    if (!player) {
        return false;
	}
    return getPlayerData_coopTricorderPuzzleing(player);
}
void CoopManager::playerSetDoingPuzzle(Player* player, bool doingPuzzle)
{
    setPlayerData_coopTricorderPuzzleing(player, doingPuzzle);
}

bool CoopManager::sentientHandleStasis(Sentient* attacked, Entity* attacker)
{
    if (!attacked || !attacker) {
        return false;
    }

    if (attacked == attacker) {
        return false;
    }

    if (IsCoopEnabled()) {
        if (!attacker->isSubclassOf(Sentient)) {
            return true;
        }

        if (attacked->isSubclassOf(Player) && attacker->isSubclassOf(Player)) {
            Player* attackerPlayer = (Player*)attacker;
            Player* attackedPlayer = (Player*)attacked;
            Team* teamAttacker = multiplayerManager.getPlayersTeam(attackerPlayer);
            Team* teamAttacked = multiplayerManager.getPlayersTeam(attackedPlayer);

            //different teams
            if (teamAttacker && teamAttacked) {
                if (teamAttacker != teamAttacked) {
                    return true;
                }
                return true;
            }
            return true;
        }
        if (attacked->isSubclassOf(Actor) && attacker->isSubclassOf(Player)) {
            Actor *attackedActor = (Actor*)attacked;
            if (attackedActor->actortype == IS_TEAMMATE ||
                attackedActor->actortype == IS_CIVILIAN ||
                attackedActor->actortype == IS_FRIEND )
            {
                return false;
            }
        }
    }

    return true;
}

void CoopManager::flushTikis()
//fixing animation issues of actor and other models, also disappearing models
{
    if (!gameFixAPI_inMultiplayer()) {
        return;
    }

    if (gameFixAPI_isDedicatedServer()) {
        Engine_TIKI_FreeAll(1);//call to function pointer
        //gi.SendServerCommand(NULL, "stufftext flushtikis\n");
    }
    //listen servers - host + client in a singe game instance
    else {
        gi.SendServerCommand(0, "stufftext flushtikis\n");
    }
}

//used to minimize the usage of configstrings due to cl_parsegamestate issue
int CoopManager::configstringRemove(str sRem)
{
    if (!sRem.length()) {
        return 0;
    }

    int iRem = 0;
    char* s;
    for (int i = 1; i < MAX_CONFIGSTRINGS; i++) {
        s = gi.getConfigstring(i);
        str ss = "";
        ss += s;

        if (ss.length() > 0) {
            //if this is a dialog try to handle german and english localized strings as well
            if (!Q_stricmpn(ss.c_str(), "localization/", 13)) {
                //regular dialog
                if (Q_stricmp(ss.c_str(), sRem.c_str()) == 0) {
                    //DEBUG_LOG(va("#REMOVED CS: #%i: %s\n", i, ss.c_str()));
                    gi.setConfigstring(i, "");
                    iRem++;
                }

                //handle deu version of dialog
                char unlocal[96]; //MAX_QPATH + 5 <- did not work!
                memset(unlocal, 0, sizeof(unlocal));
                Q_strncpyz(unlocal, va("loc/deu/%s", sRem.c_str() + 13), sizeof(unlocal));
                if (Q_stricmp(ss.c_str(), unlocal) == 0) {
                    //DEBUG_LOG(va("#REMOVED CS: #%i: %s\n", i, ss.c_str()));
                    gi.setConfigstring(i, "");
                    iRem++;
                }

                //handle eng version of dialog
                memset(unlocal, 0, sizeof(unlocal));
                Q_strncpyz(unlocal, va("loc/eng/%s", sRem.c_str() + 13), sizeof(unlocal));
                if (Q_stricmp(ss.c_str(), unlocal) == 0) {
                    //DEBUG_LOG(va("#REMOVED CS: #%i: %s\n", i, ss.c_str()));
                    gi.setConfigstring(i, "");
                    iRem++;
                }
            }
            else {
                if (Q_stricmp(ss.c_str(), sRem.c_str()) == 0) {
                    //DEBUG_LOG(va("#REMOVED CS: #%i: %s\n", i, ss.c_str()));
                    gi.setConfigstring(i, "");
                    iRem++;
                }
            }
        }
    }
    return iRem;
}

//used to minimize the usage of configstrings due to cl_parsegamestate issue
void CoopManager::configstringCleanup()
{
    if (g_gametype->integer == GT_SINGLE_PLAYER || !IsCoopEnabled()) {
        return;
    }

    if (CoopManager_configstringList.NumObjects() < 1) {
        return;
    }

    for (int i = 1; i <= CoopManager_configstringList.NumObjects(); i++) {
        str sConfigstring = CoopManager_configstringList.ObjectAt(i);
        bool modelInuse = false;
        Player* player = nullptr;
        for (int j = 0; j < gameFixAPI_maxClients(); j++) {
            player = gamefix_getPlayer(j);

            if (!player) {
                continue;
            }
            if (Q_stricmp(sConfigstring.c_str(), player->model.c_str()) == 0) {
                modelInuse = true;
                break;
            }
        }

        if (modelInuse) {
            continue;
        }
        configstringRemove(sConfigstring);
    }
}

void CoopManager::loadClientIniData()
{
	CoopManager_clientIniData.FreeObjectList();

    str fileContents;
    if (!gamefix_getFileContents(_COOP_FILE_userlist, fileContents, true)) {
        return;
    }

    str sectionContents = gamefix_iniSectionGet(_COOP_FILE_userlist, fileContents, _COOP_USERLIST_CAT_session);
    if (!sectionContents.length()) {
        return;
    }

    str lineData = "";
	Container<str> tempLinesContainer;
    gamefix_listSeperatedItems(tempLinesContainer, sectionContents, "\n");

    for (int temp = 1; temp <= tempLinesContainer.NumObjects(); temp++) {
        lineData = tempLinesContainer.ObjectAt(temp);
        lineData = gamefix_trimWhitespace(lineData,true);
        if (!lineData.length()) {
            continue;
        }

        //grab value
        str lineKey;
        str lineValue;
        int valueStart = gamefix_findString(lineData.c_str(), "=");
        if (valueStart < 5) { //clientid has at least 5 chars
            continue;
        }

        lineKey = gamefix_trimWhitespace(lineData.substr(0, valueStart),true);
        lineValue = gamefix_getStringLength(lineData,valueStart + 1,999);
        lineValue = gamefix_trimWhitespace(lineValue,true);
        if (!lineKey.length() || !lineValue.length()) {
            continue;
        }

		//add data to container
        Container<str> tempValueContainer;
        gamefix_listSeperatedItems(tempValueContainer, lineValue, " ");
        
        coopManager_clientIniData_s clientIniData;
        clientIniData.clientId = lineKey;
        for (int i = 1; i <= tempValueContainer.NumObjects();i++) {
            switch (i)
            {
            case 1:
                clientIniData.health = atoi(tempValueContainer.ObjectAt(i));
				break;
            case 2:
                clientIniData.armor = atoi(tempValueContainer.ObjectAt(i));
				break;
            case 3:
                clientIniData.phaser = atoi(tempValueContainer.ObjectAt(i));
				break;
            case 4:
                clientIniData.plasma = atoi(tempValueContainer.ObjectAt(i));
				break;
            case 5:
                clientIniData.fed = atoi(tempValueContainer.ObjectAt(i));
				break;
            case 6:
                clientIniData.idryll = atoi(tempValueContainer.ObjectAt(i));
				break;
            case 7:
                clientIniData.deathTime = atoi(tempValueContainer.ObjectAt(i));
				break;
            case 8:
                clientIniData.mapName = tempValueContainer.ObjectAt(i);
				break;
            case 9:
                clientIniData.lmsDeaths = atoi(tempValueContainer.ObjectAt(i));
				break;
            default:
				gi.Error(ERR_FATAL, "CoopManager::loadClientIniData() - too many values in line '%s' in file '%s'", lineKey.c_str(), _COOP_FILE_userlist);
                break;
            }
        }
        CoopManager_clientIniData.AddObject(clientIniData);
    }
}


void CoopManager::saveClientIniData()
{
	str fileContents = "";
    if (!gamefix_getFileContents(_COOP_FILE_userlist, fileContents, true)) {
        gi.Printf("Info: %s not found, creating...\n", _COOP_FILE_userlist);
	}

    str sectionContents = ""; //va("", _COOP_USERLIST_CAT_session);
    coopManager_clientIniData_s clientIniData;
    for (int i = 1; i <= CoopManager_clientIniData.NumObjects(); i++) {
        clientIniData = CoopManager_clientIniData.ObjectAt(i);
        sectionContents += va("%s=%d %d %d %d %d %d %d %s %d\n",
        clientIniData.clientId.c_str(),
        clientIniData.health,
        clientIniData.armor,
        clientIniData.phaser,
        clientIniData.plasma,
        clientIniData.fed,
        clientIniData.idryll,
        clientIniData.deathTime,
        clientIniData.mapName.c_str(),
        clientIniData.lmsDeaths);
    }
    
    fileContents = gamefix_iniSectionSet(_COOP_FILE_userlist, fileContents, _COOP_USERLIST_CAT_session, sectionContents);
    if (!gamefix_setFileContents(_COOP_FILE_userlist, fileContents)) {
        gi.Printf("ERROR: Could not create file %s !\n", _COOP_FILE_userlist);
    }

    CoopManager_clientIniData.FreeObjectList();
}

//not yet in use
bool CoopManager::ShouldGiveSpawnItems() const {
    return giveSpawnItems;
}
bool CoopManager::IsFriendlyFireEnabled() const {
    return friendlyFire;
}
int CoopManager::GetPointsForEnemyKill() const {
    return enemyKillPoints;
}
int CoopManager::GetPenaltyForPlayerKill() const {
    return playerKillPenalty;
}



bool CoopManager::getPlayerData_coopAdmin(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopAdmin() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].coopAdmin;
}
void CoopManager::setPlayerData_coopAdmin(Player* player, bool state) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopAdmin() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopAdmin = state;
}
void CoopManager::setPlayerData_coopAdmin_reset(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopAdmin_reset() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopAdmin = false;
    coopManager_client_persistant_t[player->entnum].coopAdminAuthAttemps = 0;
    coopManager_client_persistant_t[player->entnum].coopAdminAuthString = "";
    coopManager_client_persistant_t[player->entnum].coopAdminAuthStarted = false;
    coopManager_client_persistant_t[player->entnum].coopAdminAuthStringLengthLast = 0; 
}

int CoopManager::getPlayerData_coopSetupTries(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopSetupTries() nullptr player");
        return 0;
    }
    return coopManager_client_persistant_t[player->entnum].coopSetupTries;
}
void CoopManager::setPlayerData_coopSetupTries_update(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopSetupTries_update() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopSetupTries++;
}
void CoopManager::setPlayerData_coopSetupTries_reset(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPayerData_coopSetupTries_reset() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopSetupTries = 0;
	coopManager_client_persistant_t[player->entnum].coopSetupNextCheckTime = -999.1f;
}

float CoopManager::getPlayerData_coopSetupNextCheckTime(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopSetupNextCheckTime() nullptr player");
        return 0;
    }
    return coopManager_client_persistant_t[player->entnum].coopSetupNextCheckTime;
}
void CoopManager::setPlayerData_coopSetupNextCheckTime_update(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopSetupNextCheckTime_update() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopSetupNextCheckTime = (level.time + 0.15f);
}
void CoopManager::setPlayerData_coopSetupNextCheckTime_reset(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPayerData_coopSetupNextCheckTime_reset() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopSetupNextCheckTime = -999.0f;
}

int CoopManager::getPlayerData_coopAdminAuthAttemps(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopAdminAuthAttemps() nullptr player");
        return 0;
    }
    return coopManager_client_persistant_t[player->entnum].coopAdminAuthAttemps;
}
void CoopManager::setPayerData_coopAdminAuthAttemps_update(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPayerData_coopAdminAuthAttemps_update() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopAdminAuthAttemps++;
}
void CoopManager::setPayerData_coopAdminAuthAttemps_reset(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPayerData_coopAdminAuthAttemps_update() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopAdminAuthAttemps = 0;
}
bool CoopManager::getPlayerData_coopAdminAuthStarted(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopAdminAuthStarted() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].coopAdminAuthStarted;
}
void CoopManager::setPayerData_coopAdminAuthStarted(Player* player, bool started) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPayerData_coopAdminAuthStarted() nullptr player");
        return;
    }

    coopManager_client_persistant_t[player->entnum].coopAdminAuthStarted = started;
}
str CoopManager::getPlayerData_coopAdminAuthString(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopAdminAuthString() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].coopAdminAuthString;
}
void CoopManager::setPlayerData_coopAdminAuthString(Player* player, str newText) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopAdminAuthString() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopAdminAuthString = newText;
}
bool CoopManager::getPlayerData_coopAdminAuthString_changed(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopAdminAuthString_changed() nullptr player");
        return false;
    }
    if (getPlayerData_coopAdminAuthStringLengthLast(player) != (int)strlen(getPlayerData_coopAdminAuthString(player))) {
        int lastAuthStringLength = getPlayerData_coopAdminAuthStringLengthLast(player);
        lastAuthStringLength++;
        setPlayerData_coopAdminAuthStringLengthLast(player,lastAuthStringLength);
        return true;
    }
    return false;
}
int CoopManager::getPlayerData_coopAdminAuthStringLengthLast(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopAdminAuthStringLengthLast() nullptr player");
        return 0;
    }
    return coopManager_client_persistant_t[player->entnum].coopAdminAuthStringLengthLast;
}
void CoopManager::setPlayerData_coopAdminAuthStringLengthLast(Player* player, int strLength) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopAdminAuthStringLengthLast() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopAdminAuthStringLengthLast = strLength;
}

Vector CoopManager::getPlayerData_radarBlipLastPos(Player* player, short int blipNum)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_radarBlipLastPos() nullptr player");
        return Vector(0.0f, 0.0f, 0.0f);
    }
    if (blipNum < 0 || blipNum >= _COOP_SETTINGS_RADAR_BLIPS_MAX) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_radarBlipLastPos() blipNum out of range");
        return Vector(0.0f, 0.0f, 0.0f);
    }
    return coopManager_client_persistant_t[player->entnum].radarBlipPositionLast[blipNum];
}
void CoopManager::setPlayerData_radarBlipLastPos(Player* player,short int blipNum,Vector blipLastPos)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_radarBlipLastPos() nullptr player");
        return;
    }
    if (blipNum < 0 || blipNum >= _COOP_SETTINGS_RADAR_BLIPS_MAX) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_radarBlipLastPos() blipNum out of range");
        return;
    }
    coopManager_client_persistant_t[player->entnum].radarBlipPositionLast[blipNum] = blipLastPos;
}

bool CoopManager::getPlayerData_radarBlipActive(Player* player, short int blipNum)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_radarBlipActive() nullptr player");
        return false;
    }
    if (blipNum < 0 || blipNum >= _COOP_SETTINGS_RADAR_BLIPS_MAX) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_radarBlipActive() blipNum out of range");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].radarBlipActive[blipNum];
}
void CoopManager::setPlayerData_radarBlipActive(Player* player,short int blipNum,bool blipActive)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_radarBlipActive() nullptr player");
        return;
    }
    if (blipNum < 0 || blipNum >= _COOP_SETTINGS_RADAR_BLIPS_MAX) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_radarBlipActive() blipNum out of range");
        return;
    }
    coopManager_client_persistant_t[player->entnum].radarBlipActive[blipNum] = blipActive;
}

bool CoopManager::getPlayerData_radarSelectedActive(Player* player)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_radarSelectedActive() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].radarSelectedActive;
}
void CoopManager::setPlayerData_radarSelectedActive(Player* player,bool selectedActive)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_radarSelectedActive() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].radarSelectedActive = selectedActive;
}

float CoopManager::getPlayerData_radarUpdatedLast(Player* player)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_radarUpdatedLast() nullptr player");
        return 99999.0f;
    }
    return coopManager_client_persistant_t[player->entnum].radarUpdateTimeLast;
}
void CoopManager::setPlayerData_radarUpdatedLast(Player* player,float lastUpdate)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_radarUpdatedLast() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].radarUpdateTimeLast = lastUpdate;
}

float CoopManager::getPlayerData_radarAngleLast(Player* player)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_radarAngleLast() nullptr player");
        return 99999.0f;
    }
    return coopManager_client_persistant_t[player->entnum].radarAngleLast;
}
void CoopManager::setPlayerData_radarAngleLast(Player* player,float lastAngle)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_radarAngleLast() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].radarAngleLast = lastAngle;
}
int CoopManager::getPlayerData_radarScale(Player* player)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_radarScale() nullptr player");
        return 1;
    }
    return coopManager_client_persistant_t[player->entnum].radarScale;
}
void CoopManager::setPlayerData_radarScale(Player* player, int radarScale)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_radarScale() nullptr player");
        return;
    }
    if (radarScale < _COOP_SETTINGS_RADAR_SCALE_MIN) { radarScale = _COOP_SETTINGS_RADAR_SCALE_MIN; }
    if (radarScale > _COOP_SETTINGS_RADAR_SCALE_MAX) { radarScale = _COOP_SETTINGS_RADAR_SCALE_MAX; }
    coopManager_client_persistant_t[player->entnum].radarScale = radarScale;
}


float CoopManager::getPlayerData_cinematicEscapePressLastTime(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_cinematicEscapePressLastTime() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].cinematicEscapePressLastTime;
}
void CoopManager::setPlayerData_cinematicEscapePressLastTime(Player* player, float lastTime) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_cinematicEscapePressLastTime() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].cinematicEscapePressLastTime = lastTime;
}

bool CoopManager::getPlayerData_coopUpdateNoticeSend(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopUpdateNoticeSend() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].coopUpdateNoticeSend;
}
void CoopManager::setPlayerData_coopUpdateNoticeSend(Player* player, bool state) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopUpdateNoticeSend() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopUpdateNoticeSend = state;
}


bool CoopManager::getPlayerData_coopClientIdDone(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopClientIdDone() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].coopClientIdDone;
}
void CoopManager::setPlayerData_coopClientIdDone(Player* player, bool state) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopClientIdDone() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopClientIdDone = state;
}
str CoopManager::getPlayerData_coopClientId(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopClientId() nullptr player");
        return "";
    }
    return coopManager_client_persistant_t[player->entnum].coopClientId;
}
void CoopManager::setPlayerData_coopClientId(Player* player, str coopClientId) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopClientId() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopClientId = coopClientId;
}
bool CoopManager::getPlayerData_coopSetupDone(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopSetupDone() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].coopSetupDone;
}
void CoopManager::setPlayerData_coopSetupDone(Player* player, bool state) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopSetupDone() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopSetupDone = state;
}

bool CoopManager::getPlayerData_coopSetupUiSpawnedDone(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopSetupUiDone() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].coopSetupUiSpawnedDone;
}
void CoopManager::setPlayerData_coopSetupUiSpawnedDone(Player* player, bool state) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopSetupUiDone() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopSetupUiSpawnedDone = state;
}

bool CoopManager::getPlayerData_coopSetupStarted(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopSetupStarted() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].coopSetupStarted;
}
void CoopManager::setPlayerData_coopSetupStarted(Player* player, bool state) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopSetupStarted() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopSetupStarted = state;
}

str CoopManager::getPlayerData_coopClass(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopClass() nullptr player");
        return "";
    }
    return coopManager_client_persistant_t[player->entnum].coopClass;
}
void CoopManager::setPlayerData_coopClass(Player* player, str className) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopClass() nullptr player");
        return;
    }

    //no known class matches, force technician
    if (Q_stricmp(className,_COOP_NAME_CLASS_heavyWeapons)  != 0 && 
        Q_stricmp(className, _COOP_NAME_CLASS_medic)        != 0 && 
        Q_stricmp(className, _COOP_NAME_CLASS_technician)   != 0 ){
        className = _COOP_NAME_CLASS_technician;
    }

    coopManager_client_persistant_t[player->entnum].coopClass = className;
}

int CoopManager::getPlayerData_coopClassRegenerationCycles(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopClassRegenerationCycles() nullptr player");
        return 9999;
    }
    return coopManager_client_persistant_t[player->entnum].coopClassRegenerationCycles;
}
void CoopManager::setPlayerData_coopClassRegenerationCycles(Player* player, int cycles) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopClassRegenerationCycles() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopClassRegenerationCycles = cycles;
}
void CoopManager::setPlayerData_coopClassRegenerationCycles_update(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopClassRegenerationCycles_update() nullptr player");
        return;
    }
	int cycles = coopManager_client_persistant_t[player->entnum].coopClassRegenerationCycles;
    
    if (cycles > 0) {
        cycles--;
    }
    coopManager_client_persistant_t[player->entnum].coopClassRegenerationCycles = cycles;
}

float CoopManager::getPlayerData_coopClassLastTimeChanged(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopClassLastTimeChanged() nullptr player");
        return 9999.9f;
    }
    return coopManager_client_persistant_t[player->entnum].coopClassLastTimeChanged;
}
void CoopManager::setPlayerData_coopClassLastTimeChanged(Player* player, float lastTime) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopClassLastTimeChanged() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopClassLastTimeChanged = lastTime;
}
void CoopManager::setPlayerData_coopClassLastTimeChanged_update(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopClassLastTimeChanged_update() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopClassLastTimeChanged = level.time;
}
float CoopManager::getPlayerData_coopClassLastTimeApplied(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopClassLastTimeApplied() nullptr player");
        return 9999.9f;
    }
    return coopManager_client_persistant_t[player->entnum].coopClassLastTimeApplied;
}
void CoopManager::setPlayerData_coopClassLastTimeApplied(Player* player, float lastTime) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopClassLastTimeApplied() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopClassLastTimeApplied = lastTime;
}
float CoopManager::getPlayerData_coopClasslastTimeUpdatedStat(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopClasslastTimeUpdatedStat() nullptr player");
        return 9999.9f;
    }
    return coopManager_client_persistant_t[player->entnum].coopClasslastTimeUpdatedStat;
}
void CoopManager::setPlayerData_coopClasslastTimeUpdatedStat(Player* player, float lastTime) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopClasslastTimeUpdatedStat() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopClasslastTimeUpdatedStat = lastTime;
}


float CoopManager::getPlayerData_revivedStepLasttime(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_revivedStepLasttime() nullptr player");
        return 9999.9f;
    }
    return coopManager_client_persistant_t[player->entnum].revivedStepLasttime;
}
void CoopManager::setPlayerData_revivedStepLasttime(Player* player, float lastTime) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_revivedStepLasttime() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].revivedStepLasttime = lastTime;
}
void CoopManager::setPlayerData_revivedStepLasttime_update(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_revivedStepLasttime_update() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].revivedStepLasttime = level.time;
}
int CoopManager::getPlayerData_revivedStepCounter(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_revivedStepCounter() nullptr player");
        return 0;
    }
    return coopManager_client_persistant_t[player->entnum].revivedStepCounter;
}
void CoopManager::setPlayerData_revivedStepCounter(Player* player, int count) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_revivedStepCounter() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].revivedStepCounter = count;
}
void CoopManager::setPlayerData_revivedStepCounter_update(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_revivedStepCounter_update() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].revivedStepCounter++;
}
float CoopManager::getPlayerData_coopClassLastTimeUsedMsg(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopClassLastTimeUsedMsg() nullptr player");
        return 0.0f;
    }
    return coopManager_client_persistant_t[player->entnum].coopClassLastTimeUsedMsg;
}
void CoopManager::setPlayerData_coopClassLastTimeUsedMsg(Player* player, float last) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopClassLastTimeUsedMsg() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopClassLastTimeUsedMsg = last;
}
void CoopManager::setPlayerData_coopClassLastTimeUsedMsg_update(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopClassLastTimeUsedMsg_update() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopClassLastTimeUsedMsg = level.time;
}


float CoopManager::getPlayerData_usingStepLasttime(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_usingStepLasttime() nullptr player");
        return 0.0f;
    }
    return coopManager_client_persistant_t[player->entnum].usingStepLasttime;
}
void CoopManager::setPlayerData_usingStepLasttime(Player* player, float last) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_usingStepLasttime() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].revivedStepCounter = last;
}
void CoopManager::setPlayerData_usingStepLasttime_update(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_usingStepLasttime_update() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].usingStepLasttime = level.time;
}



int CoopManager::getPlayerData_coopVersion(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopVersion() nullptr player");
        return 0;
    }
    return coopManager_client_persistant_t[player->entnum].coopVersion;
}
void CoopManager::setPlayerData_coopVersion(Player* player, int coopVersion) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopVersion() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopVersion = coopVersion;
}

bool CoopManager::getPlayerData_respawnMe(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_respawnMe() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].respawnMe;
}
void CoopManager::setPlayerData_respawnMe(Player* player, bool respawn) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_respawnMe() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].respawnMe = respawn;
}

bool CoopManager::getPlayerData_spawnLocationSpawnForced(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_spawnLocationSpawnForced() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].spawnLocationSpawnForced;
}
void CoopManager::setPlayerData_spawnLocationSpawnForced(Player* player, bool forced) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_spawnLocationSpawnForced() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].spawnLocationSpawnForced = forced;
}

bool CoopManager::getPlayerData_respawnLocationSpawn(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_respawnLocationSpawn() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].respawnLocationSpawn;
}
void CoopManager::setPlayerData_respawnLocationSpawn(Player* player, bool forced) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_respawnLocationSpawn() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].respawnLocationSpawn = forced;
}

Vector CoopManager::getPlayerData_lastValidLocation(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_lastValidLocation() nullptr player");
        return Vector(0.0f, 0.0f, 0.0f);
    }
    return coopManager_client_persistant_t[player->entnum].lastValidLocation;
}
void CoopManager::setPlayerData_lastValidLocation(Player* player, Vector location) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_lastValidLocation() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].lastValidLocation = location;
}

Vector CoopManager::getPlayerData_lastValidViewAngle(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_lastValidViewAngle() nullptr player");
        return Vector(0.0f, 0.0f, 0.0f);
    }
    return coopManager_client_persistant_t[player->entnum].lastValidViewAngle;
}
void CoopManager::setPlayerData_lastValidViewAngle(Player* player, Vector viewAngle) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_lastValidViewAngle() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].lastValidViewAngle = viewAngle;
}

float CoopManager::getPlayerData_lastSpawned(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_lastSpawned() nullptr player");
        return 0.0f;
    }
    return coopManager_client_persistant_t[player->entnum].lastSpawned;
}
void CoopManager::setPlayerData_lastSpawned(Player* player, float lastSpawned) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_lastSpawned() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].lastSpawned = lastSpawned;
}

int CoopManager::getPlayerData_objectiveCycle(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_objectiveCycle() nullptr player");
        return 0;
    }
    return coopManager_client_persistant_t[player->entnum].objectiveCycle;
}
void CoopManager::setPlayerData_objectiveCycle(Player* player, int objectiveCycle) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_objectiveCycle() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].objectiveCycle = objectiveCycle;
}

int CoopManager::getPlayerData_coopObjectiveStatus(Player* player, int item) {
    if (item < 0 || item >= _COOP_SETTINGS_OBJECTIVES_MAX) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopObjectiveStatus() index out of range");
        return -1;
    }
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopObjectiveStatus() nullptr player");
        return -1;
    }
    return coopManager_client_persistant_t[player->entnum].objectiveItemStatus[item];
}
void CoopManager::setPlayerData_coopObjectiveStatus(Player* player, int item, int status) {
    if (item < 0 || item >= _COOP_SETTINGS_OBJECTIVES_MAX) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopObjectiveStatus() index out of range");
        return;
    }
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopObjectiveStatus() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].objectiveItemStatus[item] = status;
}

int CoopManager::getPlayerData_coopObjectiveSend(Player* player, int item) {
    if (item < 0 || item >= _COOP_SETTINGS_OBJECTIVES_MAX) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopObjectiveSend() index out of range");
        return 0;
    }
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopObjectiveSend() nullptr player");
        return 0;
    }
    return coopManager_client_persistant_t[player->entnum].objectiveItemSend[item];
}
void CoopManager::setPlayerData_coopObjectiveSend(Player* player, int item, int status) {
    if (item < 0 || item >= _COOP_SETTINGS_OBJECTIVES_MAX) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopObjectiveSend() index out of range");
        return;
    }
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopObjectiveSend() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].objectiveItemSend[item] = status;
}

int CoopManager::getPlayerData_coopObjectiveShown(Player* player, int item) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopObjectiveShown() nullptr player");
        return 0;
    }
    if (item < 0 || item >= _COOP_SETTINGS_OBJECTIVES_MAX) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopObjectiveShown() index out of range");
        return 0;
    }
    return coopManager_client_persistant_t[player->entnum].objectiveItemShown[item];
}
void CoopManager::setPlayerData_coopObjectiveShown(Player* player, int item, int status) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopObjectiveShown() nullptr player");
        return;
    }
    if (item < 0 || item > _COOP_SETTINGS_OBJECTIVES_MAX) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopObjectiveShown() index out of range");
        return;
    }
    coopManager_client_persistant_t[player->entnum].objectiveItemShown[item] = status;
}

float CoopManager::getPlayerData_objectiveItemCompletedAt(Player* player, int item) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_objectiveItemCompletedAt() nullptr player");
        return 0.0f;
    }
    if (item < 0 || item > _COOP_SETTINGS_OBJECTIVES_MAX) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_objectiveItemCompletedAt() index out of range");
        return 0.0f;
    }
    return coopManager_client_persistant_t[player->entnum].objectiveItemCompletedAt[item];
}
void CoopManager::setPlayerData_objectiveItemCompletedAt(Player* player, int item, float status) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_objectiveItemCompletedAt() nullptr player");
        return;
    }
    if (item < 0 || item > _COOP_SETTINGS_OBJECTIVES_MAX) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_objectiveItemCompletedAt() index out of range");
        return;
    }
    coopManager_client_persistant_t[player->entnum].objectiveItemCompletedAt[item] = status;
}

float CoopManager::getPlayerData_objectiveItemLastTimePrintedTitleAt(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_objectiveItemLastTimePrintedTitleAt() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].objectiveItemPrintedTitleLastTime;
}
void CoopManager::setPlayerData_objectiveItemLastTimePrintedTitleAt(Player* player, float lastPrintedTitleAt) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_objectiveItemLastTimePrintedTitleAt() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].objectiveItemPrintedTitleLastTime = lastPrintedTitleAt;
}

str CoopManager::getPlayerData_objectiveItemLastTimePrintedTitle(Player* player) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_objectiveItemLastTimePrintedTitle() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].objectiveItemPrintedTitleLast;
}
void CoopManager::setPlayerData_objectiveItemLastTimePrintedTitle(Player* player, str lastPrintedTitle) {
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_objectiveItemLastTimePrintedTitle() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].objectiveItemPrintedTitleLast = lastPrintedTitle;
}

void CoopManager::setPlayerData_objectives_reset(Player* player)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPLayerData_objectives_reset() nullptr player");
        return;
    }

    coopManager_client_persistant_t[player->entnum].objectiveItemPrintedTitleLast = "";
    coopManager_client_persistant_t[player->entnum].objectiveItemPrintedTitleLastTime = 0.0f;
    coopManager_client_persistant_t[player->entnum].objectiveCycle = 0;
    coopManager_client_persistant_t[player->entnum].objectiveSetupDone = false;

    for (int i = 0; i < _COOP_SETTINGS_OBJECTIVES_MAX; i++) {
        setPlayerData_coopObjectiveStatus(player, i, -1);
        setPlayerData_coopObjectiveSend(player, i, 0);
        setPlayerData_coopObjectiveShown(player, i, 0);
        setPlayerData_objectiveItemCompletedAt(player, i, 0.0f);
    }
}

bool CoopManager::getPlayerData_objectives_setupDone(Player* player)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPLayerData_objectives_setupDone() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].objectiveSetupDone;
}
void CoopManager::setPlayerData_objectives_setupDone(Player* player)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPLayerData_objectives_setupDone() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].objectiveSetupDone = true;
}

bool CoopManager::getPlayerData_targetedShow(Player* player)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_targetedShow() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].targetedShow;
}
void CoopManager::setPlayerData_targetedShow(Player* player, bool status)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_targetedShow() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].targetedShow = status;
}

bool CoopManager::getPlayerData_coopClassLocked(Player* player)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_coopClassLocked() nullptr player");
        return false;
    }
    return coopManager_client_persistant_t[player->entnum].coopClassLocked;
}
void CoopManager::setPlayerData_coopClassLocked(Player* player, bool status)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_coopClassLocked() nullptr player");
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopClassLocked = status;
}

void CoopManager::setPlayerData_communicatorSendNames_reset(Player* player)
{
    if (!player) {
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_communicatorSendNames_reset() nullptr player");
        return;
    }
    
    for (int i = 0; i < _COOP_SETTINGS_PLAYER_SUPPORT_MAX; i++) {
        coopManager_client_persistant_t[player->entnum].communicatorSendNames[i] = "";
    }
}

void CoopManager::setPlayerData_disconnecting(int clientNum, bool status)
{
    if (clientNum < 0 || clientNum >= MAX_CLIENTS) { //needs to use MAX_CLIENTS because connecting clients might exceed the dynamic max clients
        gi.Error(ERR_FATAL, "CoopManager::setPlayerData_disconnecting() clientNum out of range");
        return;
    }
    coopManager_client_persistant_t[clientNum].disconnecting = status;
}

bool CoopManager::getPlayerData_disconnecting(int clientNum)
{
    if (clientNum < 0 || clientNum >= MAX_CLIENTS) { //needs to use MAX_CLIENTS because connecting clients might exceed the dynamic max clients
        gi.Error(ERR_FATAL, "CoopManager::getPlayerData_disconnecting() clientNum out of range");
        return true;
    }
    return coopManager_client_persistant_t[clientNum].disconnecting;
}

void CoopManager::setPlayerData_entityTargetedSince_reset(Player* player)
{
    if (!player) {
        return;
    }
    coopManager_client_persistant_t[player->entnum].entityTargetedSince = 0.0f;
    coopManager_client_persistant_t[player->entnum].entityTargetedLast = nullptr;
}

void CoopManager::setPlayerData_entityTargetedSince(Player* player, Entity* lastTarget)
{
    if (!player) {
        return;
    }
    coopManager_client_persistant_t[player->entnum].entityTargetedSince = level.time;

    if (lastTarget) {
        coopManager_client_persistant_t[player->entnum].entityTargetedLast = lastTarget;
    }
    else {
        coopManager_client_persistant_t[player->entnum].entityTargetedLast = nullptr;
    }
}

float CoopManager::getPlayerData_entityTargetedSince(Player* player, Entity *lastTarget)
{
    if (!player) {
        return 0.0f;
    }

    if (lastTarget) {
        if (lastTarget == coopManager_client_persistant_t[player->entnum].entityTargetedLast) {
            return coopManager_client_persistant_t[player->entnum].entityTargetedSince;
        }
    }
    return 0.0f;
}

void CoopManager::setPlayerData_coopTricorderScanHudOn(Player* player, bool on)
{
    if (!player) {
        return;
    }
    coopManager_client_persistant_t[player->entnum].coopTricorderScanHudOn = on;
}

bool CoopManager::getPlayerData_coopTricorderScanHudOn(Player* player)
{
    if (!player) {
        return false;
    }

    return coopManager_client_persistant_t[player->entnum].coopTricorderScanHudOn;
}

void CoopManager::setPlayerData_coopTricorderScanData_reset(Player* player)
{
    coopManager_client_persistant_t[player->entnum].coopTricorderScanData1 = "";
    coopManager_client_persistant_t[player->entnum].coopTricorderScanData2 = "";
    coopManager_client_persistant_t[player->entnum].coopTricorderScanData2 = "";
    coopManager_client_persistant_t[player->entnum].coopTricorderScanLastSend = 0.0f;
    coopManager_client_persistant_t[player->entnum].coopTricorderScanHudOn = false;
}

void CoopManager::setPlayerData_coopTricorderScanData(Player* player, short int dataIndex, str scanData)
{
    if (!player) {
        return;
    }
    if (dataIndex < 1 || dataIndex > 3) {
        gi.Error(ERR_DROP, "CoopManager::setPlayerData_coopTricorderScanData() dataIndex out of range\n");
    }
    if (dataIndex == 1) {
       coopManager_client_persistant_t[player->entnum].coopTricorderScanData1 = scanData;
    }
    if (dataIndex == 2) {
       coopManager_client_persistant_t[player->entnum].coopTricorderScanData2 = scanData;
    }
    if (dataIndex == 3) {
       coopManager_client_persistant_t[player->entnum].coopTricorderScanData2 = scanData;
    }
}

str CoopManager::getPlayerData_coopTricorderScanData(Player* player, short int dataIndex)
{
    if (player) {
        if (dataIndex < 1 || dataIndex > 3) {
            gi.Error(ERR_DROP, "CoopManager::getPlayerData_coopTricorderScanData() dataIndex out of range\n");
        }
        if (dataIndex == 1) {
            return coopManager_client_persistant_t[player->entnum].coopTricorderScanData1;
        }
        if (dataIndex == 2) {
            return coopManager_client_persistant_t[player->entnum].coopTricorderScanData2;
        }
        if (dataIndex == 3) {
            return coopManager_client_persistant_t[player->entnum].coopTricorderScanData2;
        }
    }
    return "";
}

void CoopManager::setPlayerData_coopTricorderPuzzleing(Player* player, bool doingAPuzzle)
{
    if (!player) {
        return;
    }
    player->entityVars.SetVariable("_playerIsModulatingPuzzle", (float)(int)doingAPuzzle);
	coopManager_client_persistant_t[player->entnum].coopTricorderPuzzleing = doingAPuzzle;
}

bool CoopManager::getPlayerData_coopTricorderPuzzleing(Player* player)
{
    if (!player) {
        return false;
    }

    if (coopManager_client_persistant_t[player->entnum].coopTricorderPuzzleing) {
		return true;
    }
    return (bool)(int)gamefix_getEntityVarFloat(player, "_playerIsModulatingPuzzle");
}


#endif