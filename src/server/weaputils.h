//-----------------------------------------------------------------------------
//
//  $Logfile:: /Code/DLLs/game/weaputils.h                                   $
// $Revision:: 43                                                             $
//   $Author:: Steven                                                         $
//     $Date:: 4/09/03 2:32p                                                  $
//
// Copyright (C) 1998 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
//
//
// DESCRIPTION:
//

#ifndef __WEAPUTILS_H__
#define __WEAPUTILS_H__

#include "g_local.h"
#include "animate.h"
#include "beam.h"

#define P_BOUNCE_TOUCH  (1 << 0)
#define P_CHARGE_LIFE   (1 << 1)
#define P_CHARGE_SPEED  (1 << 2)
#define P_BOUNCE_ALL	(1 << 3)	// projectile bounces off of *everything*
#define P_QUIET_EXPIRE	(1 << 4)	// projectile expires (from time) quietly (no explosion)

extern Event EV_Projectile_Explode;
extern Event EV_Projectile_UpdateBeam;
extern Event EV_Actor_ProjectileClose;

//------------------------- CLASS ------------------------------
//
// Name:			Projectile
// Base Class:		Entity
//
// Description:		Projectile Entity
//
// Method of Use:	Spawned into the world
//--------------------------------------------------------------
class Projectile : public Entity
{
public:
	CLASS_PROTOTYPE( Projectile );
	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	//--------------------------------------------------------------
	float			fov = 0.0f;
	int				owner = 0;
	float			speed = 0.0f;
	float			minspeed = 0.0f;
	float			bouncefactor = 0.0f;
	float			damage = 0.0f;
	float			knockback = 0.0f;
	float			life = 0.0f;
	float			minlife = 0.0f;
	float			dlight_radius = 0.0f;
	float			charge_fraction = 0.0f;
	bool			firstTimeOwnerControl = false;
	Vector			prevPlayerAngles = Vector(0.0f, 0.0f, 0.0f);
	Vector			originalPlayerAngles = Vector(0.0f, 0.0f, 0.0f);
	Vector			dlight_color = Vector(0.0f, 0.0f, 0.0f);
	Vector			addvelocity = Vector(0.0f, 0.0f, 0.0f);
	meansOfDeath_t	meansofdeath = MOD_LIGHTSWORD;
	FuncBeam		*m_beam = nullptr;
	int				projFlags = 0;
	EntityPtr		target = nullptr;
	bool			addownervelocity = false;
	bool			drunk = false;
	bool			can_hit_owner = false;
	bool			remove_when_stopped = false;
	bool			stick_on_touch = false;
	bool			ownercontrol = false;
	bool			_ownerControlLaser = false;
	bool			_ownerControlUsed = false;
	float			_controlTurnSpeed = 0.0f;
	bool			triggerdetonate = false;
	float			startTime = 0.0f;
	float			drunkAngleModifier = 0.0f;
	float			impactmarkradius = 0.0f;
	bool			_scaleByCharge = false;
	float			_minScaleFromCharge = 0.0f;
	float			_maxScaleFromCharge = 0.0f;
	bool			_scaleExplosion = false;
	bool			_notifyActors = false;
	bool			_notShootable = false;
	bool			_hitsProjectiles = false;
	float			_minOnGroundTime = 0.0f;
	bool			_onGround = false;
	float			_startOnGroundTime = 0.0f;
	bool			_heatSeek = false;
	bool			_damagedSomething = false;


	str				bouncesound;
	str				impactmarkshader;
	str				impactmarkorientation;
	str				explosionmodel;

	
					Projectile();
	virtual			~Projectile();
	virtual void	Archive( Archiver &arc );
	virtual void	Touch( Event *ev );
	virtual void	Think( void );
	virtual void	Explode( Event *ev );
	virtual void	DoDecal( void );
	
