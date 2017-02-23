#pragma once
/// \file Horse.h
/// \brief Interface for the horse intelligent object class CHorseObject.

#pragma once

/// AI state for horses.

enum StateTypeHorse { HORSE_WALKING_STATE, HORSE_CHARGING_STATE, HORSE_ATTACKING_STATE, HORSE_DEAD_STATE};

#include "ai.h"
#include "spriteman.h"
#include "objman.h"
#include "sprite.h"
#include "object.h"
/// \brief Horse intelligent object.
///
/// AI for the Horse objects. Behaviour is based on a simple state machine with
/// two states. State transitions involve an element of randomness. Horses change
/// their speed and desired position at random, and randomly look for the gator.
/// If the gator is close enough when they look, then they enter the charging state.
/// If it's where it can't do them any harm, they go back to walking.

class CHorseObject : public CIntelligentObject {
private:
  StateTypeHorse m_eState; ///< Current state.

  int m_nDesiredHeight; ///< Desired altitude.
  int m_nHeightTime; ///< Time between height changes.
  int m_nHeightDelayTime; ///< Time to next height change.

  int m_nSpeedVariationTime; ///< Last time speed varied.
  int m_nSpeedVariationDuration; ///< Time to next speed variation.

  int m_nLastAiTime; ///< Last time AI was used.
  int m_nAiDelayTime; ///< Time until AI next used.

  int horseHealth = 3; 

  void think(); ///< Artificial intelligence.
  void WalkingAi(); ///< Ai for cruising along.
  void ChargingAi(); ///< Alfredo - Ai for charging gator.
  void AttackingAi(); ///< Alfredo - Ai for attacking gator.
  void SetState(StateTypeHorse state); ///< Change state

public:
  CHorseObject(const char* name, const Vector3& location, const Vector3& velocity); ///< Constructor.
  void move(); ///< Move depending on time and speed.
}; //CHorseObject