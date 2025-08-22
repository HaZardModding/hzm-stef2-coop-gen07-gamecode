// CONTAINING PLAYER CLASS(Medic,Technician,HeavyWeapons) HANDLING RELATED FUNCTIONS FOR THE HZM CO-OP MOD

#ifdef ENABLE_COOP

#include "../../dlls/game/_pch_cpp.h"
//#include "../../dlls/game/mp_manager.hpp"
//#include "../../dlls/game/mp_modeBase.hpp"
#include "../../dlls/game/gamefix.hpp"
#include <qcommon/gameplaymanager.h>
#include "coop_class.hpp"
#include "coop_config.hpp"
#include "coop_generalstrings.hpp"
#include "coop_manager.hpp"


CoopClass coopClass;

void CoopClass::playerKilled(Player *player)
{
	player->entityVars.SetVariable("!ability", (level.time - COOP_CLASS_REGENERATION_COOLDOWN) - 1);
	player->entityVars.SetVariable("!abilityPrintout", (level.time + 3));
	CoopManager::Get().setPlayerData_coopClassRegenerationCycles(player, 0);

	player->CancelEventsOfType(EV_Player_coop_classAbilityHudRecover);
}
void CoopClass::coop_classAbilityRecoverHud(Player *player)
{
	Event* recover25 = new Event(EV_Player_coop_classAbilityHudRecover);
	recover25->AddString("coop_class25");
	player->PostEvent(recover25, (COOP_CLASS_REGENERATION_COOLDOWN / 4) * 1);

	Event* recover50 = new Event(EV_Player_coop_classAbilityHudRecover);
	recover50->AddString("coop_class50");
	player->PostEvent(recover50, (COOP_CLASS_REGENERATION_COOLDOWN / 4) * 2);

	Event* recover75 = new Event(EV_Player_coop_classAbilityHudRecover);
	recover75->AddString("coop_class75");
	player->PostEvent(recover75, (COOP_CLASS_REGENERATION_COOLDOWN / 4) * 3);

	Event* recover100 = new Event(EV_Player_coop_classAbilityHudRecover);
	recover100->AddString("coop_class100");
	player->PostEvent(recover100, COOP_CLASS_REGENERATION_COOLDOWN - 0.05); // make sure the hud reappaers just a lil bit earlier so it won't overlap
}

void CoopClass::coop_classAbilityUse(Player *player)
{
	if (!CoopManager::Get().IsCoopEnabled() || !CoopManager::Get().IsCoopLevel()) {
		return;
	}

	CoopManager::Get().setPlayerData_coopClassRegenerationCycles(player, COOP_CLASS_REGENERATION_CYCLES);

	//hide the ability status bars
	gamefix_playerDelayedServerCommand(player->entnum, "exec co-op/cfg/coop_classResetA.cfg");
	//make the status bars reappear over time
	coop_classAbilityRecoverHud(player);
}

int CoopClass::coop_classRegenerationCycleGet(Player *player)
{
	return CoopManager::Get().getPlayerData_coopClassRegenerationCycles(player);
}

void CoopClass::coop_classCheckApplay( Player *player )
{
	if (CoopManager::Get().getPlayerData_coopClassLastTimeChanged(player) != CoopManager::Get().getPlayerData_coopClassLastTimeApplied(player)){
		coop_classApplayAttributes( player , false );
		CoopManager::Get().setPlayerData_coopClassLastTimeApplied(player, CoopManager::Get().getPlayerData_coopClassLastTimeChanged(player));
	}
}

void CoopClass::coop_classCheckUpdateStat( Player *player )
{
	if (!multiplayerManager.inMultiplayer()) {
		return;
	}
	if (!CoopManager::Get().getPlayerData_coopSetupDone(player)) {
		return;
	}
	if (!CoopManager::Get().getPlayerData_coopVersion(player)) {
		return;
	}
	if (coopClass.lastUpdateSendAt > CoopManager::Get().getPlayerData_coopClasslastTimeUpdatedStat(player)) {
		CoopManager::Get().setPlayerData_coopClasslastTimeUpdatedStat(player,coopClass.lastUpdateSendAt);
		gamefix_playerDelayedServerCommand( player->entnum , va( "set coop_ch %i;set coop_ct %i;set coop_cm %i\n" ,coop_classPlayersOfClass( "HeavyWeapon" ) ,coop_classPlayersOfClass("Technician") , coop_classPlayersOfClass("Medic")));
		//DEBUG_LOG("# coop_classCheckUpdateStat sending to %s\n", player->client->pers.netname);
	}	
}

