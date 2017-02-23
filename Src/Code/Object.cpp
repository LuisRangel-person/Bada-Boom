/// \file object.cpp
/// \brief Code for the game object class CGameObject.

#include "object.h"
#include<ctime>

#include "defines.h" 
#include "timer.h"
#include "debug.h" 
#include "spriteman.h" 
#include "ObjMan.h"
#include "sound.h"
#include "random.h"

extern CSoundManager* g_pSoundManager;
extern CObjectManager g_cObjectManager; //alfredo- added to see if bullets work 

extern CRandom g_cRandom; //random number generator
extern CTimer g_cTimer;
extern int g_nScreenWidth;
extern int g_nScreenHeight;
extern XMLElement* g_xmlSettings;
extern CSpriteManager g_cSpriteManager;

/// Initialize a game object. Gets object-dependent settings from g_xmlSettings
/// from the "object" tag that has the same "name" attribute as parameter name.
/// Assumes that the sprite manager has loaded the sprites already.
/// \param object Object type
/// \param name Object name in XML settings file object tag
/// \param s Initial location of object
/// \param v Initial velocity

CGameObject::CGameObject(ObjectType object, const char* name, const Vector3& s, const Vector3& v){ 
  //defaults
  m_nCurrentFrame = 0; 
  m_nLastFrameTime = g_cTimer.time();
  m_nFrameInterval = 30; 
  
  m_bfaceRight = TRUE;//Luis-Object is not facing right
  
  m_nLifeTime = -1; //negative means immortal
  m_bVulnerable = FALSE; 
  m_bIntelligent = FALSE;
  m_bIsDead = FALSE;

  m_nHealth = 100;//Alfredo- Adding health to objects, default a three for testing, later to be added as a parameter
  m_bAttacking = FALSE;

  m_bStrong = FALSE; //Alfredo - True when gator gets strength soda
  m_bIsChampion = FALSE; //True when champions spawn

  m_bCanFly = FALSE;
  m_nBounceCount = 0;
  m_bGrounded = FALSE;

  m_nMinXSpeed = -20; 
  m_nMaxXSpeed = 20;
  m_nMinYSpeed = -20; 
  m_nMaxYSpeed = 20;

  m_pAnimation = nullptr;
  m_nAnimationFrameCount = 0;
  m_bCycleSprite = TRUE;

  //common values
  m_nObjectType = object; //type of object

  m_pSprite = g_cSpriteManager.GetSprite(object); //sprite pointer
  if(m_pSprite){
    m_nFrameCount = m_pSprite->m_nFrameCount; //get frame count
    m_nHeight = m_pSprite->m_nHeight; //get object height from sprite
    m_nWidth = m_pSprite->m_nWidth; //get object width from sprite
  } //if

  m_nLastMoveTime = g_cTimer.time(); //time
  m_nBirthTime = g_cTimer.time(); //time of creation
  m_vPos = s; //location
  m_vVelocity = v;  //velocity
  MyHitboxes[0] = new Hitbox(m_nHeight, m_nWidth, m_vPos);//Setting up an object's hitbox
  
  //object-dependent settings loaded from XML
  MorphHitBox();//Changes the Hitbox according to Object Type
  LoadSettings(name);
} //constructor

CGameObject::~CGameObject(){
  delete[] m_pAnimation;
  delete MyHitboxes[0];
} //destructor

//Luis-QuickFunction to change what direction the object is facing
void CGameObject::turnFace(BOOL direction) {
  m_bfaceRight = direction;
}

//Luis-Quick function to see which way the object is facing
BOOL CGameObject::getFace() {
  return m_bfaceRight;
}

//Luis- Zshift
//This function will change an objects z value to make things in front of gator drawn in front of him
void CGameObject::zShift() {
  //CGameObject* pgator = g_cObjectManager.GetObjectByName("gator");//Getting the gator's position
  //pgator->m_vPos.z = m_vPos.y - 48.0f;//Luis-598 so he can't go behind the wall, lowers his z value the lower he is on the screen
  if (m_nLifeTime != -1000) {
    m_vPos.z = m_vPos.y - 48.0f;//Luis-598 so he can't go behind the wall, lowers his z value the lower he is on the screen
  }
}

