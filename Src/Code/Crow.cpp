/// \file crow.cpp
/// \brief Code for the crow intelligent object class CCrowObject.

#include <string>
#include <unordered_map>

#include "crow.h"
#include "timer.h" 
#include "random.h"
#include "debug.h"
#include "Defines.h"

extern CTimer g_cTimer;  //game timer
extern CRandom g_cRandom; //random number generator

const int CLOSE_DISTANCE = 300; ///< Distance for close to plane.
const int FAR_DISTANCE = 500; ///< Distance for "far from" plane.
const int FALLBACK_DISTANCE = 150; ///< Fall back at this vertical distance from plane.
const int BEHIND_DISTANCE = -5; ///< Horizontal distance considered to be behind plane.

/// Constructor for crow.
/// \param name Object name string.
/// \param location Vector location in world space.
/// \param velocity Velocity vector.

CCrowObject::CCrowObject(const char* name, const Vector3& location, const Vector3& velocity):
CIntelligentObject(CROW_OBJECT, name, location, velocity){
  m_nDesiredHeight = 240; 
  m_nHeightTime = 0;  m_nHeightDelayTime = 0;
  m_nSpeedVariationTime = m_nSpeedVariationDuration = 0;
  m_eState = CRUISING_STATE;
  m_nLastAiTime = 0; m_nAiDelayTime = 0;
} //constructor

/// Intelligent move function.
/// Just move like a dumb object then modify your private variables "intelligently".

void CCrowObject::move(){ 
  CGameObject::move(); //move like a dumb object
  think(); //act intelligently
} //move

/// Main crow AI function.
/// The real work is done by a function for each state. Call the appropriate
/// function for the current state periodically, based on a timer.

void CCrowObject::think(){ 
  //do the following periodically
  if(g_cTimer.elapsed(m_nLastAiTime, m_nAiDelayTime)){
    CIntelligentObject::think(); //do thinking common to all intelligent objects
    switch(m_eState){ //behavior depends on state
      case CRUISING_STATE: CruisingAi(); break;
      case AVOIDING_STATE: AvoidingAi(); break;
      default: break;
    } //switch
  } //if
} //think

/// Set the current state.
/// Change the current state, taking whatever actions are necessary on entering
/// the new state.
/// \param state New state

void CCrowObject::SetState(StateType state){
  m_eState = state; //change state

  switch(m_eState){ //change behavior settings
    case CRUISING_STATE:
      m_nAiDelayTime = 400 + g_cRandom.number(0, 200);
      m_vVelocity.x = (float)-g_cRandom.number(1, 4);
      m_nHeightDelayTime = 8000 + g_cRandom.number(0, 5000);
      break;

    case AVOIDING_STATE:
      m_nAiDelayTime = 250 + g_cRandom.number(0, 250); 
      m_vVelocity.x = -3;
      m_nDesiredHeight = g_cRandom.number(100, 400);
      m_nHeightDelayTime = 3000 + g_cRandom.number(0, 2000);
      m_nSpeedVariationDuration = 5000 + g_cRandom.number(0, 2000);
      break;

    default: break;
  } //switch
} //SetState

/// AI for crows in the cruising state.
/// These crows are just cruising along, periodically looking for the plane.

void CCrowObject::CruisingAi(){ 
  //height variation
  if(m_nDesiredHeight > (int)m_vPos.y + 20)
    m_vVelocity.y = -(float)g_cRandom.number(1, 4);
  else if(m_nDesiredHeight < m_vPos.y - 20)
    m_vVelocity.y = (float)g_cRandom.number(1, 4);
  else m_vVelocity.y = 0.0f;
  if(g_cTimer.elapsed(m_nHeightTime, m_nHeightDelayTime)){
    m_nDesiredHeight = g_cRandom.number(150, 350);
    m_nHeightDelayTime = 15000+g_cRandom.number(0, 5000);
  } //if

  if(m_fDistance < CLOSE_DISTANCE && m_fXDistance < BEHIND_DISTANCE)
    SetState(AVOIDING_STATE);
} //CruisingAi

/// AI for crows in the avoiding state.
/// These crows are trying to get away from the plane.

void CCrowObject::AvoidingAi(){ 
  //height variation
  if(g_cTimer.elapsed(m_nHeightTime, m_nHeightDelayTime)){
    m_nDesiredHeight = g_cRandom.number(100 ,400);
    if(m_nDesiredHeight < m_vPos.y)
      m_vVelocity.y= -(float)g_cRandom.number(3, 7);
    if(m_nDesiredHeight > m_vPos.y)
      m_vVelocity.y = (float)g_cRandom.number(3, 7);
    m_nHeightDelayTime = 3000 + g_cRandom.number(0, 2000);
  } //if

  //speed variation
  if(g_cTimer.elapsed(m_nSpeedVariationTime, m_nSpeedVariationDuration)){
      m_vVelocity.x = (float)-g_cRandom.number(7, 10); 
      m_nSpeedVariationDuration = 10000 + g_cRandom.number(0, 3000);
  } //if
  if(m_fXDistance > BEHIND_DISTANCE) //if behind
    m_vVelocity.x = -1; //slow down 

  //look for plane, maybe leave avoiding state
  if(m_fDistance > FAR_DISTANCE || //if far away, or
    (m_fDistance < CLOSE_DISTANCE && //close and
       m_fYDistance > FALLBACK_DISTANCE)) //higher or lower
    SetState(CRUISING_STATE); //then back to cruising
} //AvoidingAi