	void			SetAvelocity( Event *ev );
	void			SetAddVelocity( Event *ev );
	void			SetDLight( Event *ev );
	void			SetLife( Event *ev );
	void			SetMinLife( Event *ev );
	void			SetBounceFactor( Event *ev );
	void			SetChargeLife( Event *ev );
	void			SetSpeed( Event *ev );
	void			SetMinSpeed( Event *ev );
	void			SetChargeSpeed( Event *ev );
	void			SetDamage( Event *ev );
	void			SetKnockback( Event *ev );
	void			SetProjectileDLight( Event *ev );
	void			SetMeansOfDeath( Event *ev );
	void			SetQuietExpire( Event *ev );
	void			SetBounceTouch( Event *ev );
	void			SetBounceSound( Event *ev );
	void			SetImpactMarkShader( Event *ev );
	void			SetImpactMarkRadius( Event *ev );
	void			SetImpactMarkOrientation( Event *ev );
	void			SetExplosionModel( Event *ev );
	void			UpdateBeam( Event *ev );
	void			BeamCommand( Event *ev );
	void			HeatSeek( Event *ev );
	void			Drunk( Event *ev );
	void			AddOwnerVelocity( Event *ev );
	float			ResolveMinimumDistance(	Entity *potential_target, float currmin );
	float			AdjustAngle( float maxadjust, float currangle, float targetangle );
	void			SetCanHitOwner( Event *ev );
	void			ClearOwner( Event *ev );
	Entity *		getOwner( void );
	void			RemoveWhenStopped( Event *ev );
	void			StickOnTouch( Event *ev );
	void			Stopped( Event *ev );
	void			SetOwnerControl( Event *ev );
	void			setOwnerControlLaser( Event *ev );
	void			setControlTurnSpeed( Event *ev );
	void			TriggerDetonate( Event *ev );
	void			AngleThink( Event *ev );
	
	void			setScaleByCharge( Event *ev );
	void			setScaleExplosion( Event *ev );
	void			setNotifyActors( Event *ev );
	
	void			NotifyActors();
	void			setNotShootable( Event *ev );
	void			setHitsProjectiles( Event *ev );
	void			setMinOnGroundTime( Event *ev );

	void			destroyed( Event *ev );

	void			didDamage( void );
};

inline void Projectile::Archive ( Archiver &arc )
{
	Entity::Archive( arc );

	arc.ArchiveFloat( &fov );
	arc.ArchiveInteger( &owner );
	arc.ArchiveFloat( &speed );
	arc.ArchiveFloat( &minspeed );
	arc.ArchiveFloat( &bouncefactor );
	arc.ArchiveFloat( &damage );
	arc.ArchiveFloat( &knockback );
	arc.ArchiveFloat( &life );
	arc.ArchiveFloat( &minlife );
	arc.ArchiveFloat( &dlight_radius );
	arc.ArchiveFloat( &charge_fraction );
	arc.ArchiveBool( &firstTimeOwnerControl );

	arc.ArchiveVector( &prevPlayerAngles );
	arc.ArchiveVector( &originalPlayerAngles );
	arc.ArchiveVector( &dlight_color );
	arc.ArchiveVector( &addvelocity );
	ArchiveEnum( meansofdeath, meansOfDeath_t );
	arc.ArchiveObjectPointer( ( Class ** )&m_beam );
	arc.ArchiveInteger( &projFlags );
	arc.ArchiveString( &bouncesound );
	arc.ArchiveString( &impactmarkshader );
	arc.ArchiveString( &impactmarkorientation );
	arc.ArchiveFloat( &impactmarkradius );
	arc.ArchiveString( &explosionmodel );

	arc.ArchiveSafePointer( &target );

	arc.ArchiveBool( &addownervelocity );
	arc.ArchiveBool( &drunk );
	arc.ArchiveBool( &can_hit_owner );
	arc.ArchiveBool( &remove_when_stopped );

	arc.ArchiveBool( &stick_on_touch );
	arc.ArchiveBool( &ownercontrol );
	arc.ArchiveBool( &_ownerControlLaser );
	arc.ArchiveBool( &_ownerControlUsed );
	arc.ArchiveFloat( &_controlTurnSpeed );
	arc.ArchiveBool( &triggerdetonate );

	arc.ArchiveFloat( &startTime );
	arc.ArchiveFloat( &drunkAngleModifier );

	arc.ArchiveBool( &_scaleByCharge );
	arc.ArchiveFloat( &_minScaleFromCharge );
	arc.ArchiveFloat( &_maxScaleFromCharge );
	arc.ArchiveBool( &_scaleExplosion );
	arc.ArchiveBool( &_notifyActors );
	arc.ArchiveBool( &_notShootable );
	arc.ArchiveBool( &_hitsProjectiles );

	arc.ArchiveFloat( &_minOnGroundTime );
	arc.ArchiveBool( &_onGround );
	arc.ArchiveFloat( &_startOnGroundTime );
	arc.ArchiveBool( &_heatSeek );
	arc.ArchiveBool( &_damagedSomething );

}

