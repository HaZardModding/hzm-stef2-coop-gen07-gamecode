//-----------------------------------------------------------------------------
//
//  $Logfile:: /Code/DLLs/game/explosion.h                                   $
// $Revision:: 12                                                             $
//   $Author:: Steven                                                         $
//     $Date:: 10/13/03 8:53a                                                 $
//
// Copyright (C) 1997 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source is may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
//
//
// DESCRIPTION:
// Standard explosion object that is spawned by other entites and not map designers.
// Explosion is used by many of the weapons for the blast effect, but is also used
// by the Exploder and MultiExploder triggers.  These triggers create one or more
// explosions each time they are activated.
//

#ifndef __EXPLOSION_H__
#define __EXPLOSION_H__

#include "g_local.h"
#include "entity.h"
#include "trigger.h"


class Exploder : public Trigger
	{
	private:
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		float			damage = 0.0f;


		void			MakeExplosion( Event *ev );
		void			SetDmg( Event *ev );
		void			SetHealth( Event *ev );
		void			setDamage( Event *ev );

	public:
		CLASS_PROTOTYPE( Exploder );

						Exploder();
		virtual void	Archive( Archiver &arc );
	};


inline void Exploder::Archive
	(
	Archiver &arc
	)

   {
   Trigger::Archive( arc );

   arc.ArchiveFloat( &damage );
   }

class MultiExploder : public Trigger
	{
	protected:
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		float			explodewait = 0.0f;
		float			explode_time = 0.0f;
		float			duration = 0.0f;
		int				damage = 0;
		float			randomness = 0.0f;


		void			MakeExplosion( Event *ev );
		void			SetDmg( Event *ev );
		void			SetDuration( Event *ev );
		void			SetWait( Event *ev );
		void			SetRandom( Event *ev );
		void			SetHealth( Event *ev );
		void			setDamage( Event *ev );

	public:
		CLASS_PROTOTYPE( MultiExploder );

						MultiExploder();
	   virtual void		Archive( Archiver &arc );
	};

inline void MultiExploder::Archive
	(
	Archiver &arc
	)
   {
   Trigger::Archive( arc );

   arc.ArchiveFloat( &explodewait );
   arc.ArchiveFloat( &explode_time );
   arc.ArchiveFloat( &duration );
   arc.ArchiveInteger( &damage );
   arc.ArchiveFloat( &randomness );
   }

void CreateExplosion
   (
   const Vector &pos,
   float  damage = 120,
   Entity *inflictor = NULL,
   Entity *attacker = NULL,
   Entity *ignore = NULL,
   const char *explosionModel = NULL,
   float  scale = 1.0f,
   float  radius = 0.0f
   );

class ExplodeObject : public MultiExploder
	{
	private:
      Container<str> debrismodels;
		str				debrissound;
		str				explosionmodel;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		int            debrisamount = 0;
		float          severity = 0.0f;
		float			explosionradius = 0.0f;
		bool		_spawnEarthquake = false;
		bool		_haveSpawnedEarthquake = false;
		float		_earthquakeMagnitude = 0.0f;
		float		_earthquakeDuration = 0.0f;
		float		_earthquakeDistance = 0.0f;


      void           SetDebrisModel( Event *ev );
      void           SetSeverity( Event *ev );
      void           SetDebrisAmount( Event *ev );
      void           MakeExplosion( Event *ev );
		void           ExplodeTiki( Event *ev );
		void				Damage( Event *ev );
		void				SetSound( Event *ev );
		void				SetExplosion( Event *ev );

		void				setSpawnEarthquake( Event *ev );
		void				setEarthquakeDuration( Event *ev );
		void				setEarthquakeMagnitude( Event *ev );
		void				setEarthquakeDistance( Event *ev );

	public:
      CLASS_PROTOTYPE( ExplodeObject );

		               ExplodeObject();
	   virtual void   Archive( Archiver &arc );
	};

inline void ExplodeObject::Archive( Archiver &arc )
{
	MultiExploder::Archive( arc );

	debrismodels.Archive( arc );
	arc.ArchiveInteger( &debrisamount );
	arc.ArchiveFloat( &severity );
	arc.ArchiveString( &debrissound );
	arc.ArchiveString( &explosionmodel );
	arc.ArchiveFloat( &explosionradius );

	arc.ArchiveBool( &_spawnEarthquake );
	arc.ArchiveBool( &_haveSpawnedEarthquake );
	arc.ArchiveFloat( &_earthquakeMagnitude );
	arc.ArchiveFloat( &_earthquakeDuration );
	arc.ArchiveFloat( &_earthquakeDistance );
}

#endif /* explosion.h */
