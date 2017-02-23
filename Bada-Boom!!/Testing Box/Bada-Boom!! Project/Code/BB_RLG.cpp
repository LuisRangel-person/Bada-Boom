//Random Level Generator
//Programmed by Luis Rangel for the Bada-Boom!! Project
//Version 0.0.1- Getting everything set up, 9-24-2016
//Version 0.0.2-Cramming this into the Bada-Boom!! Project 10-16-2016
//Version 0.0.3-Adding effects to certain floors and orienting the floors correctly in the game world 10-19-2016
#include "BB_RLG.h"
#include "sound.h"

extern CSoundManager* g_pSoundManager;
extern GameState m_nGameState;
extern CObjectManager g_cObjectManager;
extern CSpriteManager g_cSpriteManager;
extern int g_nScreenWidth;
extern int g_nScreenHeight;

///Luis-The Main Level Generation function, call this to generate a floor
///param:The current floor number and a bool representing whether the game is in Endless Mode(1) or Story Mode(0)
void BB_RLG(int floor, bool mode){
	int map[48][9] = {0};//This 2D array represents the map of a single floor, 48 * 9 tiles, initialized to 0 which reprsents an empty tile
	int tx, ty;//Temp Variables for map coordinates
	int ti_1, ti_2, ti_3, ti_4;//Temp ints
	int em_1, em_2, em_3;//Number of enemies
	int totEne = 0;//Total number of enemies
	float zMod;//For Modiflying enemy z value
  int BossCre = FALSE;
	g_pSoundManager->loop(20);

  //srand(floor);//Seeding the RLG, it will be the floor for debugging purposes
  srand(time(NULL));//Luis-Random Seeding for the final presentation, Now Seeded with Time
  //Bools to tell if the floor is special
  BOOL isALSQ = FALSE;//Did the floor spawn an ALSQ
  BOOL isBrkRm = FALSE;//Id the floor a break room?
  BOOL endCre = FALSE;//This keep track if the goal has been made yet
  BOOL teleported = FALSE;//Keeps track if gator has been teleported
  BOOL vendDrawn = FALSE;//Keeps track if the vending machines have been drawn yet
  BOOL evenFlr;//This bool keeps track if the floor number is even
  CGameObject* pgator = g_cObjectManager.GetObjectByName("gator");//Getting the gator
  Vector3 v; //Vector for placing the tiles
  Vector3 ele;//Vector for building elevator
	em_1 = 0;
	em_2 = 0;
	em_3 = 0;
	cout << "\nRLG: Floor: " << floor << endl;
	//Initialize the back windows, the first 2 rows represent the wall
	for(ty = 0; ty < 2;ty++){
		for(tx = 0; tx < 48; tx++){
			map[tx][ty] = 50;//50 represents empty wall space
		}
	}
	//Initialize end walls, each side of the wall will have an end wall made up of a column of 1 tile
	for(ty = 0; ty < 9; ty++){
		map[0][ty] = 1;//1 represents an end wall, left wall
		map[47][ty] = 1;//Right wall
	}
	//Initialize Spawn, the spawn zone is 1 * 2 tiles and is on one of the end walls
	if((floor % 2) != 0){//If the floor is odd numbered, then the spawn is on the right
	 pgator->UpdateSprite(GATORLEFT_OBJECT);//Luis- Changing the Sprite to face left
	 pgator->turnFace(FALSE);
	 ti_2 = 0;
	 ti_3 = 47;//Sets the opposite for the exit door
	 evenFlr = FALSE;
	}
	else{//Left side wall on even numbered floor
    pgator->UpdateSprite(GATOR_OBJECT);//Luis- Changing the Sprite to face left
    pgator->turnFace(TRUE);
		ti_2 = 47;
		ti_3 = 0;
		evenFlr = TRUE;
	} 
	ti_1 = 4;//Set Position to make Trasitions easier
	map[ti_2][ti_1 - 2] = 10;//10-Represents a dummywall
	map[ti_2][ti_1 - 1] = 10;//10-Represents a dummywall
	map[ti_2][ti_1] = 2;//2 represents the spawning zone
	map[ti_2][ti_1 + 1] = 2;
	//Initialize the exit on the other side of the floor
	map[ti_3][ti_1 - 2] = 10;//10-Represents a dummywall
	map[ti_3][ti_1 - 1] = 10;//10-Represents a dummywall
	map[ti_3][ti_1] = 3;//Represents the exit door
	map[ti_3][ti_1 + 1] = 3;
	
	//DESKS-Placing desks, places between 6-11 desks
	ti_1 = (rand() % 5) + 6;//ti_1 represents number of desks
	for(int i = 0; i < ti_1;i++){
		ti_2 = (rand() % 41) + 5;//Temp x coordinate, doesn't place desks within 2 tiles of the endwall
		ti_3 = (rand() % 5) + 3;//Temp y coordinate, doesn't place desks on the wall
		if(map[ti_2][ti_3] == 0){//4 represents a desk piece
			map[ti_2][ti_3] = 4;
			if((rand() % 2) == 0){//Decided if the Desk is Horizontal or Vertical, 0 Horizontal
				if(map[ti_2 + 1][ti_3] == 0){//Completing Horizontal Desk
					map[ti_2 + 1][ti_3] = 8;
				} 
				else{
					if(map[ti_2 - 1][ti_3] == 0){//Tries the other side 
						map[ti_2 - 1][ti_3] = 8;
					}
					else{
						i--;//Try again if the tests failed
					}
				}
			}
			else{//Placing Vertical Desks
				if(map[ti_2][ti_3 - 1] == 0){//Completing Horizontal Desk
					map[ti_2][ti_3 - 1] = 9;
				} 
				else{
					if(map[ti_2][ti_3 + 1] == 0){//Tries the other side 
						map[ti_2][ti_3 + 1] = 9;
					}
					else{
						i--;//Try again if the tests failed
					}
				}
			}
		}
		else{//Sets back the loop so it can try again if it can't place a desk
			i--;
		}
	}
	
	//PLANTS-Placing Potted Plants, places between 3-5 potted plants
	ti_1 = (rand() % 2) + 3;//ti_1 represents number of plants
	for(int i = 0; i < ti_1;i++){
		ti_2 = (rand() % 43) + 3;//Temp x coordinate, doesn't place potted plants within 2 tiles of the endwall
		ti_3 = (rand() % 5) + 4;//Temp y coordinate, doesn't place plants on the window
		if(map[ti_2][ti_3] == 0){//6 represents a plants piece
			map[ti_2][ti_3] = 6;
		}
		else{//Sets back the loop so it can try again if it can't place a desk
			i--;
		}
	}
	
	//WATERCOOLER-Generating a water cooler
	ti_3 = (rand() % 43) + 3;//A random x coordinate
	if(map[ti_3][0] == 50 && map[ti_3][2] == 0){//Tile in front of the cooler should be empty
		map[ti_3][0] = 7;//7 is for water cooler
		map[ti_3][1] = 7;
	}
	//Enemy Block
	//Tier 1
	//Placing enemies, number depends on floor level
	em_1 = (rand() % 5) + (floor % 10);//ti_1 represents number of Tier 1 enemies
	if(floor >= 5 && em_1 >= 4){//Keeps the enemy number under control
		em_1 -= 4;
	}
	for(int i = 0; i < em_1;i++){
		ti_2 = (rand() % 43) + 3;//Temp x coordinate, doesn't place potted enemies within 2 tiles of the endwall
		ti_3 = (rand() % 7) + 2;//Temp y coordinate, doesn't place enemies on the potted plants
		if(map[ti_2][ti_3] == 0){//90 or 91 represents Tier 1 enemies
			ti_4 = (rand() % 2);//Decided if the enemy is a dog of a cat
			if(ti_4 != 0){
				map[ti_2][ti_3] = 90;//90-Is for dog
			}
			else{
				map[ti_2][ti_3] = 91;//91-Is for cat
			}
		}
		else{//Sets back the loop so it can try again if it can't place a desk
			i--;
		}
	}
	//Tier 2
	if(floor >= 5){
		em_2 = (rand() % 2) + (floor % 5) + 1;//ti_1 represents number of Tier 2 enemies
		for(int i = 0; i < em_2;i++){
			ti_2 = (rand() % 43) + 3;//Temp x coordinate, doesn't place potted enemies within 2 tiles of the endwall
			ti_3 = (rand() % 7) + 2;//Temp y coordinate, doesn't place enemies on the wall
			if(map[ti_2][ti_3] == 0){//92 represents Tier 1 enemies
				map[ti_2][ti_3] = 92;//92-Is for Horse
			}
			else{//Sets back the loop so it can try again if it can't place a desk
				i--;
			}
		}
	}
	//Tier 3
	if(floor >= 8){
		if(floor < 25){
			em_3 = (rand() % 2) + 1;//ti_1 represents number of Tier 3 enemies
		}
		if(floor >= 25){//Higher Up Floors get More Rhinos
			em_3 = (rand() % 2) + (floor % 3);//ti_1 represents number of Tier 3 enemies
		}
		if(floor == 10){//Extra Rhino on floor 10
			em_3 += 1;
		}
		for(int i = 0; i < em_3;i++){
			ti_2 = (rand() % 43) + 3;//Temp x coordinate, doesn't place potted enemies within 2 tiles of the endwall
			ti_3 = (rand() % 7) + 2;//Temp y coordinate, doesn't place enemies on wall
			if(map[ti_2][ti_3] == 0){//93 represents Tier 3 enemies
				map[ti_2][ti_3] = 93;//93-Is for Rhino
			}
			else{//Sets back the loop so it can try again if it can't place a desk
				i--;
			}
		}
	}
	//The Boss
	//Clear all
	if(mode == 0 && floor == 11){
		cout << "\n(Boss) ";
		em_1 = 0;
		em_2 = 0;
		em_3 = 0;
		for(ty = 0; ty < 9; ty++){//Going down the screen
			for(tx = 0; tx < 48; tx++){//Going across the screen
				if(map[tx][ty] != 0){
					map[tx][ty] = 0;//If an enemy is found on a tile, empty the tile
				}
			}
		}
		//Setting up the Boss Arena, the Roof if in Story Mode
		for(ty = 0; ty < 9;ty++){
			for(tx = 0; tx < 48; tx++){
				if(ty < 3){
					map[tx][ty] = 95;//95 is open air
				}
				if(ty > 2){
					if(tx < 5 || tx > 42){
						if(ty == 3){
							map[tx][ty] = 1;
						}
						else{
							map[tx][ty] = 96;//96 is a Lower Floor
						}
					}
				}
			}
		}
		//Setting up the cage
		for(ty = 2; ty < 9;ty++){
			map[5][ty] = 1;//Endwalls count as cages in a Boss Arena
			map[42][ty] = 1;
			if(ty > 2){
				map[0][ty] = 1;
				map[47][ty] = 1;
			}
		}
		for(tx = 5; tx < 42; tx++){
			map[tx][2] = 1;
			map[tx][8] = 1;
		}
		//New Spawn
		if((floor % 2) != 0){//If the floor is odd numbered, then the spawn is on the left
			ti_2 = 0;
		}
		else{//Right side wall on even numbered floor
			ti_2 = 47;
		} 
		map[ti_2][7] = 2;//Always on Bottom
		map[ti_2][8] = 2;
	}
	cout << endl;//For Neatness
	//BONUSES
	//VENDING--1/3 chance every floor
	//Contains lots of snacks and drinks to heal up
	ti_1 = (rand() % 3);//Random number 0 - 3
	if(ti_1 == 0){//a vending machine, they come in pairs
		cout << "(Vending) ";
		ti_2 = (rand() % 41) + 5;//A random x coordinate
		map[ti_2][0] = 5;//5 is for vending machine
		map[ti_2][1] = 5;
		map[ti_2 + 1][0] = 5;//5 is for vending machine next to the other
		map[ti_2 + 1][1] = 5;
	}
	//AUTO_FLING--1/25 Chance of spawning on a Floor
	//Automatically Fling Enemies with Punches
	if((rand() % 25) == 0){
			cout << "(Auto-Fling) " ;
			ti_4 = 0;//reset temp 4
			while(ti_4 == 0){//Keep Trying until an empty space is found
				ti_2 = (rand() % 43) + 3;//Temp x coordinate, doesn't place potted plants within 2 tiles of the endwall
				ti_3 = (rand() % 7) + 2;//Temp y coordinate, doesn't place plants on the potted plants
				if(map[ti_2][ti_3] == 0){//100 represents The Auto-Fling Powerup
					map[ti_2][ti_3] = 100;
					ti_4++;
				}
			}
	}
	//BREAK ROOM--1/77 chance of spawning
	//They have a back wall made up of vending machines, and catering tables along the bottom of the screen
	if(((rand() % 77) == 0) && (floor != 11) && (floor != 1)){//Can't Spawn on the same floor as a boss, and it doesn't make sense to spawn one on the first floor
		cout << "(Break Room) ";
    isBrkRm = TRUE;
		em_1 = 0;
		em_2 = 0;
		em_3 = 0;
		//Clearing all enemies as Break Rooms are safe zones
		for(ty = 0; ty < 9; ty++){//Going down the screen
			for(tx = 0; tx < 48; tx++){//Going across the screen
				if(map[tx][ty] == 90 || map[tx][ty] == 91 || map[tx][ty] == 92 || map[tx][ty] == 93){
					map[tx][ty] = 0;//If an enemy is found on a tile, empty the tile
				}
			}
		}
		for(ty = 0; ty < 9;ty++){//Going down the screen
			for(tx = 15; tx < 32; tx++){//The Break Room
				if(map[tx][ty] != 4 && map[tx][ty] != 8 && map[tx][ty] != 9 && ty > 1){//Prevents Breaking of Desks
					map[tx][ty] = 101;//Sets up the floor tiles 
				}
				if(ty < 2 && (tx % 2) == 0){//Every other space on the wall is a vending machine
					map[tx][ty] = 5;
				}
				if(ty == 8){
					map[tx][ty] = 102;//Fills the bottom with catering tables
				}
			}
		}
	}
	//ALSQ ROOM--1/8192 chance of spawning, yes this is a nod to shiny Pokemon
	//The ALSQ is an extremely rare Mini-Boss, it drops the "Golden Gloves" when defeated
	//The backwall is made up of a wood wall and a carpet "Ring" floor
	if(((rand() % 8192) == 0) && (floor != 11) && (floor != 1)){//Can't Spawn on the same floor as a boss or on the first floor
		cout << "(ALSQ) ";
    isALSQ = TRUE;//FLOOR HAS ALSQ
		em_1 = 0;
		em_2 = 0;
		em_3 = 0;
		//Clearing all enemies as the ALSQ
		for(ty = 0; ty < 9; ty++){//Going down the screen
			for(tx = 0; tx < 48; tx++){//Going across the screen
				if(map[tx][ty] == 90 || map[tx][ty] == 91 || map[tx][ty] == 92 || map[tx][ty] == 93 || map[tx][ty] == 4 || map[tx][ty] == 8 || map[tx][ty] == 9){
					map[tx][ty] = 0;//If an enemy or desk is found on a tile, empty the tile
				}
			}
		}
		for(ty = 0; ty < 9;ty++){//Going down the screen
			for(tx = 15; tx < 32; tx++){//The Battlefield
				map[tx][ty] = 103;//Sets up the floor tiles 
				if(ty < 2){
					map[tx][ty] = 104;//Fills the backwall with a wood wall
				}
				if(tx > 21 && tx < 25 && ty == 2){//Sets up a catering table at the top
					map[tx][ty] = 102;
				}
			}
		}
		map[23][3] = 94;//Places the ALSQ
	}

	
	cout << "Tier 1 = " << em_1 << " ";//Displays # of Tier 1 enemies
	cout << "Tier 2 = " << em_2 << " ";//Displays # of Tier 2 enemies
	cout << "Tier 3 = " << em_3 << " ";//Displays # of Tier 3 enemies
  for (int i = 0; i < 48; i++) {//Using this for loop to draw the ceiling
    v = Vector3((float)i * -72.0f, 772.0f, 600.0f);
    g_cObjectManager.createObject(ROOF_OBJECT, "roof", v, Vector3(0, 0, 0));
  }
  //Graphical Output, use BB_RLGIDNUM.txt to know what the tiles mean
	for(ty = 0; ty < 9; ty++){//Going down the screen
		for(tx = 0; tx < 48; tx++){//Going across the screen
			zMod = 155 - 2.0f*(4 / 2 - ty + 0.5f);
      v.x = (float)tx * -72.0f;//Float * xPos * width of the tile(72 pixels)(Negative so the tiles appear in the right orientation
      v.y = (float)ty * -72.0f + 700.0f;//Float * yPos * width of the tile(72 pixels) + 700(to appear on screen)
      v.z = 600.0f;//Constant z value so its behind the action plane

      //Matching symbol in array to tile
			if(map[tx][ty] == 0){//Empty Space/Carpet
				cout << ". ";
        if (isALSQ) {//Gold Room
          g_cObjectManager.createObject(GDCARPET_OBJECT, "goldcarpet", v, Vector3(0, 0, 0));
        }
        else if (isBrkRm) {//Break Room
          g_cObjectManager.createObject(BKRMA_OBJECT, "breakfloorA", v, Vector3(0, 0, 0));
        }
        else {
          g_cObjectManager.createObject(CARPET_OBJECT, "carpet", v, Vector3(0, 0, 0));
        }
			}
			if(map[tx][ty] == 1){//End Wall	
				cout << "# ";
        g_cObjectManager.createObject(ENDWALL_OBJECT, "endwall", v, Vector3(0, 0, 0));
			}
			if(map[tx][ty] == 2){//Spawn Zone
				cout << "S ";
        g_cObjectManager.createObject(ELEVATOR_OBJECT, "elevator", v, Vector3(0, 0, 0));
        if (!teleported) {
          v.y += 10.0f;
          if (evenFlr) {//If on even numbered floor
            v.x -= 118.0f;
          }
          else {//Odd numbered floor
            v.x += 118.0f;
          }
          v.z = 598.0f - (ty * 10.0f);//Luis-Doing it this way to make the z values look right, 598 instead of 600 to prevent clipping
          pgator->teleport(v);//Places gator at spawn
          v.x = tx * -74.0f;//Resetting the tile values
          v.y -= 10.0f;
          v.z = 600;
          if (!evenFlr) {//If on odd numbered floor
            ele = v;
            ele.x += 72.0f + 72.0f + 36.0f;
            ele.y -= 72.0f - 36.0f;
            g_cObjectManager.createObject(ELEVATORIN_OBJECT, "elevatorin", ele, Vector3(0, 0, 0));
            ele.x += 36.0f + 72.0f + 72.0f; 
            ele.y += 72.0f;
            for (int j = 0; j < 4; j++) {
              g_cObjectManager.createObject(BLOCK_OBJECT, "block", ele, Vector3(0, 0, 0));
              ele.y -= 72.0f;
            }//for
            ele.x -= 72.0f;
            ele.y += 5.0f;
            for (int j = 0; j < 4; j++) {
              g_cObjectManager.createObject(BLOCK_OBJECT, "block", ele, Vector3(0, 0, 0));
              ele.x -= 72.0f;
            }//for
            ele.y += (72.0f * 3);
            ele.y -= 5.0f;
            ele.x += 36.0f - 72.0f - 72.0f - 72.0f + 72.0f + (72.0f * 4);
            ele.y += 258.0f;
            g_cObjectManager.createObject(ELEVATORWALL_OBJECT, "elevatorwall", ele, Vector3(0, 0, 0));
          }
          else {//Even numbered floor
            ele = v;
            ele.x -= 72.0f + 15.0f;
            ele.y -= 72.0f - 36.0f;
            g_cObjectManager.createObject(ELEVATORIN_OBJECT, "elevatorin", ele, Vector3(0, 0, 0));
            ele.x -= 36.0f + 72.0f + 72.0f;
            ele.y += 108.0f;
            for (int j = 0; j < 5; j++) {
              g_cObjectManager.createObject(BLOCK_OBJECT, "block", ele, Vector3(0, 0, 0));
              ele.y -= 72.0f;
            }//for
            ele.x += 72.0f;
            ele.y += 5.0f;
            for (int j = 0; j < 4; j++) {
              g_cObjectManager.createObject(BLOCK_OBJECT, "block", ele, Vector3(0, 0, 0));
              ele.x += 72.0f;
            }//for
            ele.y += (72.0f * 3) + 36.0f;
            ele.x += 36.0f - 72.0f + 72.0f + 72.0f - (72.0f * 4);
            ele.y += 258.0f;
           g_cObjectManager.createObject(ELEVATORWALL_OBJECT, "elevatorwall", ele, Vector3(0, 0, 0));
          }
          teleported = TRUE;
        }
			}
			if(map[tx][ty] == 3){//Exit
				cout << "E ";
        g_cObjectManager.createObject(ELEVATOR_OBJECT, "elevator", v, Vector3(0, 0, 0));
        if (!endCre) {//Place Only one Goal Point
          if (evenFlr) {//If on even numbered floor
            ele = v;
            ele.x += 72.0f + 72.0f + 36.0f;
            ele.y -= 72.0f - 36.0f;
            g_cObjectManager.createObject(ELEVATORIN_OBJECT, "elevatorin", ele, Vector3(0, 0, 0));
            ele.x += 36.0f + 72.0f + 72.0f;
            ele.y += 72.0f;
            for (int j = 0; j < 4; j++) {
              g_cObjectManager.createObject(BLOCK_OBJECT, "block", ele, Vector3(0, 0, 0));
              ele.y -= 72.0f;
            }//for
            ele.x -= 72.0f;
            ele.y += 5.0f;
            for (int j = 0; j < 4; j++) {
              g_cObjectManager.createObject(BLOCK_OBJECT, "block", ele, Vector3(0, 0, 0));
              ele.x -= 72.0f;
            }//for
            ele.y += (72.0f * 3);
            ele.y -= 5.0f;
            ele.x += 36.0f - 72.0f - 72.0f - 72.0f + 72.0f + (72.0f * 4);
            ele.y += 258.0f;
           g_cObjectManager.createObject(ELEVATORWALL_OBJECT, "elevatorwall", ele, Vector3(0, 0, 0));
          }
          else {//Odd numbered floor
            ele = v;
            ele.x -= 72.0f + 72.0f + 36.0f;
            ele.y -= 72.0f - 36.0f;
            g_cObjectManager.createObject(ELEVATORIN_OBJECT, "elevatorin", ele, Vector3(0, 0, 0));
            ele.x -= 36.0f + 72.0f + 72.0f;
            ele.y += 108.0f;
            for (int j = 0; j < 5; j++) {
              g_cObjectManager.createObject(BLOCK_OBJECT, "block", ele, Vector3(0, 0, 0));
              ele.y -= 72.0f;
            }//for
            ele.x += 72.0f;
            ele.y += 5.0f;
            for (int j = 0; j < 4; j++) {
              g_cObjectManager.createObject(BLOCK_OBJECT, "block", ele, Vector3(0, 0, 0));
              ele.x += 72.0f;
            }//for
            ele.y += (72.0f * 3) + 36.0f;
            ele.x += 36.0f - 72.0f + 72.0f + 72.0f - (72.0f * 4);
            ele.y += 258.0f;
            g_cObjectManager.createObject(ELEVATORWALL_OBJECT, "elevatorwall", ele, Vector3(0, 0, 0));
          }
          if (!evenFlr) {//If on odd numbered floor
            v.x -= 181.0f;
          }
          else {//Even numbered floor
            v.x += 181.0f;
          }
          v.z -= 3.0f;//To see goal marker
          g_cObjectManager.createObject(ELEVATOREN_OBJECT, "elevatoren", v, Vector3(0, 0, 0));
          v.z + 3.0f;
          endCre = TRUE;//A Goal has been made
        }
        v.x = tx * -74.0f;//Resetting the tile values
			}
			if(map[tx][ty] == 4){//Desk
				cout << "[ ";
        if (isBrkRm) {//Break Room, fixes weird patches
          g_cObjectManager.createObject(BKRMA_OBJECT, "breakfloorA", v, Vector3(0, 0, 0));
        }
        else {
          g_cObjectManager.createObject(DESKPRINT_OBJECT, "deskprint", v, Vector3(0, 0, 0));
        }
			}
			if(map[tx][ty] == 5){//Vending Machine
				cout << "V ";
        if (isALSQ == TRUE) {
          g_cObjectManager.createObject(GDOUTLET_OBJECT, "goldoutlet", v, Vector3(0, 0, 0));
        }
        else {
          g_cObjectManager.createObject(OUTLET_OBJECT, "outlet", v, Vector3(0, 0, 0));
          if (ty == 1) {//Only draws 1 watercooler, draws on the second row
            v.z = 600.0f - (ty * 48.0f);//Luis-Doing it this way to make the z values look right
            if (isBrkRm) {//If floor is break room, draw like normal
              g_cObjectManager.createObject(VENDINGMACHINE_OBJECT, "vendingmachine", v, Vector3(0, 0, 0)); //Alfredo - vending machine on tile
            }
            else {
              if (!vendDrawn) {//If there's no vending machines yet, darw the vending machine pair
                v.x -= 68.0f;
                g_cObjectManager.createObject(VENDINGMACHINE_OBJECT, "vendingmachine", v, Vector3(0, 0, 0));//Left Vending Machine
                v.x += 136.0f;
                g_cObjectManager.createObject(VENDINGMACHINE_OBJECT, "vendingmachine", v, Vector3(0, 0, 0));//Right Vending Machine
                v.x -= 68.0f;
                vendDrawn = TRUE;//The Vending Machines have been drawn
              }
            }
            v.z = 600.0f;
          }
        }
			}
		if(map[tx][ty] == 6){//Potted Plant
		cout << "P ";
		 if (isALSQ) {
          g_cObjectManager.createObject(GDPLANTPOST_OBJECT, "goldplantpost", v, Vector3(0, 0, 0));
        }
        else if (isBrkRm) {//Break Room, fixes weird patches
          g_cObjectManager.createObject(BKRMA_OBJECT, "breakfloorA", v, Vector3(0, 0, 0));
        }
        else {
          g_cObjectManager.createObject(PLANTPOST_OBJECT, "plantpost", v, Vector3(0, 0, 0));
        }
        v.y += 64.0f;//Makin the plants appear on the center of the groove
        v.z = 600.0f - (ty * 48.0f) + 35.0f;//Luis-Doing it this way to make the z values look right
		if (ty > 5) {//Adding this because the z shifting on the lower half of the screen acts weird
			v.z -= 55.0f;
		}
        g_cObjectManager.createObject(POTTEDPLANT_OBJECT, "pottedplant", v, Vector3(0, 0, 0)); //Alfredo - pottedplant on tile
        v.z = 600.0f;
        v.y -= 64.0f;
			}
		if(map[tx][ty] == 7){//Water Cooler
				cout << "W ";
        if (isALSQ == TRUE) {
          g_cObjectManager.createObject(GDOUTLET_OBJECT, "goldoutlet", v, Vector3(0, 0, 0));
        }
        else {
          g_cObjectManager.createObject(OUTLET_OBJECT, "outlet", v, Vector3(0, 0, 0));
          if (ty == 1) {//Only draws 1 watercooler, draws on the second row
            v.z = 600.0f - (ty * 48.0f);//Luis-Doing it this way to make the z values look right
            g_cObjectManager.createObject(WATERCOOLER_OBJECT, "watercooler", v, Vector3(0, 0, 0)); //Alfredo - water cooler on tile
            v.z = 600.0f;
          }
        }
			}
			if(map[tx][ty] == 8){//Horizontal Desk End
				cout << "> ";
        if (isBrkRm) {//Break Room, fixes weird patches
          g_cObjectManager.createObject(BKRMA_OBJECT, "breakfloorA", v, Vector3(0, 0, 0));
        }
        else {
          g_cObjectManager.createObject(DESKPRINT_OBJECT, "deskprint", v, Vector3(0, 0, 0));
        }
        v.y += 45.0f;
        v.z = 600.0f - (ty * 48.0f);//Luis-Doing it this way to make the z values look right, little offset cause desks looked weird
        v.x += 8.0f;
		if (ty > 5) {//Adding this because the z shifting on the lower half of the screen acts weird
			v.z -= 55.0f;
		}
        g_cObjectManager.createObject(DESKHORIZONTAL_OBJECT, "deskhorizontal", v, Vector3(0, 0, 0)); //Alfredo - desk horizontal on tile
        v.x -= 8.0f;
        v.z = 600.0f;
        v.y -= 45.0f;
			}
			if(map[tx][ty] == 9){//Vertical Desk End
				cout << "^ ";
				if (isALSQ) {//Gold Room
					g_cObjectManager.createObject(GDCARPET_OBJECT, "goldcarpet", v, Vector3(0, 0, 0));
				}
				else if (isBrkRm) {//Break Room
					g_cObjectManager.createObject(BKRMA_OBJECT, "breakfloorA", v, Vector3(0, 0, 0));
				}
				else {
					g_cObjectManager.createObject(CARPET_OBJECT, "carpet", v, Vector3(0, 0, 0));
				}
			}
      if (map[tx][ty] == 10) {//Dummy Wall
        cout << "= ";
        g_cObjectManager.createObject(DUMMYWALL_OBJECT, "dummywall", v, Vector3(0, 0, 0));
      }
			if(map[tx][ty] == 50){//Back Window
        if (isALSQ == TRUE) {
          g_cObjectManager.createObject(GDWINDOW_OBJECT, "goldwindow", v, Vector3(0, 0, 0));
        }
        else {
          g_cObjectManager.createObject(WINDOW_OBJECT, "window", v, Vector3(0, 0, 0));
        }
			}
			if(map[tx][ty] == 90){//Tier 1:Dog
				cout << "D ";
			   g_cObjectManager.createObject(DOGPOST_OBJECT, "dogpost", v, Vector3(0, 0, 0)); 
         if (rand() % 2 == 0) {
           v.z = v.z - zMod;
           g_cObjectManager.createObject(DOG_OBJECT, "dog", v, Vector3(0, 0, 0)); //Alfredo - Insert dog on this tile
           v.z = v.z + zMod;
         }
         else {
           v.z = v.z - zMod - 1;
          g_cObjectManager.createObject(GORILLA_OBJECT, "gorilla", v, Vector3(0, 0, 0)); //Alfredo - Gorilla instead of cat, just testing
           v.z = v.z + zMod + 1;
         }
         totEne++;
			}
			if(map[tx][ty] == 91){//Tier 1:Cat
				cout << "C ";
        g_cObjectManager.createObject(CATPOST_OBJECT, "catpost", v, Vector3(0, 0, 0));
        v.z = v.z - zMod - 1;
        g_cObjectManager.createObject(GORILLA_OBJECT, "gorilla", v, Vector3(0, 0, 0)); //Alfredo - Gorilla instead of cat, just testing
        v.z = v.z + zMod + 1;

			}
			if(map[tx][ty] == 92){//Tier 2:Horse
				cout << "H ";
        g_cObjectManager.createObject(HORSEPOST_OBJECT, "horsepost", v, Vector3(0, 0, 0));
        g_cObjectManager.createObject(DOGPOST_OBJECT, "dogpost", v, Vector3(0, 0, 0));
        if (rand() % 2 == 0) {
          v.z = v.z - zMod;
          g_cObjectManager.createObject(HORSE_OBJECT, "horse", v, Vector3(0, 0, 0)); //Alfredo - Horse addition for testing
          v.z = v.z + zMod;
        }
        else {
          v.z = v.z - zMod - 1;
          g_cObjectManager.createObject(RHINO_OBJECT, "rhino", v, Vector3(0, 0, 0)); //Alfredo - Gorilla instead of cat, just testing
          v.z = v.z + zMod + 1;
        }
        totEne++;
			}
      if (map[tx][ty] == 93) {//Tier 3:Rhino
        cout << "R ";
        g_cObjectManager.createObject(RHINOPOST_OBJECT, "rhinopost", v, Vector3(0, 0, 0));
        v.z = v.z - zMod - 1;
        //g_cObjectManager.createObject(RHINO_OBJECT, "rhino", v, Vector3(0, 0, 0)); //Alfredo - rhino addition for testing
        if ((floor % 10) == 0 && !BossCre){
          g_cObjectManager.createObject(BOSSBULL_OBJECT, "bossbull", v, Vector3(0, 0, 0)); //Alfredo - boss bull addition for testing
          BossCre = TRUE;
          m_nGameState = BOSSROOMSCREEN_STATE; 
         }
        else {
          g_cObjectManager.createObject(RHINO_OBJECT, "rhino", v, Vector3(0, 0, 0)); //Alfredo - rhino addition for testing
        }
        v.z = v.z + zMod + 1;
			}
			if(map[tx][ty] == 94){//Albino Squirrel
				cout << "$ ";
        g_cObjectManager.createObject(RINGFLOOR_OBJECT, "ringfloor", v, Vector3(0, 0, 0));
			}
			if(map[tx][ty] == 95){//Open Air
				cout << ", ";
			}
			if(map[tx][ty] == 96){//Lower Floor
				cout << "_ ";
			}
			if(map[tx][ty] == 100){//Auto-Fling Power-Up
				cout << "A ";
        g_cObjectManager.createObject(CARPET_OBJECT, "carpet", v, Vector3(0, 0, 0));
			}
			if(map[tx][ty] == 101){//Break Room Floor
				cout << "- ";
        g_cObjectManager.createObject(BKRMA_OBJECT, "breakfloorA", v, Vector3(0, 0, 0));
			}
			if(map[tx][ty] == 102){//Catering Table
				cout << "@ ";
        g_cObjectManager.createObject(BKRMA_OBJECT, "breakfloorA", v, Vector3(0, 0, 0));
			}
			if(map[tx][ty] == 103){//Ring Floor
				cout << "+ ";
        g_cObjectManager.createObject(RINGFLOOR_OBJECT, "ringfloor", v, Vector3(0, 0, 0));
			}
			if(map[tx][ty] == 104){//Wood Wall
				cout << "| ";
        g_cObjectManager.createObject(WOODWALL_OBJECT, "woodwall", v, Vector3(0, 0, 0));
			}
		}
	}
  g_cObjectManager.setEne(totEne);//Gives Game Manager the number of enemies
	cout << endl;//Extra newline to keep things neat
}

