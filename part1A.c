#include <stdio.h>
#include <math.h>
#include "part1A.h"

//Internal helper functions
static double calculateDistance(Point a, Point b)
{
    double dx=b.x - a.x;
    double dy=b.y - a.y;

    return sqrt(dx*dx + dy*dy);
}
static double degreesToRadians(double degrees)
{
    return degrees * PI/180.0;
}
static double calculateRange(double velocity, double angle)
{
    double radians=degreesToRadians(angle);
    return(velocity*velocity*sin(2.0 * radians))/GRAVITY;
}
static double calculateFlightTime(double velocity, double angle)
{
    double radians=degreesToRadians(angle);
    return (2.0 * velocity * sin(radians))/ GRAVITY;
}

//checks whether a projectile can hit the target
// Returns 1 if hit is possible, otherwise 0.
static int canHit(
    Point attacker,
    Point target,
    double minVelocity,
    double maxVelocity,
    double minAngle,
    double maxAngle,
    double *flightTime)
{
    double distance;
    double maxRange;
    double angle;
    double value;

    (void)minVelocity;

    distance=calculateDistance(attacker, target);

    
    //maximum projectile range occurs at 45 degrees
    //use 45 degrees only if it is inside the allowed range
    
    if(maxAngle >= 45.0 && minAngle <= 45.0)
    {
        angle = 45.0;
    }
    else if(maxAngle < 45.0)
    {
        angle=maxAngle;
    }
    else
    {
        angle=minAngle;
    }
    maxRange=calculateRange(maxVelocity, angle);

    if(distance > maxRange)
    {
        return 0;
    }
//R = v² sin(2θ) / g
    
    value=(distance * GRAVITY) / (maxVelocity*maxVelocity);

    if(value > 1.0)
    {
        value=1.0;
    }

    if(value < 0.0)
    {
        value=0.0;
    }

    angle=0.5 * asin(value);
    angle=angle * 180.0 / PI;

    if(angle < minAngle || angle > maxAngle)
    {
        return 0;
    }

    *flightTime = calculateFlightTime(maxVelocity, angle);
    return 1;
}

