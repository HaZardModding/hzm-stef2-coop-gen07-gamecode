//-----------------------------------------------------------------------------
//
//  $Logfile:: /Code/DLLs/game/behavior.h                                    $
// $Revision:: 72                                                             $
//   $Author:: Steven                                                         $
//     $Date:: 10/13/03 8:53a                                                 $
//
// Copyright (C) 1998 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
//
//
// DESCRIPTION:
// Standard class for creating AI behaviors
//

#ifndef __BEHAVIOR_H__
#define __BEHAVIOR_H__

#include "g_local.h"
#include "entity.h"
#include "path.h"
#include "steering.h"
#include "beam.h"
#include "sentient.h"
#include "bspline.h"
#include "waypoints.h"
#include "FollowPath.h"
#include "FollowPathToPoint.h"
#include "FollowPathToEntity.h"

extern Event EV_Behavior_Args;
extern Event EV_Behavior_AnimDone;

class Actor;

typedef enum 
{
	BEHAVIOR_SUCCESS,
	BEHAVIOR_EVALUATING,
	BEHAVIOR_FAILED,   
	BEHAVIOR_FAILED_STEERING_BLOCKED_BY_ENEMY,
	BEHAVIOR_FAILED_STEERING_BLOCKED_BY_CIVILIAN,
	BEHAVIOR_FAILED_STEERING_BLOCKED_BY_FRIEND,
	BEHAVIOR_FAILED_STEERING_BLOCKED_BY_TEAMMATE,
	BEHAVIOR_FAILED_STEERING_BLOCKED_BY_WORLD,
	BEHAVIOR_FAILED_STEERING_BLOCKED_BY_DOOR,
	BEHAVIOR_FAILED_STEERING_CANNOT_GET_TO_PATH,
	BEHAVIOR_FAILED_STEERING_NO_PATH,
	BEHAVIOR_INVALID,
	BEHAVIOR_NUMBER_OF_RETURN_VALUES			
} BehaviorReturnCode_t;

class Behavior : public Listener
	{
	private:
		str						_failureReason;
		str						_internalStateName;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		Listener			   *_controller = nullptr;
		Actor*					_self = nullptr;
	protected:
		PathNodePtr				movegoal = nullptr;


	public:
		CLASS_PROTOTYPE( Behavior );

		Behavior();
		virtual void					ShowInfo( Actor &self );
		virtual void					Begin( Actor &self );
		virtual BehaviorReturnCode_t	Evaluate( Actor &self );
		virtual void					End( Actor &self );
		virtual void					Archive( Archiver &arc );

		void							SetFailureReason( const str &reason )		{ _failureReason = reason; }
		str								GetFailureReason()							{ return _failureReason;   }

		void							SetInternalStateName( const str &reason )	{ _internalStateName = reason; }
		str								GetInternalStateName()						{ return _internalStateName;   }

		Listener*						GetController( )							{ return _controller ; }
		void							SetController( Listener *controller )		{ _controller = controller ; }

		void							SetSelf( Actor* self )						{ _self = self; }
		Actor*							GetSelf()									{ return _self; }
	};

inline void Behavior::Archive( Archiver &arc )
{
	Listener::Archive( arc );

	arc.ArchiveString		( &_failureReason		);
	arc.ArchiveString		( &_internalStateName	);
	arc.ArchiveObjectPointer( ( Class ** )&_controller	);
	arc.ArchiveObjectPointer( ( Class ** )&_self	);
	arc.ArchiveSafePointer	( &movegoal				);
}

typedef SafePtr<Behavior> BehaviorPtr;

class Idle : public Behavior
	{
	private:
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		float						nexttwitch = 0.0f;


	public:
      CLASS_PROTOTYPE( Idle );

      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t	Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void Idle::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveFloat( &nexttwitch );
   }

class Pain : public Behavior
	{
	private:
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		int						current_pain_type = 0;
		int						pain_anim_number = 0;
		qboolean					anim_done = qfalse;
		int						number_of_pains = 0;
		int						max_pains = 0;


	public:
      CLASS_PROTOTYPE( Pain );

		void						AnimDone( Event *ev );

		void						SetPainAnim( Actor &self, int new_pain_type, int new_anim_number );
		int						GetNumberOfPainAnims( Actor &self, int new_pain_type );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void Pain::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

	arc.ArchiveInteger( &current_pain_type );
	arc.ArchiveInteger( &pain_anim_number );
	arc.ArchiveBoolean( &anim_done );
	arc.ArchiveInteger( &number_of_pains );
	arc.ArchiveInteger( &max_pains );
   }

class Watch : public Behavior
	{
	private:
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		EntityPtr				ent_to_watch = nullptr;
		float						turn_speed = 0.0f;
		float						old_turn_speed = 0.0f;


	public:
      CLASS_PROTOTYPE( Watch );

									Watch();
		void						SetArgs(	Event *ev );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
		virtual void         Archive( Archiver &arc );
	};

inline void Watch::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveSafePointer( &ent_to_watch );
   arc.ArchiveFloat( &turn_speed );
	arc.ArchiveFloat( &old_turn_speed );
   }

class Turn : public Behavior
	{
	private:
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		float						turn_speed = 0.0f;


	public:
      CLASS_PROTOTYPE( Turn );

									Turn();
		void						SetArgs(	Event *ev );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
		virtual void         Archive( Archiver &arc );
	};

inline void Turn::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveFloat( &turn_speed );
   }

class CircleEnemy : public Behavior
	{
	private:
		str						center_part_name;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		EntityPtr				ent_to_circle = nullptr;
		float						last_angle_change = 0.0f;
		

	public:
      CLASS_PROTOTYPE( CircleEnemy );

		void						SetArgs( Event *ev );
		float						GetAngleDiff( const Actor &self, const Actor *center_actor, const Vector &origin );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
		virtual void         Archive( Archiver &arc );
	};

inline void CircleEnemy::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveSafePointer( &ent_to_circle );
	arc.ArchiveString( &center_part_name );
	arc.ArchiveFloat( &last_angle_change );
   }



class BurrowAttack : public Behavior
	{
	private:
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		Vector					goal = Vector( 0.0f, 0.0f, 0.0f );
		Vector					attack_origin = Vector(0.0f, 0.0f, 0.0f);
		int						burrow_mode = 0;
		EntityPtr				leg1 = nullptr;
		EntityPtr				leg2 = nullptr;
		EntityPtr				leg3 = nullptr;
		EntityPtr				leg4 = nullptr;
		int						stage = 0;
		int						attacks_left = 0;
		float						burrow_speed = 0.0f;
		qboolean					too_close = qfalse;
		qboolean					use_last_known_position = qfalse;


	public:
      CLASS_PROTOTYPE( BurrowAttack );

		void						SetArgs( Event *ev );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
		void						SpawnArm( Actor &self, EntityPtr &leg, const Vector &original_arm_origin, const char *anim_name, float angle );
		virtual void         Archive( Archiver &arc );
	};

inline void BurrowAttack::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

	arc.ArchiveVector( &goal );
	arc.ArchiveVector( &attack_origin );
	arc.ArchiveInteger( &burrow_mode );
	arc.ArchiveSafePointer( &leg1 );
	arc.ArchiveSafePointer( &leg2 );
	arc.ArchiveSafePointer( &leg3 );
	arc.ArchiveSafePointer( &leg4 );
	arc.ArchiveInteger( &stage );
	arc.ArchiveInteger( &attacks_left );
	arc.ArchiveFloat( &burrow_speed );
	arc.ArchiveBoolean( &too_close );
	arc.ArchiveBoolean( &use_last_known_position );
   }

class ShockWater : public Behavior
	{
	private:
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		EntityPtr				left_beam = nullptr;
		EntityPtr				right_beam = nullptr;
		EntityPtr				center_beam = nullptr;
		qboolean					already_started = qfalse;


	public:
      CLASS_PROTOTYPE( ShockWater );

									ShockWater();
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
		virtual void         Archive( Archiver &arc );
	};

inline void ShockWater::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveSafePointer( &left_beam );
	arc.ArchiveSafePointer( &right_beam );
	arc.ArchiveSafePointer( &center_beam );
	arc.ArchiveBoolean( &already_started );
   }

