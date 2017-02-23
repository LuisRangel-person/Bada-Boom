/// \file Rhino.cpp
/// \brief Code for the rhino intelligent object class CRhinoObject.

#include <string>
#include <unordered_map>

#include "Rhino.h"
#include "timer.h" 
#include "random.h"
#include "debug.h"
#include "Defines.h"
#include "ObjMan.h"

extern GameState m_nGameState;
extern CTimer g_cTimer;  //game timer
extern CRandom g_cRandom; //random number generator
extern CObjectManager g_cObjectManager; //alfredo- added to see access object functions 

const int CLOSE_DISTANCE = 300; ///< Distance for close to plane.
const int FAR_DISTANCE = 500; ///< Distance for "far from" plane.
const int FALLBACK_DISTANCE = 150; ///< Fall back at this vertical distance from Gator.
const int BEHIND_DISTANCE = -5; ///< Horizontal distance considered to be behind Gator.
const int FORWARD_DISTANCE = 5; ///< Horizontal distance considered to be behind Gator.

                                /// Constructor for Rhino.
                                /// \param name Object name string.
                                /// \param location Vector location in world space.
                                /// \param velocity Velocity vector.

CRhinoObject::CRhinoObject(const char* name, const Vector3& location, const Vector3& velocity) :
  CIntelligentObject(RHINO_OBJECT, name, location, velocity) {
  m_nDesiredHeight = 240;
  m_nHeightTime = 0;  m_nHeightDelayTime = 0;
  m_nSpeedVariationTime = m_nSpeedVariationDuration = 0;
  m_eState = RHINO_WALKING_STATE;
  m_nLastAiTime = 0; m_nAiDelayTime = 0;
  m_bfaceLeft = TRUE;
  m_nHealth = 60;

} //constructor

  /// Intelligent move function.
  /// Just move like a dumb object then modify your private variables "intelligently".

void CRhinoObject::move() {
  if (m_nGameState == PLAYING_STATE) {
    CGameObject::move(); //move like a dumb object
    think(); //act intelligently
  }
} //move

  /// Main Rhino AI function.
  /// The real work is done by a function for each state. Call the appropriate
  /// function for the current state periodically, based on a timer.

void CRhinoObject::think() {
  //do the following periodically
  if (g_cTimer.elapsed(m_nLastAiTime, m_nAiDelayTime)) {
    CIntelligentObject::think(); //do thinking common to all intelligent objects
    switch (m_eState) { //behavior depends on state
    case RHINO_WALKING_STATE: WalkingAi(); break;
    case RHINO_ATTACKING_STATE: AttackingAi(); break;
    case RHINO_CHARGING_STATE: ChargingAi(); break;
    default: break;
    } //switch
  } //if
} //think

  /// Set the current state.
  /// Change the current state, taking whatever actions are necessary on entering
  /// the new state.
  /// \param state New state

void CRhinoObject::SetState(StateTypeRhino state) {
  m_eState = state; //change state

  switch (m_eState) { //change behavior settings
  case RHINO_WALKING_STATE:
	 
    m_nAiDelayTime = 400 + g_cRandom.number(0, 200);

	if (m_bfaceLeft) {
		m_vVelocity.x = -1.5f;
	}
	else {
		m_vVelocity.x = 1.5f;
	}
    break;

  case RHINO_CHARGING_STATE:     //Alfredo- settings of when in charging state, should be charging towards the gator
    m_nAiDelayTime = 250 + g_cRandom.number(0, 250);
    if(m_bfaceLeft) // Alfredo - if the Rhino is facing right, go right
     m_vVelocity.x = -3.0;
    else // else, go left
     m_vVelocity.x = 3.0;
     
    break;

  case RHINO_ATTACKING_STATE:     //Alfredo- settings of when in attacking state, should not be moving in order to perform swiping attack animation 
    
    m_nAiDelayTime = 250 + g_cRandom.number(0, 250);
    //m_vVelocity.x = 0;
	//m_vVelocity.y = 0;
    break; 

  default: break;
  } //switch
} //SetState

  /// AI for Rhinos in the walking state.
  /// These Rhinos are just walking along, periodically looking for the gator.

void CRhinoObject::WalkingAi() {

  if (m_bfaceLeft) {
    UpdateSprite(RHINORUNLEFT_OBJECT);
  }
  else {
    UpdateSprite(RHINORUNRIGHT_OBJECT);
  }

	 m_vVelocity.y = 0.0f;


  if (m_fDistance < CLOSE_DISTANCE*2 && (m_fXDistance > FORWARD_DISTANCE || m_fXDistance < FORWARD_DISTANCE)) // Alfredo - If the gator is in front and close, then attack 
    SetState(RHINO_CHARGING_STATE);
} //WalkingAI

  /// AI for Rhinos in the Attacking state.
  /// These Rhinos are trying to get close to the gator.

// Alfredo- testing out attacking, aka charging to player AI 
void CRhinoObject::ChargingAi() {

  // Alfredo - Source: http://stackoverflow.com/questions/2625021/game-enemy-move-towards-player
	//Rhino finds its way to the player
  float xDistance = m_vgatorLoc.x-m_vPos.x;
  float yDistance = m_vgatorLoc.y-m_vPos.y;
  //float hypotenuse = sqrt((xDistance * xDistance) + (yDistance * yDistance));

  if (xDistance > 0) { // Alfredo - If the gator is to its left, then turn left, else turn right

    m_bfaceLeft = FALSE;
  }
  else {
    m_bfaceLeft = TRUE;
  }

  if (m_bfaceLeft)
	  UpdateSprite(RHINORUNLEFT_OBJECT);
  else
	  UpdateSprite(RHINORUNRIGHT_OBJECT);
	  
  if (yDistance < -5) {
	  m_vVelocity.y -= 1.5;
  }
  else if (yDistance > 5) {
	  m_vVelocity.y += 1.5;
  }



  /*
  if(hypotenuse < 1000){
    if (m_bfaceLeft) {
  //    UpdateSprite(RHYNO_WALKLEFT_OBJECT);
      m_vVelocity.y += 1.5f*(yDistance / hypotenuse);
      m_vVelocity.x += 1.5f*(xDistance / hypotenuse);
    }
    else {
//      UpdateSprite(RHYNO_WALKRIGHT_OBJECT);
      m_vVelocity.y += 1.5f*(yDistance / hypotenuse);
      m_vVelocity.x -= 1.5f*(xDistance / hypotenuse);
    }
  }

  */

  if (m_fDistance > FAR_DISTANCE)
	  SetState(RHINO_WALKING_STATE); //then back to cruising
  else if (yDistance >-20 && yDistance < 20){  
		SetState(RHINO_ATTACKING_STATE);
  }
  else
	  SetState(RHINO_WALKING_STATE);

} //ChargingAI

void CRhinoObject::AttackingAi() {
  enemyAttack("rhino"); 

  if (m_bfaceLeft) {
	  UpdateSprite(RHINOATTACKLEFT_OBEJCT);
	  m_vVelocity.x -= 5.0f;
  }
  else {
	  UpdateSprite(RHINOATTACKRIGHT_OBJECT);
	  m_vVelocity.x += 5.0f;
  }

  if (m_fDistance > 300 || (m_vPos.x < -3200.0f || m_vPos.x > -300.0f)) { //higher or lower
	  if (m_bfaceLeft)
		  ;// UpdateSprite(RHINOSTOPLEFT_OBJECT);
	  else
		  ;// UpdateSprite(RHINOSTOPRIGHT_OBJECT);

	  SetState(RHINO_WALKING_STATE); //then back to cruising
  }
}