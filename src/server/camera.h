//-----------------------------------------------------------------------------
//
//  $Logfile:: /Code/DLLs/game/camera.h                                      $
// $Revision:: 19                                                             $
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
// Camera.  Duh.
//

#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "g_local.h"
#include "entity.h"
#include "bspline.h"
#include "container.h"
#include "CameraPath.h"

#define CAMERA_SWITCHTIME 0.5f

#define ORBIT		( 1 << 0 )
#define START_ON	( 1 << 1 )
#define AUTOMATIC	( 1 << 2 )
#define NO_TRACE	( 1 << 3 )
#define NO_WATCH	( 1 << 4 )
#define LEVEL_EXIT	( 1 << 5 )


// CameraManager Events
extern Event EV_CameraManager_SetPath ;
extern Event EV_CameraManager_PlayPath ;
extern Event EV_CameraManager_Load ;

// Camera Events
extern Event EV_Camera_CameraThink;
extern Event EV_Camera_StartMoving;
extern Event EV_Camera_Pause;
extern Event EV_Camera_Continue;
extern Event EV_Camera_StopMoving;
extern Event EV_Camera_SetSpeed;
extern Event EV_Camera_SetFollowDistance;
extern Event EV_Camera_SetOrbitHeight;
extern Event EV_Camera_SetYaw;
extern Event EV_Camera_AbsoluteYaw;
extern Event EV_Camera_RelativeYaw;
extern Event EV_Camera_SetFOV;
extern Event SetInterpolateFOV;
extern Event EV_Camera_Orbit;
extern Event EV_Camera_Follow;
extern Event EV_Camera_Watch;
extern Event EV_Camera_WatchPath;
extern Event EV_Camera_LookAt;
extern Event EV_Camera_TurnTo;
extern Event EV_Camera_MoveToEntity;
extern Event EV_Camera_MoveToPos;
extern Event EV_Camera_NoWatch;
extern Event EV_Camera_FadeTime;
extern Event EV_Camera_Cut;
extern Event EV_Camera_LoadKFC;

class Camera;


//---------------------------------------------------------------------------
// class CameraMoveState
//---------------------------------------------------------------------------
class CameraMoveState : public Class
{
public:
	BSpline			cameraPath;


	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	//--------------------------------------------------------------
	SplinePathPtr	splinePath = nullptr;
	SplinePathPtr	currentNode = nullptr;
	SplinePathPtr	loopNode = nullptr;
	float			cameraTime = 0.0f;
	int				lastTime = 0;
	int				newTime = 0;
	qboolean		followingpath = qfalse;
	EntityPtr		followEnt = nullptr;
	EntityPtr		orbitEnt = nullptr;
	Vector			pos = Vector(0.0f, 0.0f, 0.0f);
	Vector			movedir = Vector(0.0f, 0.0f, 0.0f);    // direction of travel
	Vector			angles = Vector(0.0f, 0.0f, 0.0f);     // angles from spline camera


	void			operator=( const CameraMoveState& newstate );
	void			Evaluate( Camera* camera );
	void			Initialize( Camera* camera );
	void			DoNodeEvents( Camera* camera );
	virtual void	Archive( Archiver& arc );
};


//---------------------------------------------------------------------------
// CameraMoveState::operator=
//---------------------------------------------------------------------------
inline void CameraMoveState::operator=( const CameraMoveState& newstate )
{
	movedir = newstate.movedir;
	pos = newstate.pos;
	angles = newstate.angles;

	cameraPath = newstate.cameraPath;
	splinePath = newstate.splinePath;
	currentNode = newstate.currentNode;
	loopNode = newstate.loopNode;

	cameraTime = newstate.cameraTime;
	lastTime = newstate.lastTime;
	newTime = newstate.newTime;

	followEnt = newstate.followEnt;
	orbitEnt = newstate.orbitEnt;

	followingpath = newstate.followingpath;
}


//---------------------------------------------------------------------------
// CameraMoveState::Archive
//---------------------------------------------------------------------------
inline void CameraMoveState::Archive( Archiver& arc )
{
	Class::Archive( arc );

	arc.ArchiveVector( &pos );
	arc.ArchiveVector( &movedir );
	arc.ArchiveVector( &angles );

	cameraPath.Archive( arc );

	arc.ArchiveSafePointer( &splinePath );
	arc.ArchiveSafePointer( &currentNode );
	arc.ArchiveSafePointer( &loopNode );

	arc.ArchiveFloat( &cameraTime );
	arc.ArchiveInteger( &lastTime );
	arc.ArchiveInteger( &newTime );

	arc.ArchiveBoolean( &followingpath );

	arc.ArchiveSafePointer( &followEnt );
	arc.ArchiveSafePointer( &orbitEnt );
}


