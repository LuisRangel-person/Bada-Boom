/// \file crow.h
/// \brief Interface for the crow intelligent object class CCrowObject.

#pragma once

/// AI state for crows.

enum StateType{CRUISING_STATE, AVOIDING_STATE}; 

#include "ai.h"

/// \brief Crow intelligent object.
///
/// AI for the crow objects. Behaviour is based on a simple state machine with
/// two states. State transitions involve an element of randomness. Crows change
/// their speed and desired height at random, and randomly look for the plane.
/// If the plane is close enough when they look, then they enter the avoiding state.
/// If it's where it can't do them any harm, they go back to cruising.

class CCrowObject: public CIntelligentObject{
  private:
    StateType m_eState; ///< Current state.

    int m_nDesiredHeight; ///< Desired altitude.
    int m_nHeightTime; ///< Time between height changes.
    int m_nHeightDelayTime; ///< Time to next height change.

    int m_nSpeedVariationTime; ///< Last time speed varied.
    int m_nSpeedVariationDuration; ///< Time to next speed variation.

    int m_nLastAiTime; ///< Last time AI was used.
    int m_nAiDelayTime; ///< Time until AI next used.

    void think(); ///< Artificial intelligence.
    void CruisingAi(); ///< Ai for cruising along.
    void AvoidingAi(); ///< Ai for avoiding plane.
    void SetState(StateType state); ///< Change state

  public:
    CCrowObject(const char* name, const Vector3& location, const Vector3& velocity); ///< Constructor.
    void move(); ///< Move depending on time and speed.
}; //CCrowObject