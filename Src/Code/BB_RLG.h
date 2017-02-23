//This header file was created by Luis Rangel for the Bada-Boom!! Project on 10-16-2016
#pragma once
#include<iostream>
//These two headers are for generating random numbers!
#include<cstdlib>
#include<ctime>
//Need these four headers to draw things in the world
#include "spriteman.h"
#include "objman.h"
#include "sprite.h"
#include "object.h"
using namespace std;


//This is the function prototype for the Random Level Generator
//int floor is the current floor the player is on
//bool mode correstponds to wheter the game is in "Story" or "Endless" mode
void BB_RLG(int floor, bool mode);