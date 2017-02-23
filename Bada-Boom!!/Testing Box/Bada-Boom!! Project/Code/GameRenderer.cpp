/// \file gamerenderer.cpp
/// \brief Direct3D rendering tasks for the game.
/// DirectX stuff that won't change much is hidden away in this file
/// so you won't have to keep looking at it.

#include <algorithm>

#include "gamerenderer.h"
#include "defines.h" 
#include "abort.h"
#include "imagefilenamelist.h"
#include "debug.h"
#include "sprite.h"
#include "object.h"
#include "objman.h"
#include "spriteman.h"
#include "SpriteSheet.h"
#include "Timer.h"

extern int g_nScreenWidth;
extern int g_nScreenHeight;
extern GameState m_nGameState;
extern BOOL g_bWireFrame;
extern CImageFileNameList g_cImageFileName;
extern CObjectManager g_cObjectManager;
extern CSpriteManager g_cSpriteManager;
extern CTimer g_cTimer;

Vector3 HUDLoc = Vector3{ 0, 0, 0 };// For drawing the HUD


CGameRenderer::CGameRenderer(): m_bCameraDefaultMode(TRUE){
} //constructor

/// Initialize the vertex and constant buffers for the background, that is, the
/// ground and the sky.

void CGameRenderer::InitBackground(){
  HRESULT hr;

  //load vertex buffer
  float w = 2.0f*g_nScreenWidth;//Luis-Mess 4.0 was 2.0
  float h = 2.0f*g_nScreenHeight;
  
  //vertex information, first triangle in clockwise order
  // Changed 0 to -300 to get the back round to reach the bottom of the screen
  BILLBOARDVERTEX pVertexBufferData[6]; 
  pVertexBufferData[0].p = Vector3(w, -300, 0);
  pVertexBufferData[0].tu = 1.0f; pVertexBufferData[0].tv = 0.0f;

  pVertexBufferData[1].p = Vector3(0, -300, 0);
  pVertexBufferData[1].tu = 0.0f; pVertexBufferData[1].tv = 0.0f;

  pVertexBufferData[2].p = Vector3(w, -300, 1500);
  pVertexBufferData[2].tu = 1.0f; pVertexBufferData[2].tv = 1.0f;

  pVertexBufferData[3].p = Vector3(0, -300, 1500);
  pVertexBufferData[3].tu = 0.0f; pVertexBufferData[3].tv = 1.0f;

  pVertexBufferData[4].p = Vector3(w, h, 1500);
  pVertexBufferData[4].tu = 1.0f; pVertexBufferData[4].tv = 0.0f;

  pVertexBufferData[5].p = Vector3(0, h, 1500);
  pVertexBufferData[5].tu = 0.0f; pVertexBufferData[5].tv = 0.0f;
  
  //create vertex buffer for background
  m_pShader = new CShader(2);
    
  m_pShader->AddInputElementDesc(0, DXGI_FORMAT_R32G32B32_FLOAT, "POSITION");
  m_pShader->AddInputElementDesc(12, DXGI_FORMAT_R32G32_FLOAT,  "TEXCOORD");
  m_pShader->VSCreateAndCompile(L"VertexShader.hlsl", "main");
  m_pShader->PSCreateAndCompile(L"PixelShader.hlsl", "main");
    
  // Create constant buffers.
  D3D11_BUFFER_DESC constantBufferDesc = { 0 };
  constantBufferDesc.ByteWidth = sizeof(ConstantBuffer);
  constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  constantBufferDesc.CPUAccessFlags = 0;
  constantBufferDesc.MiscFlags = 0;
  constantBufferDesc.StructureByteStride = 0;
    
  m_pDev2->CreateBuffer(&constantBufferDesc, nullptr, &m_pConstantBuffer);
    
  D3D11_BUFFER_DESC VertexBufferDesc;
  VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  VertexBufferDesc.ByteWidth = sizeof(BILLBOARDVERTEX)* 6;
  VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  VertexBufferDesc.CPUAccessFlags = 0;
  VertexBufferDesc.MiscFlags = 0;
  VertexBufferDesc.StructureByteStride = 0;
    
  D3D11_SUBRESOURCE_DATA subresourceData;
  subresourceData.pSysMem = pVertexBufferData;
  subresourceData.SysMemPitch = 0;
  subresourceData.SysMemSlicePitch = 0;
    
  hr = m_pDev2->CreateBuffer(&VertexBufferDesc, &subresourceData, &m_pBackgroundVB);
} //InitBackground

/// Draw the game background.
/// \param x Camera x offset

