//-----------------------------------------------------------------------------
//
//  $Logfile:: /EF2/Code/DLLs/game/object.cpp                                $
// $Revision:: 9                                                              $
//   $Author:: Singlis                                                        $
//     $Date:: 9/26/03 2:36p                                                  $
//
// Copyright (C) 1998 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source is may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
//
//
// DESCRIPTION:
//


#include "_pch_cpp.h"
#include "object.h"
#include "sentient.h"
#include "misc.h"
#include "explosion.h"
#include "gibs.h"
#include "specialfx.h"

Event EV_Object_Setup
(
	"_setup",
	EV_CODEONLY,
	NULL,
	NULL,
	"Sets up an object."
);
Event EV_Object_SetAnim
(
	"anim",
	EV_DEFAULT,
	"s",
	"animname",
	"Sets up the object with the specified animname."
);
Event EV_Object_Shootable
(
	"shootable",
	EV_DEFAULT,
	NULL,
	NULL,
	"Make the object shootable but not necessarily solid to the player."
);

CLASS_DECLARATION( Entity, Object, "object" )
{
	{ &EV_Killed,							&Object::Killed },
	{ &EV_Object_Setup,						&Object::Setup },
	{ &EV_Object_SetAnim,					&Object::SetAnim },
	{ &EV_Object_Shootable,					&Object::MakeShootable },

	{ NULL, NULL }
};

Object::Object()
{
	animate = new Animate( this );
	
	if ( LoadingSavegame )
	{
		// Archive function will setup all necessary data
		return;
	}
	//
	// all objects default to not solid
	//
	setSolidType( SOLID_NOT );
	
	health = 0;
	
	takedamage = ( spawnflags & 2 ) ? DAMAGE_NO : DAMAGE_YES;
	
	//
	// we want the bounds of this model auto-rotated
	//
	flags |= FL_ROTATEDBOUNDS;
	
	if ( !com_blood->integer )
	{
		flags &= ~FL_BLOOD;
		flags &= ~FL_DIE_GIBS;
	}
	
	PostEvent( EV_Object_Setup, EV_POSTSPAWN );
}

void Object::SetAnim( Event *ev )
{
	int animnum;
	
	if ( ( ev->NumArgs() >= 1 ) && gi.IsModel( edict->s.modelindex ) )
	{
		animnum = gi.Anim_Random( edict->s.modelindex, ev->GetString( 1 ) );
		if ( animnum >= 0 )
		{
			animate->NewAnim( animnum );
		}
	}
}

//--------------------------------------------------------------
// GAMEFIX - Disabled: Because this was not in the Officially released game version - chrissstrahl
// Also Membervariables are now inizialised, so this is not needed
//--------------------------------------------------------------
void Object::Setup( Event *ev )
{
	/*
	if ( !health )
	{
		health = ( maxs - mins ).length();
		max_health = health;
	}
	*/
}

void Object::MakeShootable( Event *ev )
{
	setContents( CONTENTS_SHOOTABLE_ONLY );
	link();
}

void Object::Killed(Event *ev)
{
	Entity * ent;
	Entity * attacker;
	Vector dir;
	Event * event;
	const char * name;
	
	takedamage = DAMAGE_NO;
	setSolidType( SOLID_NOT );
	hideModel();
	
	attacker	= ev->GetEntity( 1 );
	
	if (flags & FL_DIE_EXPLODE)
	{
		CreateExplosion( origin, 50.0f, this, this, this );
	}
	
	if (flags & FL_DIE_GIBS)
	{
		setSolidType( SOLID_NOT );
		hideModel();
		
		CreateGibs( this, -150.0f, edict->s.scale, 3 );
	}
	
	//
	// kill the killtargets
	//
	name = KillTarget();
	if ( name && strcmp( name, "" ) )
	{
		ent = NULL;
		do
		{
			ent = G_FindTarget( ent, name );
			if ( !ent )
			{
				break;
			}
			ent->PostEvent( EV_Remove, 0.0f );
		}
		while ( 1 );
	}
	
	//
	// fire targets
	//
	name = Target();
	if ( name && strcmp( name, "" ) )
	{
		ent = NULL;
		do
		{
			ent = G_FindTarget( ent, name );
			if ( !ent )
			{
				break;
			}
			event = new Event( EV_Activate );
			event->AddEntity( attacker );
			ent->ProcessEvent( event );
		}
		while ( 1 );
	}
	
	PostEvent( EV_Remove, 0.0f );
}

