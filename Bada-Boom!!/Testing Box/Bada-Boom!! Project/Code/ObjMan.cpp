/// \file objman.cpp
/// \brief Code for the object manager class CObjectManager.

#include "objman.h"
#include "debug.h"
#include "defines.h"
#include "timer.h"
#include "crow.h"
#include "Gorilla.h"
#include "Dog.h"
#include "Horse.h"
#include "Rhino.h"
#include "BossBull.h"
#include "BB_RLG.h"
#include "Random.h"
#include "sound.h"

extern CSoundManager* g_pSoundManager;

extern int g_nScreenWidth;
extern int g_nScreenHeight;
extern GameState m_nGameState;
extern CRandom g_cRandom; //random number generator
extern CTimer g_cTimer; 



/// Comparison for depth sorting game objects.
/// To compare two game objects, simply compare their Z coordinates.
/// \param p0 Pointer to game object 0.
/// \param p1 Pointer to game object 1.
/// \return true If object 0 is behind object 1.

bool ZCompare(const CGameObject* p0, const CGameObject* p1){
  return p0->m_vPos.z > p1->m_vPos.z;
} //ZCompare

CObjectManager::CObjectManager(){ 
  m_stlObjectList.clear();
  m_stlNameToObject.clear();
  m_stlNameToObjectType.clear();
  m_nLastGunFireTime = 0;
} //constructor

CObjectManager::~CObjectManager(){ 
  for(auto i=m_stlObjectList.begin(); i!=m_stlObjectList.end(); i++)
    delete *i;
} //destructor

//Luis-Quick function to get the floor number
int CObjectManager::getFloor() {
  return floor;
}

//Luis-Quick function to get the number of lives
int CObjectManager::getLives() {
  return lives;
}

//Luis-Quick function to get the number of seconds left on the power up timer
int CObjectManager::getPUTimer() {
  return PUTimer;
}

//Luis-Quick function to get the score
int CObjectManager::getScore() {
  return score;
}

//Luis-Quick function to if the player is down
BOOL CObjectManager::getDown() {
  return isDowned;
}

//Luis-Quick function to set number of enemies
void CObjectManager::setEne(int n) {
  numEne = n;
}

//Luis-Quick function to get number of enemies
int CObjectManager::getEne() {
  return numEne;
}

//Luis-Quick function to get gator's health
int CObjectManager::getPCHealth() {
  CGameObject* pgator = GetObjectByName("gator");//Getting the gator
  return pgator->m_nHealth;
}

//Luis-Quick function to get gator's location
Vector3 CObjectManager::getPCPos() {
  CGameObject* pgator = GetObjectByName("gator");//Getting the gator
  return pgator->m_vPos;
}

/// Insert a map from an object name string to an object type enumeration.
/// \param name Name of an object type
/// \param t Enumerated object type corresponding to that name.

void CObjectManager::InsertObjectType(const char* name, ObjectType t){
  m_stlNameToObjectType.insert(pair<string, ObjectType>(name, t)); 
} //InsertObjectType

/// Get the ObjectType corresponding to a type name string. Returns NUM_OBJECT_TYPES
/// if the name is not in m_stlNameToObjectType.
/// \param name Name of an object type
/// \return Enumerated object type corresponding to that name.

ObjectType CObjectManager::GetObjectType(const char* name){
  unordered_map<string, ObjectType>::iterator i = 
    m_stlNameToObjectType.find(name);
  if(i == m_stlNameToObjectType.end()) //if name not in map
    return NUM_OBJECT_TYPES; //error return
  else return i->second; //return object type
} //GetObjectType

/// Create a new instance of a game object.
/// \param obj The type of the new object
/// \param name The name of object as found in name tag of XML settings file
/// \param s Location.
/// \param v Velocity.
/// \return Pointer to object created.

CGameObject* CObjectManager::createObject(ObjectType obj, const char* name, const Vector3& s, const Vector3& v){
  CGameObject* p;

  if (obj == CROW_OBJECT) {
	  p = new CCrowObject(name, s, v);
  }
  //else p = new CGameObject(obj, name, s, v);

  if (obj == GORILLA_OBJECT) {
	  p = new CGorillaObject(name, s, v);
  }

  if (obj == DOG_OBJECT) {
    p = new CDogObject(name, s, v);
  }

  if (obj == HORSE_OBJECT) {
    p = new CHorseObject(name, s, v);
  }

  if (obj == RHINO_OBJECT) {
	  p = new CRhinoObject(name, s, v);
  }

  if (obj == BOSSBULL_OBJECT) {
    p = new CBossBullObject(name, s, v);
  }

  if (obj != CROW_OBJECT && obj != GORILLA_OBJECT && obj != DOG_OBJECT && obj != HORSE_OBJECT && obj != RHINO_OBJECT && obj != BOSSBULL_OBJECT) {
	  p = new CGameObject(obj, name, s, v);
  }


  m_stlObjectList.push_front(p); //insert in object list

  if(m_stlNameToObject.find(name) == m_stlNameToObject.end()) //if name not in map
    m_stlNameToObject.insert(pair<string, CGameObject*>(name, p)); //put it there

  return p;
} //createObject

/// Create a new instance of a game object with velocity zero.
/// \param objname The name of the new object's type
/// \param name The name of object as found in name tag of XML settings file
/// \param s Location.
/// \return Pointer to object created.

CGameObject* CObjectManager::createObject(const char* objname, const char* name, const Vector3& s){
  ObjectType obj = GetObjectType(objname);
  return createObject(obj, name, s, Vector3(0.0f));
} //createObject

/// Move all game objects, while making sure that they wrap around the world correctly.

void CObjectManager::move(){
  const float dX = 2.5f * (float)g_nScreenWidth; // Wrap distance from gator. Luis-Added * 2.5f

  //find the gator
  unordered_map<string, CGameObject*>::iterator gatorIterator = 
    m_stlNameToObject.find((string)"gator");
  CGameObject* gatorObject = gatorIterator->second;

  //move nonplayer objects
    for (auto i = m_stlObjectList.begin(); i != m_stlObjectList.end(); i++) { //for each object
      CGameObject* curObject = *i; //current object
      if (m_nGameState == PLAYING_STATE) {
        curObject->move(); //move it
        curObject->setPvPos(curObject->getPos());//Records last position
      }//Playing
      else {
        if (m_nGameState == PAUSE_STATE) {
          curObject->teleport(curObject->getPvPos());//Luis-Makes sure objects don't leave their previous positions
        }
        curObject->walk(Vector3(0, 0, 0));//Luis-Freezes Moving objects so they don't shift around while pasued
        curObject->m_vVelocity.x = 0;
        curObject->m_vVelocity.y = 0;
      }//Not Playing
    /*//wrap objects a fixed distance from gator
    if(curObject != gatorObject){ //not the gator
      float gatorX=0.0f; //gator's X coordinate
      if(gatorIterator != m_stlNameToObject.end())
        gatorX = gatorObject->m_vPos.x;

      float& x = curObject->m_vPos.x; //X coordinate of current object

      if(x > gatorX + dX) //too far behind//Luis-2,0 changed to 4.0 since the whole floor was not on screen
        x -= 4.0f*dX;
      else if(x < gatorX - dX) //too far ahead
        x += 4.0f*dX;
     } //if*/
    } //for
  
  CollisionDetection(); //collision detection
  if (gatorStrong) {//Luis-If a power up is active
    PoweredUp(0);
  }
  cull(); //cull old objects
  GarbageCollect(); //bring out yer dead!
} //move

