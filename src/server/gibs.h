//-----------------------------------------------------------------------------
//
//  $Logfile:: /Code/DLLs/game/gibs.h                                        $
// $Revision:: 7                                                              $
//   $Author:: Steven                                                         $
//     $Date:: 10/13/03 8:54a                                                 $
//
// Copyright (C) 1998 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source is may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
//
//
// DESCRIPTION:
// Gibs - nuff said

#ifndef __GIBS_H__
#define __GIBS_H__

#include "g_local.h"
#include "mover.h"

#define NO_FINAL_PITCH  -1000

class Gib : public Entity
   {
   private:
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		int      sprayed = 0;
		float    scale = 0.0f;
		Entity   *blood = nullptr;
		float		blood_splat_size = 0.0f;
		float		final_pitch = 0.0f;
		float		next_bleed_time = 0.0f;


		str		blood_splat_name;
		str		blood_spurt_name;

   public:
      CLASS_PROTOTYPE( Gib );


	  //--------------------------------------------------------------
	  // GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	  //--------------------------------------------------------------
      qboolean    fadesplat = qfalse;


      Gib();
		~Gib();
      Gib( const str &name, qboolean blood_trail, const str &bloodtrailname="", const str &bloodspurtname="", 
			  const str &bloodsplatname="", float blood_splat_size = 8, float pitch=NO_FINAL_PITCH );
      void     SetVelocity( float health );
      void     SprayBlood( const Vector &start );
      void     Throw( Event *ev );
      void     Splat( Event *ev );
      void     Stop( Event *ev );
		void     Damage( Event *ev );
      void     ClipGibVelocity( void );
	   virtual void Archive( Archiver &arc );
	};

inline void Gib::Archive
	(
	Archiver &arc
	)
   {
   Entity::Archive( arc );

   arc.ArchiveBoolean( &sprayed );
   arc.ArchiveFloat( &scale );
   arc.ArchiveObjectPointer( ( Class ** )&blood );
   arc.ArchiveString( &blood_splat_name );
   arc.ArchiveFloat( &blood_splat_size );
   arc.ArchiveString( &blood_spurt_name );
   arc.ArchiveFloat( &final_pitch );
	arc.ArchiveFloat( &next_bleed_time );

	arc.ArchiveBoolean( &fadesplat );
   }


void CreateGibs
   (
   Entity * ent,
   float damage = -50,
   float scale = 1.0f,
   int num = 1,
   const char * modelname = NULL
   );

extern Event EV_ThrowGib;

#endif // gibs.h
