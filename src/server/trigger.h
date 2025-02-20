//-----------------------------------------------------------------------------
//
//  $Logfile:: /Code/DLLs/game/trigger.h                                     $
// $Revision:: 35                                                             $
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
// Environment based triggers.
//

#ifndef __TRIGGER_H__
#define __TRIGGER_H__

#include "g_local.h"
#include "animate.h"
#include "groupcoordinator.hpp"

class ScriptMaster;

extern Event EV_Trigger_ActivateTargets;
extern Event EV_Trigger_SetWait;
extern Event EV_Trigger_SetDelay;
extern Event EV_Trigger_SetCount;
extern Event EV_Trigger_SetMessage;
extern Event EV_Trigger_SetNoise;
extern Event EV_Trigger_SetKey;
extern Event EV_Trigger_SetThread;
extern Event EV_Trigger_Effect;
extern Event EV_Trigger_StartThread;
extern Event EV_Trigger_SetKey;
extern Event EV_Trigger_SetTriggerable;
extern Event EV_Trigger_SetNotTriggerable;
extern Event EV_Trigger_SetDestructible;


#define TRIGGER_PLAYERS			4
#define TRIGGER_MONSTERS		8
#define TRIGGER_PROJECTILES		16
#define TRIGGER_SCRIPTSLAVE		32

#define CHECK_TIME            .5f

class Trigger : public Entity
	{
	protected:
		str		         noise;
		str			      message;
		str               key;
		str               thread;
		Container<int>    entList;


	  //--------------------------------------------------------------
	  // GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	  //--------------------------------------------------------------
		float			      wait = 0.0f;
		float			      delay = 0.0f;
		float			      trigger_time = 0.0f;
		qboolean		      triggerActivated = qfalse;
		int			      count = 0;
		EntityPtr	      activator = nullptr;
		int			      respondto = 0;
      qboolean          useTriggerDir = qfalse;
      float             triggerCone = 0.0f;
      Vector            triggerDir = Vector(0.0f, 0.0f, 0.0f);
      float             triggerDirYaw = 0.0f;
		qboolean		      triggerable = qfalse;
      qboolean          removable = qfalse;  // if count is 0, should this be removed?
      qboolean          edgeTriggered = qfalse; // if true, trigger only triggers when entering trigger, not when standing in it
      int               multiFaceted = 0; // if 0, it isn't. if 1 it is N/S oriented, if 2 it is E/W oriented
	  bool				destructible = false;
	  int				instantdeath = 0; // set this to a MoD to have a trigger die instantly from a damage type
	  bool				triggerondamage = 0;
	  bool				triggerondeath = 0;


	  // protected methods
      void              CheckEntList(Event *ev);
      void              AddOtherToEntList(Entity *other);    
      qboolean          IsEntityInBoundingBox(Entity* ent);
	  bool				CanTrigger();
	  void				SetTakeDamage(bool cantakedamage);

	public:
      CLASS_PROTOTYPE( Trigger );

						      Trigger();
		virtual		      ~Trigger();


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Weapon accuracy bug, not counting destructibles. - chrissstrahl
		//--------------------------------------------------------------
		bool				GetDestructible(void);
		bool				GetTriggerOnDamage(void);


      // override this to allow objects other than players, projectiles, and monsters to activate the trigger
      virtual qboolean  respondTo( Entity *other );

      // override this to redirect messages to an entity other than the one who triggered it
      virtual Entity    *getActivator( Entity *other );

      void              SetModelEvent( Event *ev );
		void			      Touch( Event *ev );
		void			      EventSetWait( Event *ev );
		void			      EventSetDelay( Event *ev );
		void			      EventSetCount( Event *ev );
		void			      EventSetKey( Event *ev );
		void			      EventSetHealth( Event *ev );
		void			EventSetDestructible( Event *ev );
		void			EventSetInstantDeath( Event *ev );
		void			EventSetTriggerOnDamage( Event *ev );
		void			EventSetTriggerOnDeath( Event *ev );
      void              EventSetThread( Event *ev );
	  void				EventGetLastActivatingEntity( Event *ev );
      void              SetTriggerDir( Event *ev );
      void              SetTriggerable( Event *ev );
      void              SetNotTriggerable( Event *ev );
      void              SetMultiFaceted( Event *ev );
      void              SetEdgeTriggered( Event *ev );
      void              SetTriggerCone( Event *ev );

		void			      EventSetMessage( Event *ev );
		void			      SetMessage( const char *message );
		str               &Message( void );

		void			      EventSetNoise( Event *ev );
		void			      SetNoise( const char *text );
		str               &Noise( void );

		void			setActivatingEntity( Entity *activatingEntity );

      void              SetTriggerDir( float angle );
      Vector            GetTriggerDir( void );
      qboolean          UsingTriggerDir( void );

      void              SetMultiFaceted( int newFacet );
      void              SetEdgeTriggered( qboolean newEdge );

      int               GetMultiFaceted( void );
      qboolean          GetEdgeTriggered( void );

	  void				HandleDamage( Event *ev );
	  void				HandleKilled( Event *ev );

      void              StartThread( Event *ev );
		void		         TriggerStuff( Event *ev );
		void			      ActivateTargets( Event *ev );
      virtual void      Archive( Archiver &arc );
	};

