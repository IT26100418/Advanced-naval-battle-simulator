#ifndef TYPES_H
#define TYPES_H

/* Maximum number of escort ships */
#define MAX_ESCORTS 100

/* Maximum number of path points */
#define MAX_PATH_POINTS 100

#define GRAVITY 9.81
#define PI 3.141592653589793

/* Different escort ship types */
typedef enum
{
    EA,
    EB,
    EC,
    ED,
    EE
} EscortType;

/* Different battleship types */
typedef enum
{
    U,
    M,
    R,
    S
} BattleshipType;

/* Current status of a ship */
typedef enum
{
    ALIVE,
    SUNK
} ShipStatus;

/* 2D position */
typedef struct
{
    double x;
    double y;
} Point;


/* Stores battleship details */
typedef struct
{
    BattleshipType type;
    char notation;

    Point position;

    double maxVelocity;

    double health;
    double gamma;

    int shotsFired;

    ShipStatus status;

    /*
     * Details of the latest shell fired.
     */
    double lastShotVelocity;
    double lastShotAngle;
    double lastFlightTime;

} Battleship;


/* Stores escort ship details */
typedef struct
{
    int id;

    EscortType type;

    Point position;

    double minVelocity;
    double maxVelocity;

    double minAngle;
    double maxAngle;

    double impactPower;
    double gamma;

    double health;

    int shotsFired;

    ShipStatus status;

    /*
     * Details of the shell fired in Part 1-A.
     */
    double lastShotVelocity;
    double lastShotAngle;
    double lastFlightTime;

} EscortShip;


/* Stores one path point */
typedef struct
{
    Point position;

} PathPoint;


/* Stores the whole battlefield */
typedef struct
{
    double size;

    Battleship battleship;

    EscortShip escorts[MAX_ESCORTS];

    int escortCount;

} Battlefield;

typedef struct
{
    int battleshipSunk;
    int killerEscortId;
    double killerTime;

    int hitCount;
    int hitIds[MAX_ESCORTS];
    double hitTimes[MAX_ESCORTS];

    double duration;
} BattleResult;

/* Stores Part 1-B simulation settings */
typedef struct
{
    unsigned int randomSeed;

    int numberOfPathPoints;

    int gunJamIteration;

    double jamMinAngle;

} SimulationSettings;

#endif
