/// \file main.cpp 
/// Apadpted from the Deno 6 code written by Dr. Ian Parberry
/// This game uses the Turkeyberry Engine
/// Bada-Boom Code added by Luis Rangel and Alfredo de Leon
/// RLG added on 10-16-2016
#include <windows.h>
#include <windowsx.h>

#include "defines.h"
#include "abort.h"
#include "gamerenderer.h"
#include "imagefilenamelist.h"
#include "debug.h"
#include "timer.h"
#include "sprite.h"
#include "object.h"
#include "spriteman.h"
#include "objman.h"
#include "Random.h"
#include "BB_RLG.h"//Luis-This is the Random Level Generator File
#include "sound.h"
#include <XInput.h>
#include "CXBOXController.h"

CXBOXController* Player1;



CSoundManager* g_pSoundManager; //Alfredo- Sound manager declaration

//globals
BOOL g_bActiveApp;  ///< TRUE if this is the active application
HWND g_HwndApp; ///< Application window handle.
HINSTANCE g_hInstance; ///< Application instance handle.
char g_szGameName[256]; ///< Name of this game
char g_szShaderModel[256]; ///< The shader model version used
CImageFileNameList g_cImageFileName; ///< List of image file names.
CTimer g_cTimer; ///< The game timer.
CSpriteManager g_cSpriteManager; ///< The sprite manager.
CObjectManager g_cObjectManager; ///< The object manager.
CRandom g_cRandom; ///< The random number generator.
static int currfloor = 1;//Luis-For testing RLG
BOOL LPre = FALSE, RPre = FALSE, UPre = FALSE, DPre = FALSE;//Luis-These Bools will allow diagonal movement
BOOL Dash = FALSE; 
//graphics settings
int g_nScreenWidth; ///< Screen width.
int g_nScreenHeight; ///< Screen height.
BOOL g_bWireFrame = FALSE; ///< TRUE for wireframe rendering.
GameState m_nGameState = STARTSCREEN_STATE;//Luis-This is to keep track of what state the game is in

//XML settings
tinyxml2::XMLDocument g_xmlDocument; ///< TinyXML document for settings.
XMLElement* g_xmlSettings = nullptr; ///< TinyXML element for settings tag.

//debug variables
#ifdef DEBUG_ON
  CDebugManager g_cDebugManager; ///< The debug manager.
#endif //DEBUG_ON

CGameRenderer GameRenderer; ///< The game renderer.

//functions in Window.cpp
void InitGraphics();
HWND CreateDefaultWindow(char* name, HINSTANCE hInstance, int nCmdShow);

/// \brief Initialize XML settings.
///
/// Open an XML file and prepare to read settings from it. Settings
/// tag is loaded to XML element g_xmlSettings for later processing. Abort if it
/// cannot load the file or cannot find settings tag in loaded file.

void InitXMLSettings(){
  //open and load XML file
  const char* xmlFileName = "gamesettings.xml"; //Settings file name.
  if(g_xmlDocument.LoadFile(xmlFileName) != 0)
    ABORT("Cannot load settings file %s.", xmlFileName);

  //get settings tag
  g_xmlSettings = g_xmlDocument.FirstChildElement("settings"); //settings tag
  if(g_xmlSettings == nullptr) //abort if tag not found
    ABORT("Cannot find <settings> tag in %s.", xmlFileName);
} //InitXMLSettings

/// \brief Load game settings.
///
/// Load and parse game settings from a TinyXML element g_xmlSettings.

void LoadGameSettings(){
  if(!g_xmlSettings)return; //bail and fail

  //get game name
  XMLElement* ist = g_xmlSettings->FirstChildElement("game"); 
  if(ist){
    size_t len = strlen(ist->Attribute("name")); //length of name string
    strncpy_s(g_szGameName, len+1, ist->Attribute("name"), len); 
  } //if

  //get renderer settings
  XMLElement* renderSettings = 
    g_xmlSettings->FirstChildElement("renderer"); //renderer tag
  if(renderSettings){ //read renderer tag attributes
    g_nScreenWidth = renderSettings->IntAttribute("width");
    g_nScreenHeight = renderSettings->IntAttribute("height");

    size_t len = strlen(renderSettings->Attribute("shadermodel")); //length shader model string
    strncpy_s(g_szShaderModel, len + 1, renderSettings->Attribute("shadermodel"), len);
  } //if

  //get image file names
  g_cImageFileName.GetImageFileNames(g_xmlSettings);

  //get debug settings
  #ifdef DEBUG_ON
    g_cDebugManager.GetDebugSettings(g_xmlSettings);
  #endif //DEBUG_ON
} //LoadGameSettings