//Luis-This function will allow an object to modify it's hitbox depending on the kind of object it is
//Param // Which hitbox to morph
void CGameObject::MorphHitBox() {
  if (m_nObjectType == GATOR_OBJECT) {//Gators top half shouldn't collide with furniture, but he should be able to be punch from any place
    MyHitboxes[0]->RedefineHitbox(55.0f, 86.0f, Vector3(m_vPos.x - 6.0, m_vPos.y - 70.0f, m_vPos.z));//Bottom Hitbox
    if (MyHitboxes[1] == NULL) {//If Top Hitbox doesn't exist
      MyHitboxes[1] = new Hitbox(120.0f, 86.0f, Vector3(m_vPos.x, m_vPos.y + 30.0f, m_vPos.z));
    }
    else {//If Top HitBox exists
      MyHitboxes[1]->RedefineHitbox(120.0f, 86.0f, Vector3(m_vPos.x, m_vPos.y + 30.0f, m_vPos.z));//BTop Hitbox
    }
  }//Gator
  if (m_nObjectType == POTTEDPLANT_OBJECT) {//The characters should collide with the bottom of the sprite, not the top, tis a plant
    MyHitboxes[0]->RedefineHitbox(20.0f, 30.0f, Vector3(m_vPos.x, m_vPos.y - 47.0f, m_vPos.z));
  }//Potted Plant
  if (m_nObjectType == DESKHORIZONTAL_OBJECT) {//The characters should collide with the bottom of the desk but be able to walk behind it
    MyHitboxes[0]->RedefineHitbox(40.0f, 200.0f, Vector3(m_vPos.x, m_vPos.y - 55.0f, m_vPos.z));
  }//Hori Desk
  if (m_nObjectType == ELEVATOREN_OBJECT) {//The characters should collide with the bottom of the desk but be able to walk behind it
    MyHitboxes[0]->RedefineHitbox(20.0f, 20.0f, m_vPos);
  }//End Goal
  if (m_nObjectType == BULLET_OBJECT) {//Bullets should not be small
    MyHitboxes[0]->RedefineHitbox(100.0f, 100.0f, Vector3(m_vPos.x, m_vPos.y - 55.0f, m_vPos.z));
  }//Bullet
  if (m_nObjectType == GORILLA_OBJECT) {//Gorilla is too easy to hit
    MyHitboxes[0]->RedefineHitbox(100.0f, 120.0f, m_vPos);
  }//End Goal
}

/// Draw the current sprite frame at the current position, then
/// compute which frame is to be drawn next time.

void CGameObject::draw(){
  if(m_pSprite == nullptr)return;
  if(m_bIsDead)return; //bail if already dead

  int t = m_nFrameInterval;
  if(m_bCycleSprite)
    t = (int)(t/(1.5f + fabs(m_vVelocity.x)));

  if(m_pAnimation != nullptr){ //if there's an animation sequence
    //draw current frame
    m_pSprite->Draw(m_vPos, m_fOrientation, m_pAnimation[m_nCurrentFrame]);
    //advance to next frame
    if(g_cTimer.elapsed(m_nLastFrameTime, t)) //if enough time passed
      //increment and loop if necessary
      if(++m_nCurrentFrame >= m_nAnimationFrameCount && m_bCycleSprite) 
        m_nCurrentFrame = 0;
  } //if
  else 
    m_pSprite->Draw(m_vPos, 0.0f); //assume only one frame, Luis-Mess m_fOrientation instead of 0.0f
} //draw

/// Load settings for object from g_xmlSettings.
/// \param name name of object as found in name tag of XML settings file