/// Draw the objects from the object list and the player object. Care
/// must be taken to draw them from back to front.

void CObjectManager::draw(){
  m_stlObjectList.sort(ZCompare); //depth sort
  for(auto i = m_stlObjectList.begin(); i != m_stlObjectList.end(); i++) //for each object
    (*i)->draw();
} //draw

/// Get a pointer to an object by name, nullptr if it doesn't exist.
/// \param name Name of object.
/// \return Pointer to object created with that name, if it exists.

CGameObject* CObjectManager::GetObjectByName(const char* name){ 
  unordered_map<string, CGameObject*>::iterator 
    current = m_stlNameToObject.find((string)name);
  if(current != m_stlNameToObject.end())
    return current->second;
  else return nullptr;
} //GetObjectByName

/// Distance between objects.
/// \param pointer to first object 
/// \param pointer to second object
/// \return distance between the two objects

float CObjectManager::distance(CGameObject *g0, CGameObject *g1){ 
  if(g0 == nullptr || g1 == nullptr)return -1; //bail if bad pointer
  const float fWorldWidth = 8.0f * (float)g_nScreenWidth; //world width, Luis-Mess 2.0f
  float g_fTemp;//Luis-This float will store the x modifier based which object needs
  g_fTemp = g0->m_vPos.x;//Luis- sets the temp variable to the right float in case it's not needed
  if (g0->m_nObjectType == BULLET_OBJECT) {//Luis-Fake the position to make it look better
    g_fTemp += 150.0f;//Luis-Modifier
  }
  float x = (float)fabs(g_fTemp - g1->m_vPos.x); //x distance
  float y = (float)fabs(g0->m_vPos.y - g1->m_vPos.y); //y distance
  if(x > fWorldWidth) x -= (float)fWorldWidth; //compensate for wrap-around world
  return sqrtf(x*x + y*y);
} //distance

/// Fire a bullet from the gator's gun.
/// \param name Name of the object that is to fire the gun.

void CObjectManager::FireGun(char* name){   
  std::unordered_map<std::string, CGameObject*>::iterator gatorIterator = m_stlNameToObject.find((std::string)name);
  if(gatorIterator == m_stlNameToObject.end())return; //this should of course never happen

  const CGameObject* gatorObject = gatorIterator->second;

  if(g_cTimer.elapsed(m_nLastGunFireTime, 200)){ //slow down firing rate
    const float fAngle = gatorObject->m_fOrientation;
    const float fSine = sin(fAngle);
    const float fCosine = cos(fAngle);

    //enter the number of pixels from center of gator to gun
    const float fGunDx = -30.0f; 
    const float fGunDy = -17.0f;

    //initial bullet position
    const Vector3 s = gatorObject->m_vPos +
      Vector3(fGunDx*fCosine - fGunDy*fSine, fGunDx*fSine - fGunDy*fCosine, 0); 

    //velocity of bullet
    const float BULLETSPEED = -10.0f;
    const Vector3 v = BULLETSPEED * Vector3(-fCosine, -fSine, 0) +
      gatorObject->m_vVelocity;

    createObject(BULLET_OBJECT, "bullet", s, v); //create bullet
  } //if
} //FireGun

  /// Luis- Causes the object to throw a punch
  /// \param name Name of the object that is throwing the punch
  /// Direction is wheter the punch is being thrown left or right
  /// Derived from Dr. Parberry's FireGun method

void CObjectManager::ThrowPunch(char* name, bool isRight) {
	std::unordered_map<std::string, CGameObject*>::iterator gatorIterator =
		m_stlNameToObject.find((std::string)"gator");
	if (gatorIterator == m_stlNameToObject.end())return; //this should of course never happen
	CGameObject* gatorObject = gatorIterator->second;

	if (g_cTimer.elapsed(m_nLastGunFireTime, 400)) { //slow down firing rate
		const float fAngle = gatorObject->m_fOrientation;
		const float fSine = sin(fAngle);
		const float fCosine = cos(fAngle);

		float fGunDx ;
		float fGunDy ;

		//velocity of bullet
		float BULLETSPEED;
    if (isRight == TRUE) {//Luis-Is the object facing right
      BULLETSPEED = -5.0f;
      fGunDx = 40.0f;	//enter the number of pixels from center of gator to gun
      fGunDy = 0.0f;
    }
    else{
      BULLETSPEED = 5.0f;
      fGunDx = -40.0f;	//enter the number of pixels from center of gator to gun
      fGunDy = 0.0f;
    }
		const Vector3 v = BULLETSPEED * Vector3(-fCosine, -fSine, 0) +
	  gatorObject->m_vVelocity;
    //initial bullet position
    const Vector3 s = gatorObject->m_vPos +
      Vector3(fGunDx*fCosine - fGunDy*fSine, fGunDx*fSine - fGunDy*fCosine, -10);//Made z -= 10 to stop drawing transparent pixels, Negative=Right Arm
    if (isRight == TRUE) {
      gatorObject->UpdateSprite(GATORCHEER_OBJECT);
      createObject(PUNCH_OBJECT, "punch", s, v); //Throw Punch
      if (gatorStrong)
        createObject(PUNCHANIMATIONFIRERIGHT_OBJECT, "punchanimationfireright", s, v); //Throw Fire Punch;
      else
	      createObject(PUNCHANIMATIONRIGHT_OBJECT, "punchanimationright", s, v); //Throw Punch
    }
    else {
      gatorObject->UpdateSprite(GATORCHEERLEFT_OBJECT);
      createObject(PUNCHLEFT_OBJECT, "punch", s, v); //Throw Punch to left
      if (gatorStrong)
        createObject(PUNCHANIMATIONFIRELEFT_OBJECT, "punchanimationfireleft", s, v); //Throw Punch
      else
      createObject(PUNCHANIMATIONLEFT_OBJECT, "punchanimationleft", s, v); //Throw Punch
    }

	} //if
} //ThrowPunch

/// Cull old objects.
/// Run through the objects in the object list and compare their age to
/// their life span. Kill any that are too old. Immortal objects are
/// flagged with a negative life span, so ignore those.

void CObjectManager::cull(){ 
  for(auto i=m_stlObjectList.begin(); i!=m_stlObjectList.end(); i++){
    CGameObject* object = *i; //current object

    //died of old age
    if(object->m_nLifeTime > 0 && //if mortal and ...
    (g_cTimer.time() - object->m_nBirthTime > object->m_nLifeTime)) //...old...
      object->m_bIsDead = TRUE; //slay it

    //one shot animation 
    if(object->m_nFrameCount > 1 && !object->m_bCycleSprite && //if plays one time...
      object->m_nCurrentFrame >= object->m_nAnimationFrameCount){ //and played once already...
        object->m_bIsDead = TRUE; //slay it
        CreateNextIncarnation(object); //create next in the animation sequence
    } //if
  } //for
} //cull

/// Clean Tiles
/// Luis-This function will go though the object list and kill all tiles
/// in order to prepare to generate a new floor.

void CObjectManager::cleanTiles() {
  for (auto i = m_stlObjectList.begin(); i != m_stlObjectList.end(); i++) {
    CGameObject* object = *i; //current object
    //So any object with -1000 lifetime is a tile
    if (object->m_nLifeTime == -1000) {
      object->m_bIsDead = TRUE; //remove tile from existence
      //object->setLifetime(5000);
      //object->walk(Vector3(0.0f, -10.0f, 0.0f));//Movedown
    }//If
  }//for
} //cleanTiles

  /// Clean Enemies
  /// Luis-This function will go though the object list and kill all enemies
  /// in order to prepare to generate a new floor.