inline void Trigger::Archive
	(
	Archiver &arc
	)
   {
   Entity::Archive( arc );

   arc.ArchiveFloat( &wait );
   arc.ArchiveFloat( &delay );
   arc.ArchiveFloat( &trigger_time );
   arc.ArchiveBoolean( &triggerActivated );
   arc.ArchiveInteger( &count );
   arc.ArchiveString( &noise );
   if ( arc.Loading() )
      {
      SetNoise( noise.c_str() );
      }
   arc.ArchiveString( &message );
   arc.ArchiveString( &key );
   arc.ArchiveString( &thread );
   arc.ArchiveSafePointer( &activator );
   arc.ArchiveInteger( &respondto );
   arc.ArchiveBoolean( &useTriggerDir );
   arc.ArchiveFloat( &triggerCone );
   arc.ArchiveVector( &triggerDir );
   arc.ArchiveFloat( &triggerDirYaw );
   arc.ArchiveBoolean( &triggerable );
   arc.ArchiveBoolean( &removable );
   arc.ArchiveBoolean( &edgeTriggered );
   arc.ArchiveInteger( &multiFaceted );

   entList.Archive( arc );
   arc.ArchiveBool( &destructible );
   arc.ArchiveInteger( &instantdeath );
   arc.ArchiveBool( &triggerondamage );
   arc.ArchiveBool( &triggerondeath );
   }

class LevelInteractionTrigger : public Trigger
   {
   private:
	public:
		CLASS_PROTOTYPE( LevelInteractionTrigger );

		LevelInteractionTrigger();
		virtual qboolean respondTo( Entity *other );
		virtual void     Archive( Archiver &arc );
	};

inline void LevelInteractionTrigger::Archive
   (
	Archiver &arc
	)

	{
   Trigger::Archive( arc );
	}

class WorkTrigger;
typedef SafePtr<WorkTrigger> WorkTriggerPtr;

class WorkTrigger : public Trigger
   {
   private:
		str _animation;
		Container<str> _allowedWorkers;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
      float _time = 0.0f;
      qboolean   _reserved = qfalse;
      int        _currentEnt = 0;

      

	public:
		CLASS_PROTOTYPE( WorkTrigger );

		WorkTrigger();
		
		void SetAnimation( Event *ev );
		str  GetAnimation();

      void SetTime( Event *ev );
      float GetTime();
      
      void Reserve(int entNum );
      void Release();      

      void AddWorker( Event *ev );
      qboolean isAllowedToWork( const str &name , int entNum );
      qboolean isReserved();


      virtual void     Archive( Archiver &arc );

	};