/// \brief Create game objects. 
///
/// Create a gator and a collection of crows at hard-coded positions.

void CreateObjects(){
  //Luis-The Gator
  const int maxZ = 375;
  g_cObjectManager.createObject(GATOR_OBJECT, "gator",
    Vector3(-3500.0f, 300.0f, (float)maxZ), Vector3(0, 0, 0));
  g_cObjectManager.InsertObjectType("logo", LOGO_OBJECT);
  g_cObjectManager.InsertObjectType("gameover", GAMEOVER_OBJECT);
  g_cObjectManager.InsertObjectType("bossbullscreen", BOSSBULLSCREEN_OBJECT);
  g_cObjectManager.InsertObjectType("bossbullwinscreen", BOSSBULLWINSCREEN_OBJECT);
  g_cObjectManager.InsertObjectType("gator", GATOR_OBJECT);
  g_cObjectManager.InsertObjectType("bullet", BULLET_OBJECT);
  g_cObjectManager.InsertObjectType("horseattack", HORSEATTACK_OBJECT);
  g_cObjectManager.InsertObjectType("bossbullattackleft", BOSSBULLATTACKLEFT_OBJECT);
  g_cObjectManager.InsertObjectType("bossbullattackright", BOSSBULLATTACKRIGHT_OBJECT);
  //Crow Objects
  g_cObjectManager.InsertObjectType("crow", CROW_OBJECT);
  g_cObjectManager.InsertObjectType("exploding crow", EXPLODINGCROW_OBJECT);
  g_cObjectManager.InsertObjectType("dead crow", DEADCROW_OBJECT);
  //Gator Object Insert
  g_cObjectManager.InsertObjectType("gatorwalkleft", GATORWALKLEFT_OBJECT);
  g_cObjectManager.InsertObjectType("gatorwalkright", GATORWALKRIGHT_OBJECT);
  g_cObjectManager.InsertObjectType("gatorleft", GATORLEFT_OBJECT);
  g_cObjectManager.InsertObjectType("gatorcheer", GATORCHEER_OBJECT);
  g_cObjectManager.InsertObjectType("gatorcheerleft", GATORCHEERLEFT_OBJECT);
  g_cObjectManager.InsertObjectType("gatorhurt", GATORHURT_OBJECT);
  g_cObjectManager.InsertObjectType("gatorhurtleft", GATORHURTLEFT_OBJECT);
  //Alfredo- Gorilla insert
  g_cObjectManager.InsertObjectType("gorilla", GORILLA_OBJECT); 
  g_cObjectManager.InsertObjectType("gorilladownedleft", GORILLADOWNEDLEFT_OBJECT);
  g_cObjectManager.InsertObjectType("gorilladownedright", GORILLADOWNEDRIGHT_OBJECT);
  g_cObjectManager.InsertObjectType("deadgorilla", DEAD_GORILLA_OBJECT);
  g_cObjectManager.InsertObjectType("madgorillaleft", MADGORILLALEFT_OBJECT);
  g_cObjectManager.InsertObjectType("madgorillaright", MADGORILLARIGHT_OBJECT);
  g_cObjectManager.InsertObjectType("gorillaattackleft", GORILLAATTACKLEFT_OBJECT);
  g_cObjectManager.InsertObjectType("gorillaattackright", GORILLAATTACKRIGHT_OBJECT);
  g_cObjectManager.InsertObjectType("gorillawalkleft", GORILLAWALKLEFT_OBJECT);
  g_cObjectManager.InsertObjectType("gorillawalkright", GORILLAWALKRIGHT_OBJECT);
  g_cObjectManager.InsertObjectType("gorillablockleft", GORILLABLOCKLEFT_OBJECT);
  g_cObjectManager.InsertObjectType("gorillablockright", GORILLABLOCKRIGHT_OBJECT);
  //Alfredo- Champion Gorilla insert
  g_cObjectManager.InsertObjectType("championgorilla", CHAMPIONGORILLA_OBJECT);
  g_cObjectManager.InsertObjectType("championgorilladownedleft", CHAMPIONGORILLADOWNEDLEFT_OBJECT);
  g_cObjectManager.InsertObjectType("championgorilladownedright", CHAMPIONGORILLADOWNEDRIGHT_OBJECT);
  g_cObjectManager.InsertObjectType("championdeadgorilla", CHAMPIONDEAD_GORILLA_OBJECT);
  g_cObjectManager.InsertObjectType("championmadgorillaleft", CHAMPIONMADGORILLALEFT_OBJECT);
  g_cObjectManager.InsertObjectType("championmadgorillaright", CHAMPIONMADGORILLARIGHT_OBJECT);
  g_cObjectManager.InsertObjectType("championgorillaattackleft", CHAMPIONGORILLAATTACKLEFT_OBJECT);
  g_cObjectManager.InsertObjectType("championgorillaattackright", CHAMPIONGORILLAATTACKRIGHT_OBJECT);
  g_cObjectManager.InsertObjectType("championgorillawalkleft", CHAMPIONGORILLAWALKLEFT_OBJECT);
  g_cObjectManager.InsertObjectType("championgorillawalkright", CHAMPIONGORILLAWALKRIGHT_OBJECT);
  g_cObjectManager.InsertObjectType("championgorillablockleft", CHAMPIONGORILLABLOCKLEFT_OBJECT);
  g_cObjectManager.InsertObjectType("championgorillablockright", CHAMPIONGORILLABLOCKRIGHT_OBJECT);
  //Alfredo - Dog insert
  g_cObjectManager.InsertObjectType("dog", DOG_OBJECT);
  g_cObjectManager.InsertObjectType("dogwalkingleft", DOGWALKLEFT_OBJECT);
  g_cObjectManager.InsertObjectType("dogwalkingright", DOGWALKRIGHT_OBJECT);
  g_cObjectManager.InsertObjectType("dogmadleft", DOGMADLEFT_OBJECT);
  g_cObjectManager.InsertObjectType("dogmadright", DOGMADRIGHT_OBJECT);
  g_cObjectManager.InsertObjectType("dogdownedleft", DOGDOWNEDLEFT_OBJECT);
  g_cObjectManager.InsertObjectType("dogdownedright", DOGDOWNEDRIGHT_OBJECT);
  g_cObjectManager.InsertObjectType("dogdead", DOGDEAD_OBJECT);
  g_cObjectManager.InsertObjectType("dogattackleft", DOGATTACKLEFT_OBJECT);
  g_cObjectManager.InsertObjectType("dogattackright", DOGATTACKRIGHT_OBJECT);
  //Alfredo - Champion Dog insert
  g_cObjectManager.InsertObjectType("championdog", CHAMPIONDOG_OBJECT);
  g_cObjectManager.InsertObjectType("championdogwalkingleft", CHAMPIONDOGWALKLEFT_OBJECT);
  g_cObjectManager.InsertObjectType("championdogwalkingright", CHAMPIONDOGWALKRIGHT_OBJECT);
  g_cObjectManager.InsertObjectType("championdogmadleft", CHAMPIONDOGMADLEFT_OBJECT);
  g_cObjectManager.InsertObjectType("championdogmadright", CHAMPIONDOGMADRIGHT_OBJECT);
  g_cObjectManager.InsertObjectType("championdogdownedleft", CHAMPIONDOGDOWNEDLEFT_OBJECT);
  g_cObjectManager.InsertObjectType("championdogdownedright", CHAMPIONDOGDOWNEDRIGHT_OBJECT);
  g_cObjectManager.InsertObjectType("championdogdead", CHAMPIONDOGDEAD_OBJECT);
  g_cObjectManager.InsertObjectType("championdogattackleft", CHAMPIONDOGATTACKLEFT_OBJECT);
  g_cObjectManager.InsertObjectType("championdogattackright", CHAMPIONDOGATTACKRIGHT_OBJECT);
  //Alfredo - Horse Insert
  g_cObjectManager.InsertObjectType("horse", HORSE_OBJECT);
  g_cObjectManager.InsertObjectType("horsewalkleft", HORSEWALKLEFT_OBJECT);
  g_cObjectManager.InsertObjectType("horsewalkright", HORSEWALKRIGHT_OBJECT);
  g_cObjectManager.InsertObjectType("horsemadleft", HORSEMADLEFT_OBJECT);
  g_cObjectManager.InsertObjectType("horsemadright", HORSEMADRIGHT_OBJECT);
  g_cObjectManager.InsertObjectType("horsedead", HORSEDEAD_OBJECT);
  g_cObjectManager.InsertObjectType("horserunleft", HORSERUNLEFT_OBJECT);
  g_cObjectManager.InsertObjectType("horserunright", HORSERUNRIGHT_OBJECT);
  g_cObjectManager.InsertObjectType("horsestopleft", HORSESTOPLEFT_OBJECT);
  g_cObjectManager.InsertObjectType("horsestopright", HORSESTOPRIGHT_OBJECT);
  //Alfredo - Rhino Insert
  g_cObjectManager.InsertObjectType("rhino", RHINO_OBJECT);
  g_cObjectManager.InsertObjectType("rhinoattackleft", RHINOATTACKLEFT_OBEJCT);
  g_cObjectManager.InsertObjectType("rhinoattackright", RHINOATTACKRIGHT_OBJECT);
  g_cObjectManager.InsertObjectType("rhinodownedleft", RHINODOWNEDLEFT_OBJECT);
  g_cObjectManager.InsertObjectType("rhinodownedright", RHINODOWNEDRIGHT_OBJECT);
  g_cObjectManager.InsertObjectType("rhinohurtleft", RHINOHURTLEFT_OBJECT);
  g_cObjectManager.InsertObjectType("rhinohurtright", RHINOHURTRIGHT_OBJECT);
  g_cObjectManager.InsertObjectType("rhinorunleft", RHINORUNLEFT_OBJECT);
  g_cObjectManager.InsertObjectType("rhinorunright", RHINORUNRIGHT_OBJECT);
  g_cObjectManager.InsertObjectType("rhinodead", RHINODEAD_OBJECT);
  //Alfredo- Boss Bull Insert
  g_cObjectManager.InsertObjectType("bossbull", BOSSBULL_OBJECT);
  g_cObjectManager.InsertObjectType("bossbullleft", BOSSBULLLEFT_OBJECT);
  g_cObjectManager.InsertObjectType("bossbullchargeleft", BOSSBULLCHARGELEFT_OBJECT);
  g_cObjectManager.InsertObjectType("bossbullchargeright", BOSSBULLCHARGERIGHT_OBJECT);
  g_cObjectManager.InsertObjectType("bossbullhurtleft", BOSSBULLHURTLEFT_OBJECT);
  g_cObjectManager.InsertObjectType("bossbullhurtright", BOSSBULLHURTRIGHT_OBJECT);
  g_cObjectManager.InsertObjectType("bossbulldownedright", BOSSBULLDOWNEDRIGHT_OBJECT);
  g_cObjectManager.InsertObjectType("bossbulldownedleft", BOSSBULLDOWNEDLEFT_OBJECT);
  g_cObjectManager.InsertObjectType("bossbulldeadleft", BOSSBULLDEADLEFT_OBJECT);
  g_cObjectManager.InsertObjectType("bossbulldeadright", BOSSBULLDEADRIGHT_OBJECT);
  //Alfredo- Furniture insert
  g_cObjectManager.InsertObjectType("watercooler", WATERCOOLER_OBJECT);
  g_cObjectManager.InsertObjectType("pottedplant", POTTEDPLANT_OBJECT);
  g_cObjectManager.InsertObjectType("vendingmachine", VENDINGMACHINE_OBJECT);
  g_cObjectManager.InsertObjectType("deskhorizontal", DESKHORIZONTAL_OBJECT);
  //Alfredo - Furniture Explosion 
  g_cObjectManager.InsertObjectType("deskexplosion", DESKEXPLOSION_OBJECT);
  g_cObjectManager.InsertObjectType("watercoolerexplosion", WATERCOOLEREXPLOSION_OBJECT);
  g_cObjectManager.InsertObjectType("pottedplantexplosion", POTTEDPLANTEXPLOSION_OBJECT);
  //Alfredo - PowerUp Objects
  g_cObjectManager.InsertObjectType("healthsoda", HEALTHSODA_OBJECT);
  g_cObjectManager.InsertObjectType("strengthsoda", STRENGTHSODA_OBJECT);
  //Alfredo - MoneyBag Object 
  g_cObjectManager.InsertObjectType("moneybag", MONEYBAG_OBJECT);
  //Alfredo - Sounds
  g_pSoundManager = new CSoundManager(24);
  g_pSoundManager->Load("Sounds\\cue.wav", 1);           //Sound 0 Source: Ned's Turkey Farm 
  g_pSoundManager->Load("Sounds\\boing.wav", 2);         //Sound 1 Source: Ned's Turkey Farm 
  g_pSoundManager->Load("Sounds\\thump.wav", 4);         //Sound 2 Source: Ned's Turkey Farm 
  g_pSoundManager->Load("Sounds\\lose.wav", 1);          //Sound 3 Source: Ned's Turkey Farm 
  g_pSoundManager->Load("Sounds\\Punch.wav", 1);         //Sound 4 Source: http://soundbible.com/1773-Strong-Punch.html
  g_pSoundManager->Load("Sounds\\DogBark.wav", 1);       //Sound 5 Source: http://www.wavsource.com/animals/animals.htm
  g_pSoundManager->Load("Sounds\\DogBite.wav", 1);       //Sound 6 Source: http://soundbible.com/1529-Dog-Bite.html
  g_pSoundManager->Load("Sounds\\SodaOpen.wav", 1);      //Sound 7 Source: http://soundbible.com/1659-Open-Soda-Can.html
  g_pSoundManager->Load("Sounds\\DogDown.wav", 1);       //Sound 8 Source: https://www.youtube.com/watch?v=LulScBMT-Sg
  g_pSoundManager->Load("Sounds\\Elevator.wav", 1);      //Sound 9 Source: https://www.youtube.com/watch?v=K23KJ0PN1M4
  g_pSoundManager->Load("Sounds\\GorillaGrowl.wav", 1);  //Sound 10 Source: https://www.youtube.com/watch?v=82aUsXRG9i0
  g_pSoundManager->Load("Sounds\\GorillaAttack.wav", 4); //Sound 11 Source: https://www.youtube.com/watch?v=82aUsXRG9i0
  g_pSoundManager->Load("Sounds\\MoneyBag.wav", 1);      //Sound 12 Source: http://soundbible.com/333-Cash-Register-Cha-Ching.html
  g_pSoundManager->Load("Sounds\\DeskExplosion.wav", 1); //Sound 13 Source: http://soundbible.com/538-Blast.html
  g_pSoundManager->Load("Sounds\\WaterCoolerExplosion.wav", 1);   //Sound 14 Source: http://soundbible.com/1463-Water-Balloon.html
  g_pSoundManager->Load("Sounds\\HorseCharge.wav", 1);            //Sound 15 Source: https://www.youtube.com/watch?v=_Q-g-HiVEGU
  g_pSoundManager->Load("Sounds\\HorseHit.wav", 1);               //Sound 16 Source: https://www.youtube.com/watch?v=_Q-g-HiVEGU
  g_pSoundManager->Load("Sounds\\HorseNeigh1.wav", 1);            //Sound 17 Source: https://www.youtube.com/watch?v=_Q-g-HiVEGU
  g_pSoundManager->Load("Sounds\\HorseNeigh2.wav", 1);            //Sound 18 Source: https://www.youtube.com/watch?v=_Q-g-HiVEGU
  g_pSoundManager->Load("Sounds\\PotBreak.wav", 1);               //Sound 19 Source: http://soundbible.com/1765-Glass-Break.html
  g_pSoundManager->Load("Sounds\\GameMusic.wav", 1);              //Sound 20 Source: https://www.youtube.com/watch?v=VFou-xkECE0
  g_pSoundManager->Load("Sounds\\NoMoney.wav", 1);              //Sound 21 Source: https://www.youtube.com/watch?v=V0DGjXE_BQQ
  g_pSoundManager->Load("Sounds\\Timer.wav", 1);                //Sound 22 Source: https://www.youtube.com/watch?v=3qus_nGn0F4
  g_pSoundManager->Load("Sounds\\BullMad.wav", 1);              //Sound 23 Source: https://www.youtube.com/watch?v=3qus_nGn0F4

  //Luis-RLG Tiles
  g_cObjectManager.InsertObjectType("carpet", CARPET_OBJECT);//Luis-Carpet Tile
  g_cObjectManager.InsertObjectType("roof", ROOF_OBJECT);//Luis-Ceiling Tile
  g_cObjectManager.InsertObjectType("void", VOID_OBJECT);//Luis-Void Tile
  g_cObjectManager.InsertObjectType("widevoid", WIDEVOID_OBJECT);//Luis-Void Tile
  g_cObjectManager.InsertObjectType("window", WINDOW_OBJECT);//Luis-Window Tile
  g_cObjectManager.InsertObjectType("endwall", ENDWALL_OBJECT);//Luis-Endwall Tile
  g_cObjectManager.InsertObjectType("dummywall", DUMMYWALL_OBJECT);//Luis-Dummywall Tile
  g_cObjectManager.InsertObjectType("elevator", ELEVATOR_OBJECT);//Luis-Start Elevator Tile
  g_cObjectManager.InsertObjectType("elevatorin", ELEVATORIN_OBJECT);//Luis-Inside of Elevator
  g_cObjectManager.InsertObjectType("elevatoren", ELEVATOREN_OBJECT);//Luis-End Elevator Tile
  g_cObjectManager.InsertObjectType("elevatorwall", ELEVATORWALL_OBJECT);//Luis-Inside the elevator, wall
  g_cObjectManager.InsertObjectType("block", BLOCK_OBJECT);//Luis-Block for blocking
  g_cObjectManager.InsertObjectType("deskprint", DESKPRINT_OBJECT);//Luis-Deskprint Tile
  g_cObjectManager.InsertObjectType("outlet", OUTLET_OBJECT);//Luis-Deskprint Tile
  g_cObjectManager.InsertObjectType("dogpost", DOGPOST_OBJECT);//Luis-Dogpost Tile
  g_cObjectManager.InsertObjectType("catpost", CATPOST_OBJECT);//Luis-Catpost Tile
  g_cObjectManager.InsertObjectType("horsepost", HORSEPOST_OBJECT);//Luis-Horsepost Tile
  g_cObjectManager.InsertObjectType("rhinopost", RHINOPOST_OBJECT);//Luis-Rhinopost Tile
  g_cObjectManager.InsertObjectType("breakfloorA", BKRMA_OBJECT);//Luis-Break Room Floor A Tile
  g_cObjectManager.InsertObjectType("plantpost", PLANTPOST_OBJECT);//Luis-Marks where potted plants go
  g_cObjectManager.InsertObjectType("woodwall", WOODWALL_OBJECT);//Luis-Wooden wall
  g_cObjectManager.InsertObjectType("ringfloor", RINGFLOOR_OBJECT);//Luis-Ring Floor
  g_cObjectManager.InsertObjectType("goldcarpet", GDCARPET_OBJECT);//Luis-Golden Carpet
  g_cObjectManager.InsertObjectType("goldwindow", GDWINDOW_OBJECT);//Luis-Golden Window
  g_cObjectManager.InsertObjectType("goldoutlet", GDOUTLET_OBJECT);//Luis-Golden Outlet
  g_cObjectManager.InsertObjectType("goldplantpost", GDPLANTPOST_OBJECT);//Luis-Gold Plant Ring
  g_cObjectManager.InsertObjectType("marker", MARKER_OBJECT);//Luis-Gold Plant Ring
  //Luis-HUD Objects
  g_cObjectManager.InsertObjectType("hud", HUD_OBJECT);//Luis-Heads Up Display
  //Luis-Attack Objects
  g_cObjectManager.InsertObjectType("punch", PUNCH_OBJECT);//Luis-This is the simulated punch
  g_cObjectManager.InsertObjectType("punchleft", PUNCHLEFT_OBJECT);//Luis-This is the simulated punch
  g_cObjectManager.InsertObjectType("punchanimationleft", PUNCHANIMATIONLEFT_OBJECT);
  g_cObjectManager.InsertObjectType("punchanimationright", PUNCHANIMATIONRIGHT_OBJECT);
  g_cObjectManager.InsertObjectType("punchanimationfireright", PUNCHANIMATIONFIRERIGHT_OBJECT);
  g_cObjectManager.InsertObjectType("punchanimationfireleft", PUNCHANIMATIONFIRELEFT_OBJECT);
  g_cObjectManager.InsertObjectType("star", STAR_OBJECT);
  if(g_xmlSettings){ //got "settings" tag
    //get "instances" tag
    XMLElement* objSettings =
      g_xmlSettings->FirstChildElement("instances"); //objecttypes tag

    if(objSettings){ //got "object types" tag
      XMLElement* obj = objSettings->FirstChildElement("instance");
      while(obj != nullptr){ //for each <objecttype> tag
        //get name strings
        string tname = obj->Attribute("typename");
        string name = obj->Attribute("name");

        //get position vector
        Vector3 vPos = Vector3(0, 0, 0);
        char* element = (char*)obj->Attribute("position");
        if(element) //position present
          sscanf_s(element, "%f, %f, %f", &vPos.x, &vPos.y, &vPos.z);

        //get velocity vector
        Vector3 vVel = Vector3(0, 0, 0);
        element = (char*)obj->Attribute("velocity");
        if(element) //velocity present
          sscanf_s(element, "%f, %f, %f", &vVel.x, &vVel.y, &vVel.z);

        //get number of copies (usually 1) strung in a row
        int copies = obj->IntAttribute("copies");
        if(copies == 0) //there was no "copies" attribute
          copies = 1; //default to a single copy

        //get object type and sprite width
        ObjectType t = g_cObjectManager.GetObjectType((char*)tname.c_str());
        if(t == NUM_OBJECT_TYPES)
          ABORT("Bad object type.\n");
        else{ //finally, create objects
          C3DSprite* pSprite = g_cSpriteManager.GetSprite(t);
          int offset = pSprite->m_nWidth; //horizontal offset for row
          for(int i=0; i<copies; i++){
            g_cObjectManager.createObject(t, (char*)name.c_str(), vPos, vVel); 
            vPos.x += offset;
          } //for
        } //else

        obj = obj->NextSiblingElement(); //now for the next <objecttype> tag
      } //while
    } //if
  } //if

  //Alfredo- Gorilla Enemy
  const int NUMGORILLAS = 4;
  const float ZDELTA = 5.0f;
  BB_RLG(0, 1);//Luis-The RLG is called to generate a floor
} //CreateObjects