void CObjectManager::cleanEnemies() {
  for (auto i = m_stlObjectList.begin(); i != m_stlObjectList.end(); i++) {
    CGameObject* object = *i; //current object
    if (object->m_nObjectType == GORILLA_OBJECT || object->m_nObjectType == CROW_OBJECT 
      || object->m_nObjectType == DOG_OBJECT || object->m_nObjectType == HORSE_OBJECT 
      || object->m_nObjectType == RHINO_OBJECT || object->m_nObjectType == BOSSBULL_OBJECT
      || object->m_nObjectType == DOGDEAD_OBJECT || object->m_nObjectType == DEAD_GORILLA_OBJECT 
      || object->m_nObjectType == HORSEDEAD_OBJECT || object->m_nObjectType == RHINODEAD_OBJECT) {//If the object is an enemy
        object->m_bIsDead = TRUE; //remove enemy from existence
    }//If
  } //for
} //cleanTiles

/// Create the object next in the appropriate series (object, exploding
/// object, dead object). If there's no "next" object, do nothing.
/// \param object Pointer to the object to be replaced

void CObjectManager::CreateNextIncarnation(CGameObject* object){ 
  CGameObject* temp;
  if (object->m_nObjectType == GORILLA_OBJECT) {
    if (object->m_nHealth > 0) {
      if (object->m_bfaceLeft) {
        if (object->m_bIsChampion) {
          object->UpdateSprite(CHAMPIONMADGORILLALEFT_OBJECT);
          object->m_vVelocity.x = object->m_vVelocity.x + 10.0f;// Alfredo - Push back if attacked
        }
        else {
          object->UpdateSprite(MADGORILLALEFT_OBJECT);
          object->m_vVelocity.x = object->m_vVelocity.x + 10.0f;// Alfredo - Push back if attacked
        }

      }//if
      else {
        if (object->m_bIsChampion) {
          object->UpdateSprite(CHAMPIONMADGORILLARIGHT_OBJECT);
          object->m_vVelocity.x = object->m_vVelocity.x - 10.0f;// Alfredo - Push back if attacked
        }
        else {
          object->UpdateSprite(MADGORILLARIGHT_OBJECT);
          object->m_vVelocity.x = object->m_vVelocity.x - 10.0f;// Alfredo - Push back if attacked
        }
      } //else
    }//if
    else {
      if (object->m_bfaceRight) {
        if (object->m_bIsChampion) {
          score = score + 300; //Alfedo - 300 points for killing the champ gorilla
          createObject(CHAMPIONGORILLADOWNEDLEFT_OBJECT, "championgorilladownedleft", object->m_vPos, object->m_vVelocity); //create new one
        }
        else {
          score = score + 150; //Alfedo - 150 points for killing the gorilla
          createObject(GORILLADOWNEDLEFT_OBJECT, "gorilladownedleft", object->m_vPos, object->m_vVelocity); //create new one
        }
      }//if
      else {
        if (object->m_bIsChampion) {
          score = score + 300; //Alfedo - 300 points for killing the champ gorilla
          createObject(CHAMPIONGORILLADOWNEDRIGHT_OBJECT, "championgorilladownedright", object->m_vPos, object->m_vVelocity); //create new one
        }
        else {
          score = score + 150; //Alfedo - 150 points for killing the gorilla
          createObject(GORILLADOWNEDRIGHT_OBJECT, "gorilladownedright", object->m_vPos, object->m_vVelocity); //create new one
        }
      }//else
    }//else
  }//if 
  else if (object->m_nObjectType == GORILLADOWNEDLEFT_OBJECT || object->m_nObjectType == GORILLADOWNEDRIGHT_OBJECT) {
    object->m_vVelocity.x = 0;  
	  object->m_vVelocity.y = 0;
    createObject(DEAD_GORILLA_OBJECT, "deadgorilla", object->m_vPos,
      object->m_vVelocity); //create new one
  }
  else if (object->m_nObjectType == CHAMPIONGORILLADOWNEDLEFT_OBJECT || object->m_nObjectType == CHAMPIONGORILLADOWNEDRIGHT_OBJECT) {
    object->m_vVelocity.x = 0;
    object->m_vVelocity.y = 0;
    createObject(CHAMPIONDEAD_GORILLA_OBJECT, "championdeadgorilla", object->m_vPos,
      object->m_vVelocity); //create new one
  }


  if (object->m_nObjectType == RHINO_OBJECT) {
    if (object->m_nHealth > 0) {
      if (object->m_bfaceLeft) {
        object->UpdateSprite(RHINOHURTLEFT_OBJECT);
        //object->m_vPos.x = object->m_vPos.x + 10;// Alfredo - Push back if attacked

      }//if
      else {
        object->UpdateSprite(RHINOHURTRIGHT_OBJECT);
        //object->m_vPos.x = object->m_vPos.x - 10;// Alfredo - Push back if attacked
      } //else
    }//if
    else {
      score = score + 250; //Alfedo - 250 points for killing the rhino
      if (object->m_bfaceRight) {
        createObject(RHINODOWNEDLEFT_OBJECT, "rhinodownedleft", object->m_vPos, object->m_vVelocity); //create new one
      }//if
      else {
        createObject(RHINODOWNEDRIGHT_OBJECT, "rhinodownedright", object->m_vPos, object->m_vVelocity); //create new one
      }//else
    }//else
  }
  else if (object->m_nObjectType == RHINODOWNEDLEFT_OBJECT || object->m_nObjectType == RHINODOWNEDRIGHT_OBJECT) {
    object->m_vVelocity.x = 0;
    object->m_vVelocity.y = 0;
    createObject(RHINODEAD_OBJECT, "rhinodead", object->m_vPos,
      object->m_vVelocity); //create new one
  }

  if (object->m_nObjectType == BOSSBULL_OBJECT) {
    if (object->m_nHealth > 0) {
      if (object->m_bfaceLeft) {
        object->UpdateSprite(BOSSBULLHURTLEFT_OBJECT);
        object->m_vPos.x = object->m_vPos.x + 1;// Alfredo - Push back if attacked

      }//if
      else {
        object->UpdateSprite(BOSSBULLHURTRIGHT_OBJECT);
        object->m_vPos.x = object->m_vPos.x - 1;// Alfredo - Push back if attacked
      } //else
    }//if
    else {
      score = score + 1000; //Alfedo - 1000 points for killing the boss
      object->m_bIsDead = TRUE;
      if (object->m_bfaceRight)
        createObject(BOSSBULLDOWNEDLEFT_OBJECT, "bossbulldownedleft", object->m_vPos, object->m_vVelocity); //create new one
      else 
        createObject(BOSSBULLDOWNEDRIGHT_OBJECT, "bossbulldownedright", object->m_vPos, object->m_vVelocity); //create new one

      if (floor == 10) {
        m_nGameState = BOSSBEATENSCREEN_STATE;
      }
    }//else
  }
  else if (object->m_nObjectType == BOSSBULLDOWNEDLEFT_OBJECT || object->m_nObjectType == BOSSBULLDOWNEDRIGHT_OBJECT) {
    object->m_vVelocity.x = 0;
    object->m_vVelocity.y = 0;
    if (object->m_nObjectType == BOSSBULLDOWNEDLEFT_OBJECT)
      createObject(BOSSBULLDEADLEFT_OBJECT, "bossbulldeadleft", object->m_vPos, object->m_vVelocity); //create new one
    else if (object->m_nObjectType == BOSSBULLDOWNEDRIGHT_OBJECT)
      createObject(BOSSBULLDEADRIGHT_OBJECT, "bossbulldeadright", object->m_vPos, object->m_vVelocity); //create new one
  }
  
  if (object->m_nObjectType == CROW_OBJECT) {
    createObject(EXPLODINGCROW_OBJECT, "explodingcrow", object->m_vPos,
      object->m_vVelocity); //create new one
  }
  else if (object->m_nObjectType == EXPLODINGCROW_OBJECT) {
    createObject(DEADCROW_OBJECT, "deadcrow", object->m_vPos,
      object->m_vVelocity); //create new one
  }

  if (object->m_nObjectType == DOG_OBJECT) {
    if (object->m_nHealth > 0) {
      if (object->m_bfaceLeft) {
        if (object->m_bIsChampion) {
          object->UpdateSprite(CHAMPIONDOGMADLEFT_OBJECT);
          //object->m_vPos.x = object->m_vPos.x + 10;// Alfredo - Push back if attacked
          object->m_vVelocity.x = object->m_vVelocity.x + 10.0f;// Alfredo - Push back if attacked
        }
        else {
          object->UpdateSprite(DOGMADLEFT_OBJECT);
          //object->m_vPos.x = object->m_vPos.x + 10;// Alfredo - Push back if attacked
          object->m_vVelocity.x = object->m_vVelocity.x + 10.0f;// Alfredo - Push back if attacked
        }

      }//if
      else {
        if (object->m_bIsChampion) {
          object->UpdateSprite(CHAMPIONDOGMADRIGHT_OBJECT);
          //object->m_vPos.x = object->m_vPos.x - 10;// Alfredo - Push back if attacked
          object->m_vVelocity.x = object->m_vVelocity.x - 10.0f;// Alfredo - Push back if attacked
        }//if
        else {
          object->UpdateSprite(DOGMADRIGHT_OBJECT);
          //object->m_vPos.x = object->m_vPos.x - 10;// Alfredo - Push back if attacked
          object->m_vVelocity.x = object->m_vVelocity.x - 10.0f;// Alfredo - Push back if attacked
        }//else
      } //else
    }//if
    else {
      g_pSoundManager->play(8);
      if (object->m_bfaceRight) {
        if (object->m_bIsChampion) {
          createObject(CHAMPIONDOGDOWNEDRIGHT_OBJECT, "championdogdownedright", object->m_vPos, object->m_vVelocity); //create new one
          score = score + 200; //Alfedo - 200 points for killing the champ dog
        }
        else {
          createObject(DOGDOWNEDRIGHT_OBJECT, "dogdownedright", object->m_vPos, object->m_vVelocity); //create new one
          score = score + 100; //Alfedo - 100 points for killing the dog
        }
      }//if
      else {
        if (object->m_bIsChampion) {
          createObject(CHAMPIONDOGDOWNEDLEFT_OBJECT, "championdogdownedleft", object->m_vPos, object->m_vVelocity); //create new one
          score = score + 200; //Alfedo - 100 points for killing the champ dog
        }
        else {
          createObject(DOGDOWNEDLEFT_OBJECT, "dogdownedleft", object->m_vPos, object->m_vVelocity); //create new one
          score = score + 100; //Alfedo - 100 points for killing the dog
        }
      }//else
    }//else
  }
  else if (object->m_nObjectType == DOGDOWNEDLEFT_OBJECT || object->m_nObjectType == DOGDOWNEDRIGHT_OBJECT) {
    object->m_vVelocity.x = 0;
    object->m_vVelocity.y = 0;
    createObject(DOGDEAD_OBJECT, "dogdead", object->m_vPos,
      object->m_vVelocity); //create new one
  }
  else if (object->m_nObjectType == CHAMPIONDOGDOWNEDLEFT_OBJECT || object->m_nObjectType == CHAMPIONDOGDOWNEDRIGHT_OBJECT) {
    object->m_vVelocity.x = 0;
    object->m_vVelocity.y = 0;
    createObject(CHAMPIONDOGDEAD_OBJECT, "championdogdead", object->m_vPos,
      object->m_vVelocity); //create new one
  }

  if (object->m_nObjectType == GATOR_OBJECT) { // Alfedo - If the gator gets hit, show hurt animation (not added yet) 
    temp = createObject(STAR_OBJECT, "star", object->m_vPos,
      object->m_vVelocity); //Luis-Made this a star
    if (object->m_bfaceRight) {//If gator is facing right, darw the hurt sprite facing right
      object->UpdateSprite(GATORHURT_OBJECT);
    }
    else {
      object->UpdateSprite(GATORHURTLEFT_OBJECT);
    }
    temp->m_vPos.z -= (rand() % 10) + 5;;//Luis-So the star doesn't clip the Gator Sprite
    temp->m_fOrientation = (rand() % 360);//Luis-Gave the star a little spin
    temp->m_vVelocity.y += 40.0f;//Luis-Make the star fly in the air
    temp->m_vVelocity.x += (rand() % 5);//Luis-These make the star fly in a random x direction
    temp->m_vVelocity.x -= (rand() % 10);
  }

  if (object->m_nObjectType == HORSE_OBJECT) {
	  if (object->m_nHealth > 0) {
		  if (object->m_bfaceLeft) {
			  object->UpdateSprite(HORSEMADLEFT_OBJECT);
			  //object->m_vPos.x = object->m_vPos.x + 1;// Alfredo - Push back if attacked

		  }//if
		  else {
			  object->UpdateSprite(HORSEMADRIGHT_OBJECT);
			  //object->m_vPos.x = object->m_vPos.x - 1;// Alfredo - Push back if attacked
		  } //else
	  }//if
	  else {
			  object->m_vVelocity.x = 0;
			  object->m_vVelocity.y = 0;
			  createObject(HORSEDEAD_OBJECT, "horsedead", object->m_vPos, object->m_vVelocity); //create new one
        score = score + 200; //Alfedo - 200 points for killing the horse
	  }//else
  }//if 

} //CreateNextIncarnation

