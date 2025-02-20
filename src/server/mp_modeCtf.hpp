//-----------------------------------------------------------------------------
//
//  $Logfile:: /Code/DLLs/game/teamArena.h                                    $
// $Revision:: 20                                                             $
//   $Author:: Steven                                                         $
//     $Date:: 7/17/02 4:00p                                                  $
//
// Copyright (C) 2002 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
// 
// 
// Description:
//

#ifndef __MP_MODECTF_HPP__
#define __MP_MODECTF_HPP__

#include "mp_modeTeamBase.hpp"

class MultiplayerPlayerCtfData
{
public:
	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	//--------------------------------------------------------------
	int					_hasFlag = 0;
	EntityPtr			_carriedFlag = nullptr;
	MultiplayerItem *	_flag = nullptr;

	MultiplayerPlayerCtfData() { _hasFlag = false; }
	void				init( void ) { _hasFlag = false; }
};

class CtfFlag
{
public:
	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	//--------------------------------------------------------------
	MultiplayerItem	*		_tempFlag = nullptr;
	float					_tempFlagTime = 0.0f;
	MultiplayerItem	*		_realFlag = nullptr;
	str						_teamName;
};

//---------------------------------------------------------------------
// ModeCaptureTheFlag -- Capture the Flag Multiplayer.  Classic CTF
//                        style gameplay.
//---------------------------------------------------------------------
//CLASS_ARENA( ModeCaptureTheFlag );
class ModeCaptureTheFlag : public ModeTeamBase
	{
	private:
		static const float					_maxGuardingDist;
		static const float					_ctfCapturePoints;
		static const float					_maxDroppedFlagTime;

		static const int					_pointsForFlagReturn;
		static const int					_pointsForKillingFlagCarrier;
		static const int					_pointsForAssist;
		static const int					_pointsForDefense;
		static const int					_pointsForTakingTheFlag;
		static const int					_pointsForCapturingTheFlag;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		Team*                               _redTeam = nullptr;
		Team*                               _blueTeam = nullptr;


		Container<CtfFlag>					_flags;

		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		MultiplayerPlayerCtfData			*_playerCtfData = nullptr;

		int									_flagCarrierIconIndex = 0;

		int									_oneFlagTakenIconIndex = 0;
		int									_oneFlagMissingIconIndex = 0;
		int									_oneFlagInBaseIconIndex = 0;

		int									_redFlagTakenIconIndex = 0;
		int									_redFlagMissingIconIndex = 0;
		int									_redFlagInBaseIconIndex = 0;

		int									_blueFlagTakenIconIndex = 0;
		int									_blueFlagMissingIconIndex = 0;
		int									_blueFlagInBaseIconIndex = 0;

	protected:
		///* virtual */ void					_endMatch();    // Notifies teams of end of match

		void								grabTheFlag( Player *player, MultiplayerItem *item );
		void								score( Player *player );
		void								putFlagBack( Player *player );

		CtfFlag *							findFlag( MultiplayerItem *item );
		float								findNearestTeamFlagDist( const str &teamName, const Vector &position );
		float								findNearestTeamFlagCarrierDist( const str &teamName, const Vector &position );

		void								returnFlag( MultiplayerItem *item, Player *player );
		void								dropFlag( Player *player );
		void								attachFlag( Player *player, Entity *obj );

	public:
		CLASS_PROTOTYPE( ModeCaptureTheFlag );

											ModeCaptureTheFlag();
											~ModeCaptureTheFlag();

		/* virtual */ void					init( int maxPlayers );

		/* virtual */ bool					shouldKeepItem( MultiplayerItem *item );
		/* virtual */ void					itemKept( MultiplayerItem *item );

		/* virtual */ void					RemovePlayer( Player *player );

		/* virtual */ void					addPlayerToTeam( Player *player, Team *team );

		void								playerKilled( Player *killedPlayer, Player *attackingPlayer, Entity *inflictor, int meansOfDeath );
		/* virtual */ void					itemTouched( Player *player, MultiplayerItem *item );

		bool								isEndOfMatch( void );

		virtual int							getStat( Player *player, int statNum, int value );
		/* virtual */ int					getIcon( Player *player, int statNum, int value );
		/* virtual */ int					getScoreIcon( Player *player, int index, int value );

		/* virtual */ void					setupMultiplayerUI( Player *player );

		/* virtual */ void					score( const Player *player );

		/* virtual */ void					update( float frameTime );

		/* virtual */ bool					checkGameType( const char *rule );
		/* virtual */ bool					doesPlayerHaveItem( Player *player, const char *itemName );

		/* virtual */ void					playerEventNotification( const char *eventName, const char *eventItemName, Player *eventPlayer );

		/* virtual */ void					playerChangedModel( Player *player );
	};

#endif // __MP_MODECTF_HPP__