inline void WorkTrigger::Archive ( Archiver &arc )
{
	Trigger::Archive( arc );

	arc.ArchiveString( &_animation );
	arc.ArchiveFloat( &_time );
	arc.ArchiveBoolean( &_reserved );
	arc.ArchiveInteger( &_currentEnt );

	_allowedWorkers.Archive( arc );
}

class TouchField : public Trigger
	{
	private:
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		Event				*ontouch = nullptr;
		EntityPtr      owner = nullptr;


	public:
      CLASS_PROTOTYPE( TouchField );

		TouchField();
		virtual void	Setup( Entity *ownerentity, const Event &ontouch, const Vector &min, const Vector &max, int respondto );
		void				SendEvent( Event *ev );
      virtual void   Archive(	Archiver &arc );
	};

inline void TouchField::Archive
	(
	Archiver &arc
	)
   {
   Trigger::Archive( arc );

   arc.ArchiveEventPointer( &ontouch );
   arc.ArchiveSafePointer( &owner );
   }

typedef SafePtr<TouchField> TouchFieldPtr;

class TriggerOnce : public Trigger
	{
	public:
      CLASS_PROTOTYPE( TriggerOnce );
		TriggerOnce();
	};

typedef enum
   {
   VAR_INCREMENT,
   VAR_DECREMENT,
   VAR_TOGGLE,
   VAR_NORMAL
   } variableType_t;

class TriggerSetVariable : public Trigger
	{
	private:
      str            variableName;
      str            variableValue;


	  //--------------------------------------------------------------
	  // GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	  //--------------------------------------------------------------
      variableType_t variableType = VAR_NORMAL;


	public:
      CLASS_PROTOTYPE( TriggerSetVariable );

		TriggerSetVariable();

      void           SetVariableName( Event * ev );
      void           SetVariableValue( Event * ev );
      void           SetVariable( Event * ev );
      virtual void   Archive(	Archiver &arc );
	};

inline void TriggerSetVariable::Archive
	(
	Archiver &arc
	)
   {
   Trigger::Archive( arc );

   arc.ArchiveString( &variableName );
   arc.ArchiveString( &variableValue );
   ArchiveEnum( variableType, variableType_t );
   }

class TriggerRelay : public Trigger
	{
	public:
      CLASS_PROTOTYPE( TriggerRelay );

		TriggerRelay();
	};

class TriggerSecret : public TriggerOnce
	{
	public:
      CLASS_PROTOTYPE( TriggerSecret );

				TriggerSecret();
		void	FoundSecret( Event *ev );
		void	Activate( Event *ev );
	};

class TriggerPush : public Trigger
	{
	protected:


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
      float    speed = 0.0f;


	public:
      CLASS_PROTOTYPE( TriggerPush );

					TriggerPush();
		void		Push( Event *ev );
		void		SetPushDir( Event *ev );
		void		SetPushSpeed( Event *ev );
      virtual void Archive(	Archiver &arc );
	};

inline void TriggerPush::Archive
	(
	Archiver &arc
	)
   {
   Trigger::Archive( arc );

   arc.ArchiveFloat( &speed );
   }

class TriggerPushAny : public Trigger
	{
	protected:


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
      float    speed = 0.0f;


	public:
      CLASS_PROTOTYPE( TriggerPushAny );

					      TriggerPushAny();
		void		      Push( Event *ev );
		void		      SetSpeed( Event *ev );
      virtual void   Archive(	Archiver &arc );
	};

inline void TriggerPushAny::Archive
	(
	Archiver &arc
	)
   {
   Trigger::Archive( arc );

   arc.ArchiveFloat( &speed );
   }

#define AMBIENT_ON   ( 1 << 0 )
#define AMBIENT_OFF  ( 1 << 1 )
#define TOGGLESOUND  ( 1 << 5 )

