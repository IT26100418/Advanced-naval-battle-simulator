#ifndef SIMULATION_H
#define SIMULATION_H

#include "types.h"

//physics
double calculateDistance(Point a, Point b);
double calculateProjectileRange(double velocity, double angle);

//battle
int isInRange(Point attacker, Point target, double range);
void attackEscort(Battleship *battleship, EscortShip *escort);
void attackBattleship(Battleship *battleship, EscortShip *escort);

//strategy
int chooseTarget(EscortShip escorts[], int count);
void runSimulation(Battlefield *field);
#endif
