#ifndef COMMON_H
#define COMMON_H
#include "types.h"

//calculate the distance between two points
double calculateDistance(Point a, Point b);

//convert degrees to radians
double degreesToRadians(double degrees);

//calculate projectile range
double calculateRange(double velocity, double angle);

//calculate projectile flight time
double calculateFlightTime(double velocity, double angle);

//check whether a shell can hit a target
int canHit(
    Point attacker,
    Point target,
    double velocity,
    double minAngle,
    double maxAngle,
    double *firingAngle,
    double *flightTime);

//convert EscortType into readable text
const char *getEscortTypeName(EscortType type);

//generate a random position inside the battlefield
Point generateRandomPosition(double battlefieldSize);

//generate a random double value between min and max
double randomDouble(double min, double max);

#endif
