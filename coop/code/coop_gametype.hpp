//--------------------------------------------------------------
// COOP Generation 7.000 - Gamemode based on mp_modeTeamDM - chrissstrahl
//--------------------------------------------------------------
#pragma once
#include "../../dlls/game/_pch_cpp.h"
#include "../../dlls/game/mp_modeTeamDm.hpp"  // Inherit from team deathmatch


class ModeCoop : public ModeTeamDeathmatch
{
private:
	Team* _redTeam;
	Team* _blueTeam;

protected:

public:
	CLASS_PROTOTYPE(ModeCoop);

	ModeCoop();
	~ModeCoop();

public:

	/* virtual */ void					init(int maxPlayers);
	/* virtual */ void					initItems(void);
	/* virtual */ void					_giveInitialConditions(Player* player);
	/* virtual */ int					getInfoIcon(Player* player);
	/* virtual */ void					playerChangedModel(Player* player);
	/* virtual */ void					updatePlayerSkin(Player* player);
	/* virtual */ void					AddPlayer(Player* player);
	/* virtual */ void					addPlayerToTeam(Player* player, Team* team);
	/* virtual */ bool					canJoinTeam(Player* player, const str& teamName);
	/* virtual */ void					joinTeam(Player* player, const str& teamName);
	/* virtual */ Entity*				getSpawnPoint(Player* player);

	/* virtual */ void					playerKilled(Player* killedPlayer, Player* attackingPlayer, Entity* inflictor, int meansOfDeath);
	/* virtual */ bool					isEndOfMatch(void);
	/* virtual */ int					getTeamPoints(Player* player);
	/* virtual */ void					setupMultiplayerUI(Player* player);
	/* virtual */ void					score(const Player* player);
	/* virtual */ bool					checkGameType(const char* rule);
	/* virtual */ void					ActivatePlayer(Player* player);
	/* virtual */ void					playerDead(Player* player);
};
