#ifndef TYPES_H
#define TYPES_H
#define MAX_ESCORTS 100
#define MAX_PATH_POINTS 100
#define GRAVITY 9.81
#define PI 3.141592653589793

//escort ship types
typedef enum
{
    EA,
    EB,
    EC,
    ED,
    EE
} EscortType;

//battleship types
typedef enum
{
    U,
    M,
    R,
    S
} BattleshipType;


//ship status
typedef enum
{
    ALIVE,
    SUNK
} ShipStatus;

//2D position
typedef struct
{
    double x;
    double y;
} Point;


//battleship details
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
    double lastShotVelocity;
    double lastShotAngle;
    double lastFlightTime;

} Battleship;


//escort ship details
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

    //part 2-B
    double firingInterval;
    double lastShotVelocity;
    double lastShotAngle;
    double lastFlightTime;

} EscortShip;

//one path point
typedef struct
{
    Point position;

} PathPoint;


//complete battlefield
typedef struct
{
    double size;
    Battleship battleship;
    EscortShip escorts[MAX_ESCORTS];
    int escortCount;

} Battlefield;

//result of one battle round
typedef struct
{
    int battleshipSunk;
    int killerEscortId;
    double killerTime;
    int hitCount;
    int hitIds[MAX_ESCORTS];
    double hitTimes[MAX_ESCORTS];
    double duration;

    //used in Part 1-C
    double cumulativeImpact;

} BattleResult;

//part 1-B settings
typedef struct
{
    unsigned int randomSeed;
    int numberOfPathPoints;
    int gunJamIteration;
    double jamMinAngle;

} SimulationSettings;


#endif
