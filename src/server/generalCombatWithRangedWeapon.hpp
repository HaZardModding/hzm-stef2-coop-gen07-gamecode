//-----------------------------------------------------------------------------
//
//  $Logfile:: /Code/DLLs/game/generalCombatWithRangedWeapon.hpp              $
// $Revision:: 169                                                            $
//   $Author:: Bschofield                                                     $
//     $Date:: 4/26/02 2:22p                                                  $
//
// Copyright (C) 2002 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
//
//
// DESCRIPTION:
// GeneralCombatWithMeleeWeapon Behavior Definition
//
//--------------------------------------------------------------------------------

//==============================
// Forward Declarations
//==============================
class GeneralCombatWithRangedWeapon;

#ifndef __GENERALCOMBAT_WITH_RANGEDWEAPON_HPP__
#define __GENERALCOMBAT_WITH_RANGEDWEAPON_HPP__

#include "behavior.h"
#include "behaviors_general.h"

//------------------------- CLASS ------------------------------
//
// Name:           GeneralCombatWithRangedWeapon
// Base Class:     Behavior
//
// Description:    MetaBehavior for Sentient Type ( No relation to the sentient class )
//                 of Actor for executing Combat with no additional information 
//                 ( i.e. Helper Nodes ) in the near vicinity. 
//                 We may need to subclass this behavior for additional
//                 individuality, like "teammate general combat"
//
// Method of Use:  Called From State Machine
//--------------------------------------------------------------
class GeneralCombatWithRangedWeapon : public Behavior
	{
	//------------------------------------
	// States
	//------------------------------------
	public:  
		typedef enum
		{		
		GC_WRW_CHANGE_POSTURE_TO_ADVANCE,
		GC_WRW_CHANGE_POSTURE_TO_RETREAT,
		GC_WRW_ADVANCE,
		GC_WRW_ADVANCE_FIRING,
		GC_WRW_RETREAT,
		GC_WRW_RETREAT_FIRING,
		GC_WRW_STRAFE,		
		GC_WRW_CHANGE_POSTURE_DUCK,
		GC_WRW_DUCKED,
		GC_WRW_DUCKED_FIRING,
		GC_WRW_CHANGE_POSTURE_STAND,
		GC_WRW_STAND,
		GC_WRW_STAND_FIRING,
		GC_WRW_FAILED
		} generalCombatStates_t;

	//------------------------------------
	// Parameters
	//------------------------------------
	private:
		str								_movementAnim;
		str								_torsoAnim;
		str								_fireAnim;
		str								_preFireAnim;
		str								_postFireAnim;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		float							_approachDist = 0.0f;
		float							_retreatDist = 0.0f;
		float							_strafeChance = 0.0f;
		float							_strafeTime = 0.0f;
		float							_strafeRandomFactor = 0.0f;
		float							_postureChangeChance = 0.0f;
		float							_fireTimeMin = 0.0f;
		float							_fireTimeMax = 0.0f;
		float							_fireTimeRandomFactor = 0.0f;
		float							_pauseTimeMin = 0.0f;
		float							_pauseTimeMax = 0.0f;
		float							_pauseTimeRandomFactor = 0.0f;


	//-------------------------------------
	// Internal Functionality
	//-------------------------------------
	protected:
		void	transitionToState			( generalCombatStates_t state );
		void	setInternalState			( generalCombatStates_t state , const str &stateName );
		void	init						( Actor &self );
		void	think						();
		void	updateEnemy					();
		void	setTorsoAnim				();			
		bool	checkShouldApproach			( float distToEnemy );
		bool	checkShouldRetreat			( float distToEnemy );
		bool	checkShouldStrafe			( float distToEnemy );
		bool	checkShouldChangePosture	( float distToEnemy );		


		void					setupRotate		();
		BehaviorReturnCode_t	evaluateRotate	();

		void					setupStateChangePostureToAdvance	();
		BehaviorReturnCode_t	evaluateStateChangePostureToAdvance	();
		void					failureStateChangePostureToAdvance	( const str& failureReason );

		void					setupStateChangePostureToRetreat	();
		BehaviorReturnCode_t	evaluateStateChangePostureToRetreat	();
		void					failureStateChangePostureToRetreat	( const str& failureReason );

		void					setupStateAdvance					();
		BehaviorReturnCode_t	evaluateStateAdvance				();
		void					failureStateAdvance					( const str& failureReason );

		void					setupStateAdvanceFiring				();
		BehaviorReturnCode_t	evaluateStateAdvanceFiring			();
		void					failureStateAdvanceFiring			( const str& failureReason );

		void					setupStateRetreat					();
		BehaviorReturnCode_t	evaluateStateRetreat				();
		void					failureStateRetreat					( const str& failureReason );

		void					setupStateRetreatFiring				();
		BehaviorReturnCode_t	evaluateStateRetreatFiring			();
		void					failureStateRetreatFiring			( const str& failureReason );

		void					setupStateStrafe					();
		BehaviorReturnCode_t	evaluateStateStrafe					();
		void					failureStateStrafe					( const str& failureReason );

		void					setupStateChangePostureDuck			();
		BehaviorReturnCode_t	evaluateStateChangePostureDuck		();
		void					failureStateChangePostureDuck		( const str& failureReason );

		void					setupStateDucked					();
		BehaviorReturnCode_t	evaluateStateDucked					();
		void					failureStateDucked					( const str& failureReason );

		void					setupStateDuckedFiring				();
		BehaviorReturnCode_t	evaluateStateDuckedFiring			();
		void					failureStateDuckedFiring			( const str& failureReason );

		void					setupStateChangePostureStand		();
		BehaviorReturnCode_t	evaluateStateChangePostureStand		();
		void					failureStateChangePostureStand		( const str& failureReason );

		void					setupStateStand						();
		BehaviorReturnCode_t	evaluateStateStand					();
		void					failureStateStand					( const str& failureReason );

		void					setupStateStandFiring				();
		BehaviorReturnCode_t	evaluateStateStandFiring			();
		void					failureStateStandFiring				( const str& failureReason );

	//-------------------------------------
	// Public Interface
	//-------------------------------------
	public:
		CLASS_PROTOTYPE( GeneralCombatWithRangedWeapon );

										GeneralCombatWithRangedWeapon();
									   ~GeneralCombatWithRangedWeapon();

		void							SetArgs					( Event *ev );      
		void							AnimDone				( Event *ev );
		void							PostureDone				( Event *ev );

		void							Begin					( Actor &self );		
		BehaviorReturnCode_t			Evaluate				( Actor &self );
		void							End						( Actor &self );
	

		void							SetMovementAnim			 ( const str& movementAnim		) { _movementAnim = movementAnim;					}
		void							SetTorsoAnim			 ( const str& torsoAnim			) { _torsoAnim = torsoAnim;							}
		void							SetFireAnim				 ( const str& fireAnim			) { _fireAnim = fireAnim;							}
		void							SetPreFireAnim			 ( const str& preFireAnim		) { _preFireAnim = preFireAnim;						}
		void							SetPostFireAnim			 ( const str& postFireAnim		) {	_postFireAnim = postFireAnim;					}
		void							SetApproachDist			 ( float approachDist			) { _approachDist = approachDist;					}
		void							SetRetreatDist			 ( float retreatDist			) { _retreatDist = retreatDist;						}
		void							SetStrafeChance			 ( float strafeChance			) { _strafeChance = strafeChance;					}
		void							SetStrafeTime			 ( float strafeTime				) { _strafeTime = strafeTime;						}
		void							SetStrafeRandomFactor	 ( float strafeRandomFactor		) { _strafeRandomFactor = strafeRandomFactor;		}
		void							SetPostureChangeChance	 ( float postureChangeChance	) { _postureChangeChance = postureChangeChance;		}		
		void							SetFireTimeMin			 ( float fireTimeMin			) { _fireTimeMin = fireTimeMin;						}
		void							SetFireTimeMax			 ( float fireTimeMax			) { _fireTimeMax = fireTimeMax;						}
		void							SetFireTimeRandomFactor  ( float fireTimeRandomFactor	) { _fireTimeRandomFactor = fireTimeRandomFactor;	}
		void							SetPauseTimeMin			 ( float pauseTimeMin			) { _pauseTimeMin = pauseTimeMin;					}
		void							SetPauseTimeMax			 ( float pauseTimeMax			) { _pauseTimeMax = pauseTimeMax;					}
		void							SetPauseTimeRandomFactor ( float pauseTimeRandomFactor	) { _pauseTimeRandomFactor = pauseTimeRandomFactor;	}

		virtual void					Archive  ( Archiver &arc );

	//-------------------------------------
	// Components
	//-------------------------------------
	private: 	
		RotateToEntity					_rotate;
		MoveRandomDirection				_moveRandomDir;		
		FireWeapon						_fireWeapon;
		Strafe							_strafeComponent;


	//-------------------------------------
	// Member Variables
	//-------------------------------------
	private: 
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		generalCombatStates_t			_state = GC_WRW_FAILED;
		Actor						   *_self = nullptr;
		EntityPtr						_currentEnemy = nullptr;
		bool							_finishedPostureTransition = false;
		bool							_randomAdvanceLockOut = false;
		bool							_randomRetreatLockOut = false;

		float							_nextRotateTime = 0.0f;
		float							_nextStrafeAttemptTime = 0.0f;
		float							_nextPostureChange = 0.0f;
		float							_nextClearRetreatLockOutTime = 0.0f;
		float							_nextClearAdvanceLockOutTime = 0.0f;
		      
		int								_randomAdvanceFailures = 0;
		int								_randomRetreatFailures = 0;

		float							_nextFireTime = 0.0f;
		float							_nextPauseTime = 0.0f;
};


