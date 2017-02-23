//Random Level Generator
//Programmed by Luis Rangel for the Bada-Boom!! Project
//Version 0.0.1- Getting everything set up, 9-24-2016

#include<iostream>
#include<cstdlib>//For Rand
#include<ctime>//For Srand
using namespace std;

void BB_RLG(int floor, bool mode);//This function will generate levels for Bada-Boom!!
void BB_RLG(int floor, bool mode){//Passes in the floor number and the mode, 0-Regular Mode, 1-Endless Mode
	int map[48][9] = {0};//This 2D array represents the map of a single floor, 48 * 9 tiles, initialized to 0 which reprsents an empty tile
	int cx, cy;//These represent map coordinates
	int tx, ty;//Temp Variables for map coordinates
	int ti_1, ti_2, ti_3, ti_4;//Temp ints
	int em_1, em_2, em_3;//Number of enemies
	em_1 = 0;
	em_2 = 0;
	em_3 = 0;
	char tc_1;//Temp char
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
	if((floor % 2) != 0){//If the floor is odd numbered, then the spawn is on the left
		ti_2 = 0;
		ti_3 = 47;//Sets the opposite for the exit door
	}
	else{//Right side wall on even numbered floor
		ti_2 = 47;
		ti_3 = 0;
	} 
	ti_1 = (rand() % 6) + 2;//Number between 2 and 8, to set the spawn position, can't spawn on wall 
	if(ti_1 == 8){//Preventing spawn from going off screen
		ti_1--;//Sets ti to 7			
	}
	map[ti_2][ti_1] = 2;//2 represents the spawning zone
	map[ti_2][ti_1 + 1] = 2;
	//Initialize the exit on the other side of the floor
	map[ti_3][ti_1] = 3;//Represents the exit door
	map[ti_3][ti_1 + 1] = 3;
	
	//DESKS-Placing desks, places between 6-11 desks
	ti_1 = (rand() % 5) + 6;//ti_1 represents number of desks
	for(int i = 0; i < ti_1;i++){
		ti_2 = (rand() % 43) + 3;//Temp x coordinate, doesn't place desks within 2 tiles of the endwall
		ti_3 = (rand() % 7) + 2;//Temp y coordinate, doesn't place desks on the wall
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
		ti_3 = (rand() % 7) + 2;//Temp y coordinate, doesn't place plants on the potted plants
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
		ti_2 = (rand() % 43) + 3;//A random x coordinate
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
				if(map[tx][ty] != 4 && map[tx][ty] != 8 && map[tx][ty] != 9){//Prevents Breaking of Desks
					map[tx][ty] = 101;//Sets up the floor tiles 
				}
				if(ty < 2){
					map[tx][ty] = 5;//Fills the backwall with vending machines
				}
				if(ty == 8){
					map[tx][ty] = 102;//Fills the bottom with catering tables
				}
			}
		}
	}
	//ALBINO SQUIRREL ROOM--1/8192 chance of spawning, yes this is a nod to shiny Pokemon
	//The Albino Squirrel is an extremely rare Mini-Boss, it drops the "Golden Gloves" when defeated
	//The backwall is made up of a wood wall and a carpet "Ring" floor
	if(((rand() % 8192) == 0) && (floor != 11)){//Can't Spawn on the same floor as a boss
		cout << "(Albino Squirrel Room) ";
		em_1 = 0;
		em_2 = 0;
		em_3 = 0;
		//Clearing all enemies as the Albino Squirrel Fights Alone
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
	/*//Displaying the map Ids
	for(ty = 0; ty < 9; ty++){//Going down the screen
		for(tx = 0; tx < 48; tx++){//Going across the screen
			if((tx % 48) == 0){//If the tx variable makes it to the end of a row
				cout << endl;//Prints a newline to make things neat
			}
			cout << map[tx][ty] << " ";//Prints out the contents of the map
		}
	}
	cout << endl;//For neatness*/
	
	//Graphical Output, use BB_RLGIDNUM.txt to know what the tiles mean
	cout << "Tier 1 = " << em_1 << " ";//Displays # of Tier 1 enemies
	cout << "Tier 2 = " << em_2 << " ";//Displays # of Tier 1 enemies
	cout << "Tier 3 = " << em_3 << " ";//Displays # of Tier 1 enemies
	for(ty = 0; ty < 9; ty++){//Going down the screen
		for(tx = 0; tx < 48; tx++){//Going across the screen
			if((tx % 48) == 0){//If the tx variable makes it to the end of a row
				cout << endl;//Prints a newline to make things neat
			}
			if(map[tx][ty] == 0){//Empty Space
				cout << ". ";
			}
			if(map[tx][ty] == 1){//End Wall	
				cout << "# ";
			}
			if(map[tx][ty] == 2){//Spawn Zone
				cout << "S ";
			}
			if(map[tx][ty] == 3){//Exit
				cout << "E ";
			}
			if(map[tx][ty] == 4){//Desk
				cout << "[ ";
			}
			if(map[tx][ty] == 5){//Vending Machine
				cout << "V ";
			}
			if(map[tx][ty] == 6){//Potted Plant
				cout << "P ";
			}
			if(map[tx][ty] == 7){//Water Cooler
				cout << "W ";
			}
			if(map[tx][ty] == 8){//Horizontal Desk End
				cout << "> ";
			}
			if(map[tx][ty] == 9){//Vertical Desk End
				cout << "^ ";
			}
			if(map[tx][ty] == 50){//Back Window
				cout << "/ ";
			}
			if(map[tx][ty] == 90){//Tier 1:Dog
				cout << "D ";
			}
			if(map[tx][ty] == 91){//Tier 1:Cat
				cout << "C ";
			}
			if(map[tx][ty] == 92){//Tier 2:Horse
				cout << "H ";
			}
			if(map[tx][ty] == 93){//Tier 3:Horse
				cout << "R ";
			}
			if(map[tx][ty] == 94){//Albino Squirrel
				cout << "$ ";
			}
			if(map[tx][ty] == 95){//Open Air
				cout << ", ";
			}
			if(map[tx][ty] == 96){//Lower Floor
				cout << "_ ";
			}
			if(map[tx][ty] == 100){//Auto-Fling Power-Up
				cout << "A ";
			}
			if(map[tx][ty] == 101){//Break Room Floor
				cout << "- ";
			}
			if(map[tx][ty] == 102){//Catering Table
				cout << "@ ";
			}
			if(map[tx][ty] == 103){//Ring Floor
				cout << "+ ";
			}
			if(map[tx][ty] == 104){//Wood Wall
				cout << "| ";
			}
		}
	}
	cout << endl;//Extra newline to keep things neat
}

int main(){
	srand(time(NULL));//Seeding the RNG
	cout << "\nWelcome to the RLG testing!!" << endl;
	for(int tf = 0; tf < 11; tf++){
		BB_RLG(tf + 1, 0);//The Meat and Potatoes, 0 bit means story mode
	}
	return 0;
}