/*****************************************************************************/
/*QUAKED func_throwobject (0 0.25 0.5) (-16 -16 0) (16 16 32)

This is an object you can pickup and throw at people
******************************************************************************/

Event EV_ThrowObject_Pickup
(
	"pickup",
	EV_DEFAULT,
	"es",
	"entity tag_name",
	"Picks up this throw object and attaches it to the entity."
);
Event EV_ThrowObject_Throw
(
	"throw",
	EV_DEFAULT,
	"efeF",
	"owner speed targetent grav",
	"Throw this throw object."
);
Event EV_ThrowObject_PickupOffset
(
	"pickupoffset",
	EV_DEFAULT,
	"v",
	"pickup_offset",
	"Sets the pickup_offset."
);
Event EV_ThrowObject_ThrowSound
(
	"throwsound",
	EV_DEFAULT,
	"s",
	"throw_sound",
	"Sets the sound to play when object is thrown."
);

CLASS_DECLARATION( Object, ThrowObject, "func_throwobject" )
{
	{ &EV_Touch,								&ThrowObject::Touch },
	{ &EV_ThrowObject_Pickup,					&ThrowObject::Pickup },
	{ &EV_ThrowObject_Throw,					&ThrowObject::Throw },
	{ &EV_ThrowObject_PickupOffset,				&ThrowObject::PickupOffset },
	{ &EV_ThrowObject_ThrowSound,				&ThrowObject::ThrowSound },

	{ NULL, NULL }
};

ThrowObject::ThrowObject()
{
	if ( LoadingSavegame )
	{
		// Archive function will setup all necessary data
		return;
	}
	pickup_offset = vec_zero;
	damage = 0;
	hurt_target = false;


	//--------------------------------------------------------------
	// GAMEFIX - Fixed: warning: converting to non-pointer type ‘int’ from NULL [-Wconversion-null] - chrissstrahl
	//--------------------------------------------------------------
	owner = 0;
}

void ThrowObject::PickupOffset( Event *ev )
{
	pickup_offset = edict->s.scale * ev->GetVector( 1 );
}

void ThrowObject::ThrowSound( Event *ev )
{
	throw_sound = ev->GetString( 1 );
}

void ThrowObject::Touch( Event *ev )
{
	Entity *other;
	
	if ( movetype != MOVETYPE_BOUNCE )
	{
		return;
	}
	
	other = ev->GetEntity( 1 );
	assert( other );
	
	if ( other->isSubclassOf( Teleporter ) )
	{
		return;
	}
	
	if ( other->entnum == owner )
	{
		return;
	}
	
	if ( throw_sound.length() )
	{
		StopLoopSound();
	}
	
	if ( other->takedamage && !hurt_target )
	{
		// other->Damage( this, G_GetEntity( owner ), size.length() * velocity.length() / 400, origin, velocity,
		//    level.impact_trace.plane.normal, 32, 0, MOD_THROWNOBJECT );
		
		other->Damage( this, G_GetEntity( owner ), damage, origin, velocity,
			level.impact_trace.plane.normal, 32, 0, MOD_THROWNOBJECT );
		hurt_target = true;
	}
	
	//Damage( this, this, max_health, origin, velocity, level.impact_trace.plane.normal, 32, 0, MOD_THROWNOBJECT );
	Damage( this, this, 0.0f, origin, velocity, level.impact_trace.plane.normal, 32, 0, MOD_THROWNOBJECT );
}


