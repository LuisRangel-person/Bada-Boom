/// \file Gorilla.cpp
/// \brief Code for the gorilla intelligent object class CGorillaObject.

#include <string>
#include <unordered_map>

#include "Gorilla.h"
#include "timer.h" 
#include "random.h"
#include "debug.h"
#include "Defines.h"
#include "ObjMan.h"
#include "sound.h"

extern CSoundManager* g_pSoundManager;
extern GameState m_nGameState;

extern CTimer g_cTimer;  //game timer
extern CRandom g_cRandom; //random number generator
extern CObjectManager g_cObjectManager; //alfredo- added to see if 

const int CLOSE_DISTANCE = 300; ///< Distance for close to plane.
const int FAR_DISTANCE = 500; ///< Distance for "far from" plane.
const int FALLBACK_DISTANCE = 150; ///< Fall back at this vertical distance from Gator.
const int BEHIND_DISTANCE = -5; ///< Horizontal distance considered to be behind Gator.
const int FORWARD_DISTANCE = 5; ///< Horizontal distance considered to be behind Gator.

                                /// Constructor for Gorilla.
                                /// \param name Object name string.
                                /// \param location Vector location in world space.
                                /// \param velocity Velocity vector.

CGorillaObject::CGorillaObject(const char* name, const Vector3& location, const Vector3& velocity) :
  CIntelligentObject(GORILLA_OBJECT, name, location, velocity) {
  m_nDesiredHeight = 240;
  m_nHeightTime = 0;  m_nHeightDelayTime = 0;
  m_nSpeedVariationTime = m_nSpeedVariationDuration = 0;
  m_eState = WALKING_STATE;
  m_nLastAiTime = 0; m_nAiDelayTime = 0;
  m_bfaceLeft = TRUE;

  int champRand = g_cRandom.number(1, 5);
  if (champRand == 1) {
    gorillaChampion = TRUE;
    m_bIsChampion = TRUE;
	m_nHealth = 50 ;

  }
  else {
    gorillaChampion = FALSE;
    m_bIsChampion = FALSE;
	m_nHealth = 30;
  }

} //constructor

  /// Intelligent move function.
  /// Just move like a dumb object then modify your private variables "intelligently".

void CGorillaObject::move() {
  if (m_nGameState == PLAYING_STATE) {
    CGameObject::move(); //move like a dumb object
    think(); //act intelligently
  }
} //move

  /// Main gorilla AI function.
  /// The real work is done by a function for each state. Call the appropriate
  /// function for the current state periodically, based on a timer.

void CGorillaObject::think() {
  //do the following periodically
  if (g_cTimer.elapsed(m_nLastAiTime, m_nAiDelayTime)) {
    CIntelligentObject::think(); //do thinking common to all intelligent objects
    switch (m_eState) { //behavior depends on state
    case WALKING_STATE: WalkingAi(); break;
    case ATTACKING_STATE: AttackingAi(); break;
    case CHARGING_STATE: ChargingAi(); break; 
    default: break;
    } //switch
  } //if
} //think

  /// Set the current state.
  /// Change the current state, taking whatever actions are necessary on entering
  /// the new state.
  /// \param state New state

void CGorillaObject::SetState(StateTypeGorilla state) {
  m_eState = state; //change state

  switch (m_eState) { //change behavior settings
  case WALKING_STATE:
	 
    m_nAiDelayTime = 400 + g_cRandom.number(0, 200);

    if(m_bfaceLeft)
      m_vVelocity.x = (float)-g_cRandom.number(1, 4);
    else
      m_vVelocity.x = (float)g_cRandom.number(1, 4);

    m_nHeightDelayTime = 8000 + g_cRandom.number(0, 5000);
    break;

  case CHARGING_STATE:     //Alfredo- settings of when in charging state, should be charging towards the gator
    m_nAiDelayTime = 250 + g_cRandom.number(0, 250);
    if(m_bfaceLeft) // Alfredo - if the gorilla is facing right, go right
     m_vVelocity.x = -3;
    else // else, go left
     m_vVelocity.x = 3;
     
    m_nDesiredHeight = g_cRandom.number(100, 400);
    m_nHeightDelayTime = 3000 + g_cRandom.number(0, 2000);
    m_nSpeedVariationDuration = 5000 + g_cRandom.number(0, 2000);
    break;

  case ATTACKING_STATE:     //Alfredo- settings of when in attacking state, should not be moving in order to perform swiping attack animation 
    
    m_nAiDelayTime = 250 + g_cRandom.number(0, 250);
    m_vVelocity.x = 0;
	m_vVelocity.y = 0;
    break; 

  default: break;
  } //switch
} //SetState

  /// AI for gorillas in the walking state.
  /// These gorillas are just walking along, periodically looking for the gator.