class TriggerPlaySound : public Trigger
	{
	protected:
      friend class SoundManager;


	  //--------------------------------------------------------------
	  // GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	  //--------------------------------------------------------------
		int	   state = 0;
		float	   min_dist = 0.0f;
		float	   volume = 0.0f;
      int	   channel = 0;
      qboolean ambient = qfalse;


	public:
      CLASS_PROTOTYPE( TriggerPlaySound );

				         TriggerPlaySound();
		void	         ToggleSound( Event *ev );
		void	         SetVolume( Event *ev );
		void	         SetMinDist( Event *ev );
		void	         SetChannel( Event *ev );

      void           StartSound( void );
      void           SetVolume( float vol );
      void           SetMinDist( float dist );

      virtual void   Archive(	Archiver &arc );
	};

inline void TriggerPlaySound::Archive
	(
	Archiver &arc
	)
   {
   Trigger::Archive( arc );

   arc.ArchiveInteger( &state );
   arc.ArchiveFloat( &min_dist );
   arc.ArchiveFloat( &volume );
   arc.ArchiveInteger( &channel );
   arc.ArchiveBoolean( &ambient );
   if ( arc.Loading() )
      {
      //
      // see if its a toggle sound, if it is, lets start its sound again
      //
      if ( spawnflags & TOGGLESOUND )
         {
         //
         // invert state so that final state will be right
         //


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning lnt-logical-bitwise-mismatch - chrissstrahl
		//--------------------------------------------------------------
         state = static_cast<int>(!state);


         PostEvent( EV_Trigger_Effect, EV_POSTSPAWN );
         }
      }
   }

class TriggerSpeaker : public TriggerPlaySound
	{
	public:
      CLASS_PROTOTYPE( TriggerSpeaker );

				         TriggerSpeaker();
	};

class RandomSpeaker : public TriggerSpeaker
	{
	protected:
      friend class SoundManager;


	  //--------------------------------------------------------------
	  // GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	  //--------------------------------------------------------------
      float chance = 0.0f;
	  float mindelay = 0.0f;
      float maxdelay = 0.0f;


	public:
      CLASS_PROTOTYPE( RandomSpeaker );

				         RandomSpeaker();
      void           TriggerSound( Event *ev );
      void           SetMinDelay( Event *ev );
      void           SetMaxDelay( Event *ev );
      void           SetChance( Event *ev );

      void           SetMinDelay( float value );
      void           SetMaxDelay( float value );
      void           SetChance( float value );
      void           ScheduleSound( void );

      virtual void   Archive(	Archiver &arc );
	};

inline void RandomSpeaker::Archive
	(
	Archiver &arc
	)
   {
   TriggerSpeaker::Archive( arc );

   arc.ArchiveFloat( &chance );
   arc.ArchiveFloat( &mindelay );
   arc.ArchiveFloat( &maxdelay );
   }

class TriggerChangeLevel : public Trigger
	{
	protected:
		str			map;
		str			spawnspot;
      str         changethread;

	public:
      CLASS_PROTOTYPE( TriggerChangeLevel );

						TriggerChangeLevel();
      void        SetMap( Event *ev );
      void        SetSpawnSpot( Event *ev );
      void        SetThread( Event *ev );
		void			ChangeLevel( Event *ev );
		const char	*Map( void );
		const char	*SpawnSpot( void );
      virtual void Archive(	Archiver &arc );
	};

inline void TriggerChangeLevel::Archive
	(
	Archiver &arc
	)
   {
   Trigger::Archive( arc );

   arc.ArchiveString( &map );
   arc.ArchiveString( &spawnspot );
   arc.ArchiveString( &changethread );
   }

class TriggerExit : public Trigger
	{
	public:
      CLASS_PROTOTYPE( TriggerExit );

			  TriggerExit();
		void DisplayExitSign( Event *ev );
      void TurnExitSignOff( Event *ev );
	};


class TriggerUse : public Trigger
	{
	public:
      CLASS_PROTOTYPE( TriggerUse );

		TriggerUse();
	};

