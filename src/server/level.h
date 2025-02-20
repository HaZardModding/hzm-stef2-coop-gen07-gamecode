//-----------------------------------------------------------------------------
//
//  $Logfile:: /Code/DLLs/game/level.h                                       $
// $Revision:: 31                                                             $
//     $Date:: 3/02/03 8:36p                                                  $
//
// Copyright (C) 2002 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source is may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
//
//
// DESCRIPTION:
//

#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "g_local.h"
#include "container.h"

class CThread;
class Camera;
class HelperNodeController;
class Earthquake;

//
// this structure is cleared as each map is entered
// it is read/written to the level.sav file for savegames
//

enum fadetype_t      { fadein, fadeout };
enum fadestyle_t     { alphablend, additive };
enum letterboxdir_t  { letterbox_in, letterbox_out };

class Level : public Class
{
	private:
		Container< SafePtr<Earthquake> >	_earthquakes;

		int									_totalEnemiesSpawned;

	public:
		const char		*current_map;
		const char		*current_entities;      
		str				level_name;	            // the descriptive name (Outer Base, etc)
		str				mapname;		            // the server name (base1, etc)
		str				spawnpoint;             // targetname of spawnpoint
		str				nextmap;		            // go here when fraglimit is hit						
		str				game_script;
		str				current_soundtrack;
		str				saved_soundtrack;
		str				_playerDeathThread;
		//
		// list of automatic_cameras on the level
		//
		Container<Camera*>	automatic_cameras;
		
		// FIXME - r	emove this later when it is passed in the event.
		trace_t			impact_trace;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		int				spawn_entnum = 0;
		int				currentInstanceNumber = 0;
		int				spawnflags = 0;
						
		int				framenum = 0;
		int				inttime = 0;                // level time in millisecond integer form
		float			time = 0.0f;
		float			timeInLevel = 0.0f;
		float			frametime = 0.0f;
		float			fixedframetime = 0.0f;         // preset frame time based on sv_fps
		int				startTime = 0;				   // level.time the map was started
						
		qboolean		restart = qfalse;                // set true when game loop should restart
		qboolean		started = qfalse;				// set when the level is started.
						
		// used for		cinematics
		qboolean		playerfrozen = qfalse;
						
		// intermiss	ion state
		float			intermissiontime = 0.0f;			// time the intermission was started
		int				exitintermission = 0;
		float			intermission_advancetime = 0.0f;
		bool			_showIntermission = false;
		bool			_saveOrientation = false;
						
		gentity_s		*next_edict = nullptr;				// Used to keep track of the next edict to process in G_RunFrame
						
		int				total_secrets = 0;
		int				found_secrets = 0;
		int				total_specialItems = 0;
		int				found_specialItems = 0;
						
		qboolean		cinematic = qfalse;
						
		qboolean		ai_on = qfalse;
						
		qboolean		mission_failed = qfalse;
		qboolean		died_already = qfalse;
						
		qboolean		near_exit = qfalse;
						
		// Blending		color for water, light volumes,lava
		Vector			water_color = Vector(0.0f, 0.0f, 0.0f);
		float			water_alpha = 0.0f;
						
		Vector			slime_color = Vector(0.0f, 0.0f, 0.0f);
		float			slime_alpha = 0.0f;
						
		Vector			lava_color = Vector(0.0f, 0.0f, 0.0f);
		float			lava_alpha = 0.0f;

		CThread			*consoleThread = nullptr;

		Vector			m_fade_color = Vector(0.0f, 0.0f, 0.0f);
		float			m_fade_alpha = 0.0f;
		float			m_fade_time = 0.0f;
		float			m_fade_time_start = 0.0f;
		fadetype_t		m_fade_type = fadein;
		fadestyle_t		m_fade_style = alphablend;

		float			m_letterbox_fraction = 0.0f;
		float			m_letterbox_time = 0.0f;
		float			m_letterbox_time_start = 0.0f;
		letterboxdir_t	m_letterbox_dir = letterbox_in;

		bool			_cleanup = false;

		HelperNodeController* hNodeController = nullptr;

		Vector         m_intermission_origin = Vector(0.0f, 0.0f, 0.0f);
		Vector         m_intermission_angle = Vector(0.0f, 0.0f, 0.0f);



		CLASS_PROTOTYPE( Level );

                     Level();
					 ~Level();

      void           Init( void );
      void           CleanUp( qboolean restart );

      void           ResetEdicts( void );
      gentity_t      *AllocEdict( Entity *ent );
      void           FreeEdict( gentity_t *ed );
      void           InitEdict( gentity_t *e );

      void           Start( void );
      void           Restart( void );
      void           PlayerRestart( void );
	  void			 update(int levelTime, int frameTime);
      void           Precache( void );
      void			   FindTeams( void );

      void           SpawnEntities( const char *mapname, const char *entities, int levelTime );
      void           NewMap( const char *mapname, const char *entities, int levelTime );
	  void			postLoad( void );
	  void			postSublevelLoad( const char *spawnPosName );

      qboolean       inhibitEntity( int spawnflags );
      void           setSkill( int value );
		int            getSkill( void );
      void           setTime( int levelTime, int frameTime );
		void			   SetGameScript( const char *scriptname );
      void           AddAutomaticCamera( Camera * cam );
	   virtual void   Archive( Archiver &arc );
	   
	   void			loadLevelStrings( void );

	void						addEarthquake( Earthquake *earthquake );
	void						removeEarthquake( Earthquake *earthquake );
	float						getEarthquakeMagnitudeAtPosition( const Vector &origin );

	void			enemySpawned( Entity *enemy );		
      //1st Playable hack
	  void			SetIntermissionAdvanceTime(float);
	  void			EndIntermission();

	  void			setPlayerDeathThread( const str &threadName );
	  str			getPlayerDeathThread( void );
	};

extern Level level;

#endif /* !__LEVEL_H__ */