class Shock : public Behavior
	{
	private:
		str						tag_name;
		str                  beamShader;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		EntityPtr				beam = nullptr;
		float						damage = 0.0f;
		qboolean					already_started = qfalse;
		float						random_angle = 0.0f;
		float                z_offset = 0.0f;


	public:
      CLASS_PROTOTYPE( Shock );

									Shock();
		void						SetArgs( Event *ev );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
		virtual void         Archive( Archiver &arc );
	};

inline void Shock::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveSafePointer( &beam );
	arc.ArchiveString( &tag_name );
	arc.ArchiveFloat( &damage );
	arc.ArchiveBoolean( &already_started );
	arc.ArchiveFloat( &random_angle );
	arc.ArchiveString( &beamShader );
	arc.ArchiveFloat( &z_offset );
   }

class MultiShock : public Behavior
	{
	private:
		str						tag_name1;
		str                  tag_name2;
		str                  beamShader;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		EntityPtr				beam1 = nullptr;
		EntityPtr            beam2 = nullptr;
		float						damage = 0.0f;
		qboolean					already_started = qfalse;
		float						random_angle = 0.0f;
		float                z_offset = 0.0f;


	public:
      CLASS_PROTOTYPE( MultiShock );

									MultiShock();
		void						SetArgs( Event *ev );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
		virtual void         Archive( Archiver &arc );
	};

inline void MultiShock::Archive
	(
	Archiver &arc
	)

   {
   Behavior::Archive( arc );

   arc.ArchiveSafePointer( &beam1 );
	arc.ArchiveSafePointer( &beam2 );
	arc.ArchiveString( &tag_name1 );
	arc.ArchiveString( &tag_name2 );
	arc.ArchiveFloat( &damage );
	arc.ArchiveBoolean( &already_started );
	arc.ArchiveFloat( &random_angle );
	arc.ArchiveString( &beamShader );
	arc.ArchiveFloat( &z_offset );	
   }

class ShockDown : public Behavior
	{
	private:
		str						tag_name;
		str                  beamShader;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		EntityPtr				beam = nullptr;
		float						damage = 0.0f;
		qboolean					already_started = qfalse;
		

	public:
      CLASS_PROTOTYPE( ShockDown );

									ShockDown();
		void						SetArgs( Event *ev );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
		virtual void         Archive( Archiver &arc );
	};

inline void ShockDown::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveSafePointer( &beam );
	arc.ArchiveString( &tag_name );
	arc.ArchiveFloat( &damage );
	arc.ArchiveBoolean( &already_started );	
	arc.ArchiveString( &beamShader );	
   }

class CircleAttack : public Behavior
	{
	private:
		str						command;
		str						direction;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		EntityPtr				first_part = nullptr;
		EntityPtr				current_part = nullptr;
		float						next_time = 0.0f;
		int						current_direction = 0;
		int						number_of_attacks = 0;


	public:
      CLASS_PROTOTYPE( CircleAttack );

									CircleAttack();
		Actor						*FindClosestPart( const Actor &self, float angle );
		void						SetArgs( Event *ev );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
		virtual void         Archive( Archiver &arc );
	};

inline void CircleAttack::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveSafePointer( &first_part );
	arc.ArchiveSafePointer( &current_part );
	arc.ArchiveString( &command );
	arc.ArchiveString( &direction );
	arc.ArchiveFloat( &next_time );
	arc.ArchiveInteger( &current_direction );
	arc.ArchiveInteger( &number_of_attacks );
   }

class DragEnemy : public Behavior
	{
	private:
		str						tag_name;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		EntityPtr				ent_to_drag = nullptr;
		float						damage = 0.0f;
		float						target_yaw = 0.0f;
		float						last_turn_time = 0.0f;
		qboolean					attached = qfalse;
		Vector					offset = Vector( 0.0f , 0.0f , 0.0f );
		qboolean					drop = qfalse;


	public:
      CLASS_PROTOTYPE( DragEnemy );

		void						SetArgs( Event *ev );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
		virtual void         Archive( Archiver &arc );
	};

inline void DragEnemy::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveSafePointer( &ent_to_drag );
	arc.ArchiveString( &tag_name );
	arc.ArchiveFloat( &damage );
	arc.ArchiveFloat( &target_yaw );
	arc.ArchiveFloat( &last_turn_time );
	arc.ArchiveBoolean( &attached );
	arc.ArchiveVector( &offset );
	arc.ArchiveBoolean( &drop );
   }

class PickupEnemy : public Behavior
	{
	private:
		str						tag_name;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		EntityPtr				ent_to_drag = nullptr;
		float						damage = 0.0f;
		float						target_yaw = 0.0f;
		float						last_turn_time = 0.0f;
		qboolean					attached = qfalse;
		Vector					offset = Vector( 0.0f , 0.0f , 0.0f );
		qboolean					drop = qfalse;


	public:
      CLASS_PROTOTYPE( PickupEnemy );

		void						SetArgs( Event *ev );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
		virtual void         Archive( Archiver &arc );
	};

inline void PickupEnemy::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveSafePointer( &ent_to_drag );
	arc.ArchiveString( &tag_name );
	arc.ArchiveFloat( &damage );
	arc.ArchiveFloat( &target_yaw );
	arc.ArchiveFloat( &last_turn_time );
	arc.ArchiveBoolean( &attached );
	arc.ArchiveVector( &offset );
	arc.ArchiveBoolean( &drop );
   }

class Aim : public Behavior
	{
	private:
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		EntityPtr				target = nullptr;

	public:
      CLASS_PROTOTYPE( Aim );

		void						SetTarget( Entity *ent );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void Aim::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveSafePointer( &target );
   }

class TurnTo : public Behavior
	{
	private:
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		EntityPtr				ent = nullptr;
		Vector					dir = Vector( 0.0f , 0.0f , 0.0f );
		float						yaw = 0.0f;
		int						mode = 0;
		qboolean					anim_done = qfalse;
		bool					useTurnAnim = false;
		int						extraFrames = 0;
		bool					_useAnims = false;


	public:
      CLASS_PROTOTYPE( TurnTo );

									TurnTo();
		void						SetDirection( float yaw );
		void						SetTarget( Entity *ent );
		void						AnimDone( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
		void						SetUseTurnAnim( bool useAnim );
		void						useAnims( bool useAnims );
	   virtual void         Archive( Archiver &arc );
	};

inline void TurnTo::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveSafePointer( &ent );
   arc.ArchiveVector( &dir );
   arc.ArchiveFloat( &yaw );
   arc.ArchiveInteger( &mode );
	arc.ArchiveBoolean( &anim_done );
	arc.ArchiveBool( &useTurnAnim );
	arc.ArchiveInteger ( &extraFrames );
	arc.ArchiveBool( &_useAnims );
   }

class RotateToEnemy : public Behavior
   {
	private:
		str   anim;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		float turnSpeed = 0.0f;
		

	public:
   CLASS_PROTOTYPE( RotateToEnemy );

		void						SetArgs( Event *ev );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void RotateToEnemy::Archive( Archiver &arc )
{
	Behavior::Archive( arc );	     

	arc.ArchiveFloat( &turnSpeed );
	arc.ArchiveString( &anim );
}

class PickupEntity : public Behavior
	{
	private:
		str						pickup_anim_name;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		qboolean					anim_done = qfalse;
		EntityPtr				ent_to_pickup = nullptr;


	public:
      CLASS_PROTOTYPE( PickupEntity );

		void						SetArgs( Event *ev );
		void						AnimDone( Event *ev );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void PickupEntity::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveString( &pickup_anim_name );
   arc.ArchiveBoolean( &anim_done );
   arc.ArchiveSafePointer( &ent_to_pickup );
   }

class ThrowEntity : public Behavior
	{
	private:
		str						throw_anim_name;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		qboolean					anim_done = qtrue;


	public:
      CLASS_PROTOTYPE( ThrowEntity );

		void						SetArgs( Event *ev );
		void						AnimDone( Event *ev );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void ThrowEntity::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveString( &throw_anim_name );
   arc.ArchiveBoolean( &anim_done );
   }

class HeadWatch : public Behavior
	{
	private:
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		EntityPtr				ent_to_watch = nullptr;
		Vector					current_head_angles = Vector( 0.0f , 0.0f , 0.0f );
		float						max_speed = 0.0f;
		qboolean					forever = qfalse;		
		qboolean             usingEyes = qfalse;


	public:
      CLASS_PROTOTYPE( HeadWatch );

									HeadWatch();
		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
		void                 useEyes( qboolean moveEyes );
	};

inline void HeadWatch::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

	arc.ArchiveSafePointer( &ent_to_watch );
	arc.ArchiveVector( &current_head_angles );
	arc.ArchiveFloat( &max_speed );
   arc.ArchiveBoolean( &forever );
	arc.ArchiveBoolean( &usingEyes );
   }