void CoopClass::coop_classRegenerate( Player *player )
{
	//do not regenerate if dead, neutralized or under fire
	if (	player->health <= 0 ||
			!multiplayerManager.inMultiplayer() ||
			!CoopManager::Get().IsCoopEnabled() ||
			!CoopManager::Get().IsCoopLevel()
			)
	{
		return;
	}

	//[b60017] chrissstrahl - changed regeneration that a player gets some ammount of regeneration cycles for him self after player gave something to other player class
	if (CoopManager::Get().getPlayerData_coopClassRegenerationCycles(player)) {
		CoopManager::Get().setPlayerData_coopClassRegenerationCycles_update(player);

		Entity* eOther = nullptr;
		for (int i = 0; i < maxclients->integer; i++) {
			eOther = (Player*)g_entities[i].entity;
			if (!eOther || !eOther->isSubclassOf(Player)) {
				continue;
			}

			Player* other = (Player*)eOther;

			//criteria that prevent player getting regenerated
			if (gameFixAPI_isBot(other) || other->getHealth() < 1 || gameFixAPI_isBot(other) || gameFixAPI_isSpectator_stef2(other)) {
				continue;
			}

			//medic
			if (!Q_stricmp(CoopManager::Get().getPlayerData_coopClass(player).c_str(), COOP_CLASS_NAME_MEDIC)) {
				other->AddHealth(COOP_CLASS_REGENERATE_HEALTH);
			}
			//technician
			else if (!Q_stricmp(CoopManager::Get().getPlayerData_coopClass(player).c_str(), COOP_CLASS_NAME_TECHNICIAN)) {
				float fArmorCurrent = other->GetArmorValue();
				if ((fArmorCurrent + COOP_CLASS_REGENERATE_ARMOR) <= _COOP_SETTINGS_PLAYER_ITEM_ARMOR_MAX) {
					fArmorCurrent++;
					Event* armorEvent;
					armorEvent = new Event(EV_Sentient_GiveArmor);
					armorEvent->AddString("BasicArmor");
					armorEvent->AddInteger(COOP_CLASS_REGENERATE_ARMOR);
					other->ProcessEvent(armorEvent);
				}
			}
			//heavy weapons
			else if (!Q_stricmp(CoopManager::Get().getPlayerData_coopClass(player).c_str(), COOP_CLASS_NAME_HEAVYWEAPONS)) {
				other->GiveAmmo("Fed", COOP_CLASS_REGENERATE_AMMO, false, _COOP_CLASS_HW_AMMO_FED_MAX);
				other->GiveAmmo("Plasma", COOP_CLASS_REGENERATE_AMMO, false, _COOP_CLASS_HW_AMMO_PLASMA_MAX);
				other->GiveAmmo("Idryll", COOP_CLASS_REGENERATE_AMMO, false, _COOP_CLASS_HW_AMMO_IDRYLL_MAX);
			}
		}
	}
}

void CoopClass::coop_classSet( Player *player , str classToSet )
{
	if (!player || !gameFixAPI_inMultiplayer() || !CoopManager::Get().IsCoopEnabled() || !CoopManager::Get().IsCoopLevel()) {
		return;
	}
	if ( !Q_stricmp( "current", classToSet.c_str()) || !Q_stricmp( "" , classToSet.c_str()) ){
		classToSet = CoopManager::Get().getPlayerData_coopClass(player);
	}else{
		CoopManager::Get().setPlayerData_coopClassLastTimeChanged_update(player);
	}

	if (Q_stricmpn("h", classToSet.c_str(), 1) == 0) {
		classToSet = COOP_CLASS_NAME_HEAVYWEAPONS;
	}
	else if (Q_stricmpn("m", classToSet.c_str(), 1) == 0) {
		classToSet = COOP_CLASS_NAME_MEDIC;
	}
	else {
		classToSet = COOP_CLASS_NAME_TECHNICIAN;
	}

	CoopManager::Get().setPlayerData_coopClass(player, classToSet);

	gi.Printf(va("%s: %s\n", classToSet.c_str(), player->client->pers.netname));
}