void CGameObject::LoadSettings(const char* name){
  if(g_xmlSettings){ //got "settings" tag
    //get "objects" tag
    XMLElement* objSettings =
      g_xmlSettings->FirstChildElement("objects"); //objects tag

    if(objSettings){ //got "objects" tag
      //set obj to the first "object" tag with the correct name
      XMLElement* obj = objSettings->FirstChildElement("object");
      while(obj && strcmp(name, obj->Attribute("name"))){
        obj = obj->NextSiblingElement("object");
      } //while

      if(obj){ //got "object" tag with right name
        //get object information from tag
        m_nMinXSpeed = obj->IntAttribute("minxspeed");
        m_nMaxXSpeed = obj->IntAttribute("maxxspeed");
        m_nMinYSpeed = obj->IntAttribute("minyspeed");
        m_nMaxYSpeed = obj->IntAttribute("maxyspeed");
        m_nFrameInterval = obj->IntAttribute("frameinterval");
        m_bVulnerable = obj->BoolAttribute("vulnerable");
        m_bCanFly = obj->BoolAttribute("fly");
        m_bCycleSprite = obj->BoolAttribute("cycle");
        m_nLifeTime = obj->IntAttribute("lifetime");   
        m_bGrounded = obj->BoolAttribute("grounded"); 

        //parse animation sequence
        if(obj->Attribute("animation")){ //sequence present

          //get sequence length
          size_t length = strlen(obj->Attribute("animation"));
          m_nAnimationFrameCount = 1; //one more than number of commas
          for(size_t i = 0; i<length; i++) //for each character
          if(obj->Attribute("animation")[i] == ',')
            m_nAnimationFrameCount++; //count commas

          m_pAnimation = new int[m_nAnimationFrameCount]; //memory for animation sequence

          size_t i = 0; //character index
          int count = 0; //number of frame numbers input
          int num; //frame number
          char c = obj->Attribute("animation")[i]; //character in sequence string
          while(i < length){
            //get next frame number
            num = 0;
            while(i<length && c >= '0' && c <= '9'){
              num = num * 10 + c - '0';
              c = obj->Attribute("animation")[++i];
            }
            //process frame number
            c = obj->Attribute("animation")[++i]; //skip over comma
            m_pAnimation[count++] = num; //record frame number
          } //while
        } //if
      } //if
    } //if
  } //if
} //LoadSettings

/// Luis-UpdateSprite
/// This function will change an object's sprite when called
/// Param-Object Type Representing new Sprite
void CGameObject::UpdateSprite(ObjectType neo) {
  m_pSprite = g_cSpriteManager.GetSprite(neo); //sprite pointer
  if (m_pSprite) {
    m_nFrameCount = m_pSprite->m_nFrameCount; //get frame count
    m_nHeight = m_pSprite->m_nHeight; //get object height from sprite
    m_nWidth = m_pSprite->m_nWidth; //get object width from sprite
  } //if
}
 
/// The distance that an object moves depends on its speed, 
/// and the amount of time since it last moved.

