/// \file object.h
/// \brief Interface for the game object class CGameObject.

#pragma once

#include "sprite.h"
#include "defines.h"
#include "HitBox.h"


/// \brief The game object. 
///
/// Game objects are responsible for remembering information about 
/// themselves - including their image, location, and speed - and
/// for moving and drawing themselves.

class CGameObject{ //class for a game object
  friend class CIntelligentObject;
  friend class CObjectManager;
  friend class CGameRenderer;
  friend BOOL KeyboardHandler(WPARAM keystroke); //for keyboard control of objects
  friend bool ZCompare(const CGameObject* p0, const CGameObject* p1); //for depth sorting

  protected:
    ObjectType m_nObjectType; ///< Object type.

    Vector3 m_vPos; ///< Current location.
    Vector3 m_vVelocity; ///< Current velocity.
    int m_nLastMoveTime; ///< Last time moved.
    int m_nMinXSpeed; ///< Min horizontal speed.
    int m_nMaxXSpeed; ///< Max horizontal speed.
    int m_nMinYSpeed; ///< Min vertical speed.
    int m_nMaxYSpeed; ///< Max vertical speed.

    float m_fOrientation; ///< Orientation, angle to rotate about the Z axis.
    bool m_bfaceRight;///< Luis-This bool is checking wheter the object is facing right
    bool m_bfaceLeft;

    C3DSprite* m_pSprite; ///< Pointer to sprite.
    int m_nCurrentFrame; ///< Frame to be displayed.
    int m_nFrameCount; ///< Number of frames in animation.
    int m_nLastFrameTime; ///< Last time the frame was changed.
    int m_nFrameInterval; ///< Interval between frames.
    int* m_pAnimation; ///< Sequence of frame numbers to be repeated
    int m_nAnimationFrameCount; ///< Number of entries in m_pAnimation
    
    
    int m_nWidth; ///< Width of object.
    int m_nHeight; ///< Height of object.
    int m_nBirthTime; ///< Time of creation.
    int m_nLifeTime; ///< Time that object lives.
    BOOL m_bVulnerable; ///< Vulnerable to bullets.
    BOOL m_bIntelligent; ///< TRUE for an intelligent object.
    BOOL m_bCanFly; ///< TRUE for an object that can fly, all other fall.
    BOOL m_bCycleSprite; ///< TRUE to cycle sprite frames, otherwise play once.
    BOOL m_bIsDead; ///< TRUE if the object is dead.
    int m_nBounceCount; ///< Number of bounces.
    BOOL m_bGrounded; ///< TRUE for objects that can't leave the ground.

    Hitbox* MyHitboxes[5] = { NULL };///The Object's collection of hitboxes
    Vector3 m_vPrevPos;//Luis-Keeps track of the previous position while paused


	  BOOL m_bAttacking; ///< Alfredo - if the object is attacking 
    BOOL m_bStrong;
    BOOL m_bIsChampion;

    int m_nHealth = 100; /// Alfredo- Health of an object


    void LoadSettings(const char* name); //< Load object-dependent settings from XML element.

    

  public:
    CGameObject(ObjectType object, const char* name, const Vector3& s, const Vector3& v); ///< Constructor.
    ~CGameObject(); //< Destructor.
    void draw(); ///< Draw at current location.
    virtual void move(); ///< Change location depending on time and speed 
    void accelerate(const Vector3& a); ///< Change speed.
    void walk(const Vector3& a); ///< Luis- Change walking speed.
    Vector3 getPos() { return m_vPos; }
    Vector3 getPvPos() { return m_vPrevPos; }//Luis-Get previous position
    void setPvPos(Vector3& a) { m_vPrevPos = a; }//Luis-Sets the previous positions
    void turnFace(BOOL direction);
    BOOL getFace();
    void zShift();
    void MorphHitBox();
    //Luis-Getting the direction the object is facing
    //Luis-For changing which direction the object is facing
    //Luis-Get Position
    void teleport(Vector3& a);//Luis-This changes an objects position
    void dash(bool LPre, bool RPre, bool UPre, bool DPre); // Alfredo - Dashes an object
    void enemyAttack(char* enemy); /// Alfredo - Enemy attack function. Takes in name and does the appropriate enemy's attack
    void takeHit(int damage); /// Alfredo- Health is subtracted by damage
    void giveHealth(int health); /// Alfredo- Health is added to object 
    void UpdateSprite(ObjectType neo);//Luis-Changes an objects sprite
    void setLifetime(int n) { m_nLifeTime = n; }//Luis-Sets a new lifetime when needed
}; //CGameObject

