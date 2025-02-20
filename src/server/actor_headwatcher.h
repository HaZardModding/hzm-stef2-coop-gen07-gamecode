//-----------------------------------------------------------------------------
//
//  $Logfile:: /Code/DLLs/game/actor_headwatcher.h                            $
// $Revision:: 10                                                             $
//   $Author:: Steven                                                         $
//     $Date:: 10/13/03 8:53a                                                 $
//
// Copyright (C) 2001 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
//
//
// DESCRIPTION:
// Head Watcher Class
//


#ifndef __ACTOR_HEADWATCHER_H__
#define __ACTOR_HEADWATCHER_H__

#include "actor.h"
#include "actorincludes.h"
#include "weapon.h"

//============================
// Class HeadWatcher
//============================
class HeadWatcher
   {
   public:
       HeadWatcher();
       HeadWatcher( Actor *actor );
      ~HeadWatcher();

      void SetWatchTarget( Entity *ent );
      void SetWatchTarget( const str &targetName );
      void SetWatchSpeed ( float speed );
	  void SetMaxHeadYaw ( float max );
	  void SetMaxHeadPitch ( float max );
	  void SetMaxDistance( float distance );
	  void SetIgnoreWatchTarget( bool ignore );

      Entity* GetWatchTarget();

      void ClearWatchTarget();
      void HeadWatchTarget();
      void AdjustHeadAngles( const Vector &tagPos , const Vector &watchPosition );            
      void LerpHeadBySpeed( const Vector &angleDelta , bool useTorsoAngles = true );

	  void setHeadTwitch( bool twitchHead );
	  void twitchHead( void );

	// Archiving
	   virtual void Archive( Archiver &arc );		 
		void         DoArchive( Archiver &arc , Actor *actor );

   protected:
      void         _init();

   private:
	   //--------------------------------------------------------------
	   // GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	   //--------------------------------------------------------------
      EntityPtr _watchTarget = nullptr;
      Vector    _currentHeadAngles = Vector( 0.0f , 0.0f, 0.0f );
      float     _maxHeadTurnSpeed = 0.0f;
      float     _turnThreshold = 0.0f;
      qboolean  _explicitSet = qfalse;
	  float		_maxHeadYaw = 0.0f;
	  float		_maxHeadPitch = 0.0f;

	  bool		_twitchHead = false;
	  float		_nextTwitchHeadTime = 0.0f;
	  Vector	_headTwitchAngles = Vector( 0.0f , 0.0f, 0.0f );
	  float		_maxDistance = 0.0f;
	  bool		_ignoreWatchTarget = false;

      Actor *act = nullptr;

   };

inline void HeadWatcher::SetMaxHeadPitch( float max )
{
	_maxHeadPitch = max;
}

inline void HeadWatcher::SetMaxHeadYaw( float max )
{
	_maxHeadYaw = max;
}

inline void HeadWatcher::SetMaxDistance( float distance )
{
	_maxDistance = distance;
}

inline void HeadWatcher::SetIgnoreWatchTarget( bool ignore )
{
	_ignoreWatchTarget = ignore;
}

#endif /* __ACTOR_HEADWATCHER_H__ */