inline void GeneralCombatWithRangedWeapon::Archive( Archiver &arc	)
{
	Behavior::Archive ( arc );	 
    
		
	//
	// Archive Parameters
	//
	arc.ArchiveString( &_movementAnim );
	arc.ArchiveString( &_torsoAnim );
	arc.ArchiveString( &_fireAnim );
	arc.ArchiveString( &_preFireAnim );
	arc.ArchiveString( &_postFireAnim );
	arc.ArchiveFloat( &_approachDist );
	arc.ArchiveFloat( &_retreatDist );
	arc.ArchiveFloat( &_strafeChance );
	arc.ArchiveFloat( &_strafeTime );
	arc.ArchiveFloat( &_strafeRandomFactor );
	arc.ArchiveFloat( &_postureChangeChance );
	arc.ArchiveFloat( &_fireTimeMin );
	arc.ArchiveFloat( &_fireTimeMax );
	arc.ArchiveFloat( &_fireTimeRandomFactor );
	arc.ArchiveFloat( &_pauseTimeMin );
	arc.ArchiveFloat( &_pauseTimeMax );
	arc.ArchiveFloat( &_pauseTimeRandomFactor );

	//
	// Archive Components
	//
	arc.ArchiveObject( &_rotate );
	arc.ArchiveObject( &_moveRandomDir );
	arc.ArchiveObject( &_fireWeapon );
	arc.ArchiveObject( &_strafeComponent );	

	//
	// Archive Member Variables
	//
	ArchiveEnum					( _state, generalCombatStates_t   );
	arc.ArchiveObjectPointer	( ( Class ** )&_self			);
	arc.ArchiveSafePointer		( &_currentEnemy );
	arc.ArchiveBool				( &_finishedPostureTransition );
	arc.ArchiveBool				( &_randomAdvanceLockOut );
	arc.ArchiveBool				( &_randomRetreatLockOut );
	arc.ArchiveFloat			( &_nextRotateTime );
	arc.ArchiveFloat			( &_nextStrafeAttemptTime );
	arc.ArchiveFloat			( &_nextPostureChange );
	arc.ArchiveFloat			( &_nextClearRetreatLockOutTime );
	arc.ArchiveFloat			( &_nextClearAdvanceLockOutTime );
	arc.ArchiveInteger			( &_randomAdvanceFailures );
	arc.ArchiveInteger			( &_randomRetreatFailures );
	arc.ArchiveFloat			( &_nextFireTime );
	arc.ArchiveFloat			( &_nextPauseTime );	
	

}


#endif /* __GENERALCOMBAT_WITH_RANGEDWEAPON_HPP__ */

