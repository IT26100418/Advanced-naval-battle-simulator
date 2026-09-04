#include <stdlib.h>
#include <math.h>

#include "common.h"


/*
 * Calculate the distance between two points.
 *
 * Formula:
 *
 * distance = sqrt(dx^2 + dy^2)
 */
double calculateDistance(Point a, Point b)
{
    double dx;
    double dy;

    dx = b.x - a.x;
    dy = b.y - a.y;

    return sqrt(dx * dx + dy * dy);
}


/*
 * Convert degrees to radians.
 */
double degreesToRadians(double degrees)
{
    return degrees * PI / 180.0;
}


/*
 * Calculate projectile range.
 *
 * Formula:
 *
 * R = v^2 sin(2theta) / g
 */
double calculateRange(double velocity, double angle)
{
    double radians;

    radians = degreesToRadians(angle);

    return (velocity * velocity *
            sin(2.0 * radians)) / GRAVITY;
}


/*
 * Calculate projectile flight time.
 *
 * Formula:
 *
 * T = 2v sin(theta) / g
 */
double calculateFlightTime(double velocity, double angle)
{
    double radians;

    radians = degreesToRadians(angle);

    return (2.0 * velocity *
            sin(radians)) / GRAVITY;
}


/*
 * Check whether a shell can hit the target.
 *
 * Projectile range equation:
 *
 * R = v^2 sin(2theta) / g
 *
 * Therefore:
 *
 * sin(2theta) = Rg / v^2
 *
 * There can be two possible angles.
 *
 * The first valid angle inside the allowed
 * angle range is selected.
 */
int canHit(
    Point attacker,
    Point target,
    double velocity,
    double minAngle,
    double maxAngle,
    double *firingAngle,
    double *flightTime)
{
    double distance;
    double value;

    double lowerAngle;
    double upperAngle;


    /*
     * A shell with zero or negative velocity
     * cannot reach another point.
     */
    if (velocity <= 0.0)
    {
        return 0;
    }


    /*
     * Check that the angle range is valid.
     */
    if (minAngle < 0.0 ||
        maxAngle > 90.0 ||
        minAngle > maxAngle)
    {
        return 0;
    }


    /*
     * Find the distance to the target.
     */
    distance = calculateDistance(
        attacker,
        target
    );


    /*
     * If attacker and target are at the
     * same position, no firing is needed.
     */
    if (distance <= 0.000001)
    {
        *firingAngle = minAngle;
        *flightTime = 0.0;

        return 1;
    }


    /*
     * Calculate:
     *
     * value = Rg / v^2
     */
    value =
        (distance * GRAVITY) /
        (velocity * velocity);


    /*
     * If value is greater than 1,
     * asin() cannot produce a solution.
     *
     * Therefore the target is outside
     * the maximum possible range.
     */
    if (value > 1.0)
    {
        return 0;
    }


    /*
     * Small floating point errors can
     * sometimes make value slightly
     * smaller than zero.
     */
    if (value < 0.0)
    {
        return 0;
    }


    /*
     * First possible firing angle.
     *
     * theta = 1/2 asin(value)
     */
    lowerAngle =
        0.5 * asin(value) * 180.0 / PI;


    /*
     * Second possible firing angle.
     *
     * theta2 = 90 - theta1
     */
    upperAngle =
        90.0 - lowerAngle;


    /*
     * Check the lower angle first.
     */
    if (lowerAngle >= minAngle &&
        lowerAngle <= maxAngle)
    {
        *firingAngle = lowerAngle;

        *flightTime =
            calculateFlightTime(
                velocity,
                lowerAngle
            );

        return 1;
    }


    /*
     * If lower angle is not allowed,
     * try the upper angle.
     */
    if (upperAngle >= minAngle &&
        upperAngle <= maxAngle)
    {
        *firingAngle = upperAngle;

        *flightTime =
            calculateFlightTime(
                velocity,
                upperAngle
            );

        return 1;
    }


    /*
     * Neither angle is allowed.
     */
    return 0;
}


/*
 * Convert EscortType into readable text.
 */
const char *getEscortTypeName(EscortType type)
{
    switch (type)
    {
        case EA:
            return "EA";

        case EB:
            return "EB";

        case EC:
            return "EC";

        case ED:
            return "ED";

        case EE:
            return "EE";

        default:
            return "UNKNOWN";
    }
}


/*
 * Generate a random position inside
 * the battlefield.
 *
 * Both x and y are between 0 and D.
 */
Point generateRandomPosition(double battlefieldSize)
{
    Point position;

    position.x =
        randomDouble(0.0, battlefieldSize);

    position.y =
        randomDouble(0.0, battlefieldSize);

    return position;
}


/*
 * Generate a random double between
 * min and max.
 */
double randomDouble(double min, double max)
{
    double randomValue;

    randomValue =
        (double)rand() / (double)RAND_MAX;

    return min +
           randomValue * (max - min);
}