/// \brief Keyboard handler.
///
/// Handler for keyboard messages from the Windows API. Takes the appropriate
/// action when the user presses a key on the keyboard.
/// \param keystroke Virtual key code for the key pressed
/// \return TRUE if the game is to exit

BOOL KeyboardHandler(WPARAM keystroke) {

  CGameObject* gator = g_cObjectManager.GetObjectByName("gator");
  const float m_fwalkSpeed = 10.0f;//This float will control Gator's walking speed

	  switch (keystroke) {
	  case VK_ESCAPE: //exit game
		  return TRUE; //exit keyboard handler
		  break;

	  case VK_UP:
      if (m_nGameState == PLAYING_STATE) {//Luis-Only do things while playing
        UPre = TRUE;//Up is down
        if (gator != NULL && g_cObjectManager.getDown() != TRUE) {
          if (LPre && !RPre) {//Up+Left
            gator->walk(Vector3(-m_fwalkSpeed, m_fwalkSpeed, 0));
          }
          else if (RPre && !LPre) {//Up+Right 
            gator->walk(Vector3(m_fwalkSpeed, m_fwalkSpeed, 0));
          }
          else {//Just Up
            gator->walk(Vector3(0.0f, m_fwalkSpeed * 2.0f, 0));
          }
        }
      }
		  break;

	  case VK_DOWN:
      if (m_nGameState == PLAYING_STATE) {//Luis-Only do things while playing
        DPre = TRUE;
        if (gator)
          if (LPre && !RPre) {//Down+Left
            gator->walk(Vector3(-m_fwalkSpeed, -m_fwalkSpeed, 0));
          }
          else if (RPre && !LPre) {//Down+Right 
            gator->walk(Vector3(m_fwalkSpeed, -m_fwalkSpeed, 0));
          }
          else {//Just Down
            gator->walk(Vector3(0.0f, -m_fwalkSpeed * 2.0f, 0));
          }
      }
			  break;

	  case VK_LEFT:
      if (m_nGameState == PLAYING_STATE) {//Luis-Only do things while playing
        LPre = TRUE;//Left is Down
        gator->m_bfaceRight = FALSE;//Luis-Gator Knows he is facing left
        gator->UpdateSprite(GATORWALKLEFT_OBJECT);//Luis- Changing the Sprite to face left
        if (gator)
          if (UPre && !DPre) {//Left+Up
            gator->walk(Vector3(-m_fwalkSpeed, m_fwalkSpeed, 0));
          }
          else if (DPre && !UPre) {//Left+Down 
            gator->walk(Vector3(-m_fwalkSpeed, -m_fwalkSpeed, 0));
          }
          else {//Just Left
            gator->walk(Vector3(-m_fwalkSpeed, 0.0f, 0));
          }
      }
			  break;

	  case VK_RIGHT:
      if (m_nGameState == PLAYING_STATE) {//Luis-Only do things while playing
        RPre = TRUE;//Right is Down
        gator->m_bfaceRight = TRUE;//Luis-Gator Knows he is facing right
        gator->UpdateSprite(GATORWALKRIGHT_OBJECT);//Luis- Changing the Sprite to face right

        if (gator)
          if (UPre && !DPre) {//Right+Up
            gator->walk(Vector3(m_fwalkSpeed, m_fwalkSpeed, 0));
          }
          else if (DPre && !UPre) {//Right+Down 
            gator->walk(Vector3(m_fwalkSpeed, -m_fwalkSpeed, 0));
          }
          else {//Just Right
            gator->walk(Vector3(m_fwalkSpeed, 0.0f, 0));
          }

      }
			  break;

	  /*case VK_F7://Luis-THIS IS FOR DEBUGGING ONLY, COMMENTED OUT TO NOT ALLOW USER TO USE CHEAT
		  g_cObjectManager.levelComplete();//Luis-Level is beat
		  break;*/

	  case VK_SPACE: //Luis-The Space Bar is for attacking
      if (m_nGameState == PLAYING_STATE) {//Luis-Only do things while playing
        g_cObjectManager.ThrowPunch("gator", gator->m_bfaceRight);//Luis-Throwing a punch
        g_pSoundManager->play(4);
        if (gator)
          if(RPre)
            gator->UpdateSprite(GATORCHEER_OBJECT);
          else if (LPre)
            gator->UpdateSprite(GATORCHEERLEFT_OBJECT);
      }
		  break;

	  case 0x44: //Alfredo- D key for dashing, currently just sprints
	   /* Dash = TRUE;
		if(gator)
		  gator->dash(LPre, RPre, UPre, DPre);*/
		  break;

    case VK_RETURN://Luis-The Enterkey will trigger the pause state
      if (m_nGameState == PLAYING_STATE) {
        m_nGameState = PAUSE_STATE;//Game State is now paused
        gator->walk(Vector3(0.0f, 0, 0));//Freezes gator so he doesnt move
      }
      else if (m_nGameState == GAMEOVER_STATE) {//Luis-Restart the game
        g_cObjectManager.GameReset();//Reset the game
        m_nGameState = PLAYING_STATE;
      }//Game is Over
      else {
        m_nGameState = PLAYING_STATE;//Game State is now playing
      }
      break;
    default:
      gator->walk(Vector3(0.0f, 0, 0));
      break;
    } //switch

      return FALSE; //normal exit
 } //KeyboardHandler