void CGameRenderer::DrawBackground(float x){
  const float delta = 2.0f * g_nScreenWidth;
  float fQuantizeX = delta * (int)(x / delta - 1.0f) + g_nScreenWidth; //Quantized x coordinate

  UINT nVertexBufferOffset = 0;
  
  UINT nVertexBufferStride = sizeof(BILLBOARDVERTEX);
  m_pDC2->IASetVertexBuffers(0, 1, &m_pBackgroundVB, &nVertexBufferStride, &nVertexBufferOffset);
  m_pDC2->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
  m_pShader->SetShaders();

  //draw floor
  if(g_bWireFrame)
    m_pDC2->PSSetShaderResources(0, 1, &m_pWireframeTexture); //set wireframe texture
  else
    m_pDC2->PSSetShaderResources(0, 1, &m_pFloorTexture); //set floor texture
  
  SetWorldMatrix(Vector3(fQuantizeX, 0, 0));
  
  ConstantBuffer constantBufferData; ///< Constant buffer data for shader.

  constantBufferData.wvp = CalculateWorldViewProjectionMatrix();
  m_pDC2->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &constantBufferData, 0, 0);
  m_pDC2->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
  m_pDC2->Draw(4, 0);

  SetWorldMatrix(Vector3(fQuantizeX - delta, 0, 0));

  constantBufferData.wvp = CalculateWorldViewProjectionMatrix();
  m_pDC2->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &constantBufferData, 0, 0);
  m_pDC2->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
  m_pDC2->Draw(4, 0);

  //draw backdrop
  if(!g_bWireFrame)
    m_pDC2->PSSetShaderResources(0, 1, &m_pWallTexture);

  SetWorldMatrix(Vector3(fQuantizeX, 0, 0));

  constantBufferData.wvp = CalculateWorldViewProjectionMatrix();
  m_pDC2->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &constantBufferData, 0, 0);
  m_pDC2->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
  m_pDC2->Draw(4, 2);

  SetWorldMatrix(Vector3(fQuantizeX - delta, 0, 0));

  constantBufferData.wvp = CalculateWorldViewProjectionMatrix();
  m_pDC2->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &constantBufferData, 0, 0);
  m_pDC2->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
  m_pDC2->Draw(4, 2);
} //DrawBackground
 
/// Load the background and sprite textures.