class HeadWatchEnemy : public Behavior
	{
	private:
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		EntityPtr				ent_to_watch = nullptr;
		Vector					current_head_angles = Vector( 0.0f , 0.0f, 0.0f );
      Vector               current_torso_angles = Vector( 0.0f, 0.0f, 0.0f );
		float						max_speed = 0.0f;
		qboolean					forever = qfalse;
		float                threshold = 0.0f;
		qboolean             usingEyes = qfalse;		


	public:
      CLASS_PROTOTYPE( HeadWatchEnemy );

									HeadWatchEnemy();
		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
		void                 useEyes( qboolean moveEyes );
	};

inline void HeadWatchEnemy::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

	arc.ArchiveSafePointer( &ent_to_watch );
	arc.ArchiveVector( &current_head_angles );
   arc.ArchiveVector( &current_torso_angles );
	arc.ArchiveFloat( &max_speed );
   arc.ArchiveBoolean( &forever );
	arc.ArchiveFloat( &threshold );
	arc.ArchiveBoolean( &usingEyes );
   }

class EyeWatch : public Behavior
   {
	private:
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		EntityPtr				ent_to_watch = nullptr;
		Vector					current_left_eye_angles = Vector(0.0f, 0.0f, 0.0f);
		Vector               current_right_eye_angles = Vector( 0.0f , 0.0f, 0.0f );
		float						max_speed = 0.0f;
		qboolean					forever = qfalse;
		float                threshold = 0.0f;


	public:
      CLASS_PROTOTYPE( EyeWatch );

									EyeWatch();
		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void EyeWatch::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

	arc.ArchiveSafePointer( &ent_to_watch );
	arc.ArchiveVector( &current_left_eye_angles );
	arc.ArchiveVector( &current_right_eye_angles );
	arc.ArchiveFloat( &max_speed );
   arc.ArchiveBoolean( &forever );
	arc.ArchiveFloat( &threshold );
   }

class EyeWatchEnemy : public Behavior
   {
	private:
		EntityPtr				ent_to_watch;
		Vector					current_left_eye_angles;
		Vector               current_right_eye_angles;
		float						max_speed;
		qboolean					forever;
		float                threshold;

	public:
      CLASS_PROTOTYPE( EyeWatchEnemy );

									EyeWatchEnemy();
		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void EyeWatchEnemy::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

	arc.ArchiveSafePointer( &ent_to_watch );
	arc.ArchiveVector( &current_left_eye_angles );
	arc.ArchiveVector( &current_right_eye_angles );
	arc.ArchiveFloat( &max_speed );
   arc.ArchiveBoolean( &forever );
	arc.ArchiveFloat( &threshold );
   }

class HeadAndEyeWatch : public Behavior
   {
	private:
		HeadWatch      headWatch;
		EyeWatch       eyeWatch;

	public:
      CLASS_PROTOTYPE( HeadAndEyeWatch );

									HeadAndEyeWatch();
		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void HeadAndEyeWatch::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );
	
	arc.ArchiveObject(&headWatch);
	arc.ArchiveObject(&eyeWatch);
   }

class TorsoTurn : public Behavior
	{
	private:
		str						tag_name;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		int						turn_towards_enemy = 0;
		float						speed = 0.0f;
		int						forever = 0;
		qboolean             use_pitch = qfalse;
		float						current_yaw = 0.0f;
		float						current_pitch = 0.0f;
		float						tolerance = 0.0f;
		float                offset = 0.0f;

	public:
      CLASS_PROTOTYPE( TorsoTurn );

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );

		void SetRequiredParameters( int TurnTowardsEnemy, int Speed, int Forever );		
	};

inline void TorsoTurn::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

	arc.ArchiveInteger( &turn_towards_enemy );
	arc.ArchiveFloat( &speed );
	arc.ArchiveInteger( &forever );
	arc.ArchiveBoolean( &use_pitch);
	arc.ArchiveFloat( &current_yaw );
	arc.ArchiveFloat( &current_pitch );
	arc.ArchiveString( &tag_name );
	arc.ArchiveFloat( &tolerance );
	arc.ArchiveFloat( &offset );
   }


class TorsoWatchEnemy : public Behavior
	{
	private:
		str						tag_name;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		float						speed = 0.0f;
		int						forever = 0;
		qboolean             use_pitch = qfalse;
		float						current_yaw = 0.0f;
		float						current_pitch = 0.0f;
		float						threshold = 0.0f;
		float                offset = 0.0f;
		qboolean             invert = qfalse;
		qboolean             reset = qfalse;
      qboolean             invertLegs = qfalse;
      float                nextFlipTime = 0.0f;
      

	public:
      CLASS_PROTOTYPE( TorsoWatchEnemy );

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void TorsoWatchEnemy::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

	arc.ArchiveFloat( &speed );
	arc.ArchiveInteger( &forever );
	arc.ArchiveBoolean( &use_pitch);
	arc.ArchiveFloat( &current_yaw );
	arc.ArchiveFloat( &current_pitch );
	arc.ArchiveString( &tag_name );
	arc.ArchiveFloat( &threshold );
	arc.ArchiveFloat( &offset );
	arc.ArchiveBoolean( &invert );
	arc.ArchiveBoolean( &reset );
	arc.ArchiveBoolean( &invertLegs );
	arc.ArchiveFloat( &nextFlipTime );
   }

class FallToDeath : public Behavior
   {
   private:
		str   startAnim;
		str   fallAnim;
		str   deathAnim;


	   //--------------------------------------------------------------
	   // GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	   //--------------------------------------------------------------
		float forwardmove = 0.0f;
		float sidemove = 0.0f;
		float distance = 0.0f;
		float time = 0.0f;
		float speed = 0.0f;
		Vector yaw_forward = Vector( 0.0f , 0.0f, 0.0f );
		Vector yaw_left = Vector( 0.0f, 0.0f, 0.0f );
		qboolean did_impulse = qfalse;
		float  impulse_time = 0.0f;
		qboolean animdone = qfalse;
		int state = 0;
	
		
	public:
		CLASS_PROTOTYPE( FallToDeath );
		
		                     FallToDeath();
		void						SetArgs( Event *ev );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
      
	};

inline void FallToDeath::Archive
   (
	Archiver &arc
	)
	{
   Behavior::Archive( arc );

	arc.ArchiveFloat( &forwardmove );
	arc.ArchiveFloat( &sidemove );
	arc.ArchiveFloat( &distance );
	arc.ArchiveFloat( &time );
	arc.ArchiveFloat( &speed );
	arc.ArchiveString( &startAnim );
	arc.ArchiveString( &fallAnim );
	arc.ArchiveString( &deathAnim );
	arc.ArchiveVector( &yaw_forward );
	arc.ArchiveVector( &yaw_left );
	arc.ArchiveBoolean( &did_impulse );
	arc.ArchiveFloat( &impulse_time );
	arc.ArchiveBoolean( &animdone );
	arc.ArchiveInteger( &state );
	}

