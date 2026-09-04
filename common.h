#ifndef COMMON_H
#define COMMON_H

#include "types.h"


/*
 * Calculate the distance between two points.
 */
double calculateDistance(Point a, Point b);


/*
 * Convert degrees to radians.
 */
double degreesToRadians(double degrees);


/*
 * Calculate projectile range.
 */
double calculateRange(double velocity, double angle);


/*
 * Calculate projectile flight time.
 */
double calculateFlightTime(double velocity, double angle);


/*
 * Check whether a shell can hit a target.
 *
 * velocity:
 * Actual velocity of the shell.
 *
 * minAngle:
 * Minimum allowed firing angle.
 *
 * maxAngle:
 * Maximum allowed firing angle.
 *
 * The selected firing angle and flight time
 * are returned through the pointer parameters.
 *
 * Returns:
 * 1 -> target can be hit
 * 0 -> target cannot be hit
 */
int canHit(
    Point attacker,
    Point target,
    double velocity,
    double minAngle,
    double maxAngle,
    double *firingAngle,
    double *flightTime);


/*
 * Convert EscortType into readable text.
 */
const char *getEscortTypeName(EscortType type);


/*
 * Generate a random position inside
 * the battlefield.
 */
Point generateRandomPosition(double battlefieldSize);


/*
 * Generate a random double value
 * between min and max.
 */
double randomDouble(double min, double max);

#endif