void CGameObject::move(){ //move object 
  const float SCALE = 32.0f; //to scale back motion
  const float MARGIN = 120.0f; //margin on top of page, mess was 120.0
  const float GRAVITY = 9.8f; //gravity

  const int time = g_cTimer.time(); //current time
  const int tdelta = time - m_nLastMoveTime; //time since last move
  const float tfactor = tdelta/SCALE; //scaled time factor
  //change in vertical speed due to gravity
  if(!m_bCanFly && !m_bGrounded && m_nLifeTime != -1000) //falling objects
    m_vVelocity.y -= tdelta/GRAVITY; //fall
  
   m_vPos += m_vVelocity*tfactor; //motion

  //limit top
  if(m_bCanFly && (m_nObjectType != BULLET_OBJECT || m_nObjectType != CROW_OBJECT))//Luis-Bullets and Crows can Fly
    if(m_vPos.y > (float)(g_nScreenHeight - MARGIN)){
      m_vPos.y = (float)(g_nScreenHeight - MARGIN - 1);
      m_vVelocity.y = 0.0f; 
    } //if
 
  //hitting the ground
  if(!m_bGrounded && m_vPos.y <= m_nHeight/2.0f + 85.0f && m_nLifeTime != -1000) //MOD, Luis-Added an offset to make the "ground" higher
    if(m_bCanFly){ //limit flying objects to bottom of screen
      m_vPos.y = m_nHeight/2.0f + 86.0f; //put a little above ground
      if (m_nObjectType == GATOR_OBJECT) {//If the object is a gator
        m_vVelocity.y = 0.0; //Stay Put
      }
      else {
        m_vVelocity.y = -0.2f*m_vVelocity.y; //Bounce off the Wall
      }
    } //if
    else{ //can't fly, meaning it is falling, so it bounces off the ground
      const int MAXBOUNCES = 3;
      m_nBirthTime = time; //death time should be from now
      if(m_nBounceCount++ < MAXBOUNCES && fabs(m_vVelocity.y) > 10.0f){ //let it bounce
         m_vPos.y = m_nHeight/2.0f + 106.0f; //put a little above ground
         m_vVelocity.y *= -0.6f; //rebound speed
         m_vVelocity.x *= 0.5f;  //fake friction
       } //if
       else{ //stop bouncing
         m_bCanFly = TRUE; //prevent from falling, switches gravity off
         m_vVelocity = Vector3(0.0f); //stop
         m_nLifeTime = 5000; //force cull after a short period
       } //else
     } //else
 
  //Moving the Hitboxes
  MyHitboxes[0]->UpdateHitbox(m_vPos);//Move the hitbox with the object, every object will have at least this hitbox
  if (MyHitboxes[1] != NULL) {
    MyHitboxes[1]->UpdateHitbox(m_vPos);//Move the hitbox with the object
  }
  if (MyHitboxes[2] != NULL) {
    MyHitboxes[2]->UpdateHitbox(m_vPos);//Move the hitbox with the object
  }
  if (MyHitboxes[3] != NULL) {
    MyHitboxes[3]->UpdateHitbox(m_vPos);//Move the hitbox with the object
  }
  if (MyHitboxes[4] != NULL) {
    MyHitboxes[4]->UpdateHitbox(m_vPos);//Move the hitbox with the object
  }
  if (m_nObjectType != PUNCH_OBJECT && m_nObjectType != PUNCHLEFT_OBJECT  
    && m_nObjectType != PUNCHANIMATIONFIRELEFT_OBJECT && m_nObjectType != PUNCHANIMATIONFIRERIGHT_OBJECT 
    && m_nObjectType != PUNCHANIMATIONLEFT_OBJECT && m_nObjectType != PUNCHANIMATIONRIGHT_OBJECT  
    && m_nObjectType != HEALTHSODA_OBJECT && m_nObjectType != STRENGTHSODA_OBJECT 
    && m_nObjectType != MONEYBAG_OBJECT && m_nObjectType != DESKEXPLOSION_OBJECT
    && m_nObjectType != POTTEDPLANTEXPLOSION_OBJECT && m_nObjectType != WATERCOOLEREXPLOSION_OBJECT) {//Looks weird with power ups
    zShift();//Making the z value what it should be for it to look correct
  }
  MorphHitBox();//Move Hitboxes to correct place
  m_nLastMoveTime = time;  //record time of move
} //move

/// Change the object's speed, and check against speed limits to make
/// sure that the object doesn't break them.
/// \param a Acceleration vector.

void CGameObject::accelerate(const Vector3& a){  
  m_vVelocity += a; //change velocity

  //check bounds
  m_vVelocity.x = min(m_vVelocity.x, m_nMaxXSpeed);
  m_vVelocity.x = max(m_vVelocity.x, m_nMinXSpeed);
  m_vVelocity.y = min(m_vVelocity.y, m_nMaxYSpeed);
  m_vVelocity.y = max(m_vVelocity.y, m_nMinYSpeed);
} //accelerate

  ///Luis-Changes the velocity to make the character move normally instead of 
  ///accelerating. 
  ///param:The new walking speed/direction

void CGameObject::walk(const Vector3& a) {
  m_vVelocity = a; //Luis-Sets velocity
  //check bounds
  m_vVelocity.x = min(m_vVelocity.x, m_nMaxXSpeed);
  m_vVelocity.x = max(m_vVelocity.x, m_nMinXSpeed);
  m_vVelocity.y = min(m_vVelocity.y, m_nMaxYSpeed);
  m_vVelocity.y = max(m_vVelocity.y, m_nMinYSpeed);
} //walk

///Luis-Teleport
///Changes the postition of an object instantly
///param;The new location
void CGameObject::teleport(Vector3& a) {
  m_vPos = a;//TELEPORT!!!
}

/// Alfredo- function that deals damage to an object's health
/// param; int damage value that will be subtracted from health
void CGameObject::takeHit(int damage) {
  m_nHealth = m_nHealth - damage; 
}

/// Alfredo- function that adds health to an object 
/// param; int health value that will be added to health
void CGameObject::giveHealth(int health) {
  m_nHealth = m_nHealth + health;
}