void CoopClass::coop_classApplayAttributes( Player *player , bool changeOnly )
{
	if (	!player ||
			!multiplayerManager.inMultiplayer() ||
			multiplayerManager.isPlayerSpectator( player ) ||
			level.time < mp_warmUpTime->integer ||
			!CoopManager::Get().getPlayerData_coopSetupDone(player))
	{
		return;
	}
	str currentClass = CoopManager::Get().getPlayerData_coopClass(player);
	if (currentClass.length()==0) {
		DEBUG_LOG("coop_classApplayAttributes - no class set on player yet\n");
		return;
	}

	float classMaxHealth = 100.0f;
	float classStartArmor = 20.0f;
	int classMaxAmmoPhaser = 1000;
	int classMaxAmmoFed = 400;
	int classMaxAmmoPlasma = 400;
	int classMaxAmmoIdryll = 400;
	int classGiveAmmoPhaser = 0;
	int classGiveAmmoFed = 0;
	int classGiveAmmoPlasma = 0;
	int classGiveAmmoIdryll = 0;

	//circle menu related
	str circleText1 = "";
	str circleImg1 = "";
	
	if ( !Q_stricmp( currentClass.c_str(), COOP_CLASS_NAME_MEDIC) ){
		//handle circle menu
		circleImg1 = COOP_CLASS_MEDIC_ICON;
		if (player->coop_hasLanguageGerman()) {circleText1 = COOP_TEXT_CLASS_MEDIC_ABILITY_DEU;}
		else {circleText1 = COOP_TEXT_CLASS_MEDIC_ABILITY_ENG;}

		classMaxHealth		= COOP_CLASS_MEDIC_MAX_HEALTH;
		classStartArmor		= COOP_CLASS_MEDIC_START_ARMOR;
		classMaxAmmoPhaser	= COOP_CLASS_MEDIC_MAX_AMMO_PHASER;
		classMaxAmmoFed		= COOP_CLASS_MEDIC_MAX_AMMO_FED;
		classMaxAmmoPlasma	= COOP_CLASS_MEDIC_MAX_AMMO_PLASMA;
		classMaxAmmoIdryll	= COOP_CLASS_MEDIC_MAX_AMMO_IDRYLLUM;
		player->gravity		= COOP_CLASS_MEDIC_GRAVITY;
		player->mass		= COOP_CLASS_MEDIC_MASS;
	}
	else if ( !Q_stricmp( currentClass.c_str(), COOP_CLASS_NAME_HEAVYWEAPONS) ){
		//handle circle menu
		circleImg1 = COOP_CLASS_HEAVYWEAPONS_ICON;
		if (player->coop_hasLanguageGerman()) { circleText1 = COOP_TEXT_CLASS_HEAVYWEAPONS_ABILITY_DEU; }
		else { circleText1 = COOP_TEXT_CLASS_HEAVYWEAPONS_ABILITY_ENG; }

		classMaxHealth		= COOP_CLASS_HEAVYWEAPONS_MAX_HEALTH;
		classStartArmor		= COOP_CLASS_HEAVYWEAPONS_START_ARMOR;
		classMaxAmmoPhaser	= COOP_CLASS_HEAVYWEAPONS_MAX_AMMO_PHASER;
		classMaxAmmoFed		= COOP_CLASS_HEAVYWEAPONS_MAX_AMMO_FED;
		classMaxAmmoPlasma	= COOP_CLASS_HEAVYWEAPONS_MAX_AMMO_PLASMA;
		classMaxAmmoIdryll	= COOP_CLASS_HEAVYWEAPONS_MAX_AMMO_IDRYLLUM;
		player->gravity		= COOP_CLASS_HEAVYWEAPONS_GRAVITY;
		player->mass		= COOP_CLASS_HEAVYWEAPONS_MASS;
	}
	else{ //technician
		//handle circle menu
		circleImg1 = COOP_CLASS_TECHNICIAN_ICON;
		if (player->coop_hasLanguageGerman()) { circleText1 = COOP_TEXT_CLASS_TECHNICIAN_ABILITY_DEU; }
		else { circleText1 = COOP_TEXT_CLASS_TECHNICIAN_ABILITY_ENG; }

		currentClass		= COOP_CLASS_NAME_TECHNICIAN;
		classMaxHealth		= COOP_CLASS_TECHNICIAN_MAX_HEALTH;
		classStartArmor		= COOP_CLASS_TECHNICIAN_START_ARMOR;
		classMaxAmmoPhaser	= COOP_CLASS_TECHNICIAN_MAX_AMMO_PHASER;
		classMaxAmmoFed		= COOP_CLASS_TECHNICIAN_MAX_AMMO_FED;
		classMaxAmmoPlasma	= COOP_CLASS_TECHNICIAN_MAX_AMMO_PLASMA;
		classMaxAmmoIdryll	= COOP_CLASS_TECHNICIAN_MAX_AMMO_IDRYLLUM;
		player->gravity		= COOP_CLASS_TECHNICIAN_GRAVITY;
		player->mass		= COOP_CLASS_TECHNICIAN_MASS;
	}

	//hzm coop mod chrissstrahl - set health
	player->setMaxHealth( classMaxHealth );

	//hzm coop mod chrissstrahl - add a background shader to the hud, this shows the player his current class
	if ( player->coop_hasCoopInstalled() ) {
		//[b60021] chrissstrahl - add circlemenu features
		/*Event* evCircleSet1;
		evCircleSet1 = new Event(EV_Player_coop_circleMenuSet);
		evCircleSet1->AddInteger(1);
		evCircleSet1->AddString(circleText1.c_str());
		evCircleSet1->AddString("!ability");
		evCircleSet1->AddString(circleImg1.c_str());
		evCircleSet1->AddInteger(0);
		player->ProcessEvent(evCircleSet1);
		*/

		//DelayedServerCommand( player->entnum , va( "globalwidgetcommand classBg shader coop_%s" , currentClass.c_str() ) );
		gamefix_playerDelayedServerCommand( player->entnum , va( "exec co-op/cfg/%s.cfg" , currentClass.c_str() ) );
	}
	else {
		player->hudPrint( va( "^5Coop:^2 You are now: ^5$$%s$$\n" , currentClass.c_str() ) );
	}

	//hzm coop mod chrissstrahl - make sure we don't give more than we can
	if ( classMaxAmmoPhaser < classGiveAmmoPhaser )	{ classGiveAmmoPhaser	= classMaxAmmoPhaser; }
	if ( classMaxAmmoFed < classGiveAmmoFed )		{ classGiveAmmoFed		= classMaxAmmoFed; }
	if ( classMaxAmmoPlasma < classGiveAmmoPlasma )	{ classGiveAmmoPlasma	= classMaxAmmoPlasma; }
	if ( classMaxAmmoIdryll < classGiveAmmoIdryll )	{ classGiveAmmoIdryll	= classMaxAmmoIdryll; }

	//hzm coop mod chrissstrahl - give some starting ammo
	player->GiveAmmo( "Phaser" , 0 , true , classMaxAmmoPhaser );
	player->GiveAmmo( "Fed" , 0 , true , classMaxAmmoFed );
	player->GiveAmmo( "Plasma" , 0 , true , classMaxAmmoPlasma );
	player->GiveAmmo( "Idryll" , 0 , true , classMaxAmmoIdryll );

	int i = program.coop_getFloatVariableValue( "coop_integer_startingArmor" );
	if ( i > 0 ){
		classStartArmor = i;
	}
		
	//hzm coop mod chrissstrahl - if player has no or low armor, give armor
	//float fArmorCurrent = player->GetArmorValue();
	//if ( fArmorCurrent < classStartArmor ){
		//Event *armorEvent;
		//armorEvent = new Event( EV_Sentient_GiveArmor );
		//armorEvent->AddString( "BasicArmor" );
		//armorEvent->AddInteger( ( classStartArmor - fArmorCurrent ) );
		//player->ProcessEvent( armorEvent );
	//}
	
	//update statistic for all players
	coop_classUpdateClassStats();

	//prevent player from freezing each other with the idryll staff
	Event *event;
	event = new Event( EV_Sentient_AddResistance );
	event->AddString( "stasis" );
	event->AddInteger( 100 );
	player->ProcessEvent( event );
	
	ExecuteThread("coop_justChangedClass", true, (Entity*)player);
}