//---------------------------------------------------------------------------
// class CameraWatchState
//---------------------------------------------------------------------------
class CameraWatchState : public Class
{
public:
	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	//--------------------------------------------------------------
	Vector			watchAngles = Vector(0.0f, 0.0f, 0.0f);
	EntityPtr		watchEnt = nullptr;
	qboolean		watchNodes = qfalse;
	qboolean		watchPath = qfalse;


	void			Evaluate( const Camera* camera, const CameraMoveState* move );
	void			Initialize( Camera* camera );
	virtual void	Archive( Archiver& arc );
};



//---------------------------------------------------------------------------
// CameraWatchState::Archive
//---------------------------------------------------------------------------
inline void CameraWatchState::Archive( Archiver& arc )
{
	Class::Archive( arc );

	arc.ArchiveVector( &watchAngles );
	arc.ArchiveSafePointer( &watchEnt );
	arc.ArchiveBoolean( &watchNodes );
	arc.ArchiveBoolean( &watchPath );
}


//---------------------------------------------------------------------------
// class CameraState
//---------------------------------------------------------------------------
class CameraState : public Class
{
public:
	CameraMoveState		move;
	CameraWatchState	watch;


	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	//--------------------------------------------------------------
	float				fov = 0.0f;


	void				Evaluate( Camera * camera );
	void				Initialize( Camera * camera );
	virtual void		Archive( Archiver &arc );
};


//---------------------------------------------------------------------------
// CameraState::Archive
//---------------------------------------------------------------------------
inline void CameraState::Archive( Archiver& arc )
{
	Class::Archive( arc );

	move.Archive( arc );
	watch.Archive( arc );
	arc.ArchiveFloat( &fov );
}


//---------------------------------------------------------------------------
// class Camera
//---------------------------------------------------------------------------
class Camera : public Entity
{
private:
	friend class	CameraState;
	friend class	CameraWatchState;
	friend class	CameraMoveState;
	//
	// follow parameters
	//
	Container<str>	automatic_states;


	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	//--------------------------------------------------------------
	float			follow_yaw = 0.0f;
	qboolean		follow_yaw_fixed = qfalse;
	float			follow_dist = 0.0f;
	int				follow_mask = 0;

	// camera speed
	float			camera_speed = 0.0f;
	// current camera fov
	float			camera_fov = 0.0f;
	// orbit height
	float			orbit_height = 0.0f;
	// orbit_dotrace
	qboolean		orbit_dotrace = qfalse;
	// whether or not auto calculate fov, a non-zero value means yes
	float			auto_fov = 0.0f;

	// automatic variables
	float			automatic_startTime = 0.0f;
	float			automatic_stopTime = 0.0f;
	float			automatic_radius = 0.0f;
	float			automatic_maxFOV = 0.0f;
	qboolean		automatic_active = qfalse;

	// members supporting new key-framed camera system
	CameraPath*		newCameraPath = nullptr; // NULL if not currently in use (fall back to old system)
	float			newCameraPathSeconds = 0.0f; // the number of seconds the camera is into its key-framed path


protected:
	CameraState		currentstate;
	CameraState		newstate;


	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	//--------------------------------------------------------------
	float			watchTime = 0.0f;  // if non-zero, camera view is transitioning
	float			followTime = 0.0f; // if non-zero, camera position is tranisitioning
	float			fovTime = 0.0f;    // if non-zero, fov is being lerped

	float			fadeTime = 0.0f; // time to transition over
	float			fovFadeTime = 0.0f; // time for fov transition
	float			followFadeTime = 0.0f; // time for fov transition
	float			watchFadeTime = 0.0f; // time for fov transition

	qboolean		showcamera = qfalse;


	str				nextCamera;
	str				thread;