class TriggerUseOnce : public TriggerUse
	{
	public:
      CLASS_PROTOTYPE( TriggerUseOnce );

		TriggerUseOnce();
	};

class TriggerHurt : public TriggerUse
	{
	protected:
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		float		damage = 0.0f;


	  void		Hurt( Event *ev );
      void		SetDamage( Event *ev );
	  void		setDamage( Event *ev ); // Gameplay version

	public:
      CLASS_PROTOTYPE( TriggerHurt );

		             TriggerHurt();
      virtual void Archive(	Archiver &arc );
	};

inline void TriggerHurt::Archive
	(
	Archiver &arc
	)
   {
   TriggerUse::Archive( arc );

   arc.ArchiveFloat( &damage );
   }

class TriggerDamageTargets : public Trigger
	{
	protected:
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		float		    damage = 0.0f;


	  void		    DamageTargets( Event *ev );
      void		    SetDamage( Event *ev );
	  void			setDamage( Event *ev ); // Gameplay version

	public:
      CLASS_PROTOTYPE( TriggerDamageTargets );

		             TriggerDamageTargets();
		void	       PassDamage( Event *ev );
      virtual void Archive(	Archiver &arc );
	};

inline void TriggerDamageTargets::Archive
	(
	Archiver &arc
	)

   {
   Trigger::Archive( arc );

   arc.ArchiveFloat( &damage );
   }

class TriggerCameraUse : public TriggerUse
	{
	public:
      CLASS_PROTOTYPE( TriggerCameraUse );

		void TriggerCamera( Event * ev );
	};

class TriggerBox : public Trigger
	{
	public:
      CLASS_PROTOTYPE( TriggerBox );

      void           SetMins( Event *ev );
      void           SetMaxs( Event *ev );
	};

class TriggerMusic : public Trigger
	{
	private:
      friend class   SoundManager;
		str				current;
		str				fallback;
		str				altcurrent;
		str				altfallback;


	  //--------------------------------------------------------------
	  // GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	  //--------------------------------------------------------------
      qboolean       oneshot = qfalse;


	public:
      CLASS_PROTOTYPE( TriggerMusic );

							TriggerMusic();
		void				SetCurrentMood( Event *ev );
		void				SetFallbackMood( Event *ev );
		void				SetAltCurrentMood( Event *ev );
		void				SetAltFallbackMood( Event *ev );
		void				ChangeMood(	Event *ev );
		void				AltChangeMood(	Event *ev );
      void           SetOneShot( Event *ev );

      void           SetMood( const str &crnt, const str &fback );
      void           SetAltMood( const str &crnt, const str &fback );
      void           SetOneShot( qboolean once );

		virtual void Archive(	Archiver &arc );
	};

inline void TriggerMusic::Archive
	(
	Archiver &arc
	)

   {
   Trigger::Archive( arc );

   arc.ArchiveBoolean( &oneshot );
   arc.ArchiveString( &current );
	arc.ArchiveString( &fallback );
   arc.ArchiveString( &altcurrent );
	arc.ArchiveString( &altfallback );
   }

class TriggerReverb : public Trigger
	{
	private:
      friend class   SoundManager;


	  //--------------------------------------------------------------
	  // GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	  //--------------------------------------------------------------
      qboolean       oneshot = qfalse;
		int				reverbtype = 0;
		int				altreverbtype = 0;
      float          reverblevel = 0.0f;
      float          altreverblevel = 0.0f;


	public:
      CLASS_PROTOTYPE( TriggerReverb );

							TriggerReverb();
		void				SetReverbLevel( Event *ev );
		void				SetReverbType( Event *ev );
		void				SetAltReverbType( Event *ev );
		void				SetAltReverbLevel( Event *ev );
		void				ChangeReverb(	Event *ev );
		void				AltChangeReverb(	Event *ev );
      void           SetOneShot( Event *ev );

      void           SetReverb( int type, float level );
      void           SetAltReverb( int type, float level );
      void           SetOneShot( qboolean once );

		virtual void Archive(	Archiver &arc );
	};

