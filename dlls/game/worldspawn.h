//-----------------------------------------------------------------------------
//
//  $Logfile:: /Code/DLLs/game/worldspawn.h                                  $
// $Revision:: 28                                                             $
//   $Author:: Steven                                                         $
//     $Date:: 3/02/03 4:13p                                                  $
//
// Copyright (C) 2002 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source is may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
//
//
// DESCRIPTION:
// Base class for worldspawn objects.  This should be subclassed whenever
// a DLL has new game behaviour that needs to be initialized before any other
// entities are created, or before any entity thinks each frame.  Also controls
// spawning of clients.
//


//--------------------------------------------------------------
// COOP Generation 7.000 - Added coop specific events - chrissstrahl
//--------------------------------------------------------------
#ifdef ENABLE_COOP
extern Event EV_World_coop_loadMap;
#endif


class TargetList;
class DynamicLightInfo;
class WindInfo;
class World;

#ifndef __WORLDSPAWN_H__
#define __WORLDSPAWN_H__

#include "entity.h"
#include <qcommon/qfiles.h>

typedef enum 
{
	WORLD_PHYSICS_MAXSPEED,
	WORLD_PHYSICS_AIRACCELERATE,
	WORLD_PHYSICS_GRAVITY,

	WORLD_PHYSICS_TOTAL_NUMBER
} WorldPhysicsVarTypes;

class TargetList : public Class
{
public:
	CLASS_PROTOTYPE( TargetList );
	
	Container<Entity *>		list;
	str						targetname;


	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	//--------------------------------------------------------------
	int						index = 0;
	

						TargetList();
						TargetList( const str &tname );
						~TargetList();
	
	void				AddEntity( Entity * ent );
	void				RemoveEntity( Entity * ent );
	Entity				*GetNextEntity( Entity * ent );
	int					GetNumTargets()						{ return list.NumObjects() ; }
	
	virtual void		Archive( Archiver &arc );
};

class DynamicLightInfo
{
public:
	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	//--------------------------------------------------------------
	float		defaultIntensity = 0.0f;
	float		intensity = 0.0f;
	float		start_intensity = 0.0f;
	float		end_intensity = 0.0f;
	float		start_fade_time = 0.0f;
	float		fade_time = 0.0f;
	int			current_lightstyle_position = 0;
	qboolean	stop_at_end = qfalse;	


	str			lightstyle;

	
					DynamicLightInfo();
	virtual void	Archive( Archiver &arc );
};

inline void DynamicLightInfo::Archive( Archiver &arc )
{
	arc.ArchiveFloat( &defaultIntensity );

	arc.ArchiveFloat( &intensity );
	arc.ArchiveFloat( &start_intensity );
	arc.ArchiveFloat( &end_intensity );

	arc.ArchiveFloat( &start_fade_time );
	arc.ArchiveFloat( &fade_time );

	arc.ArchiveString( &lightstyle );
	arc.ArchiveInteger( &current_lightstyle_position );
	arc.ArchiveBoolean( &stop_at_end );
}

class WindInfo
{
public:
	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	//--------------------------------------------------------------
	Vector		direction = Vector(0.0f, 0.0f, 0.0f);
	float		intensity = 0.0f;
	float		start_intensity = 0.0f;
	float		end_intensity = 0.0f;
	float		min_intensity = 0.0f;
	float		max_intensity = 0.0f;
	float		start_fade_time = 0.0f;
	float		fade_time = 0.0f;
	float		gust_time = 0.0f;
	float		max_change = 0.0f;
	

					WindInfo();
	virtual void	Archive( Archiver &arc );
};

inline void WindInfo::Archive( Archiver &arc )
{
	arc.ArchiveVector( &direction );
	arc.ArchiveFloat( &intensity );
	arc.ArchiveFloat( &start_intensity );
	arc.ArchiveFloat( &end_intensity );

	arc.ArchiveFloat( &min_intensity );
	arc.ArchiveFloat( &max_intensity );

	arc.ArchiveFloat( &start_fade_time );
	arc.ArchiveFloat( &fade_time );

	arc.ArchiveFloat( &gust_time );

	arc.ArchiveFloat( &max_change );
}

class WeatherInfo
{
public:
	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	//--------------------------------------------------------------
	weather_t	type = WEATHER_NONE;
	int			intensity = 0;
	

					WeatherInfo();
	virtual void	Archive( Archiver &arc );
};

inline void WeatherInfo::Archive( Archiver &arc )
{
	ArchiveEnum( type, weather_t );
	arc.ArchiveInteger( &intensity );
}


//--------------------------------------------------------------
// COOP Generation 7.000 - coop specific script function - chrissstrahl
//--------------------------------------------------------------
#ifdef ENABLE_COOP
	extern Event EV_World_coop_getPhysicsVar;
#endif


class World : public Entity
{
#ifdef ENABLE_COOP
	//--------------------------------------------------------------
	// COOP Generation 7.000 - coop specific function - chrissstrahl
	//--------------------------------------------------------------
public:
	void coop_loadMap(Event* ev);
	void coop_getPhysicsVar(Event* ev);
#endif




private:
	Container<TargetList *>		targetList;
	Container<str>				_brokenThings;
	Container<str>				_availableViewModes;


	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	//--------------------------------------------------------------
	qboolean					world_dying = qfalse;
	

public:
	CLASS_PROTOTYPE( World );
	