//--------------------------------------------------------------
// GAMEFIX - Fixed: Warning C4458: declaration of owner hides class member. Renamed to: temp_owner - chrissstrahl
// GAMEFIX - Fixed: Warning C4458: declaration of gravity hides class member. Renamed to: temp_gravity - chrissstrahl
//--------------------------------------------------------------
void ThrowObject::Throw( const Entity *temp_owner, float speed, const Sentient *targetent, float temp_gravity, float throw_damage )
{
	float    traveltime;
	float    vertical_speed;
	Vector   dir;
	Vector   xydir;
	Event    *e;

	
	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Warning C4458: declaration of target hides class member. Renamed to: temp_target - chrissstrahl
	//--------------------------------------------------------------
	Vector   temp_target;
	
	
	e = new Event( EV_Detach );
	ProcessEvent( e );
	
	this->owner = temp_owner->entnum;
	edict->ownerNum = temp_owner->entnum;
	
	damage = throw_damage;
	temp_target = targetent->origin;
	temp_target.z += targetent->viewheight;
	
	setMoveType( MOVETYPE_BOUNCE );
	setSolidType( SOLID_BBOX );
	edict->clipmask = MASK_PROJECTILE;
	
	dir = temp_target - origin;
	xydir = dir;
	xydir.z = 0;
	traveltime = xydir.length() / speed;
	vertical_speed = ( dir.z / traveltime ) + ( 0.5f * temp_gravity * sv_currentGravity->value * traveltime );
	xydir.normalize();
	
	// setup ambient flying sound
	if ( throw_sound.length() )
	{
		LoopSound( throw_sound.c_str() );
	}
	
	velocity = speed * xydir;
	velocity.z = vertical_speed;
	
	angles = velocity.toAngles();
	setAngles( angles );
	
	avelocity.x = crandom() * 200.0f;
	avelocity.y = crandom() * 200.0f;
	takedamage = DAMAGE_YES;
}

void ThrowObject::Throw( Event *ev )
{
	Sentient *targetent;
	float    speed;
	float    grav;
	float    throw_damage;


	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Warning C4458: declaration of owner hides class member. Renamed to: temp_owner - chrissstrahl
	//--------------------------------------------------------------
	Entity   *temp_owner;

	
	temp_owner = ev->GetEntity( 1 );
	assert( temp_owner );
	
	if ( !temp_owner )
	{
		return;
	}
	
	speed = ev->GetFloat( 2 );
	if ( !speed )
	{
		speed = 1;
	}
	
	targetent = ( Sentient * )ev->GetEntity( 3 );
	assert( targetent );
	if (!targetent)
	{
		return;
	}
	
	if ( ev->NumArgs() > 3 )
	{
		grav = ev->GetFloat( 4 );
	}
	else
	{
		grav = 1;
	}
	
	if ( ev->NumArgs() > 4 )
		throw_damage = ev->GetFloat( 5 );
	else
		throw_damage = 0;
	
	Throw( temp_owner, speed, targetent, grav, throw_damage );
	
	/*
	e = new Event( EV_Detach );
	ProcessEvent( e );
	
	this->owner = owner->entnum;
	edict->ownerNum = owner->entnum;
	  
	gravity = grav;
		
		  
	target = targetent->origin;
	target.z += targetent->viewheight;
			
	setMoveType( MOVETYPE_BOUNCE );
	setSolidType( SOLID_BBOX );
	edict->clipmask = MASK_PROJECTILE;

	dir = target - origin;
	xydir = dir;
	xydir.z = 0;
	traveltime = xydir.length() / speed;
	vertical_speed = ( dir.z / traveltime ) + ( 0.5f * gravity * sv_currentGravity->value * traveltime );
	xydir.normalize();
				
	// setup ambient flying sound
	if ( throw_sound.length() )
	{
		LoopSound( throw_sound.c_str() );
	}

	velocity = speed * xydir;
	velocity.z = vertical_speed;

	angles = velocity.toAngles();
	setAngles( angles );

	avelocity.x = crandom() * 200;
	avelocity.y = crandom() * 200;
	takedamage = DAMAGE_YES;
	*/
}

void ThrowObject::Pickup( Entity *ent, const str &bone )
{
	Event *e;
	
	setOrigin( pickup_offset );
	
	e = new Event( EV_Attach );
	e->AddEntity( ent );
	e->AddString( bone );
	ProcessEvent( e );
	hurt_target = false;
	edict->s.renderfx &= ~RF_FRAMELERP;
}

void ThrowObject::Pickup( Event *ev )
{
	Entity * ent;
	//Event * e;
	str bone;
	
	ent = ev->GetEntity( 1 );
	
	assert( ent );
	if ( !ent )
	{
		return;
	}
	bone = ev->GetString( 2 );
	
	/*
	setOrigin( pickup_offset );
	
	  e = new Event( EV_Attach );
	  e->AddEntity( ent );
	  e->AddString( bone );
	  ProcessEvent( e );
	  
		edict->s.renderfx &= ~RF_FRAMELERP;
	*/
	
	Pickup( ent , bone );
}
