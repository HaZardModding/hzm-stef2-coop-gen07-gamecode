//-----------------------------------------------------------------------------
//
//  $Logfile:: /Code/DLLs/game/multiplayerArena.cpp                           $
// $Revision:: 48                                                             $
//   $Author:: Steven                                                         $
//     $Date:: 7/23/02 2:55p                                                  $
//
// Copyright (C) 2002 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
//
// Description:
//

#ifndef __MP_AWARDSYSTEM_HPP__
#define __MP_AWARDSYSTEM_HPP__

#include "g_local.h"          // common game stuff
#include "player.h"           // for Player
#include "item.h"
#include "mp_shared.hpp"

/* typedef enum
{
	AFTERMATCH_AWARD1,
	AFTERMATCH_AWARD2,
	AFTERMATCH_AWARD3,
	AFTERMATCH_AWARD4,
	AFTERMATCH_AWARD5,
	AFTERMATCH_AWARD6,
	MAX_AFTER_MATCH_AWARDS
} AfterMatchAwardType; */

typedef enum
{
	AFTERMATCH_TEAM_AWARD_MVP,
	AFTERMATCH_TEAM_AWARD_DEFENDER,
	AFTERMATCH_TEAM_AWARD_WARRIOR,
	AFTERMATCH_TEAM_AWARD_CARRIER,
	AFTERMATCH_TEAM_AWARD_INTERCEPTOR,
	AFTERMATCH_TEAM_AWARD_BRAVERY
} AfterMatchTeamAwardType;

class MultiplayerPlayerAwardData
{
public:
	float				_lastKillTime;
	int					_killStreak;
	int					_highestKillStreak;
	int					_numImpressives;
	int					_numDeaths;
	int					_shotsFired;
	int					_shotsHit;
	float				_lastHitTime;
	unsigned int		_weaponsKilledWith;
	unsigned int		_powerupsUsed;
	int					_numFlagCaptures;
	int					_numFlagReturns;
	int					_numFlagGuardingKills;

	int					_numKills;
	int					_numKillsWithExplosives;
	int					_numVaporizedKills;

	int					_entnum;

	bool				_playing;

	int					_numFirstStrikeAwards;
	int					_numImpressiveAwards;
	int					_numExcellentAwards;
	int					_numAceAwards;
	int					_numExpertAwards;
	int					_numMasterAwards;
	int					_numChampionAwards;
	int					_numDeniedAwards;

	int					_lastAwardIconIndex;
	float				_lastAwardTime;

	int					_afterMatchAwardIndexes[ MAX_SCORE_ICONS ];

						MultiplayerPlayerAwardData();
	void				init( void );
	void				reset( void );
};

class AwardSystem : public Class
{
private:
	static const float			_minEfficiencyForEfficiencyAward;
	static const int			_minImpressivesForSharpshooter;
	static const float			_minPercentForDemolitionist;
	static const float			_minPercentForVaporizer;
	static const int			_minKillsForChampion;
	static const int			_minKillsForMaster;
	static const int			_minKillsForExpert;
	static const int			_minKillsForAce;
	static const float			_maxExcellentTime;
	static const float			_deniedDistance;

	Container<str>				_powerups;
	Container<str>				_weapons;


	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	//--------------------------------------------------------------
	int							_maxPlayers = 0;
	bool						_hadFirstStrike = false;
	MultiplayerPlayerAwardData	*_playerAwardData = nullptr;

	int							_efficiencyIconIndex = 0;
	int							_sharpshooterIconIndex = 0;
	int							_untouchableIconIndex = 0;

	int							_logisticsIconIndex = 0;
	int							_tacticianIconIndex = 0;
	int							_demolitionistIconIndex = 0;

	int							_aceIconIndex = 0;
	int							_expertIconIndex = 0;
	int							_masterIconIndex = 0;
	int							_championIconIndex = 0;

	int							_mvpIconIndex = 0;
	int							_defenderIconIndex = 0;
	int							_warriorIconIndex = 0;
	int							_carrierIconIndex = 0;
	int							_interceptorIconIndex = 0;
	int							_braveryIconIndex = 0;

	int							_firstStrikeIconIndex = 0;
	int							_impressiveIconIndex = 0;
	int							_excellentIconIndex = 0;
	int							_deniedIconIndex = 0;


	int							getItemIndex( Container<str> &itemContainer, const str &itemName );
	int							getNumItems( Container<str> &itemContainer );
	int							getNumItemBits( unsigned int bits );

	void						awardTeamAward( AfterMatchTeamAwardType teamAward, const char *teamName );
	int							getLastAward( Player *player );
	int							getLastAwardCount( Player *player );

public:
								AwardSystem(); 
								~AwardSystem(); 
	void						init( int maxPlayers );
	void						initItems( void );
	virtual void				playerDamaged( Player *damagedPlayer, Player *attackingPlayer, float damage, int meansOfDeath );
	virtual void				playerFired( Player *attackingPlayer );
	virtual void				playerKilled( Player *killedPlayer, Player *attackingPlayer, Entity *inflictor, int meansOfDeath );
	virtual void				addPlayer( Player *player );
	virtual void				removePlayer( Player *player );
	virtual void				update( float frameTime ) {};

	virtual int					getStat( Player *player, int statNum, int value );
	virtual int					getIcon( Player *player, int statNum, int value );
	virtual int					getInfoIcon( Player *player );
	int							getAfterMatchAward( Player *player, int index );

	void						pickedupItem( Player *player, MultiplayerItemType itemType, const char *itemName );

	void						playerEventNotification( const char *eventName, const char *eventItemName, Player *eventPlayer );

	virtual void				matchOver( void );
};

#endif // __MP_AWARDSYSTEM_HPP__
