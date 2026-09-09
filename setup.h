#ifndef SETUP_H
#define SETUP_H

#include "types.h"
void initializeBattlefield(Battlefield *field);

void setupBattleship(Battleship *battleship,double battlefieldSize);

void setupEscortShips(EscortShip escorts[],int count,double battlefieldSize,Point battleshipPosition);

void setupEscortDetails(EscortShip *escort);

void displayBattlefield(const Battlefield *field);

#endif