/// Luis-Keyboard Cleaner
/// This function will take care of things that happen when keys are 
/// released
/// param:keystroke
BOOL KeyboardCleaner(WPARAM keystroke) {
  CGameObject* gator = g_cObjectManager.GetObjectByName("gator");

  switch (keystroke) {
    //Luis-Undoing Moving Actions
   case VK_UP:
     if (gator)
      gator->walk(Vector3(0, 0.0f, 0));
      UPre = FALSE;//Up is down
     break;

    case VK_DOWN:
      if (gator)
        gator->walk(Vector3(0, 0.0f, 0));
        DPre = FALSE;//Down is down
      break;

    case VK_LEFT:
      if (gator) {
        gator->walk(Vector3(0.0f, 0, 0));
        gator->UpdateSprite(GATORLEFT_OBJECT);
      }
      LPre = FALSE;//Left is down
      break;

   case VK_RIGHT:
     RPre = FALSE;//Right is down
     if (gator) {
       gator->walk(Vector3(0.0f, 0, 0));
       gator->UpdateSprite(GATOR_OBJECT);
     }
     break;

   case VK_SPACE:
     if (gator->getFace() == TRUE) {
       gator->UpdateSprite(GATOR_OBJECT);
     }
     else {
       gator->UpdateSprite(GATORLEFT_OBJECT);
     }
     break;

   case 0x44: // Alfredo- Undoing dash movement 
     Dash = FALSE; 
     if (gator)
       gator->walk(Vector3(0.0f, 0, 0));
     break;
  } //switch

  return FALSE; //normal exit
} //KeyboardCleaner

