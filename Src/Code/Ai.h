/// \file ai.h
/// \brief Interface for the intelligent object class CIntelligentObject.

#pragma once

#include "object.h"

/// \brief The intelligent object class.
///
/// This is the base class for our artificial intelligence. It maintains
/// the functionality that all intelligent objects have in common, that is,
/// knowledge of the plane's location.

class CIntelligentObject: public CGameObject{
  protected:
    Vector3 m_vgatorLoc; ///< Gator location.
    float m_fDistance; ///< Euclidean distance to gator.
    float m_fYDistance; ///< Vertical distance to gator.
    float m_fXDistance; ///< horizontal distance to gator.

  public:
    CIntelligentObject(ObjectType object, const char* name, const Vector3& location,
      const Vector3& velocity); ///< Constructor.
    virtual void think(); ///< AI function.
}; //CIntelligentObject