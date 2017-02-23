/// \file objman.h
/// \brief Interface for the object manager class CObjectManager.

#pragma once

#include <list>
#include <string>
#include <unordered_map>

#include "object.h"

/// \brief The object manager. 
///
/// The object manager is responsible for the care and feeding of
/// game objects. Objects can be named on creation so that they
/// can be accessed later - this is needed in particular for the player
/// object or objects.

class CObjectManager{
  private:
    list<CGameObject*> m_stlObjectList; ///< List of game objects.
    unordered_map<string, CGameObject*> m_stlNameToObject; ///< Map names to objects.
    unordered_map<string, ObjectType> m_stlNameToObjectType; ///< Map names to object types.
    
    int m_nLastGunFireTime; ///< Time gun was last fired.

    //Game Stuff
    int floor = 0;//Luis-This int will keep track of what floor the player is on
    int lives = 3;//Luis-Number of lives
    int score = 0;//Luis-The current score
    int PUTimer = 0;//Luis-Power-Up Timer
    int levelTime = 0;//Luis-When the level starts
    int PowerTime = 0;//Luis-Power Up Time
    BOOL isDowned = FALSE;//Luis-This keeps track of wheter Gator is downed(lost a life)
	  BOOL gatorStrong = FALSE; 
    int numEne;//Luis-Number of enemies
    

    //creation functions
    CGameObject* createObject(const char* obj, const char* name, const Vector3& s); ///< Create new object by name.
    
    //distance functions
    float distance(CGameObject *g0, CGameObject *g1); ///< Distance between objects.

    //collision detection
    void CollisionDetection(); ///< Process all collisions.
    void CollisionDetection(CGameObject* i); ///< Process collisions of all with one object.
    void HitboxCollision(CGameObject* p1, CGameObject* p2);

    //managing dead objects
    void cull(); ///< Cull dead objects
    void CreateNextIncarnation(CGameObject* object); ///< Replace object by next in series.
    void GarbageCollect(); ///< Collect dead objects from object list.
  public:
    CObjectManager(); ///< Constructor.
    ~CObjectManager(); ///< Destructor.

    //Game Stuff, retrivers
    int getFloor();
    int getPCHealth();
    Vector3 getPCPos();
    int getLives();
    int getPUTimer();
    int getScore();

    BOOL getDown();

    void setEne(int n);

    int getEne();

    CGameObject* createObject(ObjectType obj, const char* name, const Vector3& s, const Vector3& v); ///< Create new object.

    void move(); ///< Move all objects.
    void draw(); ///< Draw all objects.

    CGameObject* GetObjectByName(const char* name); ///< Get pointer to object by name.
    void InsertObjectType(const char* objname, ObjectType t); ///< Map name string to object type enumeration.
    ObjectType GetObjectType(const char* name); ///< Get object type corresponding to name string.
    
    void FireGun(char* name); ///< Fire a gun from named object.
	  void ThrowPunch(char* name, bool direction); ///< Luis- Causes the object to punch something
    void cleanTiles();
    void cleanEnemies();
    ///game management functions
    void LoseALife(CGameObject* p0);
    void GameReset();
    void PoweredUp(int n);
    ///< Luis-Player loses a life 
    void levelComplete();/// <Luis-This will be called when a level is completed
}; //CObjectManager