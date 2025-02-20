//-----------------------------------------------------------------------------
//
//  $Logfile:: /Code/DLLs/game/actor_posturecontroller.h                      $
// $Revision:: 15                                                             $
//   $Author:: Sketcher                                                       $
//     $Date:: 5/20/02 3:55p                                                  $
//
// Copyright (C) 2001 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
//
//-----------------------------------------------------------------------------

class PostureController;

#ifndef __ACTOR_POSTURECONTROLLER_HPP__
#define __ACTOR_POSTURECONTROLLER_HPP__

#include "actor.h"
#include "actorincludes.h"

extern Event EV_Posture_Anim_Done;
extern Event EV_PostureChanged_Completed;

//------------------------- CLASS ------------------------------
//
// Name:			PostureController
// Base Class:		None
//
// Description:		This class will control the posture of the Actor.  It will maintain
//					an enum of the current posture and will have methods for changing
//					posture via Animations... Eventually, these will need to be modified
//					to call functions on some type of animation manager
//
// Method of Use:	Instantiated by the Actor 
//
//--------------------------------------------------------------
class PostureController
{
	public:
						PostureController();
						PostureController( Actor *actor );
					   ~PostureController();

		virtual void	Archive( Archiver &arc );		 
		void			DoArchive( Archiver &arc , Actor *actor );
		
		void			evaluate();		

		bool			requestPosture( const str &postureState , Listener *requestor );
		
		const str&		getRequestedPostureName();
		const str&		getCurrentPostureName();

		void			setPostureStateMap( const str &stateMap , bool loading );
		void			setPostureState( const str &postureState );						
		void			setPostureState( const str &postureState , const str &requestedState );

	protected:
		void			init();		

	private:
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		StateMap		*_postureStateMap = nullptr;
		State			*_currentPostureState = nullptr;
		State			*_requestedPostureState = nullptr;
		SafePtr<Listener> _requestor = nullptr;
		Actor		    *act = nullptr;


		str				 _postureStateMap_Name;		
		str				 _currentPostureState_Name;
		str				 _requestedPostureState_Name;
		Container<Conditional*>	 _postureConditionals;
};

inline const str& PostureController::getRequestedPostureName()
{
	return _requestedPostureState_Name;
}

inline const str& PostureController::getCurrentPostureName()
{
	return _currentPostureState_Name;
}

#endif /* __ACTOR_POSTURECONTROLLER_HPP__ */