class GotoPathNode : public Behavior
	{
	private:
		str						anim;
		TurnTo					turnto;
		FollowPath* chase;
		HeadWatch				head_watch;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		int						state = 0;
		qboolean					usevec = qfalse;
		//float						time;
		EntityPtr				goalent = nullptr;
		Vector					goal = Vector( 0.0f , 0.0f , 0.0f );
		EntityPtr				entity_to_watch = nullptr;
		bool					_followingEntity = false;


	public:
      CLASS_PROTOTYPE( GotoPathNode );

									GotoPathNode();
		void						SetArgs( Event *ev );
		void						SetGoal( PathNode *node );
		void						AnimDone( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void GotoPathNode::Archive( Archiver &arc )
{
	Behavior::Archive( arc );

	arc.ArchiveObject( &turnto );

	// Archive chase last

	arc.ArchiveInteger( &state );
	arc.ArchiveBoolean( &usevec );
	//arc.ArchiveFloat( &time );
	arc.ArchiveString( &anim );
	arc.ArchiveSafePointer( &goalent );
	arc.ArchiveVector( &goal );
	arc.ArchiveSafePointer( &entity_to_watch );
	arc.ArchiveObject( &head_watch );
	arc.ArchiveBool( &_followingEntity );

	if ( arc.Saving() )
	{
	   arc.ArchiveObject( chase );
	}
	else
	{
		if ( _followingEntity )
			chase = new FollowPathToEntity;
		else
			chase = new FollowPathToPoint;

	   arc.ArchiveObject( chase );
	}
}


class Flee : public Behavior
	{
	private:
		FollowPathToPoint		chase;
		str						anim;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		PathNodePtr				flee_node = nullptr;


	public:
      CLASS_PROTOTYPE( Flee );

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		void						FindFleeNode( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void Flee::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

	arc.ArchiveObject( &chase );
   arc.ArchiveString( &anim );
	arc.ArchiveSafePointer( &flee_node );
   }

/*
class PlayAnim : public Behavior
	{
	private:
		str						anim;

	public:
      CLASS_PROTOTYPE( PlayAnim );

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void PlayAnim::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveString( &anim );
   }

 */

class FindCover : public Behavior
	{
	private:
		str						anim;
		str						crouch_anim;
		FollowPathToPoint		chase;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		int						state = 0;
		float						nextsearch = 0.0f;

	public:
      CLASS_PROTOTYPE( FindCover );

		void						SetArgs( Event *ev );
		PathNode					*FindCoverNode( Actor &self );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void FindCover::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveString( &anim );
   arc.ArchiveString( &crouch_anim );
   arc.ArchiveObject( &chase );
   arc.ArchiveInteger( &state );
   arc.ArchiveFloat( &nextsearch );
   }

class FindFlee : public Behavior
	{
	private:
		str						anim;
		FollowPathToPoint		chase;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		int						state = 0;
		float						nextsearch = 0.0f;


	public:
      CLASS_PROTOTYPE( FindFlee );

		void						SetArgs( Event *ev );
		PathNode					*FindFleeNode( Actor &self );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void FindFlee::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveString( &anim );
   arc.ArchiveObject( &chase );
   arc.ArchiveInteger( &state );
   arc.ArchiveFloat( &nextsearch );
   }

class FindEnemy : public Behavior
	{
	private:
		str						anim;
		FollowPathToPoint		chase;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		int						state = 0;
		float						nextsearch = 0.0f;
      PathNodePtr          lastSearchNode = nullptr;
      Vector               lastSearchPos = Vector( 0.0f, 0.0f, 0.0f );

	public:
      CLASS_PROTOTYPE( FindEnemy );

		PathNode					*FindClosestSightNode( Actor &self );
		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void FindEnemy::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveString( &anim );
   arc.ArchiveObject( &chase );
   arc.ArchiveInteger( &state );
   arc.ArchiveFloat( &nextsearch );
   arc.ArchiveSafePointer( &lastSearchNode );
   arc.ArchiveVector( &lastSearchPos );
   }

class AimAndShoot : public Behavior
	{
	private:
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		int						mode = 0;
		int						maxshots = 0;
		int						numshots = 0;
		qboolean					animdone = qfalse;
      float                enemy_health = 0.0f;
      float                aim_time = 0.0f;


		Aim						aim;
		TorsoTurn            torsoTurn;
      str                  animprefix;
      str                  aimanim;
      str                  fireanim;

	public:
      CLASS_PROTOTYPE( AimAndShoot );

									AimAndShoot();
		void						SetMaxShots( int num );
		void						SetArgs( Event *ev );
		void						AnimDone( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void AimAndShoot::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveObject( &aim );
	arc.ArchiveObject( &torsoTurn );
   arc.ArchiveInteger( &mode );
   arc.ArchiveInteger( &maxshots );
   arc.ArchiveInteger( &numshots );
   arc.ArchiveBoolean( &animdone );
   arc.ArchiveFloat( &enemy_health );
   arc.ArchiveFloat( &aim_time );
   arc.ArchiveString( &animprefix );
   arc.ArchiveString( &aimanim );
   arc.ArchiveString( &fireanim );
   }

class AimAndMelee : public Behavior
	{
	private:
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		int						mode = 0;
		int						maxshots = 0;
		int						numshots = 0;
		qboolean					animdone = qfalse;


		Aim						aim;
		str						anim_name;

	public:
      CLASS_PROTOTYPE( AimAndMelee );

									AimAndMelee();
		void						SetArgs( Event *ev );
		void						AnimDone( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void AimAndMelee::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveObject( &aim );
   arc.ArchiveInteger( &mode );
   arc.ArchiveInteger( &maxshots );
   arc.ArchiveInteger( &numshots );
   arc.ArchiveBoolean( &animdone );
	arc.ArchiveString( &anim_name );
   }

class JumpToPathNode : public Behavior
	{
	private:
		Jump						jump;
	public:
      CLASS_PROTOTYPE( JumpToPathNode );

		void						SetArgs( Event *ev );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void JumpToPathNode::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveObject( &jump );
   }

class LeapToEnemy : public Behavior
   {
	private:		
		Jump  jump;

	public:
      CLASS_PROTOTYPE( LeapToEnemy );

		void						SetArgs( Event *ev );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void LeapToEnemy::Archive
   (
	Archiver &arc
	)
	{
	Behavior::Archive( arc );
	arc.ArchiveObject(&jump );

	}

class FlyToPoint : public Behavior
	{
	private:
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
      float                avoidtime = 0.0f;
		Vector					target_angle;
		float						turn_speed = 0.0f;
		float						old_turn_speed = 0.0f;
		float						speed = 0.0f;
		float						old_forward_speed = 0.0f;
		Vector					goal = Vector( 0.0f , 0.0f, 0.0f );
		qboolean					random_allowed = qfalse;
		qboolean					force_goal = qfalse;
		int						stuck = 0;
		Vector					temp_goal = Vector(0.0f, 0.0f, 0.0f);
		qboolean					use_temp_goal = qfalse;
		qboolean					adjustYawAndRoll = qfalse;
		qboolean             offsetOrigin = qfalse;
		

	public:
      CLASS_PROTOTYPE( FlyToPoint );

									FlyToPoint();

		void						SetTurnSpeed( float new_turn_speed );
		void						SetGoalPoint( const Vector &goal_point );
		void						SetRandomAllowed( qboolean allowed );
		void						SetSpeed( float speed_value );
		void						ForceGoal( void );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		float						LerpAngle( float old_angle, float new_angle, float lerp_amount );
		void						End( Actor &self );
		virtual void         Archive( Archiver &arc );
		void						setAdjustYawAndRoll( bool adjust ) { adjustYawAndRoll = adjust; }					
		void						setOffsetOrigin( bool setOffset ) { offsetOrigin = setOffset;		}
	};

inline void FlyToPoint::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveFloat( &avoidtime );
   arc.ArchiveVector( &target_angle );
	arc.ArchiveFloat( &turn_speed );
	arc.ArchiveFloat( &old_turn_speed );
	arc.ArchiveFloat( &speed );
	arc.ArchiveFloat( &old_forward_speed );
   arc.ArchiveVector( &goal );
	arc.ArchiveBoolean( &random_allowed );
	arc.ArchiveBoolean( &force_goal );
	arc.ArchiveInteger( &stuck );
	arc.ArchiveVector( &temp_goal );
	arc.ArchiveBoolean( &use_temp_goal );
	arc.ArchiveBoolean( &adjustYawAndRoll );
	arc.ArchiveBoolean( &offsetOrigin );	
   }

class FlyCloseToEnemy : public Behavior
	{
	private:
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		float						turn_speed = 0.0f;
		float						speed = 0.0f;
		float						next_goal_time = 0.0f;
		qboolean					adjustPitch = qfalse;


		FlyToPoint				fly;
		str						anim;

	public:
      CLASS_PROTOTYPE( FlyCloseToEnemy );

									FlyCloseToEnemy();

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void FlyCloseToEnemy::Archive( Archiver &arc	)
{
	Behavior::Archive( arc );

	arc.ArchiveString( &anim );
	arc.ArchiveFloat( &turn_speed );
	arc.ArchiveFloat( &speed );
	arc.ArchiveObject( &fly );
	arc.ArchiveFloat( &next_goal_time );
	arc.ArchiveBoolean( &adjustPitch );
}

class FlyCloseToPlayer : public Behavior
	{
	private:
		str						anim;
		FlyToPoint				fly;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		float						turn_speed = 0.0f;
		float						speed = 0.0f;
		float						next_goal_time = 0.0f;


	public:
      CLASS_PROTOTYPE( FlyCloseToPlayer );

									FlyCloseToPlayer();

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void FlyCloseToPlayer::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveString( &anim );
	arc.ArchiveFloat( &turn_speed );
	arc.ArchiveFloat( &speed );
	arc.ArchiveObject( &fly );
	arc.ArchiveFloat( &next_goal_time );
   }

class FlyCloseToParent : public Behavior
	{
	private:
		str						anim;
		FlyToPoint				fly;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		float						turn_speed = 0.0f;
		float						speed = 0.0f;
		float						next_goal_time = 0.0f;


	public:
      CLASS_PROTOTYPE( FlyCloseToParent );

									FlyCloseToParent();

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void FlyCloseToParent::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveString( &anim );
	arc.ArchiveFloat( &turn_speed );
	arc.ArchiveFloat( &speed );
	arc.ArchiveObject( &fly );
	arc.ArchiveFloat( &next_goal_time );
   }


class FlyDescend: public Behavior
	{
	private:
		str						anim;
		FlyToPoint				fly;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		Vector					goal = Vector( 0.0f, 0.0f, 0.0f );
		float						height = 0.0f;
		float						speed = 0.0f;
		float						next_height_check = 0.0f;
		float						last_check_height = 0.0f;

	public:
      CLASS_PROTOTYPE( FlyDescend );

									FlyDescend();

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void FlyDescend::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveString( &anim );
	arc.ArchiveObject( &fly );
	arc.ArchiveVector( &goal );
	arc.ArchiveFloat( &height );
	arc.ArchiveFloat( &speed );
	arc.ArchiveFloat( &next_height_check );
	arc.ArchiveFloat( &last_check_height );
   }


class FlyWander : public Behavior
	{
	private:
		str						anim;
		FlyToPoint				fly;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		float						turn_speed = 0.0f;
		float						speed = 0.0f;
		float						change_course_time = 0.0f;
		float						next_change_course_time = 0.0f;
		float						original_z = 0.0f;
		Vector					goal = Vector( 0.0f , 0.0f, 0.0f );
		qboolean					try_to_go_up = qfalse;


	public:
      CLASS_PROTOTYPE( FlyWander );

									FlyWander();

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void FlyWander::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveString( &anim );
	arc.ArchiveFloat( &turn_speed );
	arc.ArchiveFloat( &speed );
	arc.ArchiveObject( &fly );
	arc.ArchiveFloat( &change_course_time );
	arc.ArchiveFloat( &next_change_course_time );
	arc.ArchiveFloat( &original_z );
	arc.ArchiveVector( &goal );
	arc.ArchiveBoolean( &try_to_go_up );
   }

class FlyToNode : public Behavior
	{
	private:
		str						anim;
		FlyToPoint				fly;
		str						NodeType;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		float						turn_speed = 0.0f;
		float						speed = 0.0f;
		//float						original_z;
		//Vector					goal;
		//qboolean					try_to_go_up;
		//int						NodeIdx;
		int						NumberOfNodes = 0;


	public:
      CLASS_PROTOTYPE( FlyToNode );

									FlyToNode();

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void FlyToNode::Archive( Archiver &arc )
{
	Behavior::Archive( arc );

	arc.ArchiveString( &anim );
	arc.ArchiveFloat( &turn_speed );
	arc.ArchiveFloat( &speed );
	arc.ArchiveObject( &fly );
	//arc.ArchiveFloat( &original_z );
	arc.ArchiveString( &NodeType );
	arc.ArchiveInteger( &NumberOfNodes );
   }

class FlyToRandomNode : public Behavior
	{
	private:
		str						anim;
		float						turn_speed;
		float						speed;
		FlyToPoint				fly;
		//float						original_z;
		str						NodeType;
		int						NumberOfNodes;
		int                  CurrentNode;
      PathNodePtr            goal;
		qboolean					NeedNextNode;

	public:
      CLASS_PROTOTYPE( FlyToRandomNode );

									FlyToRandomNode();

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void FlyToRandomNode::Archive( Archiver &arc )
{
	Behavior::Archive( arc );

	arc.ArchiveString( &anim );
	arc.ArchiveFloat( &turn_speed );
	arc.ArchiveFloat( &speed );
	arc.ArchiveObject( &fly );
	//arc.ArchiveFloat( &original_z );
	arc.ArchiveString( &NodeType );
	arc.ArchiveInteger( &NumberOfNodes );
	arc.ArchiveInteger( &CurrentNode );
	arc.ArchiveSafePointer( &goal );
	arc.ArchiveBoolean( &NeedNextNode );
   }

class FlyToNodeNearestPlayer : public Behavior
   {
	private:
		str						anim;
		float						turn_speed;
		float						speed;
		FlyToPoint				fly;
		//float						original_z;
		str						NodeType;
		int						NumberOfNodes;
		int                  CurrentNode;
      PathNodePtr            goal;
		qboolean					NeedNextNode;

	public:
      CLASS_PROTOTYPE( FlyToNodeNearestPlayer );

									FlyToNodeNearestPlayer();

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void FlyToNodeNearestPlayer::Archive( Archiver &arc )
{
	Behavior::Archive( arc );

	arc.ArchiveString( &anim );
	arc.ArchiveFloat( &turn_speed );
	arc.ArchiveFloat( &speed );
	arc.ArchiveObject( &fly );
	//arc.ArchiveFloat( &original_z );
	arc.ArchiveString( &NodeType );
	arc.ArchiveInteger( &NumberOfNodes );
	arc.ArchiveInteger( &CurrentNode );
	arc.ArchiveSafePointer( &goal );
	arc.ArchiveBoolean( &NeedNextNode );
}

class FlyNodePath : public Behavior
   {
	private:
		str						anim;
		float						turn_speed;
		float						speed;
		FlyToPoint				fly;
		//float						original_z;
		str						NodeType;
		int						NumberOfNodes;
		int                  CurrentNode;
      PathNodePtr            goal;
		qboolean					NeedNextNode;

	public:
      CLASS_PROTOTYPE( FlyNodePath );

									FlyNodePath();

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void FlyNodePath::Archive( Archiver &arc )
{
	Behavior::Archive( arc );

	arc.ArchiveString( &anim );
	arc.ArchiveFloat( &turn_speed );
	arc.ArchiveFloat( &speed );
	arc.ArchiveObject( &fly );
	//arc.ArchiveFloat( &original_z );
	arc.ArchiveString( &NodeType );
	arc.ArchiveInteger( &NumberOfNodes );
	arc.ArchiveInteger( &CurrentNode );
	arc.ArchiveSafePointer( &goal );
	arc.ArchiveBoolean( &NeedNextNode );
}

class FlyCircle : public Behavior
	{
	private:
		str						anim;
		FlyToPoint				fly;
		float						original_z;
		qboolean					fly_clockwise;
		qboolean             circle_player;			

	public:
      CLASS_PROTOTYPE( FlyCircle );

									FlyCircle();

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void FlyCircle::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveString( &anim );
	arc.ArchiveObject( &fly );
	arc.ArchiveFloat( &original_z );
	arc.ArchiveBoolean( &fly_clockwise );
	arc.ArchiveBoolean( &circle_player );	
   }

class FlyCircleRandomPoint : public Behavior
   {
	private:
		str						anim;
		FlyToPoint				fly;
		//float						original_z;
		qboolean					fly_clockwise;
		float						change_course_time;
		float						next_change_course_time;
		Vector					goal;		
		qboolean					try_to_go_up;
		//float						speed;

	public:
		CLASS_PROTOTYPE( FlyCircleRandomPoint );

									FlyCircleRandomPoint();
		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void FlyCircleRandomPoint::Archive( Archiver &arc )
{
	Behavior::Archive( arc );

	arc.ArchiveString( &anim );
	arc.ArchiveObject( &fly );
	//arc.ArchiveFloat( &original_z );
	arc.ArchiveBoolean( &fly_clockwise );

	arc.ArchiveFloat( &change_course_time );
	arc.ArchiveFloat( &next_change_course_time );
	arc.ArchiveVector( &goal );
	arc.ArchiveBoolean( &try_to_go_up );
}

class FlyDive : public Behavior
	{
	private:
		str						anim;
		FlyToPoint				fly;
		Vector					goal;
		float						speed;
		//float						old_speed;
		float						damage;

	public:
      CLASS_PROTOTYPE( FlyDive );

									FlyDive();

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void FlyDive::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveString( &anim );
	arc.ArchiveObject( &fly );
	arc.ArchiveVector( &goal );
	arc.ArchiveFloat( &speed );
	//arc.ArchiveFloat( &old_speed );
	arc.ArchiveFloat( &damage );
   }

class FlyCharge : public Behavior
	{
	private:
		str						anim;
		FlyToPoint				fly;
		Vector					goal;
		float						speed;
		//float						old_speed;
		float						damage;

	public:
      CLASS_PROTOTYPE( FlyCharge );

									FlyCharge();

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void FlyCharge::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveString( &anim );
	arc.ArchiveObject( &fly );
	arc.ArchiveVector( &goal );
	arc.ArchiveFloat( &speed );
	//arc.ArchiveFloat( &old_speed );
	arc.ArchiveFloat( &damage );
   }

class FlyStrafe : public Behavior
	{
	private:
		str						anim;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		float						speed = 0.0f;
		qboolean             right = qfalse;
		float                roll = 0.0f;
		

	public:
      CLASS_PROTOTYPE( FlyStrafe );

									FlyStrafe();

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void FlyStrafe::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveString( &anim );
	arc.ArchiveFloat( &speed );
	arc.ArchiveBoolean( &right );
	arc.ArchiveFloat( &roll );
   }

class FlyClimb : public Behavior
	{
	private:
		str						anim;
		FlyToPoint				fly;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		Vector					goal = Vector( 0.0f, 0.0f, 0.0f );
		float						height = 0.0f;
		float						speed = 0.0f;
		float						next_height_check = 0.0f;
		float						last_check_height = 0.0f;
		float                collision_buffer = 0.0f;

	public:
      CLASS_PROTOTYPE( FlyClimb );

									FlyClimb();

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void FlyClimb::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveString( &anim );
	arc.ArchiveObject( &fly );
	arc.ArchiveVector( &goal );
	arc.ArchiveFloat( &height );
	arc.ArchiveFloat( &speed );
	arc.ArchiveFloat( &next_height_check );
	arc.ArchiveFloat( &last_check_height );
	arc.ArchiveFloat( &collision_buffer );
   }

class FlySplinePath : public Behavior
   {
   private:
	   BSpline  splinePath;


	   //--------------------------------------------------------------
	   // GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	   //--------------------------------------------------------------
		EntityPtr ent = nullptr;
		SplinePathPtr currentNode = nullptr;
		qboolean clamp = qfalse;
		qboolean ignoreAngles = qfalse;
		qboolean splineAngles = qfalse;
		float startTime = 0.0f;		
		Vector oldGoal = Vector( 0.0f, 0.0f, 0.0f );
		qboolean havePath = qfalse;
		

	public:
      CLASS_PROTOTYPE( FlySplinePath );

									FlySplinePath();
		
		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );

		void                 CreatePath( SplinePath *path,	splinetype_t type );
	};

inline void FlySplinePath::Archive
   (
	Archiver &arc
	)
	{
	Behavior::Archive( arc );

	arc.ArchiveSafePointer( &ent );
	arc.ArchiveObject( &splinePath );
	arc.ArchiveSafePointer( &currentNode );
	arc.ArchiveBoolean( &clamp );
	arc.ArchiveBoolean( &ignoreAngles );
	arc.ArchiveBoolean( &splineAngles );
	arc.ArchiveFloat( &startTime );
	arc.ArchiveVector( &oldGoal );
	arc.ArchiveBoolean( &havePath );
	}

class Land : public Behavior
	{
	private:
		str						anim;

	public:
      CLASS_PROTOTYPE( Land );

									Land();

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void Land::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveString( &anim );
   }

class VerticalTakeOff : public Behavior
	{
	private:
		str					anim;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		float             speed = 0.0f;
		float             height = 0.0f;


	public:
      CLASS_PROTOTYPE( VerticalTakeOff );

									VerticalTakeOff();

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void VerticalTakeOff::Archive
   (
	Archiver &arc 
	)
	{
	Behavior::Archive( arc );

	arc.ArchiveString( &anim );
	arc.ArchiveFloat( &speed );
	arc.ArchiveFloat( &height );
	}

class Hover : public Behavior
	{
	private:
		str						anim;
		FlyToPoint				fly;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		Vector					goal = Vector( 0.0f , 0.0f , 0.0f );


	public:
      CLASS_PROTOTYPE( Hover );

									Hover();

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void Hover::Archive( Archiver &arc )
{
	Behavior::Archive( arc );

	arc.ArchiveString( &anim );
	arc.ArchiveObject( &fly );
	arc.ArchiveVector( &goal );
}


// Fixme / don't think this works right now

class Wander : public Behavior
	{
	private:
		//ObstacleAvoidance    avoid;
		str						anim;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
      float                avoidtime = 1.0f;
      Vector               avoidvec = Vector( 0.0f, 0.0f, 0.0f );


	public:
      CLASS_PROTOTYPE( Wander );

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void Wander::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   //arc.ArchiveObject( &avoid );
   arc.ArchiveString( &anim );
   arc.ArchiveFloat( &avoidtime );
   arc.ArchiveVector( &avoidvec );
   }

class CircleCurrentEnemy : public Behavior
	{
	private:
		str anim;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		float radius = 0.0f;
		qboolean maintainDistance = qfalse;
		qboolean clockwise = qfalse;
		Vector dirToEnemy = Vector( 0.0f , 0.0f, 0.0f );

		float turnAngle = 0.0f;
		float oldAngle = 0.0f;
		float angleStep = 0.0f;

		int stuck = 0;
		int stuckCheck = 0;

		qboolean angleAdjusted = qfalse;
		
		
	public:
      CLASS_PROTOTYPE( CircleCurrentEnemy );

		void						SetArgs( Event *ev );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
		virtual void         Archive( Archiver &arc );
	};

inline void CircleCurrentEnemy::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveString( &anim );	
	arc.ArchiveFloat( &radius );
	arc.ArchiveBoolean( &maintainDistance );
	arc.ArchiveBoolean( &clockwise );
	arc.ArchiveVector( &dirToEnemy );
	arc.ArchiveFloat( &turnAngle );
	arc.ArchiveFloat( &oldAngle );
	arc.ArchiveFloat( &angleStep );
	arc.ArchiveInteger( &stuck );
	arc.ArchiveInteger( &stuckCheck );
	arc.ArchiveBoolean( &angleAdjusted );

   }

class ChaoticDodge : public Behavior
	{
	private:
		str anim;
		

		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		float turnAngle = 0.0f;
		float oldAngle = 0.0f;
		float angleStep = 0.0f;
		float time = 0.0f;
		float changeTime = 0.0f;

		int stuck = 0;
		int stuckCheck = 0;
		qboolean adjusting = qfalse;

		qboolean angleAdjusted = qfalse;		
		float turnspeed = 0.0f;
		float turnTime = 0.0f;
		
		
	public:
      CLASS_PROTOTYPE( ChaoticDodge );

		void						SetArgs( Event *ev );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
		float						GetNewYaw();
		virtual void         Archive( Archiver &arc );

	};

inline void ChaoticDodge::Archive( Archiver &arc )
{
	Behavior::Archive( arc );

	arc.ArchiveString( &anim );	
	arc.ArchiveFloat( &turnAngle );
	arc.ArchiveFloat( &oldAngle );
	arc.ArchiveFloat( &angleStep );
	arc.ArchiveFloat( &time );
	arc.ArchiveFloat( &changeTime );
	
	arc.ArchiveInteger( &stuck );
	arc.ArchiveInteger( &stuckCheck );
	arc.ArchiveBoolean( &adjusting );

	arc.ArchiveBoolean( &angleAdjusted );
	arc.ArchiveFloat( &turnspeed );
	arc.ArchiveFloat( &turnTime );
}

class GetCloseToEnemy : public Behavior
	{
	private:
		str						anim;
		FollowPathToEntity	chase;
		Wander					wander;

		
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		qboolean					forever = qfalse;
		float						next_think_time = 0.0f;


	public:
      CLASS_PROTOTYPE( GetCloseToEnemy );

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void GetCloseToEnemy::Archive
	(
	Archiver &arc
	)

   {
   Behavior::Archive( arc );

   arc.ArchiveString( &anim );
   arc.ArchiveObject( &chase );
	arc.ArchiveObject( &wander );
	arc.ArchiveBoolean( &forever );
	arc.ArchiveFloat( &next_think_time );
   }

class GetCloseToPlayer : public Behavior
	{
	private:
		str						anim;
		FollowPathToEntity	chase;
		Wander					wander;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		qboolean					forever = qfalse;
		float						next_think_time = 0.0f;
		float                speed = 0.0f;		

	public:
      CLASS_PROTOTYPE( GetCloseToPlayer );

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void GetCloseToPlayer::Archive
	(
	Archiver &arc
	)

   {
   Behavior::Archive( arc );

   arc.ArchiveString( &anim );
   arc.ArchiveObject( &chase );
	arc.ArchiveObject( &wander );
	arc.ArchiveBoolean( &forever );
	arc.ArchiveFloat( &next_think_time );
	arc.ArchiveFloat( &speed );	
   }

class GetWithinRangeOfPlayer : public Behavior
	{
	private:
		str						anim;
		FollowPathToEntity	chase;
		Wander					wander;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		qboolean					forever = qfalse;
		float						next_think_time = 0.0f;
		float                speed = 0.0f;
      float                startRangeMax = 0.0f;
      float                startRangeMin = 0.0f;


	public:
      CLASS_PROTOTYPE( GetWithinRangeOfPlayer );

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void GetWithinRangeOfPlayer::Archive( Archiver &arc )
{
	Behavior::Archive( arc );

	arc.ArchiveString( &anim );
	arc.ArchiveObject( &chase );
	arc.ArchiveObject( &wander );
	arc.ArchiveBoolean( &forever );
	arc.ArchiveFloat( &next_think_time );
	arc.ArchiveFloat( &speed );	
	arc.ArchiveFloat( &startRangeMax );
	arc.ArchiveFloat( &startRangeMin );
}

class RetreatFromEnemy : public Behavior
	{
	private:
		str						anim;
		FollowPathToPoint		chase;
		Wander					wander;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		qboolean					forever = qfalse;
		float						next_think_time = 0.0f;


	public:
      CLASS_PROTOTYPE( RetreatFromEnemy );

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void RetreatFromEnemy::Archive
	(
	Archiver &arc
	)

   {
   Behavior::Archive( arc );

   arc.ArchiveString( &anim );
   arc.ArchiveObject( &chase );
	arc.ArchiveObject( &wander );
	arc.ArchiveBoolean( &forever );
	arc.ArchiveFloat( &next_think_time );
   }

class GotoDeadEnemy : public Behavior
	{
	private:
		str						anim;
		FollowPathToPoint		chase;

	public:
      CLASS_PROTOTYPE( GotoDeadEnemy );

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void GotoDeadEnemy::Archive
	(
	Archiver &arc
	)

   {
   Behavior::Archive( arc );

   arc.ArchiveString( &anim );
   arc.ArchiveObject( &chase );
   }

class Investigate : public Behavior
{
private:
	FollowPathToPoint		chase;
	str						moveanim;
	str						lookaroundanim;
	TurnTo					turnto;


	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
	//--------------------------------------------------------------
	Vector					goal = Vector( 0.0f, 0.0f, 0.0f );
	float					curioustime = 0.0f;
	float					lookaroundtime = 0.0f;
	float					lookaroundtime_end = 0.0f;
	float					investigate_time = 0.0f;
	int						mode = 0;
	Vector					start_pos = Vector(0.0f, 0.0f, 0.0f);
	float					start_yaw = 0.0f;
	qboolean				return_to_original_location = qfalse;
	

public:
	CLASS_PROTOTYPE( Investigate );
	Investigate();
	void					SetArgs( Event *ev );
	void					ShowInfo( Actor &self );
	void					Begin( Actor &self );
	BehaviorReturnCode_t	Evaluate( Actor &self );
	void					End( Actor &self );
	virtual void			Archive( Archiver &arc );
};

inline void Investigate::Archive
(
	Archiver &arc
	)
{
	Behavior::Archive( arc );
	
	arc.ArchiveObject( &chase );
	arc.ArchiveString( &moveanim );
	arc.ArchiveString( &lookaroundanim );
	arc.ArchiveVector( &goal );
	arc.ArchiveFloat( &curioustime );
	arc.ArchiveFloat( &lookaroundtime );
	arc.ArchiveFloat( &lookaroundtime_end );
	arc.ArchiveObject( &turnto );
	arc.ArchiveFloat( &investigate_time );
	arc.ArchiveInteger( &mode );
	arc.ArchiveVector( &start_pos );
	arc.ArchiveFloat( &start_yaw );
	arc.ArchiveBoolean( &return_to_original_location  );
}


class TurnInvestigate : public Behavior
	{
	private:
		str						left_anim;
		str						right_anim;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		float						turn_speed = 0.0f;
		Vector					goal = Vector( 0.0f, 0.0f, 0.0f );


	public:
      CLASS_PROTOTYPE( TurnInvestigate );

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void TurnInvestigate::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveString( &left_anim );
	arc.ArchiveString( &right_anim );
	arc.ArchiveFloat( &turn_speed );
   arc.ArchiveVector( &goal );
   }

class TurnToEnemy : public Behavior
	{
	private:
		str						left_anim;
		str						right_anim;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		float						turn_speed = 0.0f;
		qboolean					forever = qfalse;
		qboolean					anim_done = qfalse;
		qboolean					use_last_known_position = qfalse;


	public:
      CLASS_PROTOTYPE( TurnToEnemy );

		void						SetArgs( Event *ev );
		void						AnimDone( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void TurnToEnemy::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveString( &left_anim );
	arc.ArchiveString( &right_anim );
	arc.ArchiveFloat( &turn_speed );
	arc.ArchiveBoolean( &forever );
	arc.ArchiveBoolean( &anim_done );
	arc.ArchiveBoolean( &use_last_known_position );
   }

class Teleport : public Behavior
	{
	private:
	public:
      CLASS_PROTOTYPE( Teleport );

      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		qboolean					TestPosition( Actor &self, int test_pos, Vector &good_position, qboolean use_enemy_dir );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
	};

class TeleportToPlayer : public Behavior
	{
	private:
	public:
		CLASS_PROTOTYPE( TeleportToPlayer );

      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		qboolean					TestPosition( Actor &self, int test_pos, Vector &good_position, Entity* player, qboolean use_player_dir );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );		
	};

class TeleportToPosition : public Behavior
	{
	private:
		str						teleport_position_name;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		int						number_of_teleport_positions = 0;


	public:
      CLASS_PROTOTYPE( TeleportToPosition );

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		//qboolean					TestPosition( Actor &self, int test_pos, const Vector &good_position );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
		virtual void         Archive( Archiver &arc );
	};

inline void TeleportToPosition::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

	arc.ArchiveString( &teleport_position_name );
	arc.ArchiveInteger( &number_of_teleport_positions );
   }

class GhostAttack : public Behavior
	{
	private:
		FlyToPoint				fly;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		int						mode = 0;
		Vector					attack_dir = Vector( 0.0f , 0.0f , 0.0f );
		Vector					attack_position = Vector(0.0f, 0.0f, 0.0f);;
		Vector					retreat_position = Vector(0.0f, 0.0f, 0.0f);;
		qboolean					real_attack = qfalse;


	public:
      CLASS_PROTOTYPE( GhostAttack );

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
		virtual void         Archive( Archiver &arc );
	};

inline void GhostAttack::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

	arc.ArchiveInteger( &mode );
	arc.ArchiveVector( &attack_dir );
	arc.ArchiveVector( &attack_position );
	arc.ArchiveVector( &retreat_position );
	arc.ArchiveObject( &fly );
	arc.ArchiveBoolean( &real_attack );
   }

class Levitate : public Behavior
	{
	private:
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		float						distance = 0.0f;
		float						speed = 0.0f;
		float						final_z = 0.0f;


	public:
      CLASS_PROTOTYPE( Levitate );

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
		virtual void         Archive( Archiver &arc );
	};

inline void Levitate::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

	arc.ArchiveFloat( &distance );
	arc.ArchiveFloat( &speed );
	arc.ArchiveFloat( &final_z );
   }

//
// WayPoint Behaviors
//
class GotoWayPoint : public Behavior
	{
	private:
		str						anim;
		str                  path_name;
		str                  start_point;
		FollowPathToPoint		chase;
		Wander					wander;
		str                  current_waypoint_name;
		WayPointNodePtr        current_waypoint;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		float						next_think_time = 0.0f;


	public:
      CLASS_PROTOTYPE( GotoWayPoint );

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
		WayPointNode*        GetWayPoint( const str& waypoint_name );		

	   virtual void         Archive( Archiver &arc );
	};

inline void GotoWayPoint::Archive( Archiver &arc )
{
	Behavior::Archive( arc );

	arc.ArchiveString( &anim );
	arc.ArchiveString( &path_name );
	arc.ArchiveString( &start_point );
	arc.ArchiveObject( &chase );
	arc.ArchiveObject( &wander );	
	arc.ArchiveString( &current_waypoint_name );
	arc.ArchiveSafePointer( &current_waypoint );
	arc.ArchiveFloat( &next_think_time );
}

//=============================================
// FlyCircleAroundWaypoint
// Allows a flying creature to circle around a given
// Waypoint node.
//=============================================
class FlyCircleAroundWaypoint : public Behavior
	{
	private:
		str						anim;
		FlyToPoint				fly;
		//float						original_z;
		str                  waypointname;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		qboolean				fly_clockwise = qfalse;
		qboolean             nearestPlayer = qfalse;

	public:
      CLASS_PROTOTYPE( FlyCircleAroundWaypoint );

									FlyCircleAroundWaypoint();

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
		WayPointNode         *GetWayPoint( Actor &self );
		WayPointNode         *GetWayPointNearestPlayer( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void FlyCircleAroundWaypoint::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveString( &anim );
	arc.ArchiveObject( &fly );
	//arc.ArchiveFloat( &original_z );
	arc.ArchiveBoolean( &fly_clockwise );
	arc.ArchiveString( &waypointname );
	arc.ArchiveBoolean( &nearestPlayer );
   }

//================================================================
// Sort of a hack for now.  This is basically a specialized version
// of Fly to point.  But it's not really cleaned up.
//===============================================================
class HelicopterFlyToPoint : public Behavior
	{
	private:
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
      float                avoidtime;
		Vector					target_angle = Vector( 0.0f , 0.0f , 0.0f );
		float						turn_speed = 0.0f;
		float						old_turn_speed = 0.0f;
		float						speed = 0.0f;
		float						old_forward_speed = 0.0f;
		Vector					goal = Vector(0.0f, 0.0f, 0.0f);
		qboolean					random_allowed = qfalse;
		qboolean					force_goal = qfalse;
		int						stuck = 0;
		Vector					temp_goal = Vector(0.0f, 0.0f, 0.0f);
		qboolean					use_temp_goal = qfalse;
		qboolean					adjustYawAndRoll = qfalse;
		qboolean             offsetOrigin = qfalse;
		

	public:
      CLASS_PROTOTYPE( HelicopterFlyToPoint );

									HelicopterFlyToPoint();

		void						SetTurnSpeed( float new_turn_speed );
		void						SetGoalPoint( const Vector &goal_point );
		void						SetRandomAllowed( qboolean allowed );
		void						SetSpeed( float speed_value );
		void						ForceGoal( void );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		float						LerpAngle( float old_angle, float new_angle, float lerp_amount );
		void						End( Actor &self );
		virtual void         Archive( Archiver &arc );
		void						setAdjustYawAndRoll( bool adjust ) { adjustYawAndRoll = adjust; }					
		void						setOffsetOrigin( bool setOffset ) { offsetOrigin = setOffset;		}
	};

inline void HelicopterFlyToPoint::Archive( Archiver &arc )
{
	Behavior::Archive( arc );

	arc.ArchiveFloat( &avoidtime );
	arc.ArchiveVector( &target_angle );
	arc.ArchiveFloat( &turn_speed );
	arc.ArchiveFloat( &old_turn_speed );
	arc.ArchiveFloat( &speed );
	arc.ArchiveFloat( &old_forward_speed );
	arc.ArchiveVector( &goal );
	arc.ArchiveBoolean( &random_allowed );
	arc.ArchiveBoolean( &force_goal );
	arc.ArchiveInteger( &stuck );
	arc.ArchiveVector( &temp_goal );
	arc.ArchiveBoolean( &use_temp_goal );
	arc.ArchiveBoolean( &adjustYawAndRoll );
	arc.ArchiveBoolean( &offsetOrigin );	
}

//================================================================
// Sort of a hack for now.  This is basically a specialized version
// of Fly to point.  But it's not really cleaned up.
//===============================================================
class HelicopterFlyCircle : public Behavior
	{
	private:
		str						anim;
		HelicopterFlyToPoint	fly;
		//float						original_z;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		qboolean					fly_clockwise = qfalse;
		qboolean             circle_player = qfalse;			

	public:
      CLASS_PROTOTYPE( HelicopterFlyCircle );

									HelicopterFlyCircle();

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );		
	   virtual void         Archive( Archiver &arc );
	};

inline void HelicopterFlyCircle::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveString( &anim );
	arc.ArchiveObject( &fly );
	//arc.ArchiveFloat( &original_z );
	arc.ArchiveBoolean( &fly_clockwise );
	arc.ArchiveBoolean( &circle_player );	
   }

//===================================================
// Helicopter Strafe Attack
//==================================================
class HelicopterStrafeAttack : public Behavior
	{
	private:
		str						anim;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		Vector               dir = Vector(0.0f, 0.0f, 0.0f);
		Vector               targetAngles = Vector(0.0f, 0.0f, 0.0f);
		//float                angleDelta;
		float                turnTime = 0.0f;
		float                lerpStart = 0.0f;
		float                lerpEnd = 0.0f;
		float                startYaw = 0.0f;
		float                endYaw = 0.0f;
		qboolean             setUpLerp = qfalse;
		qboolean             completedLerp = qfalse;
		Vector               goal = Vector(0.0f, 0.0f, 0.0f);

				
	public:
      CLASS_PROTOTYPE( HelicopterStrafeAttack );

									HelicopterStrafeAttack();

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
		WayPointNode         *GetWayPointNearestPlayer( Actor &self );
		WayPointNode         *GetWayPoint( Actor &self , const str &name );
		void                 LerpToNewAngle( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void HelicopterStrafeAttack::Archive( Archiver &arc )
{
	Behavior::Archive( arc );

	arc.ArchiveString( &anim );
	arc.ArchiveVector( &dir );
	arc.ArchiveVector( &targetAngles );
	arc.ArchiveFloat( &turnTime );

	arc.ArchiveFloat( &lerpStart );
	arc.ArchiveFloat( &lerpEnd );
	arc.ArchiveFloat( &startYaw );
	arc.ArchiveFloat( &endYaw );
	arc.ArchiveBoolean( &setUpLerp );
	arc.ArchiveBoolean( &completedLerp );

	arc.ArchiveVector( &goal );
}

//=============================================
// HelicopterFlyToWaypoint
// Allows a flying creature to circle around a given
// Waypoint node.
//=============================================
class HelicopterFlyToWaypoint : public Behavior
	{
	private:
		str						anim;
		HelicopterFlyToPoint	fly;
		str                  waypointname;

		
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		qboolean             nearestPlayer = qfalse;
		qboolean             nearestPlayerTarget = qfalse;


	public:
      CLASS_PROTOTYPE( HelicopterFlyToWaypoint );

									HelicopterFlyToWaypoint();

		void						SetArgs( Event *ev );
      void                 ShowInfo( Actor &self );
		void						Begin( Actor &self );
		BehaviorReturnCode_t					Evaluate( Actor &self );
		void						End( Actor &self );
		WayPointNode         *GetWayPoint( Actor &self );
		WayPointNode         *GetWayPointNearestPlayer( Actor &self );
	   virtual void         Archive( Archiver &arc );
	};

inline void HelicopterFlyToWaypoint::Archive
	(
	Archiver &arc
	)
   {
   Behavior::Archive( arc );

   arc.ArchiveString( &anim );
	arc.ArchiveObject( &fly );
	arc.ArchiveString( &waypointname );
	arc.ArchiveBoolean( &nearestPlayer );
	arc.ArchiveBoolean( &nearestPlayerTarget );
   }

//
//I know, I know... It hurts me too, but until I get all this behavior transition stuff squared away
//there's not much that can be done about it.
//
#include "PlayAnim.hpp"

#endif /* behavior.h */
