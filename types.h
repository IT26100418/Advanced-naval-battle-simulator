#ifndef TYPES_H
#define TYPES_H
//This file contains the main data types  used in the naval battle simulator

//maximum number of escort ships
#define MAX_ESCORTS 100

//maximum number of path points
#define MAX_PATH_POINTS 100

#define GRAVITY 9.81
#define PI 3.141592653589793

//different escort ship types
typedef enum
{
    EA,
    EB,
    EC,
    ED,
    EE
} EscortType;
//different battleship types
typedef enum
{
    U, M, R, S
} BattleshipType;
//current status of a ship
typedef enum
{
    ALIVE,
    SUNK
} ShipStatus;

typedef struct
{
    double x;
    double y;
} Point;


//stores battleship details
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

} Battleship;


//stores escort ship details
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

} EscortShip;


//stores one path point
typedef struct
{
    Point position;

} PathPoint;


//stores the whole battlefield
typedef struct
{
    double size;
    Battleship battleship;
    EscortShip escorts[MAX_ESCORTS];
    int escortCount;

} Battlefield;

typedef struct
{
    unsigned int randomSeed;
    int numberOfPathPoints;
    int gunJamIteration;
    double jamMinAngle;

} SimulationSettings;
#endif
