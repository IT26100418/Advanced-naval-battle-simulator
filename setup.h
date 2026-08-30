#ifndef SETUP_H
#define SETUP_H
#include "types.h"

//create the initial battlefield
void initializeBattlefield(Battlefield *field);

//battleship details
void setupBattleship(Battleship *battleship);

//create the escort ships
void setupEscortShips(EscortShip escorts[], int count);

//set a random position
void generateRandomPosition(Point *position, double battlefieldSize);

//set random values for escort ship
void setupEscortDetails(EscortShip *escort);

void displayBattlefield(const Battlefield *field);

#endif