void CGameRenderer::LoadTextures(){ 
  //background
  LoadTexture(m_pWallTexture, g_cImageFileName[0]);
  LoadTexture(m_pFloorTexture, g_cImageFileName[1]);

  LoadTexture(m_pWireframeTexture, g_cImageFileName[2]); //black for wireframe

  //sprites
  //Luis-Logo Screen Sprites
  g_cSpriteManager.Load(LOGO_OBJECT, "logo");
  g_cSpriteManager.Load(GAMEOVER_OBJECT, "gameover");
  //Alfredo- Speech Screens
  g_cSpriteManager.Load(BOSSBULLSCREEN_OBJECT, "bossbullscreen");
  g_cSpriteManager.Load(BOSSBULLWINSCREEN_OBJECT, "bossbullwinscreen");
  //Luis-Gator Sprites
  g_cSpriteManager.Load(GATOR_OBJECT, "gator");
  g_cSpriteManager.Load(GATORLEFT_OBJECT, "gatorleft");
  g_cSpriteManager.Load(GATORWALKLEFT_OBJECT, "gatorwalkleft");
  g_cSpriteManager.Load(GATORWALKRIGHT_OBJECT, "gatorwalkright");
  g_cSpriteManager.Load(GATORCHEER_OBJECT, "gatorcheer");
  g_cSpriteManager.Load(GATORCHEERLEFT_OBJECT, "gatorcheerleft");
  g_cSpriteManager.Load(GATORHURT_OBJECT, "gatorhurt");
  g_cSpriteManager.Load(GATORHURTLEFT_OBJECT, "gatorhurtleft");
  g_cSpriteManager.Load(STAR_OBJECT, "star");
  //Luis-RLG Tiles
  g_cSpriteManager.Load(CARPET_OBJECT, "carpet");
  g_cSpriteManager.Load(WINDOW_OBJECT, "window");
  g_cSpriteManager.Load(ENDWALL_OBJECT, "endwall");
  g_cSpriteManager.Load(DUMMYWALL_OBJECT, "dummywall");
  g_cSpriteManager.Load(ELEVATOR_OBJECT, "elevator");
  g_cSpriteManager.Load(ELEVATORIN_OBJECT, "elevatorin");
  g_cSpriteManager.Load(ELEVATOREN_OBJECT, "elevatoren");
  g_cSpriteManager.Load(BLOCK_OBJECT, "block");
  g_cSpriteManager.Load(ELEVATORWALL_OBJECT, "elevatorwall");
  g_cSpriteManager.Load(DESKPRINT_OBJECT, "deskprint");
  g_cSpriteManager.Load(OUTLET_OBJECT, "outlet");
  g_cSpriteManager.Load(DOGPOST_OBJECT, "dogpost");
  g_cSpriteManager.Load(CATPOST_OBJECT, "catpost");
  g_cSpriteManager.Load(HORSEPOST_OBJECT, "horsepost");
  g_cSpriteManager.Load(RHINOPOST_OBJECT, "rhinopost");
  g_cSpriteManager.Load(BKRMA_OBJECT, "breakfloorA");
  g_cSpriteManager.Load(PLANTPOST_OBJECT, "plantpost");
  g_cSpriteManager.Load(WOODWALL_OBJECT, "woodwall");
  g_cSpriteManager.Load(RINGFLOOR_OBJECT, "ringfloor");
  g_cSpriteManager.Load(GDCARPET_OBJECT, "goldcarpet");
  g_cSpriteManager.Load(GDWINDOW_OBJECT, "goldwindow");
  g_cSpriteManager.Load(GDOUTLET_OBJECT, "goldoutlet");
  g_cSpriteManager.Load(GDPLANTPOST_OBJECT, "goldplantpost");
  g_cSpriteManager.Load(VOID_OBJECT, "void");
  g_cSpriteManager.Load(WIDEVOID_OBJECT, "widevoid");
  g_cSpriteManager.Load(ROOF_OBJECT, "roof");
  g_cSpriteManager.Load(MARKER_OBJECT, "star");
  //Luis-HUD Objects
  g_cSpriteManager.Load(HUD_OBJECT, "hud");
  m_cScreenText = new CSpriteSheet(15, 19);//Black Text
  m_cScreenText->Load("Images\\BB_Text_Black.png");
  m_cTextRed = new CSpriteSheet(15, 19);//Red Text
  m_cTextRed->Load("Images\\BB_Text_Red.png");
  m_cTextWhite = new CSpriteSheet(15, 19);//White Text
  m_cTextWhite->Load("Images\\BB_Text_White.png");
  m_cTextOrange = new CSpriteSheet(15, 19);//Orange Text
  m_cTextOrange->Load("Images\\BB_Text_Orange.png");
  m_cTextGold = new CSpriteSheet(15, 19);//Gold Text
  m_cTextGold->Load("Images\\BB_Text_Gold.png");
  //ALfredo - Gorilla Object 
  g_cSpriteManager.Load(DEAD_GORILLA_OBJECT, "deadgorilla");
  g_cSpriteManager.Load(GORILLADOWNEDLEFT_OBJECT, "gorilladownedleft");
  g_cSpriteManager.Load(GORILLADOWNEDRIGHT_OBJECT, "gorilladownedright");
  g_cSpriteManager.Load(GORILLA_OBJECT, "gorilla");
  g_cSpriteManager.Load(MADGORILLALEFT_OBJECT, "madgorillaleft");
  g_cSpriteManager.Load(MADGORILLARIGHT_OBJECT, "madgorillaright");
  g_cSpriteManager.Load(GORILLAATTACKLEFT_OBJECT, "gorillaattackleft");
  g_cSpriteManager.Load(GORILLAATTACKRIGHT_OBJECT, "gorillaattackright");
  g_cSpriteManager.Load(GORILLAWALKLEFT_OBJECT, "gorillawalkleft");
  g_cSpriteManager.Load(GORILLAWALKRIGHT_OBJECT, "gorillawalkright");
  g_cSpriteManager.Load(GORILLABLOCKLEFT_OBJECT, "gorillablockleft");
  g_cSpriteManager.Load(GORILLABLOCKRIGHT_OBJECT, "gorillablockright");
  //Alfredo - Gorilla Champion
  g_cSpriteManager.Load(CHAMPIONDEAD_GORILLA_OBJECT, "championdeadgorilla");
  g_cSpriteManager.Load(CHAMPIONGORILLADOWNEDLEFT_OBJECT, "championgorilladownedleft");
  g_cSpriteManager.Load(CHAMPIONGORILLADOWNEDRIGHT_OBJECT, "championgorilladownedright");
  g_cSpriteManager.Load(CHAMPIONGORILLA_OBJECT, "championgorilla");
  g_cSpriteManager.Load(CHAMPIONMADGORILLALEFT_OBJECT, "championmadgorillaleft");
  g_cSpriteManager.Load(CHAMPIONMADGORILLARIGHT_OBJECT, "championmadgorillaright");
  g_cSpriteManager.Load(CHAMPIONGORILLAATTACKLEFT_OBJECT, "championgorillaattackleft");
  g_cSpriteManager.Load(CHAMPIONGORILLAATTACKRIGHT_OBJECT, "championgorillaattackright");
  g_cSpriteManager.Load(CHAMPIONGORILLAWALKLEFT_OBJECT, "championgorillawalkleft");
  g_cSpriteManager.Load(CHAMPIONGORILLAWALKRIGHT_OBJECT, "championgorillawalkright");
  g_cSpriteManager.Load(CHAMPIONGORILLABLOCKLEFT_OBJECT, "championgorillablockleft");
  g_cSpriteManager.Load(CHAMPIONGORILLABLOCKRIGHT_OBJECT, "championgorillablockright");
  //Alfredo - Dog Object
  g_cSpriteManager.Load(DOG_OBJECT, "dog");
  g_cSpriteManager.Load(DOGWALKLEFT_OBJECT, "dogwalkingleft");
  g_cSpriteManager.Load(DOGWALKRIGHT_OBJECT, "dogwalkingright");
  g_cSpriteManager.Load(DOGMADLEFT_OBJECT, "dogmadleft");
  g_cSpriteManager.Load(DOGMADRIGHT_OBJECT, "dogmadright");
  g_cSpriteManager.Load(DOGDOWNEDLEFT_OBJECT, "dogdownedleft");
  g_cSpriteManager.Load(DOGDOWNEDRIGHT_OBJECT, "dogdownedright");
  g_cSpriteManager.Load(DOGDEAD_OBJECT, "dogdead");
  g_cSpriteManager.Load(DOGATTACKLEFT_OBJECT, "dogattackleft");
  g_cSpriteManager.Load(DOGATTACKRIGHT_OBJECT, "dogattackright");
  //Alfredo - Dog Champion Object
  g_cSpriteManager.Load(CHAMPIONDOG_OBJECT, "championdog");
  g_cSpriteManager.Load(CHAMPIONDOGWALKLEFT_OBJECT, "championdogwalkingleft");
  g_cSpriteManager.Load(CHAMPIONDOGWALKRIGHT_OBJECT, "championdogwalkingright");
  g_cSpriteManager.Load(CHAMPIONDOGMADLEFT_OBJECT, "championdogmadleft");
  g_cSpriteManager.Load(CHAMPIONDOGMADRIGHT_OBJECT, "championdogmadright");
  g_cSpriteManager.Load(CHAMPIONDOGDOWNEDLEFT_OBJECT, "championdogdownedleft");
  g_cSpriteManager.Load(CHAMPIONDOGDOWNEDRIGHT_OBJECT, "championdogdownedright");
  g_cSpriteManager.Load(CHAMPIONDOGDEAD_OBJECT, "championdogdead");
  g_cSpriteManager.Load(CHAMPIONDOGATTACKLEFT_OBJECT, "championdogattackleft");
  g_cSpriteManager.Load(CHAMPIONDOGATTACKRIGHT_OBJECT, "championdogattackright");
  //Alfredo- Horse Object
  g_cSpriteManager.Load(HORSE_OBJECT, "horse");
  g_cSpriteManager.Load(HORSEWALKLEFT_OBJECT, "horsewalkleft");
  g_cSpriteManager.Load(HORSEWALKRIGHT_OBJECT, "horsewalkright");
  g_cSpriteManager.Load(HORSEMADLEFT_OBJECT, "horsemadleft");
  g_cSpriteManager.Load(HORSEMADRIGHT_OBJECT, "horsemadright");
  g_cSpriteManager.Load(HORSEDEAD_OBJECT, "horsedead");
  g_cSpriteManager.Load(HORSERUNLEFT_OBJECT, "horserunleft");
  g_cSpriteManager.Load(HORSERUNRIGHT_OBJECT, "horserunright");
  g_cSpriteManager.Load(HORSESTOPLEFT_OBJECT, "horsestopleft");
  g_cSpriteManager.Load(HORSESTOPRIGHT_OBJECT, "horsestopright");
  //Alfredo- Rhino Object
  g_cSpriteManager.Load(RHINO_OBJECT, "rhino");
  g_cSpriteManager.Load(RHINOATTACKLEFT_OBEJCT, "rhinoattackleft");
  g_cSpriteManager.Load(RHINOATTACKRIGHT_OBJECT, "rhinoattackright");
  g_cSpriteManager.Load(RHINODOWNEDLEFT_OBJECT, "rhinodownedleft");
  g_cSpriteManager.Load(RHINODOWNEDRIGHT_OBJECT, "rhinodownedright");
  g_cSpriteManager.Load(RHINOHURTLEFT_OBJECT, "rhinohurtleft");
  g_cSpriteManager.Load(RHINOHURTRIGHT_OBJECT, "rhinohurtright");
  g_cSpriteManager.Load(RHINORUNLEFT_OBJECT, "rhinorunleft");
  g_cSpriteManager.Load(RHINORUNRIGHT_OBJECT, "rhinorunright");
  g_cSpriteManager.Load(RHINODEAD_OBJECT, "rhinodead");
  //Alfredo - Boss Bull Objects
  g_cSpriteManager.Load(BOSSBULL_OBJECT, "bossbull");
  g_cSpriteManager.Load(BOSSBULLLEFT_OBJECT, "bossbullleft");
  g_cSpriteManager.Load(BOSSBULLCHARGELEFT_OBJECT, "bossbullchargeleft");
  g_cSpriteManager.Load(BOSSBULLCHARGERIGHT_OBJECT, "bossbullchargeright");
  g_cSpriteManager.Load(BOSSBULLHURTLEFT_OBJECT, "bossbullhurtleft");
  g_cSpriteManager.Load(BOSSBULLHURTRIGHT_OBJECT, "bossbullhurtright");
  g_cSpriteManager.Load(BOSSBULLDOWNEDLEFT_OBJECT, "bossbulldownedleft");
  g_cSpriteManager.Load(BOSSBULLDOWNEDRIGHT_OBJECT, "bossbulldownedright");
  g_cSpriteManager.Load(BOSSBULLDEADRIGHT_OBJECT, "bossbulldeadright");
  g_cSpriteManager.Load(BOSSBULLDEADLEFT_OBJECT, "bossbulldeadleft");
  //Alfredo - Furniture Objects
  g_cSpriteManager.Load(WATERCOOLER_OBJECT, "watercooler");
  g_cSpriteManager.Load(POTTEDPLANT_OBJECT, "pottedplant");
  g_cSpriteManager.Load(VENDINGMACHINE_OBJECT, "vendingmachine");
  g_cSpriteManager.Load(DESKHORIZONTAL_OBJECT, "deskhorizontal");
  //Alfredo- Furniture Explosion Objects
  g_cSpriteManager.Load(DESKEXPLOSION_OBJECT, "deskexplosion");
  g_cSpriteManager.Load(WATERCOOLEREXPLOSION_OBJECT, "watercoolerexplosion");
  g_cSpriteManager.Load(POTTEDPLANTEXPLOSION_OBJECT, "pottedplantexplosion");
  //Alfredo - PowerUp Objects
  g_cSpriteManager.Load(HEALTHSODA_OBJECT, "healthsoda");
  g_cSpriteManager.Load(STRENGTHSODA_OBJECT, "strengthsoda");
  //Alfredo - Moneybag Object
  g_cSpriteManager.Load(MONEYBAG_OBJECT, "moneybag");
  //Luis- Attacks
  g_cSpriteManager.Load(PUNCH_OBJECT, "punch");
  g_cSpriteManager.Load(PUNCHLEFT_OBJECT, "punchleft");
  g_cSpriteManager.Load(PUNCHANIMATIONLEFT_OBJECT, "punchanimationleft");
  g_cSpriteManager.Load(PUNCHANIMATIONRIGHT_OBJECT, "punchanimationright");
  g_cSpriteManager.Load(PUNCHANIMATIONFIRERIGHT_OBJECT, "punchanimationfireright");
  g_cSpriteManager.Load(PUNCHANIMATIONFIRELEFT_OBJECT, "punchanimationfireleft");
  //Misc Sprites
  g_cSpriteManager.Load(CROW_OBJECT, "crow");
  g_cSpriteManager.Load(EXPLODINGCROW_OBJECT, "explodingcrow");
  g_cSpriteManager.Load(DEADCROW_OBJECT, "deadcrow");
  g_cSpriteManager.Load(BULLET_OBJECT, "bullet");
  g_cSpriteManager.Load(HORSEATTACK_OBJECT, "horseattack");
  g_cSpriteManager.Load(BOSSBULLATTACKLEFT_OBJECT, "bossbullattackleft");
  g_cSpriteManager.Load(BOSSBULLATTACKRIGHT_OBJECT, "bossbullattackright");
} //LoadTextures