void CoopClass::coop_classPlayerUsed( Player *usedPlayer , Player *usingPlayer , Equipment *equipment )
{
	//handle only in multiplayer
	if (	usedPlayer && usingPlayer &&
			multiplayerManager.inMultiplayer() &&
			!multiplayerManager.isPlayerSpectator( usedPlayer ) &&
			usedPlayer->health > 0.0f )
	{
		float fMessageTime = 2.0f;

		//get current weapon name
		str sEquipment = "USE";
		if ( equipment ){
			sEquipment = equipment->getTypeName();
		}

		//check if player was constantly revived or if it stopped
		if ( !equipment ){
			if ( ( CoopManager::Get().getPlayerData_revivedStepLasttime(usedPlayer) + 0.1f ) > level.time ){
				CoopManager::Get().setPlayerData_revivedStepLasttime_update(usedPlayer);
				CoopManager::Get().setPlayerData_revivedStepCounter(usedPlayer, 0);
				return;
			}
			fMessageTime = 1.0f;
			CoopManager::Get().setPlayerData_revivedStepCounter_update(usedPlayer);
		}

		//check if player is modulating, do not allow the class modulation
		if ((CoopManager::Get().getPlayerData_usingStepLasttime(usingPlayer) + 0.1 ) > level.time ){
			return;
		}

		//print this on the player that has been used
		if ( (CoopManager::Get().getPlayerData_revivedStepLasttime(usedPlayer) + fMessageTime ) < level.time ){
			CoopManager::Get().setPlayerData_revivedStepLasttime_update(usedPlayer);

			if ( !equipment ){
				if ( usedPlayer->coop_hasLanguageGerman() ){
					usedPlayer->hudPrint( va( "^5COOP^8 - You are beeing revived by: %s \n" , usingPlayer->client->pers.netname ) );
				}else{
					usedPlayer->hudPrint( va( "^5COOP^8 - Sie werden wiederbelebt von: %s \n" , usingPlayer->client->pers.netname ) );
				}
			}
			else{
				if ( equipment ){
					gamefix_playerDelayedServerCommand(usingPlayer->entnum, "!ability");
				}				
			}

		}
		//print this on the hud of the player that was using
		if ( (CoopManager::Get().getPlayerData_revivedStepLasttime(usingPlayer) + fMessageTime ) < level.time ){
			CoopManager::Get().setPlayerData_revivedStepLasttime_update(usingPlayer);

			if ( !equipment ){
				if ( usedPlayer->coop_hasLanguageGerman() ){
					usingPlayer->hudPrint( va( "^5COOP^8 - Wiederbeleben von: %s, bitte weitermachen!\n" , usedPlayer->client->pers.netname ) );
				}else{
					usingPlayer->hudPrint( va( "^5COOP^8 - You are reviving: %s, please continue!\n" , usedPlayer->client->pers.netname ) );
				}	
			}
			else{
				if ( equipment ){
					gamefix_playerDelayedServerCommand(usingPlayer->entnum, "!ability");
				}
			}
		}

		//player has been sucessfully revived
		if (CoopManager::Get().getPlayerData_revivedStepCounter(usedPlayer) >= COOP_REVIVE_TIME ){
			CoopManager::Get().setPlayerData_revivedStepCounter(usedPlayer, 0);
			usedPlayer->health = usedPlayer->max_health;
		}
	}
}