/// Master collision detection function.
/// Compare every object against every other object for collision. Only
/// bullets can collide right now.

void CObjectManager::CollisionDetection(){ 
  for (auto i = m_stlObjectList.begin(); i != m_stlObjectList.end(); i++) {
    if ((*i)->m_nObjectType == BULLET_OBJECT || (*i)->m_nObjectType == HORSEATTACK_OBJECT || (*i)->m_nObjectType == BOSSBULLATTACKLEFT_OBJECT || (*i)->m_nObjectType == BOSSBULLATTACKRIGHT_OBJECT) // if its an enemy attack
      CollisionDetection(*i); //check every object for collision with this bullet
    if ((*i)->m_nObjectType == PUNCH_OBJECT || (*i)->m_nObjectType == PUNCHLEFT_OBJECT) //Luis-and is a punch
      CollisionDetection(*i); //check every object for collision with this punch
    if ((*i)->m_nObjectType == GATOR_OBJECT || (*i)->m_nObjectType == GORILLA_OBJECT 
      || (*i)->m_nObjectType == DOG_OBJECT || (*i)->m_nObjectType == HORSE_OBJECT 
      || (*i)->m_nObjectType == RHINO_OBJECT || (*i)->m_nObjectType == BOSSBULL_OBJECT) //and is a gator
      CollisionDetection(*i); //check every object for collision with this gator
  }
} //CollisionDetection