//convert EscortType to readable text
static const char *getEscortTypeName(EscortType type)
{
    switch(type)
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

//file handling

static void saveInitialConditions(const Battlefield *field)
{
    FILE *file;
    int i;
    file=fopen("part1A_initial.txt", "w");

    if(file == NULL)
    {
        printf("Error: Cannot create part1A_initial.txt\n");
        return;
    }

    fprintf(file, "PART 1-A INITIAL CONDITIONS\n\n");
    fprintf(file, "Battlefield size: %.2f\n\n", field->size);
    fprintf(file, "BATTLESHIP\n");
    fprintf(file, "Type: %c\n", field->battleship.notation);
    fprintf(file, "Position: (%.2f, %.2f)\n", field->battleship.position.x, field->battleship.position.y);

    fprintf(file, "Maximum velocity: %.2f\n\n", field->battleship.maxVelocity);

    fprintf(file, "ESCORT SHIPS\n");

    for(i = 0; i < field->escortCount; i++)
    {
        const EscortShip *e = &field->escorts[i];

        fprintf(file, "\nEscort #%d\n", e->id);
        fprintf(file, "Type: %s\n", getEscortTypeName(e->type));
        fprintf(file, "Position: (%.2f, %.2f)\n", e->position.x, e->position.y);
        fprintf(file, "Minimum velocity: %.2f\n", e->minVelocity);
	fprintf(file, "Maximum velocity: %.2f\n", e->maxVelocity);
	fprintf(file, "Minimum angle: %.2f\n", e->minAngle);
	fprintf(file, "Maximum angle: %.2f\n", e->maxAngle);
    }
    fclose(file);
}

static void saveFinalConditions(const Battlefield *field, int killerEscort,double killerTime)
{
    FILE *file;
    int i;

    file=fopen("part1A_final.txt", "w");

    if(file == NULL)
    {
        printf("Error: Cannot create part1A_final.txt\n");
        return;
    }

    fprintf(file, "PART 1-A FINAL CONDITIONS\n\n");
    fprintf(file, "Battlefield size: %.2f\n\n", field->size);
    fprintf(file, "BATTLESHIP\n");
    fprintf(file, "Type: %c\n", field->battleship.notation);
    fprintf(file, "Position: (%.2f, %.2f)\n", field->battleship.position.x, field->battleship.position.y);
    fprintf(file, "Health: %.2f\n", field->battleship.health);
    fprintf(file, "Status: %s\n", field->battleship.status == ALIVE
                ? "ALIVE"
                : "SUNK");

    fprintf(file, "Shots fired: %d\n", field->battleship.shotsFired);

    if(killerEscort != -1)
    {
        fprintf(file, "Sunk by Escort: #%d\n", field->escorts[killerEscort].id);
        fprintf(file, "Time to hit: %.2f seconds\n", killerTime);
    }

    fprintf(file, "\nESCORT SHIPS\n");

    for(i = 0; i < field->escortCount; i++)
    {
        const EscortShip *e = &field->escorts[i];
        fprintf(file, "\nEscort #%d\n", e->id);
        fprintf(file, "Type: %s\n", getEscortTypeName(e->type));
	fprintf(file, "Position: (%.2f, %.2f)\n", e->position.x, e->position.y);
        fprintf(file, "Minimum velocity: %.2f\n", e->minVelocity);
        fprintf(file, "Maximum velocity: %.2f\n", e->maxVelocity);
	fprintf(file, "Minimum angle: %.2f\n", e->minAngle);
        fprintf(file, "Maximum angle: %.2f\n", e->maxAngle);
        fprintf(file, "Health: %.2f\n", e->health);
        fprintf(file, "Status: %s\n",
                e->status == ALIVE
                    ? "ALIVE"
                    : "SUNK");

        fprintf(file, "Shots fired: %d\n", e->shotsFired);
    }

    fclose(file);
}

static void saveHitEscorts(
    const Battlefield *field,
    int hitCount,
    const int hitIds[],
    const double hitTimes[])
{
    FILE *file;
    int i;

    file=fopen("part1A_hit_escorts.txt", "w");

    if(file == NULL)
    {
        printf("Error: Cannot create hit escorts file.\n");
        return;
    }

    fprintf(file,
            "PART 1-A ESCORTS HIT BY BATTLESHIP\n\n");

    fprintf(file,
            "Number of escorts hit: %d\n\n",
            hitCount);

    for(i = 0; i < hitCount; i++)
    {
        fprintf(file,
                "Escort #%d\n",
                hitIds[i]);

        fprintf(file,
                "Time to hit: %.2f seconds\n\n",
                hitTimes[i]);
    }
    (void)field;
    fclose(file);
}

//Main Part 1-A simulation

void runPart1A(Battlefield *field)
{
    int i;
    int hitCount = 0;
    int hitIds[MAX_ESCORTS];
    double hitTimes[MAX_ESCORTS];
    int killerEscort = -1;
    double killerTime = 0.0;
    printf("\n----PART 1-A SIMULATION START----\n");

    //save initial conditions
    saveInitialConditions(field);

//sTEP 1:every Escort ship gets one firing opportunity//
    for(i = 0; i < field->escortCount; i++)
    {
        double flightTime;
	if (field->escorts[i].status != ALIVE)
        {
            continue;
        }
        //One Escort ship can fire only once

        field->escorts[i].shotsFired = 1;
        if(canHit(
                field->escorts[i].position,
                field->battleship.position,
                field->escorts[i].minVelocity,
                field->escorts[i].maxVelocity,
                field->escorts[i].minAngle,
                field->escorts[i].maxAngle,
                &flightTime))
        {
            
// If several Escorts can hit the Battleship,the first shell to arrive sinks it
             
            if(killerEscort == -1 ||
                flightTime < killerTime)
            {
                killerEscort = i;
                killerTime = flightTime;
            }
        }
    }
//step 2:If an Escort can hit the Battleship, one shell is enough to destroy it
    
    if(killerEscort != -1)
    {
        field->battleship.health = 0.0;
        field->battleship.status = SUNK;
        printf("Escort #%d sank the Battleship.\n", field->escorts[killerEscort].id);

        printf("Time to hit: %.2f seconds\n", killerTime);
        saveFinalConditions(
            field,
            killerEscort,
            killerTime);

        printf("----PART 1-A SIMULATION END----\n");

        return;
    }
//STEP 3:Battleship survived,it can hit ALL Escort ships inside
     
    for(i = 0; i < field->escortCount; i++)
    {
        double flightTime;

        if (field->escorts[i].status != ALIVE)
        {
            continue;
        }

        if (canHit(
                field->battleship.position,
                field->escorts[i].position,
                0.0,
                field->battleship.maxVelocity,
                0.0,
                90.0,
                &flightTime))
        {
            field->escorts[i].health = 0.0;
            field->escorts[i].status =SUNK;
            field->battleship.shotsFired++;
            hitIds[hitCount] =field->escorts[i].id;
            hitTimes[hitCount] = flightTime;
            hitCount++;
        }
    }

//step 4: Display results when Battleship survives
    
    printf("Battleship survived.\n");

    printf("Number of Escort ships hit by Battleship: %d\n",  hitCount);

    if(hitCount > 0)
    {
        double battleTime = 0.0;
        for(i = 0; i < hitCount; i++)
        {
            if(hitTimes[i] > battleTime)
            {
                battleTime = hitTimes[i];
            }
        }

        printf("Battle duration: %.2f seconds\n",  battleTime);

        saveHitEscorts(
            field,
            hitCount,
            hitIds,
            hitTimes);
    }
    else
    {
        printf("No Escort ships were hit.\n");

        saveHitEscorts(
            field,
            0,
            hitIds,
            hitTimes);
    }

    saveFinalConditions(
        field,
        -1,
        0.0);

    printf("----PART 1-A SIMULATION END----\n");
}