int CoopClass::coop_classPlayersOfClass(str className)
{
	if (className.length() < 1) {
		return -1;
	}
	
	if (!gameFixAPI_inMultiplayer() || !CoopManager::Get().IsCoopEnabled() || !CoopManager::Get().IsCoopLevel()) {
		return 0;
	}

	str temp;
	temp = className.tolower();

	if ( temp[0] == 'm' )
		temp = COOP_CLASS_NAME_MEDIC;
	else if ( temp[0] == 'h' )
		temp = COOP_CLASS_NAME_HEAVYWEAPONS;
	else
		temp = COOP_CLASS_NAME_TECHNICIAN;
	
	gentity_t   *other;
	Player      *player;
	int i;
	int iActive=0;

	for ( i = 0; i < game.maxclients;i++ ){
		other = &g_entities[i];
		if ( other->inuse && other->client ){
			player = ( Player * )other->entity;
			if (	player &&
					player->health >= 1 &&
					CoopManager::Get().getPlayerData_coopClass(player) == temp &&
					!multiplayerManager.isPlayerSpectator( player ) )
			{
				iActive++;
			}
		}
	}
	return iActive;
}

void CoopClass::coop_classUpdateClassStats( void )
{
	//set current time, so the client think function can send class statistics to each client
	coopClass.lastUpdateSendAt = level.time;
}

#endif