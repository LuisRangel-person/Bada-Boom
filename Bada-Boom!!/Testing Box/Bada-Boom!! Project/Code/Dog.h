#pragma once
/// \file Dog.h
/// \brief Interface for the Dog intelligent object class CDogObject.

#pragma once

/// AI state for Dog.

enum StateTypeDog { WALKING_STATE_DOG, CHARGING_STATE_DOG, ATTACKING_STATE_DOG,  };

#include "ai.h"
#include "spriteman.h"
#include "objman.h"
#include "sprite.h"
#include "object.h"
/// \brief Dog intelligent object.
///
/// AI for the Dog objects. Behaviour is based on a simple state machine with
/// two states. State transitions involve an element of randomness. Dog change
/// their speed and desired position at random, and randomly look for the gator.
/// If the gator is close enough when they look, then they enter the avoiding state.
/// If it's where it can't do them any harm, they go back to walking.

class CDogObject : public CIntelligentObject {
private:
  StateTypeDog m_eState; ///< Current state.

  int m_nDesiredHeight; ///< Desired altitude.
  int m_nHeightTime; ///< Time between height changes.
  int m_nHeightDelayTime; ///< Time to next height change.

  int m_nSpeedVariationTime; ///< Last time speed varied.
  int m_nSpeedVariationDuration; ///< Time to next speed variation.

  int m_nLastAiTime; ///< Last time AI was used.
  int m_nAiDelayTime; ///< Time until AI next used.

  int dogHealth = 3; 
  bool dogChampion; 

  void think(); ///< Artificial intelligence.
  void WalkingAi(); ///< Ai for cruising along.
  void ChargingAi(); ///< Alfredo - Ai for charging to gator.
  void AttackingAi(); ///< Alfredo - Ai for attacking to gator.
  void SetState(StateTypeDog state); ///< Change state

public:
  CDogObject(const char* name, const Vector3& location, const Vector3& velocity); ///< Constructor.
  void move(); ///< Move depending on time and speed.
}; //CDogObject