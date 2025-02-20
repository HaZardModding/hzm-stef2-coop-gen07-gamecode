//-----------------------------------------------------------------------------
//
//  $Logfile:: /Code/DLLs/game/healGroupMember.hpp				              $
// $Revision:: 1                                                            $
//   $Author:: Sketcher                                                     $
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
// healGroupMember Behavior Definition
//
//--------------------------------------------------------------------------------

//==============================
// Forward Declarations
//==============================
class HealGroupMember;

#ifndef __HEAL_GROUP_MEMBER_HPP__
#define __HEAL_GROUP_MEMBER_HPP__

#include "behavior.h"
#include "behaviors_general.h"
#include "rotateToEntity.hpp"

//------------------------- CLASS ------------------------------
//
// Name:           HealGroupMember
// Base Class:     Behavior
//
// Description:    Will have the actor run to another actor in its
//				   group, play an animation, and send them the 
//					healovertime event once the actor is close enough
//
// Method of Use:  Called From State Machine
//--------------------------------------------------------------
class HealGroupMember : public Behavior
	{
	//------------------------------------
	// States
	//------------------------------------
	public:  
		typedef enum
		{			
		HGM_STATE_GOTO_ENTITY,		
		HGM_STATE_FACE_TARGET,
		HGM_STATE_ANIMATE,
		HGM_STATE_HEAL,
		HGM_STATE_FAILED,
		HGM_STATE_SUCCESS,
		} healGroupMemberStates_t;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		struct triageEntry
		{
		EntityPtr	ent = nullptr;
		float		health = 0.0f;
		float		dist = 0.0f;
		bool		treated  = false;
		};
		using triageEntry_t = triageEntry;

	//------------------------------------
	// Parameters
	//------------------------------------
	private:
		str								_anim;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		float							_healDistance = 0.0f;
		float							_maxDistance = 0.0f;
		float							_initialHealPercentage = 0.0f;
		float							_regenHealPercentage = 0.0f;
		float							_regenInterval = 0.0f;
		float							_maxPercentage = 0.0f;


	//-------------------------------------
	// Internal Functionality
	//-------------------------------------
	protected:
		void							init						( Actor &self );
		void							doHeal						( Actor &self );		

		void							setupGotoEntity				( Actor &self );
		void							doGotoEntity				( Actor &self );
		void							gotoEntityFailed			( Actor &self );

		void							setupRotateToEntity			( Actor &self );
		void							doRotateToEntity			( Actor &self );
		void							rotateToEntityFailed		( Actor &self );

		void							setupAnimate				( Actor &self );
		void							doAnimate					( Actor &self );
		void							animateFailed				( Actor &self );

		void							updateTriageList			( Actor &self );
		void							updateTriageEntry			( Actor &self , Entity *ent );
		bool							allPatientsTreated			( Actor &self );
		bool							patientTreated				( Actor &self , Entity *ent );
		void							treatedPatient				( Actor &self , Entity *ent );
		Actor*							findHighestPriorityPatient	( Actor &self );
	

	//-------------------------------------
	// Public Interface
	//-------------------------------------
	public:
		CLASS_PROTOTYPE( HealGroupMember );

										HealGroupMember();
									   ~HealGroupMember();

		void							SetArgs					( Event *ev );      
		void							AnimDone				( Event *ev );

		void							Begin					( Actor &self );		
		BehaviorReturnCode_t			Evaluate				( Actor &self );
		void							End						( Actor &self );
		
		virtual void					Archive  ( Archiver &arc );

	//-------------------------------------
	// Components
	//-------------------------------------
	private: 
		GotoEntity						_gotoEntity;
		RotateToEntity					_rotateToEntity;


	//-------------------------------------
	// Member Variables
	//-------------------------------------
	private: 
		Container<triageEntry_t*>		_triageList;
		str								_legAnim;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		unsigned int					_state = 0;	
		ActorPtr						_currentPatient = nullptr;
		float							_nextTriageUpdate = 0.0f;
		

	};


inline void HealGroupMember::Archive( Archiver &arc	)
{
	int num , i;
	triageEntry_t* checkEntry;

	Behavior::Archive ( arc );	     

	// Archive Parameters
	arc.ArchiveString			( &_anim					);	
	arc.ArchiveFloat			( &_healDistance			);
	arc.ArchiveFloat			( &_maxDistance				);
	arc.ArchiveFloat			( &_initialHealPercentage	);
	arc.ArchiveFloat			( &_regenHealPercentage		);
	arc.ArchiveFloat			( &_regenInterval			);
	arc.ArchiveFloat			( &_maxPercentage			);

	// Archive Components
	arc.ArchiveObject			( &_gotoEntity				);
	arc.ArchiveObject			( &_rotateToEntity			);

	// Archive Member Vars      
	arc.ArchiveUnsigned			( &_state					);
	arc.ArchiveSafePointer		( &_currentPatient			);
	arc.ArchiveString			( &_legAnim					);
	arc.ArchiveFloat			( &_nextTriageUpdate		);
	
	if ( arc.Saving() )
		{
		num = _triageList.NumObjects();
		arc.ArchiveInteger( &num );

		for ( i = num ; i > 0 ; i-- )
			{
			checkEntry = _triageList.ObjectAt( i );
			arc.ArchiveSafePointer( &checkEntry->ent );
			arc.ArchiveFloat( &checkEntry->health );
			arc.ArchiveFloat( &checkEntry->dist );
			arc.ArchiveBool( &checkEntry->treated );		
			}
		}
	else
		{
		arc.ArchiveInteger( &num );
		_triageList.ClearObjectList();
		_triageList.Resize( num );

		for ( i = 1 ; i<= num ; i++ )
			{
			checkEntry = new triageEntry_t;			
			arc.ArchiveSafePointer( &checkEntry->ent);
			arc.ArchiveFloat( &checkEntry->health );
			arc.ArchiveFloat( &checkEntry->dist );
			arc.ArchiveBool( &checkEntry->treated );			
			}

		}
}



#endif /* __HEAL_GROUP_MEMBER_HPP__ */