/// All textures used in the game are released - the release function is kind
/// of like a destructor for DirectX entities, which are COM objects.

void CGameRenderer::Release(){ 
  g_cSpriteManager.Release();

  SAFE_RELEASE(m_pWallTexture);
  SAFE_RELEASE(m_pFloorTexture);
  SAFE_RELEASE(m_pWireframeTexture);
  SAFE_RELEASE(m_pBackgroundVB);
  SAFE_RELEASE(m_cScreenText);
  SAFE_RELEASE(m_cTextRed);
  SAFE_RELEASE(m_cTextWhite);
  SAFE_RELEASE(m_cTextOrange);
  SAFE_RELEASE(m_cTextGold);
  SAFE_DELETE(m_pShader);

  CRenderer::Release();
} //Release

  /// Draw text to HUD using a sprite sheet.
  /// \param text The text to be drawn.
  /// \param p Position at which to draw it.
  /// \param c is what color the text should be
  /// Each Color will be a different sprite sheet

void CGameRenderer::DrawHUDText(char* text, Vector3 p, int c) {
  switch (c) {
  case 0://Black Text
    for (unsigned int i = 0; i < strlen(text); i++) {
      char c = text[i];
      if (c >= 'A' && c <= 'Z')
        p = m_cScreenText->Draw(p, 1, 48, c - 'A');
      else if (c >= 'a' && c <= 'z')
        p = m_cScreenText->Draw(p, 1, 95, c - 'a');
      else if (c >= '0' && c <= '9')
        p = m_cScreenText->Draw(p, 1, 1, c - '0');
      else p = m_cScreenText->Draw(p, 1, 1, 10); //blank
    } //for
    break;
  case 1://Red Text
    for (unsigned int i = 0; i < strlen(text); i++) {
      char c = text[i];
      if (c >= 'A' && c <= 'Z')
        p = m_cTextRed->Draw(p, 1, 48, c - 'A');
      else if (c >= 'a' && c <= 'z')
        p = m_cTextRed->Draw(p, 1, 95, c - 'a');
      else if (c >= '0' && c <= '9')
        p = m_cTextRed->Draw(p, 1, 1, c - '0');
      else p = m_cTextRed->Draw(p, 1, 1, 10); //blank
    } //for
    break;
  case 2://White Text
    for (unsigned int i = 0; i < strlen(text); i++) {
      char c = text[i];
      if (c >= 'A' && c <= 'Z')
        p = m_cTextWhite->Draw(p, 1, 48, c - 'A');
      else if (c >= 'a' && c <= 'z')
        p = m_cTextWhite->Draw(p, 1, 95, c - 'a');
      else if (c >= '0' && c <= '9')
        p = m_cTextWhite->Draw(p, 1, 1, c - '0');
      else p = m_cTextWhite->Draw(p, 1, 1, 10); //blank
    } //for
    break;
  case 3://Orange Text
    for (unsigned int i = 0; i < strlen(text); i++) {
      char c = text[i];
      if (c >= 'A' && c <= 'Z')
        p = m_cTextOrange->Draw(p, 1, 48, c - 'A');
      else if (c >= 'a' && c <= 'z')
        p = m_cTextOrange->Draw(p, 1, 95, c - 'a');
      else if (c >= '0' && c <= '9')
        p = m_cTextOrange->Draw(p, 1, 1, c - '0');
      else p = m_cTextOrange->Draw(p, 1, 1, 10); //blank
    } //for
    break;
  case 4://Gold Text
    for (unsigned int i = 0; i < strlen(text); i++) {
      char c = text[i];
      if (c >= 'A' && c <= 'Z')
        p = m_cTextGold->Draw(p, 1, 48, c - 'A');
      else if (c >= 'a' && c <= 'z')
        p = m_cTextGold->Draw(p, 1, 95, c - 'a');
      else if (c >= '0' && c <= '9')
        p = m_cTextGold->Draw(p, 1, 1, c - '0');
      else p = m_cTextGold->Draw(p, 1, 1, 10); //blank
    } //for
    break;
  }
} //DrawHUDText

