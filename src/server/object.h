//-----------------------------------------------------------------------------
//
//  $Logfile:: /Code/DLLs/game/object.h                                      $
// $Revision:: 5                                                              $
//   $Author:: Steven                                                         $
//     $Date:: 10/13/03 8:54a                                                 $
//
// Copyright (C) 1997 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source is may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
//
//
// DESCRIPTION:
// Object class
//

#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "g_local.h"
#include "sentient.h"
#include "animate.h"
#include "specialfx.h"

class Object : public Entity
	{
	public:
      CLASS_PROTOTYPE( Object );

							   Object();
      void              Killed( Event *ev );
      void              SetAngle( Event *ev );
      void              Setup( Event *ev );
      void              SetAnim( Event *ev );
      void              MakeShootable( Event *ev );
	};

extern Event EV_ThrowObject_Pickup;
extern Event EV_ThrowObject_Throw;

class ThrowObject : public Object
	{
   private:
	   //--------------------------------------------------------------
	   // GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	   //--------------------------------------------------------------
		int               owner = 0;
		Vector            pickup_offset = Vector(0.0f, 0.0f, 0.0f);
		float             damage = 0.0f;
		qboolean          hurt_target = qfalse;


      str               throw_sound;


	public:
      CLASS_PROTOTYPE( ThrowObject );
							   ThrowObject();
      void              Touch(Event *ev);
      void              Throw( Event * ev );		
		void              Throw( const Entity *owner, float speed, const Sentient *targetent, float gravity, float throw_damage );
      void              Pickup( Event * ev );
		void              Pickup( Entity* ent, const str &bone );
      void              PickupOffset( Event * ev );
      void              ThrowSound( Event * ev );
	   virtual void      Archive( Archiver &arc );
	};

inline void ThrowObject::Archive
	(
	Archiver &arc
	)
   {
   Object::Archive( arc );

   arc.ArchiveInteger( &owner );
   arc.ArchiveVector( &pickup_offset );
   arc.ArchiveString( &throw_sound );
	arc.ArchiveFloat( &damage );
	arc.ArchiveBoolean( &hurt_target );
   }

#endif /* object.h */
