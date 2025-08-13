// CONTAINING PLAYER CLASS(Medic,Technician,HeavyWeapons) HANDLING RELATED FUNCTIONS FOR THE HZM CO-OP MOD
#pragma once

#ifdef ENABLE_COOP

#include "../../dlls/game/equipment.h"
#include "../../dlls/game/_pch_cpp.h"


extern Event EV_Player_RecoverAbilityHud;
constexpr auto COOP_CLASS_MEDIC_ICON = "sysimg/icons/mp/specialty_medic";
constexpr auto COOP_CLASS_TECHNICIAN_ICON = "sysimg/icons/mp/specialty_technician";
constexpr auto COOP_CLASS_HEAVYWEAPONS_ICON = "sysimg/icons/mp/specialty_heavyweapons";

#define COOP_CLASS_NAME_TECHNICIAN "Technician"
#define COOP_CLASS_NAME_MEDIC "Medic"
#define COOP_CLASS_NAME_HEAVYWEAPONS "HeavyWeapons"

#define COOP_CLASS_HURT_WAITTIME 6
#define COOP_CLASS_REGENERATE_HEALTH 10
#define COOP_CLASS_REGENERATE_ARMOR 15
#define COOP_CLASS_REGENERATE_AMMO 50

#define COOP_CLASS_MEDIC_MAX_HEALTH 110.0f
#define COOP_CLASS_TECHNICIAN_MAX_HEALTH 100.0f
#define COOP_CLASS_HEAVYWEAPONS_MAX_HEALTH 120.0f

#define COOP_CLASS_MEDIC_START_ARMOR 25.0f
#define COOP_CLASS_TECHNICIAN_START_ARMOR 100.0f
#define COOP_CLASS_HEAVYWEAPONS_START_ARMOR 50.0f

#define COOP_CLASS_MEDIC_MAX_AMMO_PHASER 400.0f
#define COOP_CLASS_TECHNICIAN_MAX_AMMO_PHASER 400.0f
#define COOP_CLASS_HEAVYWEAPONS_MAX_AMMO_PHASER 1000.0f

#define COOP_CLASS_MEDIC_MAX_AMMO_FED 250.0f
#define COOP_CLASS_TECHNICIAN_MAX_AMMO_FED 300.0f
#define COOP_CLASS_HEAVYWEAPONS_MAX_AMMO_FED 500.0f

#define COOP_CLASS_MEDIC_MAX_AMMO_PLASMA 250.0f
#define COOP_CLASS_TECHNICIAN_MAX_AMMO_PLASMA 250.0f
#define COOP_CLASS_HEAVYWEAPONS_MAX_AMMO_PLASMA 500.0f

#define COOP_CLASS_MEDIC_MAX_AMMO_IDRYLLUM 250.0f
#define COOP_CLASS_TECHNICIAN_MAX_AMMO_IDRYLLUM 300.0f
#define COOP_CLASS_HEAVYWEAPONS_MAX_AMMO_IDRYLLUM 500.0f

#define COOP_CLASS_MEDIC_GRAVITY 0.85f
#define COOP_CLASS_TECHNICIAN_GRAVITY 0.9f
#define COOP_CLASS_HEAVYWEAPONS_GRAVITY 1.0f

#define COOP_CLASS_MEDIC_MASS 270
#define COOP_CLASS_TECHNICIAN_MASS 270
#define COOP_CLASS_HEAVYWEAPONS_MASS 350

#define COOP_REVIVE_TIME 100 //was 400
#define _COOP_SETTINGS_PLAYER_ITEM_ARMOR_MAX_TO_GIVE 200
#define COOP_MAX_AMMO_TO_GIVE_PLASMA 800
#define COOP_MAX_AMMO_TO_GIVE_IDRYLL 800
#define COOP_MAX_AMMO_TO_GIVE_FED 800

//[b60017] chrissstrahl - how many cycles a player regenerates after giving stuff to other class
#define COOP_CLASS_REGENERATION_CYCLES 5
//[b60021] chrissstrahl - added cooldowntime for using class ability
#define COOP_CLASS_REGENERATION_COOLDOWN 60


constexpr auto COOP_TEXT_CLASS_MEDIC_ABILITY_ENG = "Heal Team";
constexpr auto COOP_TEXT_CLASS_MEDIC_ABILITY_DEU = "Team heilen";
constexpr auto COOP_TEXT_CLASS_TECHNICIAN_ABILITY_ENG = "Charge Team shields";
constexpr auto COOP_TEXT_CLASS_TECHNICIAN_ABILITY_DEU = "Schilde aufladen";
constexpr auto COOP_TEXT_CLASS_HEAVYWEAPONS_ABILITY_ENG = "Charge Team ammo";
constexpr auto COOP_TEXT_CLASS_HEAVYWEAPONS_ABILITY_DEU = "Munition aufladen";

constexpr auto COOP_TEXT_SECRETSHIP_FOUND_BY_ENG = "^5Secret golden Starship^2 found by:^8 ";
constexpr auto COOP_TEXT_SECRETSHIP_FOUND_BY_DEU = "^5Geheimes goldenes Raumschiff^2 gefunden von:^8 ";

class CoopClass {
public:
	void playerKilled(Player* player);
	void coop_classAbilityRecoverHud(Player* player);
	void coop_classAbilityUse(Player* player);
	int coop_classRegenerationCycleGet(Player* player);
	void coop_classCheckApplay(Player* player);
	void coop_classCheckUpdateStat(Player* player);
	void coop_classRegenerate(Player* player);
	void coop_classSet(Player* player, str classToSet);
	void coop_classApplayAttributes(Player* player, bool changeOnly);
	void coop_classPlayerUsed(Player* usedPlayer, Player* usingPlayer, Equipment* equipment);
	int coop_classPlayersOfClass(str className);
	void coop_classUpdateClassStats(void);

	float lastUpdateSendAt = 0.0f;
};
extern CoopClass coopClass;


#endif /* coopClass */



