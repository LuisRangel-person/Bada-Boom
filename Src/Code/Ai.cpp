/// \file ai.cpp
/// \brief Code for the intelligent object class CIntelligentObject.

#include "ai.h"
#include "debug.h"
#include "ObjMan.h"

extern CObjectManager g_cObjectManager; //object manager
extern int g_nScreenWidth;

/// Constructor for intelligent object.
/// \param object Object type.
/// \param name Object name string.
/// \param location Vector location in world space.
/// \param velocity Velocity vector.

CIntelligentObject::CIntelligentObject(ObjectType object, const char* name,
  const Vector3& location, const Vector3& velocity):
CGameObject(object, name, location, velocity){ //constructor
  m_bIntelligent = TRUE;
  m_fDistance = m_fXDistance = m_fYDistance = 0.0f;
  m_vgatorLoc.x = m_vgatorLoc.y = 0.0f;
} //constructor

/// Compute the distance to the gator. Intelligent objects need to make
/// decisions based on how clsoe the gator is.

void CIntelligentObject::think(){ 
  //look for gator
  CGameObject* pgator = g_cObjectManager.GetObjectByName("gator");
  m_vgatorLoc = pgator->m_vPos; //remember gator location

  //Euclidean and axial distances from gator

  m_fYDistance = fabs(m_vPos.y - m_vgatorLoc.y); //vertical distance

  //horizontal distance
  m_fXDistance = m_vPos.x - m_vgatorLoc.x;
  //wrap horizontal distance to half of world width in magnitude
  const int nWorldWidth = 2 * g_nScreenWidth; //should be at least 2 times screen width
  if(m_fXDistance > nWorldWidth/2)
    m_fXDistance -= nWorldWidth;
  if(m_fXDistance < -nWorldWidth/2)
    m_fXDistance += nWorldWidth;

  //Euclidean distance
  m_fDistance = sqrt(m_fXDistance*m_fXDistance + m_fYDistance*m_fYDistance);
} //think