/// Given an object pointer, compare that object against every other 
/// object for collision. If a collision is detected, replace the object hit
/// with the next in series (if one exists), and kill the object doing the
/// hitting (bullets don't go through objects in this game).
/// \param p Pointer to the object to be compared against.
//Luis-Added a call to the HitBox Collision function

void CObjectManager::CollisionDetection(CGameObject* p){ 
  for (auto j = m_stlObjectList.begin(); j != m_stlObjectList.end(); j++) {
    HitboxCollision(p, *j);//Check all the boxes!
  }//for
} //CollisionDetection



/// Luis-Exprimental Hitbox Collision
/// This will use Hitboxes to detect collision rather than the distance function
/// Is p1 colliding with p2
void CObjectManager::HitboxCollision(CGameObject* p0, CGameObject* p1) {
  //Check each point
  BOOL Collision = FALSE;
  BOOL Collision2 = FALSE;
  float yfl;//Luis-Keeps track of the y distance between two objects
  float xfl;//Luis-THIS IS THE XFL!, the float keeping track of x distance between two objects
  BOOL Test = FALSE;//Set this to true if you want to see collison 
  yfl = p0->m_vPos.y - p1->m_vPos.y;//Luis-Getting Y distance
  xfl = p0->m_vPos.x - p1->m_vPos.x;//Luis-Getting Y distance
  CGameObject* temp;
  //Check both boxes, if there are ANY Collsions at all, set to TRUE, this makes the boxes solid
  if (p0->MyHitboxes[0]->intersect(p1->MyHitboxes[0]->getUL()) || p0->MyHitboxes[0]->intersect(p1->MyHitboxes[0]->getUR())
    || p0->MyHitboxes[0]->intersect(p1->MyHitboxes[0]->getLL()) || p0->MyHitboxes[0]->intersect(p1->MyHitboxes[0]->getLR())
    || p1->MyHitboxes[0]->intersect(p0->MyHitboxes[0]->getUL()) || p1->MyHitboxes[0]->intersect(p0->MyHitboxes[0]->getUR())
    || p1->MyHitboxes[0]->intersect(p0->MyHitboxes[0]->getLL()) || p1->MyHitboxes[0]->intersect(p0->MyHitboxes[0]->getLR())) {
    Collision = TRUE;
  }
  //What happens when something collides? Place that here.
  if (Collision) {
    //Gator Punches collision with other objects
    if ((p0->m_nObjectType == PUNCHLEFT_OBJECT || (p0->m_nObjectType == PUNCH_OBJECT)) && (p1->m_nObjectType == GORILLA_OBJECT)) { // Alfredo - If punch hits gorilla, gorilla takes damage
		p0->m_bIsDead = TRUE;
		int blockChance = g_cRandom.number(1, 5);
    int randDamage = g_cRandom.number(8, 10);
        if (blockChance != 5) { //Alfredo- chance the gorilla can block itself
			if (gatorStrong == TRUE)
				p1->takeHit(500); //Alfredo - if the gorilla takes a punch, it takes 5 damage
		else {
			p1->takeHit(randDamage); //Alfredo - if the champ gorilla takes a punch, it takes 2 damage
		}
				
        if (p1->m_nHealth <= 0) { // Alfredo - if the gorilla has no health, it dies
			p1->m_bIsDead = TRUE;
			CreateNextIncarnation(p1); //replace with dead object, if any
        }//if 
        else {
			CreateNextIncarnation(p1); //Alfredo- show mad gorilla animation because it got hit
        }
        }//if
        else {
          if (p1->m_bfaceLeft) {
            if (p1->m_bIsChampion) {
              p1->UpdateSprite(CHAMPIONGORILLABLOCKLEFT_OBJECT);
              p1->m_vPos.x = p1->m_vPos.x + 5;// Alfredo - Push back if attacked
            }
            else {
              p1->UpdateSprite(GORILLABLOCKLEFT_OBJECT);
              p1->m_vPos.x = p1->m_vPos.x + 5;// Alfredo - Push back if attacked
            }

          }//if
          else {
            if (p1->m_bIsChampion) {
              p1->UpdateSprite(CHAMPIONGORILLABLOCKRIGHT_OBJECT);
              p1->m_vPos.x = p1->m_vPos.x - 5;// Alfredo - Push back if attacked 
            }
            else {
              p1->UpdateSprite(GORILLABLOCKRIGHT_OBJECT);
              p1->m_vPos.x = p1->m_vPos.x - 5;// Alfredo - Push back if attacked 
            }
          }//else
        }//else
    }//Punching a gorilla
    if ((p0->m_nObjectType == PUNCHLEFT_OBJECT || (p0->m_nObjectType == PUNCH_OBJECT)) && (p1->m_nObjectType == DOG_OBJECT)) {
		p0->m_bIsDead = TRUE;
		int randDamage = g_cRandom.number(8, 10);
		if (gatorStrong == TRUE)
			p1->takeHit(500); //Alfredo - if the gorilla takes a punch, it takes 5 damage
		else
			p1->takeHit(randDamage); //Alfredo - if the gorilla takes a punch, it takes 5 damage
        if (p1->m_nHealth <= 0) { // Alfredo - if the dog has no health, it dies
          p1->m_bIsDead = TRUE;
          CreateNextIncarnation(p1); //replace with dead object, if any
        }//if
        else
         CreateNextIncarnation(p1); //Alfredo- show mad dog animation because it got hit
    }//Punching a dog
    if ((p0->m_nObjectType == PUNCHLEFT_OBJECT || (p0->m_nObjectType == PUNCH_OBJECT)) && (p1->m_nObjectType == RHINO_OBJECT)) {
		p0->m_bIsDead = TRUE;
    int randDamage = g_cRandom.number(8, 10);

		if (gatorStrong == TRUE)
        p1->takeHit(500); //Alfredo - if the rhino takes a punch, it takes 5 damage
      else
        p1->takeHit(randDamage); //Alfredo - if the rhino takes a punch, it takes 5 damage
      if (p1->m_nHealth <= 0) { // Alfredo - if the dog has no health, it dies
        p1->m_bIsDead = TRUE;
        CreateNextIncarnation(p1); //replace with dead object, if any
      }//if
      else
        CreateNextIncarnation(p1); //Alfredo- show mad dog animation because it got hit
    }//Punching a rhino
    if ((p0->m_nObjectType == PUNCHLEFT_OBJECT || (p0->m_nObjectType == PUNCH_OBJECT)) && (p1->m_nObjectType == BOSSBULL_OBJECT)) {
		p0->m_bIsDead = TRUE;
    int randDamage = g_cRandom.number(8, 10);
		if (gatorStrong == TRUE)
        p1->takeHit(500); //Alfredo - if the rhino takes a punch, it takes 5 damage
      else
        p1->takeHit(randDamage); //Alfredo - if the rhino takes a punch, it takes 5 damage
      if (p1->m_nHealth <= 0) { // Alfredo - if the dog has no health, it dies
        p1->m_bIsDead = TRUE;
        CreateNextIncarnation(p1); //replace with dead object, if any
      }//if
      else
        CreateNextIncarnation(p1); //Alfredo- show mad dog animation because it got hit
    }//Punching a rhino
  	if ((p0->m_nObjectType == PUNCHLEFT_OBJECT || (p0->m_nObjectType == PUNCH_OBJECT)) && (p1->m_nObjectType == HORSE_OBJECT)) {
		p0->m_bIsDead = TRUE;
    int randDamage = g_cRandom.number(8, 10);

		if (gatorStrong == TRUE)
			p1->takeHit(500); //Alfredo - if the gorilla takes a punch, it takes 5 damage
		else
			p1->takeHit(randDamage); //Alfredo - if the gorilla takes a punch, it takes 5 damage

    if(p0->m_nObjectType == PUNCHLEFT_OBJECT)
      p1->m_vPos.x = p1->m_vPos.x - 50.0f;// Alfredo - Push back if attacked
    else
      p1->m_vPos.x = p1->m_vPos.x + 50.0f;// Alfredo - Push back if attacked

		if (p1->m_nHealth <= 0) { // Alfredo - if the dog has no health, it dies
			p1->m_bIsDead = TRUE;
			CreateNextIncarnation(p1); //replace with dead object, if any
		}//if 
		else 
      CreateNextIncarnation(p1); //Alfredo- show mad horse animation because it got hit
	}//Punching a horse
    if ((p0->m_nObjectType == PUNCHLEFT_OBJECT || (p0->m_nObjectType == PUNCH_OBJECT)) && (p1->m_nObjectType == VENDINGMACHINE_OBJECT)) {
      // Drop power up 
        if (score >= 50) // Alfredo - Using the vending machine costs 50 points
          score = score - 50;
		else {
			g_pSoundManager->play(21);
			return;
		}
        p0->m_bIsDead = TRUE;
        p1->takeHit(50);

        if (p1->m_nHealth <= 0) { // Alfredo - if the gorilla has no health, it dies
          p1->m_bIsDead = TRUE;
        }//if
        int sodaChance, sodaRandom;
		sodaChance = g_cRandom.number(0, 9);  //Randomizes the soda drop from vending machine
    if (sodaChance == 6) { sodaRandom = 1; }
    else {sodaRandom = 0;}
		switch (sodaRandom) {
			case 0: //Health Soda
        temp = createObject(HEALTHSODA_OBJECT, "healthsoda", p1->m_vPos, p1->m_vVelocity); //create new one
				temp->m_vPos.y -= 50.0f;//Luis-Make the soda shoot out of the slot
				temp->m_vVelocity.x += (rand() % 5);//Luis-These make the soda fly in a random x direction
				temp->m_vVelocity.x -= (rand() % 10);
				temp->m_vPos.z -= (rand() % 20) + 5;;//Luis-So the sodas don't clip with each other
				temp->m_bCanFly = FALSE;
				break;
			case 1: //Strength Soda
				temp = createObject(STRENGTHSODA_OBJECT, "strengthsoda", p1->m_vPos, p1->m_vVelocity); //create new one
				temp->m_vPos.y -= 50.0f;//Luis-Make the soda shoot out of the slot
				temp->m_vVelocity.x += (rand() % 5);//Luis-These make the soda fly in a random x direction
				temp->m_vVelocity.x -= (rand() % 10);
				temp->m_vPos.z -= (rand() % 20) + 5;;//Luis-So the sodas don't clip with each other
				temp->m_bCanFly = FALSE;
				break;
			default:
				;//Nothing 
		}
    }//Punching a vending machine
    if ((p0->m_nObjectType == PUNCHLEFT_OBJECT || (p0->m_nObjectType == PUNCH_OBJECT)) && (p1->m_nObjectType == CROW_OBJECT)) {
        p1->m_bIsDead = TRUE; //they're dead, Jim
		p0->m_bIsDead = TRUE;
        CreateNextIncarnation(p1); //replace with dead object, if any
    }//Punching a Crow
    if ((p0->m_nObjectType == PUNCHLEFT_OBJECT || (p0->m_nObjectType == PUNCH_OBJECT)) && (p1->m_nObjectType == DESKHORIZONTAL_OBJECT)) {
      int deskHitRand = g_cRandom.number(40,60);//Random damage to desk
      int deskDropChance = g_cRandom.number(0, 4);//Random of dropping an item
      p1->takeHit(deskHitRand);
	  p0->m_bIsDead = TRUE;
      if (p1->m_nHealth <= 0) { // Alfredo - if the gorilla has no health, it dies
        p1->m_bIsDead = TRUE;
        g_pSoundManager->play(13);
        temp = createObject(DESKEXPLOSION_OBJECT, "deskexplosion", p1->m_vPos, p1->m_vVelocity); //Alfredo- exploding desk 
        if (temp->m_nObjectType == DESKEXPLOSION_OBJECT) {
          temp->m_vPos.z = temp->m_vPos.z - 1;
          if (deskDropChance == 0) // 1/5 chance of dropping money
            temp = createObject(MONEYBAG_OBJECT, "moneybag", p1->m_vPos, p1->m_vVelocity); //drop a moneybag
        }//if
            temp->m_bCanFly = TRUE;
      }//if
      else
        return;
    }
    if ((p0->m_nObjectType == PUNCHLEFT_OBJECT || (p0->m_nObjectType == PUNCH_OBJECT)) && (p1->m_nObjectType == WATERCOOLER_OBJECT)) {
      int coolerHitRand = g_cRandom.number(40, 60);//Random damage to water cooler
      int deskDropChance = g_cRandom.number(0, 4);//Random chance of dropping an item
      p1->takeHit(coolerHitRand);
	  p0->m_bIsDead = TRUE;
      if (p1->m_nHealth <= 0) { // Alfredo - if the gorilla has no health, it dies
        p1->m_bIsDead = TRUE;
        g_pSoundManager->play(14);
        temp = createObject(WATERCOOLEREXPLOSION_OBJECT, "watercoolerexplosion", p1->m_vPos, p1->m_vVelocity); //Alfredo- exploding desk 
        if (temp->m_nObjectType == WATERCOOLEREXPLOSION_OBJECT) {
          temp->m_vPos.z = temp->m_vPos.z - 1;
          if (deskDropChance == 0) // 1/5 chance of dropping money
            temp = createObject(HEALTHSODA_OBJECT, "healthsoda", p1->m_vPos, p1->m_vVelocity); //drop a moneybag
        }//if
        temp->m_bCanFly = TRUE;
      }//if
      else
        return;
    }
    if ((p0->m_nObjectType == PUNCHLEFT_OBJECT || (p0->m_nObjectType == PUNCH_OBJECT)) && (p1->m_nObjectType == POTTEDPLANT_OBJECT)) {
      int plantHitRand = g_cRandom.number(40,60);//Random damage to potted plant
      int deskDropChance = g_cRandom.number(0, 4);//Random chance of dropping an item
      p1->takeHit(plantHitRand);
	  p0->m_bIsDead = TRUE;
      if( p1->m_nHealth <= 0) { // Alfredo - if the gorilla has no health, it dies
        p1->m_bIsDead = TRUE;
        g_pSoundManager->play(19);
        temp = createObject(POTTEDPLANTEXPLOSION_OBJECT, "pottedplantexplosion", p1->m_vPos, p1->m_vVelocity); //Alfredo- exploding potted plant 
        if (temp->m_nObjectType == POTTEDPLANTEXPLOSION_OBJECT) {
          temp->m_vPos.z = temp->m_vPos.z - 1;
          if (deskDropChance == 0) // 1/5 chance of dropping money
            temp = createObject(HEALTHSODA_OBJECT, "healthsoda", p1->m_vPos, p1->m_vVelocity); //drop a moneybag
        }//if
        temp->m_bCanFly = TRUE;
      }//if
      else
        return;
    }
    //Enemy Attacks to Gator 
    if ((p0->m_nObjectType == BULLET_OBJECT) && (p1->m_nObjectType == GATOR_OBJECT)) {
      if (p0->MyHitboxes[0]->intersect(p1->MyHitboxes[1]->getUL()) || p0->MyHitboxes[0]->intersect(p1->MyHitboxes[1]->getUR())
        || p0->MyHitboxes[0]->intersect(p1->MyHitboxes[1]->getLL()) || p0->MyHitboxes[0]->intersect(p1->MyHitboxes[1]->getLR())
        || p1->MyHitboxes[1]->intersect(p0->MyHitboxes[0]->getUL()) || p1->MyHitboxes[1]->intersect(p0->MyHitboxes[0]->getUR())
        || p1->MyHitboxes[1]->intersect(p0->MyHitboxes[0]->getLL()) || p1->MyHitboxes[1]->intersect(p0->MyHitboxes[0]->getLR())) {
        Collision2 = TRUE;
      }
      if (Collision || Collision2) {//If the bullet hits either hitbox
        p1->takeHit(1);
        p0->m_bIsDead = TRUE;
	    	g_pSoundManager->play(2);
        LoseALife(p1);//Checks is Gator lost a life
        CreateNextIncarnation(p1);
      }
      if (Test) {//Move this around to test different collisions
        createObject(MARKER_OBJECT, "marker", p1->MyHitboxes[1]->getUL(), Vector3(0, 0, 0));
        createObject(MARKER_OBJECT, "marker", p1->MyHitboxes[1]->getUR(), Vector3(0, 0, 0));
        createObject(MARKER_OBJECT, "marker", p1->MyHitboxes[1]->getLL(), Vector3(0, 0, 0));
        createObject(MARKER_OBJECT, "marker", p1->MyHitboxes[1]->getLR(), Vector3(0, 0, 0));
      }
    }//Attack on Gator
  	if ((p0->m_nObjectType == HORSEATTACK_OBJECT) && (p1->m_nObjectType == GATOR_OBJECT)) {
		if (p0->MyHitboxes[0]->intersect(p1->MyHitboxes[1]->getUL()) || p0->MyHitboxes[0]->intersect(p1->MyHitboxes[1]->getUR())
			|| p0->MyHitboxes[0]->intersect(p1->MyHitboxes[1]->getLL()) || p0->MyHitboxes[0]->intersect(p1->MyHitboxes[1]->getLR())
			|| p1->MyHitboxes[1]->intersect(p0->MyHitboxes[0]->getUL()) || p1->MyHitboxes[1]->intersect(p0->MyHitboxes[0]->getUR())
			|| p1->MyHitboxes[1]->intersect(p0->MyHitboxes[0]->getLL()) || p1->MyHitboxes[1]->intersect(p0->MyHitboxes[0]->getLR())) {
			Collision2 = TRUE;
		}
		if (Collision || Collision2) {//If the bullet hits either hitbox
			p1->takeHit(5);
			p0->m_bIsDead = TRUE;
			g_pSoundManager->play(2);
			LoseALife(p1);//Checks is Gator lost a life
			CreateNextIncarnation(p1);
		}
		if (Test) {//Move this around to test different collisions
			createObject(MARKER_OBJECT, "marker", p1->MyHitboxes[1]->getUL(), Vector3(0, 0, 0));
			createObject(MARKER_OBJECT, "marker", p1->MyHitboxes[1]->getUR(), Vector3(0, 0, 0));
			createObject(MARKER_OBJECT, "marker", p1->MyHitboxes[1]->getLL(), Vector3(0, 0, 0));
			createObject(MARKER_OBJECT, "marker", p1->MyHitboxes[1]->getLR(), Vector3(0, 0, 0));
		}
	}//Attack on Gator
    if ((p0->m_nObjectType == BOSSBULLATTACKLEFT_OBJECT) && (p1->m_nObjectType == GATOR_OBJECT)) {
      if (p0->MyHitboxes[0]->intersect(p1->MyHitboxes[1]->getUL()) || p0->MyHitboxes[0]->intersect(p1->MyHitboxes[1]->getUR())
        || p0->MyHitboxes[0]->intersect(p1->MyHitboxes[1]->getLL()) || p0->MyHitboxes[0]->intersect(p1->MyHitboxes[1]->getLR())
        || p1->MyHitboxes[1]->intersect(p0->MyHitboxes[0]->getUL()) || p1->MyHitboxes[1]->intersect(p0->MyHitboxes[0]->getUR())
        || p1->MyHitboxes[1]->intersect(p0->MyHitboxes[0]->getLL()) || p1->MyHitboxes[1]->intersect(p0->MyHitboxes[0]->getLR())) {
        Collision2 = TRUE;
      }
      if (Collision || Collision2) {//If the bullet hits either hitbox
			p1->takeHit(10);
			p0->m_bIsDead = TRUE;
			g_pSoundManager->play(2);
			p1->m_vVelocity.x = p1->m_vVelocity.x - 35.0f;// Alfredo - Push back if attacked       
			LoseALife(p1);//Checks is Gator lost a life
			CreateNextIncarnation(p1);
      }
      if (Test) {//Move this around to test different collisions
        createObject(MARKER_OBJECT, "marker", p1->MyHitboxes[1]->getUL(), Vector3(0, 0, 0));
        createObject(MARKER_OBJECT, "marker", p1->MyHitboxes[1]->getUR(), Vector3(0, 0, 0));
        createObject(MARKER_OBJECT, "marker", p1->MyHitboxes[1]->getLL(), Vector3(0, 0, 0));
        createObject(MARKER_OBJECT, "marker", p1->MyHitboxes[1]->getLR(), Vector3(0, 0, 0));
      }
    }
	if ((p0->m_nObjectType == BOSSBULLATTACKRIGHT_OBJECT) && (p1->m_nObjectType == GATOR_OBJECT)) {
		if (p0->MyHitboxes[0]->intersect(p1->MyHitboxes[1]->getUL()) || p0->MyHitboxes[0]->intersect(p1->MyHitboxes[1]->getUR())
			|| p0->MyHitboxes[0]->intersect(p1->MyHitboxes[1]->getLL()) || p0->MyHitboxes[0]->intersect(p1->MyHitboxes[1]->getLR())
			|| p1->MyHitboxes[1]->intersect(p0->MyHitboxes[0]->getUL()) || p1->MyHitboxes[1]->intersect(p0->MyHitboxes[0]->getUR())
			|| p1->MyHitboxes[1]->intersect(p0->MyHitboxes[0]->getLL()) || p1->MyHitboxes[1]->intersect(p0->MyHitboxes[0]->getLR())) {
			Collision2 = TRUE;
		}
		if (Collision || Collision2) {//If the bullet hits either hitbox
			p1->takeHit(10);
			p0->m_bIsDead = TRUE;
			g_pSoundManager->play(2);
			p1->m_vVelocity.x = p1->m_vVelocity.x + 35.0f;// Alfredo - Push back if attacked
			LoseALife(p1);//Checks is Gator lost a life
			CreateNextIncarnation(p1);
		}
		if (Test) {//Move this around to test different collisions
			createObject(MARKER_OBJECT, "marker", p1->MyHitboxes[1]->getUL(), Vector3(0, 0, 0));
			createObject(MARKER_OBJECT, "marker", p1->MyHitboxes[1]->getUR(), Vector3(0, 0, 0));
			createObject(MARKER_OBJECT, "marker", p1->MyHitboxes[1]->getLL(), Vector3(0, 0, 0));
			createObject(MARKER_OBJECT, "marker", p1->MyHitboxes[1]->getLR(), Vector3(0, 0, 0));
		}
	}
    //Gator Collision against other objects
    if ((p0->m_nObjectType == GATOR_OBJECT || p0->m_nObjectType == GORILLA_OBJECT 
      || p0->m_nObjectType == DOG_OBJECT || p0->m_nObjectType == HORSE_OBJECT 
      || p0->m_nObjectType == RHINO_OBJECT || p0->m_nObjectType == BOSSBULL_OBJECT) 
      && (p1->m_nObjectType == ENDWALL_OBJECT || p1->m_nObjectType == ELEVATORWALL_OBJECT || p1->m_nObjectType == BLOCK_OBJECT)) {//If gator touches an endwall, bump back
      p0->m_vVelocity = 0 * p0->m_vVelocity; //Bounce off the Wall
     if (xfl >= 0) {//Left Wall
        p0->m_vPos.x += 2.0f;//Move them back just a tiny bit
      }
      else {//Right Wall
        p0->m_vPos.x -= 2.0f;//Move them back just a tiny bit
      }
      if (yfl >= 0) {//Above
        p0->m_vPos.y += 2.0f;//Move them back just a tiny bit
      }
      else {//Below
        p0->m_vPos.y -= 2.0f;//Move them back just a tiny bit
      }
   }//Endwall Colli
    if (p0->m_nObjectType == GATOR_OBJECT && p1->m_nObjectType == POTTEDPLANT_OBJECT) {//If Gator Touches a potted plant
      p0->m_vVelocity = Vector3(0,0,0);
      p0->m_vVelocity = -1.0f * p0->m_vVelocity; //Bounce off the Wall
      if ((p1->m_vPos.x - p0->m_vPos.x) >= 0) {//Horizontal Push
        p0->m_vPos.x -= 3.0f;
      }
      else {
        p0->m_vPos.x += 3.0f;
      }
      if ((p1->m_vPos.y - p0->m_vPos.y) >= 0) {//Vertical Push
        p0->m_vPos.y -= 2.0f;
      }
      else {
        p0->m_vPos.y += 2.0f;
      }
    }//Plant Colli
    if (p0->m_nObjectType == GATOR_OBJECT && p1->m_nObjectType == DESKHORIZONTAL_OBJECT) {//If gator touches a Desk, bump back
      p0->m_vVelocity = 0.0f * p0->m_vVelocity; //Bounce off the Wall
      if ((p1->m_vPos.x - p0->m_vPos.x) >= 0) {//Horizontal Push
        p0->m_vPos.x -= 3.0f;
      }
      else {
        p0->m_vPos.x += 3.0f;
      }
      if ((p1->m_vPos.y - p0->m_vPos.y) >= 0) {//Vertical Push
        p0->m_vPos.y -= 2.0f;
      }
      else {
        p0->m_vPos.y += 2.0f;
      }
      
    }//Endwall Colli
    if (p0->m_nObjectType == GATOR_OBJECT && p1->m_nObjectType == ELEVATOREN_OBJECT) {//If gator touches a the goal, level complete
      g_pSoundManager->play(9);
      levelComplete();
    }//Goal Colli
    if (p0->m_nObjectType == GATOR_OBJECT && p1->m_nObjectType == HEALTHSODA_OBJECT) {//If gator touches a soda get health
        if (p0->m_nHealth > 195) { //Alfredo - Gator health cap. Limit's Gator's health to 150 
          g_pSoundManager->play(7);
          p0->m_nHealth = 200;
        }
        else {
          g_pSoundManager->play(7);
          p0->giveHealth(5); //Alfredo give health to gator
        }
        p1->m_bIsDead = TRUE;
    }//HealthSoda Colli
    if (p0->m_nObjectType == GATOR_OBJECT && p1->m_nObjectType == STRENGTHSODA_OBJECT) {//If gator touches a soda, one punch hit enemies
        p1->m_bIsDead = TRUE;
        PUTimer += 20;//Add 20 seconds to PU Timer
        gatorStrong = TRUE;
        PowerTime = g_cTimer.time();
    }//StrongSoda Colli
    if (p0->m_nObjectType == GATOR_OBJECT && p1->m_nObjectType == MONEYBAG_OBJECT) {
      g_pSoundManager->play(12);
      p1->m_bIsDead = TRUE;
      score += 100;
    }

  }//Collision
}//HitboxCollision

 ///Luis-Game Reset
 ///This function resets the game
