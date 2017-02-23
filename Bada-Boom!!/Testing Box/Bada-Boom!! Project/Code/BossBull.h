#pragma once
/// \file BossBull.h
/// \brief Interface for the boss bull intelligent object class CBossBullObject.

#pragma once

/// AI state for boss bulls.

enum StateTypeBossBull { BOSSBULL_WALKING_STATE, BOSSBULL_CHARGING_STATE, BOSSBULL_ATTACKING_STATE, BOSSBULL_DEAD_STATE};

#include "ai.h"
#include "spriteman.h"
#include "objman.h"
#include "sprite.h"
#include "object.h"
/// \brief boss bull intelligent object.
///
/// AI for the boss bull objects. Behaviour is based on a simple state machine with
/// two states. State transitions involve an element of randomness. boss bulls change
/// their speed and desired position at random, and randomly look for the gator.
/// If the gator is close enough when they look, then they enter the charging state.
/// If it's where it can't do them any harm, they go back to walking.

class CBossBullObject : public CIntelligentObject {
private:
  StateTypeBossBull m_eState; ///< Current state.

  int m_nDesiredHeight; ///< Desired altitude.
  int m_nHeightTime; ///< Time between height changes.
  int m_nHeightDelayTime; ///< Time to next height change.

  int m_nSpeedVariationTime; ///< Last time speed varied.
  int m_nSpeedVariationDuration; ///< Time to next speed variation.

  int m_nLastAiTime; ///< Last time AI was used.
  int m_nAiDelayTime; ///< Time until AI next used.

  int bossbullHealth = 3; 

  void think(); ///< Artificial intelligence.
  void WalkingAi(); ///< Ai for cruising along.
  void ChargingAi(); ///< Alfredo - Ai for charging gator.
  void AttackingAi(); ///< Alfredo - Ai for attacking gator.
  void SetState(StateTypeBossBull state); ///< Change state

public:
  CBossBullObject(const char* name, const Vector3& location, const Vector3& velocity); ///< Constructor.
  void move(); ///< Move depending on time and speed.
}; //CBossBullObject