#include "HitBox.h"
//This file will contain all the definitions for Hitboxes used by the Turkeyberry Game Engine
//The file was started by Luis Rangel on October 31, 2016 Spooky!
//This is the Hitbox constructor
Hitbox::Hitbox(float height, float width, Vector3 & c)
{
  wide = width;
  high = height;
  //Making Bounds
  UL = Vector3(c.x - (width / 2), c.y + (height / 2), c.z);
  UR = Vector3(c.x + (width / 2), c.y + (height / 2), c.z);
  LL = Vector3(c.x - (width / 2), c.y - (height / 2), c.z);
  LR = Vector3(c.x + (width / 2), c.y - (height / 2), c.z);
  center = c;//Set the center
}
//Moves the hitbox when the object moves
void Hitbox::UpdateHitbox(Vector3& n) {
  UL = Vector3(n.x - (wide / 2), n.y + (high / 2), n.z);
  UR = Vector3(n.x + (wide / 2), n.y + (high / 2), n.z);
  LL = Vector3(n.x - (wide / 2), n.y - (high / 2), n.z);
  LR = Vector3(n.x + (wide / 2), n.y - (high / 2), n.z);
  center = n;
}
//This allows the hitbox to be modfied in case the Hitbox shouldn't match the sprite dimensions
void Hitbox::RedefineHitbox(float height, float width, Vector3 & c){
  wide = width;
  high = height;
  //Making Bounds
  UL = Vector3(c.x - (width / 2), c.y + (height / 2), c.z);
  UR = Vector3(c.x + (width / 2), c.y + (height / 2), c.z);
  LL = Vector3(c.x - (width / 2), c.y - (height / 2), c.z);
  LR = Vector3(c.x + (width / 2), c.y - (height / 2), c.z);
  center = c;//Set the center
}

BOOL Hitbox::intersect(Vector3 &p){ //Tells wheter a point is in a hitbox
  if (p.x < UR.x && p.x > LL.x && p.y < UL.y && p.y > LR.y) {//If a Point is within bounds
    return TRUE;
  }
  else {//If not within bounds
    return FALSE;
  }
}