	void			SetupCamera( Event* ev );
	void			CameraThink( Event* ev );
	void			EvaluateCameraKeyFramePath( void );
	void			CreateOrbit( const Vector& pos, float radius, const Vector& forward, const Vector& left );
	void			CreatePath( SplinePath* path, splinetype_t type );
	void			UpdateStates( void );
	Vector			CalculatePosition( void );
	Vector			CalculateOrientation( void	);
	float			CalculateFov( void );
	virtual void	bind( Entity* master, qboolean use_my_angles = false );
	virtual void	unbind( void );

public:
	CLASS_PROTOTYPE( Camera );

	Camera();
	void			Stop( void );
	void			FollowPath( SplinePath* path, qboolean loop, Entity* watch );
	void			Orbit( Entity* ent, float dist, Entity* watch, float yaw_offset = 0, qboolean dotrace = true );
	void			FollowEntity( Entity* ent, float dist, int mask, Entity* watch = NULL );
	void			Watch( const str& watch, float time );
	void			Watch( Entity* ent, float time );
	void			SetFOV( float fov, float time	);
	void			StartMoving( Event* ev );
	void			StopMoving( Event* ev );
	void			Pause( Event* ev );
	void			Continue( Event* ev );
	void			SetAnglesEvent( Event* ev );
	void			SetSpeed( Event* ev );
	void			SetFollowDistance( Event* ev );
	void			SetOrbitHeight( float height );
	void			SetOrbitHeight( Event* ev );
	void			SetFollowYaw( Event* ev );
	void			AbsoluteYaw( Event* ev );
	void			RelativeYaw( Event* ev );
	void			SetFOV( Event* ev );
	void			SetInterpolateFOV( Event* ev );
	void			LoadKFC( Event* ev );
	void			OrbitEvent( Event* ev );
	void			FollowEvent( Event* ev );
	void			WatchEvent( Event* ev );
	void			WatchPathEvent( Event* ev );
	void			WatchNodesEvent( Event* ev );
	void			NoWatchEvent( Event* ev );
	void			LookAt( Event* ev );
	void			MoveToEntity( Event* ev );
	void			MoveToPos( Event* ev );
	void			Cut( Event* ev );
	void			FadeTime( Event* ev );
	void			TurnTo( Event* ev );
	void			SetNextCamera( Event* ev );
	void			SetThread( Event* ev );
	float			CalculateScore( Entity* player, const str& state );
	float			AutomaticStart( Entity* player );
	float			AutomaticStop( Entity* player );
	qboolean		IsAutomatic( void );
	qboolean		IsLevelExit( void );
	void			SetAutoStateEvent( Event* ev );
	void			SetAutoRadiusEvent( Event* ev );
	void			SetAutoStartTimeEvent( Event* ev );
	void			SetAutoStopTimeEvent( Event* ev );
	void			SetMaximumAutoFOVEvent( Event* ev );
	void			SetAutoActiveEvent( Event* ev );
	str&			NextCamera( void );
	str&			Thread( void );
	float			Fov( void );
	void			SetPlaybackOffsets( const float yawOffsetDegrees, const Vector& originOffset );
	float			GetPathLengthInSeconds( void );
	float			GetCameraTime( void );
	void			Reset( const Vector& org, const Vector& ang );
	virtual void	Archive( Archiver& arc );
};


//---------------------------------------------------------------------------
// Camera::Archive
//---------------------------------------------------------------------------
inline void Camera::Archive( Archiver& arc )
{
	Entity::Archive( arc );

	arc.ArchiveFloat( &follow_yaw );
	arc.ArchiveBoolean( &follow_yaw_fixed );
	arc.ArchiveFloat( &follow_dist );
	arc.ArchiveInteger( &follow_mask );

	arc.ArchiveFloat( &camera_speed );
	arc.ArchiveFloat( &camera_fov );
	arc.ArchiveFloat( &orbit_height );
	arc.ArchiveBoolean( &orbit_dotrace );
	arc.ArchiveFloat( &auto_fov );

	arc.ArchiveFloat( &automatic_startTime );
	arc.ArchiveFloat( &automatic_stopTime );
	arc.ArchiveFloat( &automatic_radius );
	arc.ArchiveFloat( &automatic_maxFOV );
	arc.ArchiveBoolean( &automatic_active );

	automatic_states.Archive( arc );

	if ( arc.Saving() )
	{
		bool cameraValid;

		if ( newCameraPath )
			cameraValid = true;
		else
			cameraValid = false;

		arc.ArchiveBool( &cameraValid );

		if ( cameraValid )
			arc.ArchiveObject( newCameraPath );
	}
	else
	{
		bool cameraValid;

		arc.ArchiveBool( &cameraValid );

		if ( cameraValid )
		{
			newCameraPath = new CameraPath();

			arc.ArchiveObject( newCameraPath );
		}
		else
		{
			newCameraPath = NULL;
		}
	}

	arc.ArchiveFloat( &newCameraPathSeconds );

	// currentstate
	currentstate.Archive( arc );
	// newstate
	newstate.Archive( arc );

	arc.ArchiveFloat( &watchTime );
	arc.ArchiveFloat( &followTime );
	arc.ArchiveFloat( &fovTime );
	arc.ArchiveFloat( &fadeTime );
	arc.ArchiveFloat( &fovFadeTime );
	arc.ArchiveFloat( &followFadeTime );
	arc.ArchiveFloat( &watchFadeTime );

	arc.ArchiveString( &nextCamera );
	arc.ArchiveString( &thread );

	arc.ArchiveBoolean( &showcamera );

	if ( arc.Loading() )
	{
		if ( spawnflags & AUTOMATIC )
		{
			level.AddAutomaticCamera( this );
		}
	}
}