void CObjectManager::GameReset() {
  std::unordered_map<std::string, CGameObject*>::iterator gatorIterator =
    m_stlNameToObject.find((std::string)"gator");
  if (gatorIterator == m_stlNameToObject.end())return; //this should of course never happen
  CGameObject* gatorObject = gatorIterator->second;
  floor = 0;//Luis-This int will keep track of what floor the player is on
  lives = 3;//Luis-Number of lives
  score = 0;//Luis-The current score
  PUTimer = 0;//Luis-Power-Up Timer
  levelTime = 0;//Luis-When the level starts
  cleanEnemies();//Cleaning out old enemies
  cleanTiles();//Clearning out current floor
  BB_RLG(floor, 1);//Generates new level
  gatorObject->m_nHealth = 100;//Luis-reset health
  isDowned = FALSE;//Luis-This keeps track of wheter Gator is downed(lost a life)
}//Game Reset

///Luis-Lose A Life
///This function will handle what happens when the player's health reaches 0
///Param An object pointer to the object who has a health concern
void CObjectManager::LoseALife(CGameObject* p0) {
  string buffer;
  if (p0->m_nObjectType == GATOR_OBJECT) {
    if(p0->m_nHealth <=0){
      isDowned = TRUE;//The Gator is down
      lives--;//Minus one life
      if (lives > 0) {//If there are any lives left
        isDowned = FALSE;//Gets back up
        p0->m_nHealth = 100;//Set Health to a hundred
      }
      else {
        m_nGameState = GAMEOVER_STATE;//Luis-Game Over Sucka!
      }
    }//If
  }//If

}//LoseALife

