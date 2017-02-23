#pragma once
//This file will handle all the header information for Hitboxes used by The Bada Boom Project
//Luis Rangel October 31, 2016 Boo!
//Hitboxes will be used to improve the collisons used by the Turkeyberry Engine
//A box will be drawn around an object and if the boxes ever overlap, there is a collison
#include "defines.h"


class Hitbox {
  private:
    Vector3 center;//This vector will keep track of the center of the box
    Vector3 UL, UR, LL, LR;//These vectors will keep track of the cords of the box (Upper/Lower, Left/Right)
    float wide, high;//Keeps track of width and height
  public:
    Hitbox(float height, float width, Vector3& c);
    void UpdateHitbox(Vector3& n);
    void RedefineHitbox(float height, float width, Vector3& c);
    //This is the constructor
    BOOL intersect(Vector3 &p);//This will check to see if the box collides with other boxes
    //Retrivers
    Vector3 getUL() { return UL; }
    Vector3 getUR() { return UR; }
    Vector3 getLL() { return LL; }
    Vector3 getLR() { return LR; }
    float getWide() { return wide; }
    float getHigh() { return high; }
};