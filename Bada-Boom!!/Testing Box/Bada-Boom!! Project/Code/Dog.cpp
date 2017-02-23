/// \file Dog.cpp
/// \brief Code for the dog intelligent object class CDogObject.

#include <string>
#include <unordered_map>

#include "Dog.h"
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

                                /// Constructor for Dog.
                                /// \param name Object name string.
                                /// \param location Vector location in world space.
                                /// \param velocity Velocity vector.

CDogObject::CDogObject(const char* name, const Vector3& location, const Vector3& velocity) :
  CIntelligentObject(DOG_OBJECT, name, location, velocity) {
  m_nDesiredHeight = 240;
  m_nHeightTime = 0;  m_nHeightDelayTime = 0;
  m_nSpeedVariationTime = m_nSpeedVariationDuration = 0;
  m_eState = WALKING_STATE_DOG;
  m_nLastAiTime = 0; m_nAiDelayTime = 0;
  m_bfaceLeft = TRUE;
  int champRand = g_cRandom.number(1, 5);
  if (champRand == 1){
    dogChampion = TRUE;
    m_bIsChampion = TRUE;
	m_nHealth = 50;
  }
  else {
    dogChampion = FALSE;
    m_bIsChampion = FALSE;
	m_nHealth = 30;
  }
} //constructor

  /// Intelligent move function.
  /// Just move like a dumb object then modify your private variables "intelligently".

void CDogObject::move() {
  if (m_nGameState == PLAYING_STATE) {
    CGameObject::move(); //move like a dumb object
    think(); //act intelligently
  }
} //move

  /// Main dog AI function.
  /// The real work is done by a function for each state. Call the appropriate
  /// function for the current state periodically, based on a timer.

void CDogObject::think() {
  //do the following periodically
  if (g_cTimer.elapsed(m_nLastAiTime, m_nAiDelayTime)) {
    CIntelligentObject::think(); //do thinking common to all intelligent objects
    switch (m_eState) { //behavior depends on state
    case WALKING_STATE_DOG: WalkingAi(); break;
    case ATTACKING_STATE_DOG: AttackingAi(); break;
    case CHARGING_STATE_DOG: ChargingAi(); break; 
    default: break;
    } //switch
  } //if
} //think

  /// Set the current state.
  /// Change the current state, taking whatever actions are necessary on entering
  /// the new state.
  /// \param state New state

void CDogObject::SetState(StateTypeDog state) {
  m_eState = state; //change state

  switch (m_eState) { //change behavior settings
  case WALKING_STATE_DOG:

    m_nAiDelayTime = 400 + g_cRandom.number(0, 200);

    if(m_bfaceLeft)
      m_vVelocity.x = (float)-g_cRandom.number(1, 4);
    else
      m_vVelocity.x = (float)g_cRandom.number(1, 4);

    m_nHeightDelayTime = 8000 + g_cRandom.number(0, 5000);
    break;

  case CHARGING_STATE_DOG:     //Alfredo- settings of when in charging state, should be charging towards the gator
    m_nAiDelayTime = 250 + g_cRandom.number(0, 250);
    if(m_bfaceLeft) // Alfredo - if the dog is facing right, go right
     m_vVelocity.x = -3;
    else // else, go left
     m_vVelocity.x = 3;
     
    m_nDesiredHeight = g_cRandom.number(100, 400);
    m_nHeightDelayTime = 3000 + g_cRandom.number(0, 2000);
    m_nSpeedVariationDuration = 5000 + g_cRandom.number(0, 2000);
    break;

  case ATTACKING_STATE_DOG:     //Alfredo- settings of when in attacking state, should not be moving in order to perform swiping attack animation 
    
    m_nAiDelayTime = 250 + g_cRandom.number(0, 250);
    m_vVelocity.x = 0;
	m_vVelocity.y = 0;
    break; 

  default: break;
  } //switch
} //SetState

  /// AI for dogs in the walking state.
  /// These dogs are just walking along, periodically looking for the gator.

void CDogObject::WalkingAi() {
   
  if (m_bfaceLeft) {
    if(dogChampion)
      UpdateSprite(CHAMPIONDOGWALKLEFT_OBJECT);
    else
      UpdateSprite(DOGWALKLEFT_OBJECT);
  }
  else {
    if (dogChampion)
      UpdateSprite(CHAMPIONDOGWALKRIGHT_OBJECT);
    else
      UpdateSprite(DOGWALKRIGHT_OBJECT);
  }

	 m_vVelocity.y = 0.0f;


  if (m_fDistance < CLOSE_DISTANCE + 300 && (m_fXDistance > FORWARD_DISTANCE || m_fXDistance < FORWARD_DISTANCE)) // Alfredo - If the gator is in front and close, then attack 
    SetState(CHARGING_STATE_DOG);
} //WalkingAI

  /// AI for dogs in the Attacking state.
  /// These dogs are trying to get close to the gator.

// Alfredo- testing out attacking, aka charging to player AI 
void CDogObject::ChargingAi() {
  g_pSoundManager->play(5);
  // Alfredo - Source: http://stackoverflow.com/questions/2625021/game-enemy-move-towards-player
	//Dog finds its way to the player
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
      if (dogChampion) {
        UpdateSprite(CHAMPIONDOGWALKLEFT_OBJECT);
        m_vVelocity.y += 8.0f*(yDistance / hypotenuse);
        m_vVelocity.x += 8.0f*(xDistance / hypotenuse);
      }
      else {
        UpdateSprite(DOGWALKLEFT_OBJECT);
        m_vVelocity.y += 8.0f*(yDistance / hypotenuse);
        m_vVelocity.x += 8.0f*(xDistance / hypotenuse);
      }
    }
    else {
      if (dogChampion) {
        UpdateSprite(CHAMPIONDOGWALKRIGHT_OBJECT);
        m_vVelocity.y += 8.0f*(yDistance / hypotenuse);
        m_vVelocity.x -= 8.0f*(xDistance / hypotenuse);
      }
      else {
        UpdateSprite(DOGWALKRIGHT_OBJECT);
        m_vVelocity.y += 8.0f*(yDistance / hypotenuse);
        m_vVelocity.x -= 8.0f*(xDistance / hypotenuse);
      }
    }
  }

  if (m_fDistance > FAR_DISTANCE)
	  SetState(WALKING_STATE_DOG); //then back to cruising
  else if (m_fDistance < CLOSE_DISTANCE-125)
	  SetState(ATTACKING_STATE_DOG);
  else
	  SetState(WALKING_STATE_DOG);

} //ChargingAI

void CDogObject::AttackingAi() {
  if (dogChampion)
    enemyAttack("championdog");
  else
    enemyAttack("dog");

  if (m_bfaceLeft) {
    if(dogChampion)
      UpdateSprite(CHAMPIONDOGATTACKLEFT_OBJECT);
    else
      UpdateSprite(DOGATTACKLEFT_OBJECT);
  }
  else {
    if (dogChampion)
      UpdateSprite(CHAMPIONDOGATTACKRIGHT_OBJECT);
    else
      UpdateSprite(DOGATTACKRIGHT_OBJECT);
  }

  if (m_fDistance > CLOSE_DISTANCE - 200) //higher or lower
    SetState(WALKING_STATE_DOG); //then back to cruising
}