//Luis-Powered Up
//This Function will keep track of the power ups
void CObjectManager::PoweredUp(int n) {
  //Set m_bStrong true for 30 seconds
  if (g_cTimer.elapsed(PowerTime, 1000)) { // Test
    PUTimer -= 1;
    PowerTime = g_cTimer.time();
	  g_pSoundManager->play(22);
  }
  if (PUTimer == 0) {
    gatorStrong = FALSE;
  }
}//Powered Up

///Luis-Level Complete
///This will clean up an old level and set up a new one
void CObjectManager::levelComplete() {
  int time;//Keeps track of the time
  if (g_cTimer.elapsed(levelTime, 1000)) {//The level can't be beat within 1 second of play, prevents glitching around
    floor++;
    cleanTiles();//Luis-Cleans out old tiles
    cleanEnemies();//Luis-Cleans out old enemies
    BB_RLG(floor, 1);//Generates new level
    }
  levelTime = g_cTimer.time();
}

/// Collect garbage, that is, remove dead objects from the object list.

void CObjectManager::GarbageCollect(){
  auto i = m_stlObjectList.begin();
  while(i != m_stlObjectList.end()){
    CGameObject* p = *i; //save pointer to object temporarily
    if(p->m_bIsDead){
      i = m_stlObjectList.erase(i); //remove pointer from list
      delete p; //delete object
    } //if
    if(i != m_stlObjectList.end())
      ++i; //next object
  } //while
} //GarbageCollect