void SetCamera( Entity *ent, float switchTime );
Entity * GetWatchEntity( const str &watch );

typedef SafePtr<Camera> CameraPtr;


//---------------------------------------------------------------------------
// class CameraManager
//---------------------------------------------------------------------------
class CameraManager : public Listener
{
protected:
	Container<str>	pathList;
	BSpline			cameraPath;
	str				pathName;


	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	//--------------------------------------------------------------
	SplinePathPtr	path = nullptr;
	SplinePathPtr	current = nullptr;
	float			speed = 0.0f;
	int				watch = 0;
	CameraPtr		cam = nullptr;
	qboolean		cameraPath_dirty = qfalse;
	float			playbackStartTime = 0.0f;
	qboolean		isPreviewPlaybackRunning = qfalse;

	
	void			NewPath( Event* ev );
	void			SetPath( Event* ev );
	void			SetTargetName( Event* ev );
	void			SetTarget( Event* ev );
	void			SetThread( Event* ev );
	void			AddPoint( Event* ev );
	void			ReplacePoint( Event* ev );
	void			DeletePoint( Event* ev );
	void			MovePlayer( Event* ev );
	void			NextPoint( Event* ev );
	void			PreviousPoint( Event* ev );
	void			ShowingPath( Event* ev );
	void			ShowPath( Event* ev );
	void			HidePath( Event* ev );
	void			StopPlayback( Event* ev );
	void			PlayPath( Event* ev );
	void			PlayKFCPath( Event* ev );
	void			DestroyKFCPath( Event* ev );
	void			LoopPath( Event* ev );
	void			Watch( Event* ev );
	void			NoWatch( Event* ev );
	void			Fov( Event* ev );
	void			FadeTime( Event* ev );
	void			Speed( Event* ev );
	void			Save( Event* ev );
	void			Load( Event* ev );
	void			SaveMap( Event* ev );
	void			UpdateEvent( Event* ev );
	void			NextPath( Event* ev );
	void			PreviousPath( Event* ev );
	void			RenamePath( Event* ev );
	void			ShowPath( void );
	void			UpdateUI( void );
	void			SetPathName( const str& name );
	void			SavePath( const str& pathName );

public:
	CLASS_PROTOTYPE( CameraManager );

	CameraManager();
	void			SetPath( const str& pathName );
	virtual void	Archive( Archiver& arc );
};



//---------------------------------------------------------------------------
// CameraManager::Archive
//---------------------------------------------------------------------------
inline void CameraManager::Archive( Archiver& arc )
{
	Listener::Archive( arc );

	pathList.Archive( arc );

	// Don't need to archive cameraPath because it gets rebuilt ayways
	//BSpline			cameraPath;

	// no need to archive the cameraPath
	arc.ArchiveSafePointer( &path );
	arc.ArchiveSafePointer( &current );
	arc.ArchiveFloat( &speed );
	arc.ArchiveInteger( &watch );
	arc.ArchiveString( &pathName );
	arc.ArchiveSafePointer( &cam );

	// make sure the cameraPath gets rebuilt
	cameraPath_dirty = true;
}


extern CameraManager CameraMan;

#endif /* camera.h */