///Alfredo- Dash
///Make the character dash in a direction 
void CGameObject::dash(bool LPre, bool RPre, bool UPre, bool DPre) {

        if (UPre && LPre && !RPre) {//Up+Left
          m_vVelocity = (Vector3(-5.0f, 5.0f, 0));
        }
        else if (UPre && RPre && !LPre) {//Up+Right 
          m_vVelocity = (Vector3(5.0f, 5.0f, 0));
        }
        else if (UPre && !RPre && !LPre) {//Just Up
          m_vVelocity = (Vector3(0.0f, 5.0f, 0));
        }
        if (DPre && LPre && !RPre) {//Down+Left
          m_vVelocity = (Vector3(-5.0f, -5.0f, 0));
        }
        else if (DPre && RPre && !LPre) {//Down+Right 
          m_vVelocity = (Vector3(5.0f, -5.0f, 0));
        }
        else if (DPre && !RPre && !LPre) {//Just Down
          m_vVelocity = (Vector3(0.0f, -5.0f, 0));
        }
        else if (LPre && UPre && !DPre) {//Left+Up
          m_vVelocity = (Vector3(-5.0f, 5.0f, 0));
        }
        else if (LPre && DPre && !UPre) {//Left+Down 
          m_vVelocity = (Vector3(-5.0f, -5.0f, 0));
        }
        else if (LPre && !UPre && !DPre) {//Just Left
          m_vVelocity = (Vector3(-5.0f, 0.0f, 0));
        }
        else if (RPre && UPre && !DPre) {//Right+Up
          m_vVelocity = (Vector3(5.0f, 5.0f, 0));
        }
        else if (RPre && DPre && !UPre) {//Right+Down 
          m_vVelocity = (Vector3(5.0f, -5.0f, 0));
        }
        else if (RPre && !DPre && !UPre) {//Just Right
          m_vVelocity = (Vector3(5.0f, 0.0f, 0));
        }
        else
          return; 

        m_vVelocity = m_vVelocity * 10;
        //check bounds
        m_vVelocity.x = min(m_vVelocity.x, m_nMaxXSpeed);
        m_vVelocity.x = max(m_vVelocity.x, m_nMinXSpeed);
        m_vVelocity.y = min(m_vVelocity.y, m_nMaxYSpeed);
        m_vVelocity.y = max(m_vVelocity.y, m_nMinYSpeed);
        MyHitboxes[0]->UpdateHitbox(m_vPos);//Move the hitbox with the object
        return; 
}

void CGameObject::enemyAttack(char* enemy) {
  if (enemy == "gorilla" || enemy == "dog" || enemy == "horse" || enemy == "rhino" || enemy == "championdog" || enemy == "championgorilla" || enemy == "bossbullleft" || enemy == "bossbullright") {
    if (enemy == "dog" || enemy == "championdog") {
      g_pSoundManager->play(6);
    }
    if (enemy == "gorilla") {
		if(g_cRandom.number(0,1) == 0)
		  g_pSoundManager->play(11);
    }
    if (enemy == "horse") {
      g_pSoundManager->play(15);
    }
    // if (g_cTimer.elapsed(m_nLastGunFireTime, 200)) { //slow down firing rate
    const float fAngle = m_fOrientation;
    const float fSine = sin(fAngle);
    const float fCosine = cos(fAngle);

    //enter the number of pixels from center of gator to gun
    const float fGunDx = -30.0f;
    const float fGunDy = -17.0f;

    //initial bullet position
    const Vector3 s = m_vPos +
      Vector3(fGunDx*fCosine - fGunDy*fSine, fGunDx*fSine - fGunDy*fCosine, -1);

    //velocity of bullet
    float BULLETSPEED = -10.0f;
    if (m_bfaceLeft)
      BULLETSPEED = 10.0f;
    else
      BULLETSPEED = -10.0f;
    const Vector3 v = BULLETSPEED * Vector3(-fCosine, -fSine, -1) + m_vVelocity;
	
	if(enemy == "gorilla" || enemy == "dog")
		g_cObjectManager.createObject(BULLET_OBJECT, "bullet", s, v); //create bullet
	if(enemy == "horse" || enemy == "rhino" || enemy == "championdog")
		g_cObjectManager.createObject(HORSEATTACK_OBJECT, "horseattack", s, v);
    if(enemy == "bossbullleft")
		g_cObjectManager.createObject(BOSSBULLATTACKLEFT_OBJECT, "bossbullattackleft", s, v);
    if (enemy == "bossbullright")
		g_cObjectManager.createObject(BOSSBULLATTACKRIGHT_OBJECT, "bossbullattackright", s, v);
    // } //if
  }//if

}