class Explosion : public Projectile
{
public:
	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	//--------------------------------------------------------------
	float				flash_r = 0.0f;
	float				flash_g = 0.0f;
	float				flash_b = 0.0f;
	float				flash_a = 0.0f;
	float				flash_minradius = 0.0f;
	float				flash_radius = 0.0f;
	float				flash_mintime = 0.0f;
	float				flash_time = 0.0f;
	int					flash_type = 0;
	float				radius_damage = 0.0f;
	float				radius = 0.0f;
	qboolean			constant_damage = qfalse;
	qboolean			damage_every_frame = qfalse;
	

	CLASS_PROTOTYPE( Explosion );
	
	Explosion();
	void				SetRadius( Event *ev );
	void				SetRadiusDamage( Event *ev );
	void				SetConstantDamage( Event *ev );
	void				SetDamageEveryFrame( Event *ev );
	void				SetFlash( Event *ev );
	
	void				DamageAgain( Event *ev );
	virtual void		Archive( Archiver &arc );
};

inline void Explosion::Archive( Archiver &arc )
{
	Projectile::Archive( arc );
	
	arc.ArchiveFloat( &flash_r );
	arc.ArchiveFloat( &flash_g );
	arc.ArchiveFloat( &flash_b );
	arc.ArchiveFloat( &flash_a );
	arc.ArchiveFloat( &flash_minradius );
	arc.ArchiveFloat( &flash_radius );
	arc.ArchiveFloat( &flash_mintime );
	arc.ArchiveFloat( &flash_time );
	arc.ArchiveInteger( &flash_type );
	arc.ArchiveFloat( &radius_damage );
}

qboolean MeleeAttack
(
	const Vector &pos,
	const Vector &end,
	float damage,
	Entity *attacker,
	meansOfDeath_t means_of_death,
	float attack_width,
	float attack_min_height,
	float attack_max_height,
	float knockback = 0,
	qboolean hit_dead = true,
	Container<EntityPtr>*victimlist=NULL,
	Weapon *weapon = NULL,
	bool	critical = false
);

Projectile *ProjectileAttack
(
	const Vector   &start,
	Vector         &dir,
	Entity         *owner,
	const str      &projectileModel,
	float          speedfraction,
	float		   real_speed = 0
);

void ExplosionAttack
(
	const Vector   &pos,
	Entity         *owner,
	const str      &projectileModel,
	Vector         dir = Vector( 0.0f, 0.0f, 0.0f ),
	Entity         *ignore = NULL,
	float          scale=1.0f
);

void StunAttack
(
	const Vector   &pos,
	Entity   *attacker,
	Entity   *inflictor,
	float    radius,
	float    time,
	Entity   *ignore
);

float BulletAttack
(
	const Vector	&start,
	const Vector	&dir,
	const Vector   &right,
	const Vector   &up,
	float    range,
	float  	damage,
	float    knockback,
	int      dflags,
	int      meansofdeath,
	const Vector   &spread,
	int      count,
	Entity   *owner,
	Entity   *weap = 0
);

float RadiusDamage
(
	Entity   *inflictor,
	Entity   *attacker,
	float    damage,
	Entity   *ignore,
	int      mod,
	float		radius = 0,
	float		knockback = 0,
	qboolean	constant_damage = false
);

#endif // __WEAPUTILS_H__