	str							skipthread;
	WindInfo					wind;
	WeatherInfo					weather;


	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	//--------------------------------------------------------------
	float						farplane_distance = 0.0f;
	Vector						farplane_color = Vector(0.0f, 0.0f, 0.0f);
	qboolean					farplane_cull = qfalse;
	qboolean					farplane_fog = qfalse;
	qboolean					terrain_global = qfalse;
	float						terrain_global_min = 0.0f;
	float						entity_fade_dist = 0.0f;
	DynamicLightInfo			dynamic_lights[ MAX_LIGHTING_GROUPS ];
	float						time_scale = 0.0f;
	float						sky_alpha = 0.0f;
	qboolean					sky_portal = qfalse;
	// Movement info
	float						_physicsInfo[WORLD_PHYSICS_TOTAL_NUMBER] = {0.0f};
	bool						_canShakeCamera = false;
	

	World();
	~World();
	
	void			FreeTargetList( void );
	//TargetList     *GetTargetList( const str &targetname );
	TargetList		*GetTargetList( const str &targetname, qboolean createnew = true );
	TargetList		*GetTargetList( int index );
	void			AddTargetEntity( const str &targetname, Entity * ent );
	void			RemoveTargetEntity( const str &targetname, Entity * ent );
	Entity			*GetNextEntity( const str &targetname, Entity * ent );
	void			SetSoundtrack( Event *ev );
	void			SetSkipThread( Event *ev );
	void			SetNextMap( Event *ev );
	void			SetMessage( Event *ev );
	void			SetScript( Event *ev );
	void			SetWaterColor( Event *ev );
	void			SetWaterAlpha( Event *ev );
	void			SetSlimeColor( Event *ev );
	void			SetSlimeAlpha( Event *ev );
	void			SetLavaColor( Event *ev );
	void			SetLavaAlpha( Event *ev );
	void			SetFarPlane_Color( Event *ev );
	void			SetFarPlane_Cull( Event *ev );
	void			SetFarPlane_Fog( Event *ev );
	void			SetFarPlane( Event *ev );
	void			SetTerrainGlobal( Event *ev );
	void			SetTerrainGlobalMin( Event *ev );
	void			SetEntityFadeDist( Event *ev );
	void			SetSkyAlpha( Event *ev );
	void			SetSkyPortal( Event *ev );
	void			UpdateConfigStrings( void );
	void			UpdateFog( void );
	void			UpdateTerrain( void );
	void			UpdateEntityFadeDist( void );
	void			UpdateDynamicLights( void );
	void			UpdateWindDirection( void );
	void			UpdateWindIntensity( void );
	void			UpdateWeather( void );
	void			UpdateTimeScale( void );
	void			UpdateSky( void );
	
	void			SetLightIntensity( Event *ev );
	void			SetLightDefaultIntensity( Event *ev );
	void			SetLightFade( Event *ev );
	void			SetLightLightstyle( Event *ev );
	void			GetLightIntensity( Event *ev );
	
	void			SetWindDensity( Event *ev );
	void			SetWindDirection( Event *ev );
	void			SetWindIntensity( Event *ev );
	void			SetWindGust( Event *ev );
	void			SetWindFade( Event *ev );
	void			SetWindMinMax( Event *ev );
	void			SetWeather( Event *ev );
	
	int				worldPhysicsVarNameToIndex( const char *varName );
	void			setPhysicsVar( Event *ev );
	void			setPhysicsVar( const char *varName, float varValue );
	float			getPhysicsVar( const char *varName );
	float			getPhysicsVar( int index );
	
	void			SetTimeScale( Event *ev );
	
	int				findBrokenThing( const char *name );
	void			addBrokenThing( const char *name );
	void			removeBrokenThing( const char *name );
	bool			isThingBroken( const char *name );
	void			addBrokenThing( Event *ev );
	void			removeBrokenThing( Event *ev );
	void			freeAllBrokenThings( void );
	
	void			addAvailableViewMode( Event *ev );
	void			removeAvailableViewMode( Event *ev );
	void			clearAvailableViewModes( Event *ev );
	bool			isAnyViewModeAvailable( void );
	bool			isViewModeAvailable( const char *name );

	void			setCanShakeCamera( Event *ev );
	bool			canShakeCamera( void );
	
	void			Think( void );
	
	virtual void	Archive( Archiver &arc );
};

inline void TargetList::Archive( Archiver &arc )
{
	int num, i;

	Class::Archive( arc );

	if ( arc.Saving() )
		num = list.NumObjects();

	arc.ArchiveInteger( &num );

	if ( arc.Loading() ) 
	{
		list.ClearObjectList();
		list.Resize( num );
	}

	for ( i = 1; i <= num; i++ )
	{
		arc.ArchiveObjectPointer( ( Class ** )list.AddressOfObjectAt( i ) );
	}

	arc.ArchiveString( &targetname );
	arc.ArchiveInteger( &index );
}

inline TargetList *World::GetTargetList( int index )
{
	return targetList.ObjectAt( index );
}

typedef SafePtr<World> WorldPtr;
extern WorldPtr world;

#endif /* worldspawn.h */