/// \brief Window procedure.
///
/// Handler for messages from the Windows API. 
/// \param hwnd Window handle
/// \param message Message code
/// \param wParam Parameter for message 
/// \param lParam Second parameter for message
/// \return 0 if message is handled

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
  CGameObject* gator = g_cObjectManager.GetObjectByName("gator");


  switch(message){ //handle message
    case WM_ACTIVATEAPP: g_bActiveApp = (BOOL)wParam; break; //iconize

    case WM_KEYDOWN: //keyboard hit
      if(KeyboardHandler(wParam))DestroyWindow(hwnd);
      break;

    case WM_KEYUP://Luis-Keyboard Lifted
      KeyboardCleaner(wParam);//Luis-Cleans up pressed down keys
      break;

    case WM_DESTROY: //on exit
      GameRenderer.Release(); //release textures
      PostQuitMessage(0); //this is the last thing to do on exit
	  SAFE_DELETE(g_pSoundManager);
      break;

    default: //default window procedure
      return DefWindowProc(hwnd, message, wParam, lParam);
  } //switch(message)

  return 0;
} //WindowProc
                         
/// \brief Winmain. 
///
/// Main entry point for this application. 
/// \param hInst Handle to the current instance of this application.
/// \param hPrevInst Handle to previous instance, deprecated.
/// \param lpCmdLine Command line string, unused. 
/// \param nShow Specifies how the window is to be shown.
/// \return TRUE if application terminates correctly.

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nShow){
  MSG msg; //current message
  HWND hwnd; //handle to fullscreen window

  #ifdef DEBUG_ON
    g_cDebugManager.open(); //open debug streams, settings came from XML file
  #endif //DEBUG_ON

  g_hInstance = hInst;
  g_cTimer.start(); //start game timer
  InitXMLSettings(); //initialize XML settings reader
  LoadGameSettings();

  //create fullscreen window
  hwnd = CreateDefaultWindow(g_szGameName, hInst, nShow);
  if(!hwnd)return FALSE; //bail if problem creating window
  g_HwndApp = hwnd; //save window handle
  
  InitGraphics(); //initialize graphics
  GameRenderer.LoadTextures(); //load images
  CreateObjects(); //create game objects


  //message loop
  while(TRUE)
    if(PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE)){ //if message waiting
      if(!GetMessage(&msg, nullptr, 0, 0))return (int)msg.wParam; //get it
      TranslateMessage(&msg); DispatchMessage(&msg); //translate it
    } //if
    else 
      if (g_bActiveApp) {
        GameRenderer.ProcessFrame();
      }
      else WaitMessage();
} //WinMain