inline void TriggerReverb::Archive
	(
	Archiver &arc
	)

   {
   Trigger::Archive( arc );

   arc.ArchiveBoolean( &oneshot );
   arc.ArchiveInteger( &reverbtype );
	arc.ArchiveInteger( &altreverbtype );
   arc.ArchiveFloat( &reverblevel );
	arc.ArchiveFloat( &altreverblevel );
   }

class TriggerByPushObject : public TriggerOnce
	{
   private:
      str               triggername;

      void              setTriggerName( Event *event );

	public:
      CLASS_PROTOTYPE( TriggerByPushObject );

      virtual qboolean  respondTo( Entity *other );
      virtual Entity    *getActivator( Entity *other );

		virtual void      Archive(	Archiver &arc );
	};

inline void TriggerByPushObject::Archive
	(
	Archiver &arc
	)

   {
   TriggerOnce::Archive( arc );

	arc.ArchiveString( &triggername );
   }

class TriggerGivePowerup : public Trigger
	{
	private:
      	str				powerup_name;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		qboolean       oneshot = qfalse;


	public:
      CLASS_PROTOTYPE( TriggerGivePowerup );

							TriggerGivePowerup();
		void				SetOneShot( Event *ev );
		void				SetPowerupName( Event *ev );
		void				GivePowerup( Event *ev );
		virtual void   Archive(	Archiver &arc );
	};

inline void TriggerGivePowerup::Archive
	(
	Archiver &arc
	)

   {
   Trigger::Archive( arc );

   arc.ArchiveBoolean( &oneshot );
   arc.ArchiveString( &powerup_name );
   }

class TriggerGroupEvent : public Trigger
   {
   private:
      Event passEvent;

	  
	  //--------------------------------------------------------------
	  // GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	  //--------------------------------------------------------------
      int   groupNumber = 0;

   
   public:
      CLASS_PROTOTYPE( TriggerGroupEvent );
      
           TriggerGroupEvent();
      void SetPassEvent( Event *ev );
      void SetGroupNumber( Event *ev );
      void PassEvent( Event *ev );
      void TriggerStuff( Event *ev );

      virtual void Archive( Archiver &arc );

   };

inline void TriggerGroupEvent::Archive( Archiver &arc )
   {
   Trigger::Archive( arc );

   arc.ArchiveEvent( &passEvent );
   arc.ArchiveInteger( &groupNumber );

   }


//------------------------- CLASS ------------------------------
//
// Name:          TriggerCallVolume
// Base Class:    Trigger
//
// Description:   Allows the Level Designer to specify an arbitrary list
//                of actors that are required to be within the volume
//                before the specified trigger will fire.  This can
//                used for lifts that require teammates to be on before
//                the lift should move.  The volume is smart enough
//                to recognize that not all actors in the list will be
//                in the map, so it will fire the trigger when all 
//                available memembers of the list are in the volume
//
//                It is also important to note that while the trigger
//                will notify the actors in the list that the player
//                is inside the volume, it will do nothing to get the
//                actors in the volume itself.  Helper Nodes and AI
//                functionality will be required to utilize this
//                volume as it is intended
//
// Method of Use: Level Editor
//--------------------------------------------------------------
class TriggerCallVolume : public Trigger
	{
	private:
		Container<str>	_requiredEntities;        
		str				_exitThread;

		
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		qboolean		_ready = qfalse;


	protected:
		Entity*			_getEntity            ( const str& name );
      
		void			_notifyRequiredEnts   ( bool inCallVolume );
		void			_checkForRequiredEnts ();
		bool			_isRequiredEnt		  ( Entity *other );

   public:
		CLASS_PROTOTYPE( TriggerCallVolume );

			TriggerCallVolume();
			~TriggerCallVolume();

		void AddRequiredEntity ( Event *ev );      
		void EntityLeftVolume  ( Event *ev );
		void TriggerStuff      ( Event *ev );
		void CheckReady        ( Event *ev );
		void SetExitThread	   ( Event *ev );

		virtual void Archive( Archiver &arc );

	};