///Luis-Update HUD
///This function will update the position of the HUD
/// Draw the heads-up display.
/// \param text0 Text to draw at left of HUD.
/// \param text1 Text to draw at right of HUD.
/// Code from the Demo 8 files
void CGameRenderer::updateHUD() {
    float w = g_nScreenWidth / 2.0f;
    float h = g_nScreenHeight / 2.0f;
    int startTime = 0;
    BOOL started = FALSE;
    Vector3 gLoc;
    gLoc = g_cObjectManager.getPCPos();

    //switch to orthographic projection
    XMMATRIX tempProj = m_matProj;
    m_matProj = XMMatrixOrthographicOffCenterLH(-w, w, -h, h, 1.0f, 10000.0f);
    m_matWorld = XMMatrixIdentity();
    m_matView = XMMatrixLookAtLH(Vector3(w, h, 0), Vector3(w, h, 1000.0f), Vector3(0, 1, 0));

    ///clear the depth buffer
    m_pDC2->ClearDepthStencilView(m_pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    //draw the HUD background sprite
    C3DSprite* hud = g_cSpriteManager.GetSprite(HUD_OBJECT);
    Vector3 p = Vector3(-300.0f, g_nScreenHeight - 600.0f , 1000.0f);
    hud->Draw(p, 0.0f, 0);

    //draw the text
    char buffer0[256], buffer1[256], buffer2[256], buffer3[256], buffer4[256], buffer5[256];
    if (g_cObjectManager.getPCHealth() < 1000) {//Only update if under 1000
      sprintf_s(buffer0, "%d", g_cObjectManager.getPCHealth());//Getting the PC Health
    }
    else {//Makes it so the health counter doesn't go over 999
      sprintf_s(buffer0, "%d", 999);//Getting the PC Health
    }
    sprintf_s(buffer1, "%d", g_cObjectManager.getFloor() + 1);//Getting the floor number
    sprintf_s(buffer2, "%d", g_cObjectManager.getLives());//Getting the number of lives
    sprintf_s(buffer3, "%d", g_cObjectManager.getPUTimer());//Getting seconds left on the power-up timer
    sprintf_s(buffer4, "%d", g_cObjectManager.getScore());//Getting the score
    sprintf_s(buffer5, "%f", gLoc.z);//Testing
    DrawHUDText(buffer0, p + Vector3(g_nScreenWidth - 610.0f, 30.0f, -10.0f), 1);//Health
    DrawHUDText(buffer1, p + Vector3(g_nScreenWidth - 456.0f, -60.0f, -10.0f), 0);//Floor Number
    DrawHUDText(buffer2, p + Vector3(g_nScreenWidth - 610.0f, 0.0f, -10.0f), 2);//Lives
    DrawHUDText(buffer3, p + Vector3(g_nScreenWidth - 610.0f, -30.0f, -10.0f), 3);//PU Timer
    DrawHUDText(buffer4, p + Vector3(g_nScreenWidth - 610.0f, -60.0f, -10.0f), 4);//Score
    //DrawHUDText(buffer5, p + Vector3(g_nScreenWidth - 610.0f, -120.0f, -10.0f), 2);//Testing
    if (m_nGameState == PLAYING_STATE) {
      if (!started) {//Dont start the timer til the game starts
        startTime = 0;
        started = TRUE;
      }
      //Tutorial Text
      //startTime = g_cTimer.time();
      if (!g_cTimer.elapsed(startTime, 10000) && started) {//Tutorial disappears after 10 seconds
        DrawHUDText("LIVES", p + Vector3(g_nScreenWidth - 550.0f, 0.0f, -10.0f), 2);//Lives Tutorial Text
        DrawHUDText("HEALTH", p + Vector3(g_nScreenWidth - 550.0f, 30.0f, -10.0f), 2);//Health Tutorial Text
        DrawHUDText("POWER UPS", p + Vector3(g_nScreenWidth - 550.0f, -30.0f, -10.0f), 2);//Health Tutorial Text
        DrawHUDText("SCORE", p + Vector3(g_nScreenWidth - 610.0f, -90.0f, -10.0f), 2);//Score Tutorial Text
        DrawHUDText("FLOOR", p + Vector3(g_nScreenWidth - 456.0f, -90.0f, -10.0f), 2);//Floor Number Tutorial Text
        if (g_cTimer.elapsed(startTime, 10000) == FALSE && g_cTimer.elapsed(startTime, 6000) == TRUE) {
          DrawHUDText("   HAVE FUN GATOR             ", p + Vector3(w / 2 + 430.0f, h / 2, 0.0f), 2);//Have at it!
        }
        if (g_cTimer.elapsed(startTime, 6000) == FALSE && g_cTimer.elapsed(startTime, 3000) == TRUE) {
          DrawHUDText("YOUR JOB IS TO BEAT THINGS UP", p + Vector3(w / 2 + 330.0f, h / 2, 0.0f), 2);//Goal Line
        }
        if (g_cTimer.elapsed(startTime, 3000) == FALSE) {
          DrawHUDText("THIS IS BADA BOOM", p + Vector3(w / 2 + 430.0f, h / 2, 0.0f), 2);//Welcome Line
        }
      }
      DrawHUDText("WORK IN PROGRESS", p + Vector3(g_nScreenWidth, -120.0f, -10.0f), 2);//Testing
    }//Working in Progress
    if (m_nGameState == PAUSE_STATE) {
      DrawHUDText("PAUSED", p + Vector3(g_nScreenWidth, -120.0f, -10.0f), 2);//Testing
    }//Paused
    if (m_nGameState == STARTSCREEN_STATE) {
      startTime = 0;
      C3DSprite* logo = g_cSpriteManager.GetSprite(LOGO_OBJECT);
      logo->Draw(p + Vector3(w / 2 + 50.0f, h / 2, 0.0f), 0.0f, 0);
      DrawHUDText("PRESS ENTER", p + Vector3(g_nScreenWidth, -120.0f, -10.0f), 2);//Testing
    }//Press enter
    if (m_nGameState == GAMEOVER_STATE) {
      C3DSprite* gameover = g_cSpriteManager.GetSprite(GAMEOVER_OBJECT);
      gameover->Draw(p + Vector3(w / 2 + 50.0f, h / 2, 0.0f), 0.0f, 0);
      DrawHUDText("PRESS ENTER", p + Vector3(g_nScreenWidth, -120.0f, -10.0f), 2);//Testing
    }//Game Over
    if (m_nGameState == BOSSROOMSCREEN_STATE) {
      C3DSprite* bossDialogue = g_cSpriteManager.GetSprite(BOSSBULLSCREEN_OBJECT);
      bossDialogue->Draw(p + Vector3(w / 2 + 50.0f, h / 10, -100.0f), 0.0f, 0);
      DrawHUDText("PRESS ENTER", p + Vector3(g_nScreenWidth, -120.0f, -10.0f), 2);//Testing
    }//BossRoom Screen
    if (m_nGameState == BOSSBEATENSCREEN_STATE) {
      char buffer6[256];
      sprintf_s(buffer6, "%d", g_cObjectManager.getScore());//Getting the score
      C3DSprite* logo = g_cSpriteManager.GetSprite(BOSSBULLWINSCREEN_OBJECT);
      logo->Draw(p + Vector3(w / 2 + 50.0f, h / 2 + 150.0f, -100.0f), 0.0f, 0);
      DrawHUDText("YOU BEAT THE BOSS ROOM WITH A SCORE OF ", p + Vector3(w / 2 + 220.0f, h / 2, 0.0f), 2);//Testing
      DrawHUDText(buffer6, p + Vector3(w / 2 + 800.0f, h / 2, 0.0f), 2);//Score 
      DrawHUDText("PRESS ENTER", p + Vector3(g_nScreenWidth, -120.0f, -10.0f), 2);//Testing
    }//BossRoom Screen
    //back to perspective projection 
    //m_matProj = tempProj;
  } //updateHUD


/// Move all objects, then draw them.
/// \return TRUE if it succeeded

void CGameRenderer::ComposeFrame(){
  g_cObjectManager.move(); //move objects

  //set camera location
  CGameObject* p = g_cObjectManager.GetObjectByName("gator");
  float x = p->m_vPos.x, y = p->m_vPos.y; //gator's current location
  y = min(y, g_nScreenHeight/2.0f);
  y = max(y, g_nScreenHeight/4.0f);

  Vector3 pos, lookatpt;

  if(m_bCameraDefaultMode){
    pos = Vector3(x + g_nScreenWidth/2, y, -350);
    lookatpt = Vector3(x + g_nScreenWidth/2, y, 1000);
  } //if
  else{
    pos = Vector3(x - 2.5f*g_nScreenWidth, 1000, -3000);
    lookatpt = Vector3(x - g_nScreenWidth, 700, 0);
  } //else
  
  SetViewMatrix(pos, lookatpt);

  //prepare to draw
  m_pDC2->OMSetRenderTargets(1, &m_pRTV, m_pDSV);
  float clearColor[] = { 1.0f, 1.0f, 1.0f, 0.0f };
  m_pDC2->ClearRenderTargetView(m_pRTV, clearColor);
  m_pDC2->ClearDepthStencilView(m_pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

  //draw
  DrawBackground(x + g_nScreenWidth/2.0f); //draw background
  g_cObjectManager.draw(); //draw objects
  updateHUD();//Luis-Draws the hud
} //ComposeFrame
 
/// Compose a frame of animation and present it to the video card.

void CGameRenderer::ProcessFrame(){ 
  ComposeFrame(); //compose a frame of animation
  m_pSwapChain2->Present(0, 0); //present it
} //ProcessFrame

/// Toggle between eagle-eye camera (camera pulled back far enough to see
/// backdrop) and the normal game camera.

void CGameRenderer::FlipCameraMode(){
  m_bCameraDefaultMode = !m_bCameraDefaultMode; 
} //FlipCameraMode