void CGorillaObject::WalkingAi() {

  if (m_bfaceLeft) {
    if (gorillaChampion)
      UpdateSprite(CHAMPIONGORILLAWALKLEFT_OBJECT);
    else
      UpdateSprite(GORILLAWALKLEFT_OBJECT);
  }
  else {
    if (gorillaChampion)
      UpdateSprite(CHAMPIONGORILLAWALKRIGHT_OBJECT);
    else
      UpdateSprite(GORILLAWALKRIGHT_OBJECT);
  }

	 m_vVelocity.y = 0.0f;


  if (m_fDistance < CLOSE_DISTANCE +300 && (m_fXDistance > FORWARD_DISTANCE || m_fXDistance < FORWARD_DISTANCE)) // Alfredo - If the gator is in front and close, then attack 
    SetState(CHARGING_STATE);
} //WalkingAI

  /// AI for gorillas in the Attacking state.
  /// These gorillas are trying to get close to the gator.

// Alfredo- testing out attacking, aka charging to player AI 
void CGorillaObject::ChargingAi() {
	int soundRand = g_cRandom.number(1, 4);
	if(soundRand == 1)
		g_pSoundManager->play(10);
  // Alfredo - Source: http://stackoverflow.com/questions/2625021/game-enemy-move-towards-player
	//Gorilla finds its way to the player
  float xDistance = m_vgatorLoc.x-m_vPos.x;
  float yDistance = m_vgatorLoc.y-m_vPos.y;
  float hypotenuse = sqrt((xDistance * xDistance) + (yDistance * yDistance));

  if (xDistance > 0) { // Alfredo - If the gator is to its left, then turn left, else turn right

    m_bfaceLeft = FALSE;
  }
  else {
    m_bfaceLeft = TRUE;
  }

  if(hypotenuse < 1000){
    if (m_bfaceLeft) {
      if (gorillaChampion) {
        UpdateSprite(CHAMPIONGORILLAWALKLEFT_OBJECT);
        m_vVelocity.y += 1.5f*(yDistance / hypotenuse);
        m_vVelocity.x += 1.5f*(xDistance / hypotenuse);
      }
      else {
        UpdateSprite(GORILLAWALKLEFT_OBJECT);
        m_vVelocity.y += 1.5f*(yDistance / hypotenuse);
        m_vVelocity.x += 1.5f*(xDistance / hypotenuse);
      }

    }
    else {
      if (gorillaChampion) {
        UpdateSprite(CHAMPIONGORILLAWALKRIGHT_OBJECT);
        m_vVelocity.y += 1.5f*(yDistance / hypotenuse);
        m_vVelocity.x -= 1.5f*(xDistance / hypotenuse);
      }
      else {
        UpdateSprite(GORILLAWALKRIGHT_OBJECT);
        m_vVelocity.y += 1.5f*(yDistance / hypotenuse);
        m_vVelocity.x -= 1.5f*(xDistance / hypotenuse);
      }
    }
  }

  if (m_fDistance > FAR_DISTANCE)
	  SetState(WALKING_STATE); //then back to cruising
  else if (m_fDistance < 250)
	  SetState(ATTACKING_STATE);
  else
	  SetState(WALKING_STATE);

} //ChargingAI

void CGorillaObject::AttackingAi() {
  enemyAttack("gorilla"); 
  if (m_bfaceLeft) {
    if (gorillaChampion) {
      UpdateSprite(CHAMPIONGORILLAATTACKLEFT_OBJECT);
    }
    else {
      UpdateSprite(GORILLAATTACKLEFT_OBJECT);
    }

  }
  else {
    if (gorillaChampion) {
      UpdateSprite(CHAMPIONGORILLAATTACKRIGHT_OBJECT);
    }
    else {
      UpdateSprite(GORILLAATTACKRIGHT_OBJECT);
    }

  }

  if (m_fDistance > CLOSE_DISTANCE - 200) //higher or lower
    SetState(WALKING_STATE); //then back to cruising
}