inline void TriggerCallVolume::Archive( Archiver &arc )
	{
	Trigger::Archive( arc );
	_requiredEntities.Archive( arc );
	arc.ArchiveBoolean( &_ready );
	arc.ArchiveString( &_exitThread );
	}

inline void TriggerCallVolume::SetExitThread( Event *ev )
{
	_exitThread = ev->GetString( 1 );
}

class TriggerEntryAndExit : public Trigger
   {
   private:      
      str      _entryThread;
      str      _exitThread;


	  //--------------------------------------------------------------
	  // GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	  //--------------------------------------------------------------
      qboolean _entered = qfalse;

      //1ST PLAYABLE HACK
      float    _forcefieldtrigger = 0.0f;
      float    _triggernumber = 0.0f;


   public:
      CLASS_PROTOTYPE( TriggerEntryAndExit );

             TriggerEntryAndExit();
            ~TriggerEntryAndExit();

      void  EntityLeftVolume	( Event *ev );
      void  TriggerStuff		( Event *ev );

      void  SetEntryThread		( Event *ev );
      void  SetExitThread		( Event *ev );

	  void  EnterTrigger		( Event *ev );
	  void	ExitTrigger			( Event *ev );

	  void  EnterTrigger		();
	  void	ExitTrigger			();

	  // Hack stuff that needs to be removed
      void  HackSetParms( Event *ev );
      void  HackGetForceFieldTrigger(Event *ev );
      void  HackGetTriggerNumber(Event *ev );

      virtual void Archive( Archiver &arc );
   };

inline void TriggerEntryAndExit::SetEntryThread( Event *ev )
{
	_entryThread = ev->GetString( 1 );
}

inline void TriggerEntryAndExit::SetExitThread( Event *ev )
{
	_exitThread = ev->GetString( 1 );
}

inline void TriggerEntryAndExit::EnterTrigger()
{
	_entered = true;
}

inline void TriggerEntryAndExit::ExitTrigger()
{
	_entered = false;
}

inline void TriggerEntryAndExit::EnterTrigger( Event *ev )
{
	EnterTrigger();
}

inline void TriggerEntryAndExit::ExitTrigger( Event *ev )
{
	ExitTrigger();
}

inline void TriggerEntryAndExit::Archive( Archiver &arc )
   {
   Trigger::Archive( arc );
   arc.ArchiveString( &_entryThread );
   arc.ArchiveString( &_exitThread );
   arc.ArchiveBoolean( &_entered );

   arc.ArchiveFloat( &_forcefieldtrigger );
   arc.ArchiveFloat( &_triggernumber );
   }



//------------------------- CLASS ------------------------------
//
// Name:          TriggerVolume
// Base Class:    Trigger
//
// Description:   A TriggerVolume is a normal trigger that responds
//				  to both entrance (EV_Touch) and exit (EV_LostTouch)
//				  events.  It is meant to be a base class for other
//				  triggers that use this functionality.
//
// Method of Use: Inheritance by other triggers
//--------------------------------------------------------------
class TriggerVolume : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerVolume );
	~TriggerVolume()	{ }

	void				HandleTouchEvent( Event *ev );
	void				HandleContactEvent( Event *ev );
	void				HandleLostContactEvent( Event *ev );
	void				HandleSetExitThreadEvent( Event *ev );
	void				HandleSetEntryThreadEvent( Event *ev );

	virtual void Archive( Archiver &arc );

private:
	str					_entryThread ;
	str					_exitThread ;
};

inline void TriggerVolume::Archive( Archiver &arc )
{
   Trigger::Archive( arc );

   arc.ArchiveString( &_entryThread );
   arc.ArchiveString( &_exitThread );
}

class TriggerLadder : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerLadder );

	void				handleTouchEvent( Event *ev );
};

#endif /* trigger.h */
