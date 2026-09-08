#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "part2B.h"
#include "common.h"


/*
 * Create a random path for Part 1-B.
 */
static void generatePath(
    Point path[],
    int count,
    double size)
{
    int i;

    for (i = 0; i < count; i++)
    {
        path[i] =
            generateRandomPosition(size);
    }
}


/*
 * Reset a battlefield to the original state.
 */
static void resetBattlefield(
    Battlefield *field,
    const Battlefield *initial)
{
    *field = *initial;
}


/*
 * Check whether B can attack an escort.
 */
static int isInBAttackRange(
    Battlefield *field,
    int index,
    double minAngle,
    double maxAngle)
{
    double angle;
    double flightTime;

    return canHit(
        field->battleship.position,
        field->escorts[index].position,
        field->battleship.maxVelocity,
        minAngle,
        maxAngle,
        &angle,
        &flightTime
    );
}


/*
 * Part 2-B strategy.
 *
 * 1. Higher impact
 * 2. Shorter distance
 * 3. Smaller ID
 */
static int selectTarget(
    Battlefield *field,
    double minAngle,
    double maxAngle)
{
    int i;
    int best = -1;

    for (i = 0;
         i < field->escortCount;
         i++)
    {
        double distance;
        double bestDistance;

        if (field->escorts[i].status != ALIVE)
        {
            continue;
        }

        if (!isInBAttackRange(
                field,
                i,
                minAngle,
                maxAngle))
        {
            continue;
        }

        if (best == -1)
        {
            best = i;
            continue;
        }

        if (field->escorts[i].impactPower >
            field->escorts[best].impactPower)
        {
            best = i;
        }
        else if (
            field->escorts[i].impactPower ==
            field->escorts[best].impactPower)
        {
            distance =
                calculateDistance(
                    field->battleship.position,
                    field->escorts[i].position
                );

            bestDistance =
                calculateDistance(
                    field->battleship.position,
                    field->escorts[best].position
                );

            if (distance < bestDistance)
            {
                best = i;
            }
            else if (
                distance == bestDistance &&
                field->escorts[i].id <
                field->escorts[best].id)
            {
                best = i;
            }
        }
    }

    return best;
}


/*
 * Create and print B's attack order.
 */
static int createAttackOrder(
    Battlefield *field,
    double minAngle,
    double maxAngle,
    int order[])
{
    Battlefield temp;
    int i;
    int count = 0;
    int target;

    temp = *field;

    while (1)
    {
        target =
            selectTarget(
                &temp,
                minAngle,
                maxAngle
            );

        if (target == -1)
        {
            break;
        }

        order[count] =
            temp.escorts[target].id;

        count++;

        /*
         * Temporarily remove the target so
         * the next target can be selected.
         */
        temp.escorts[target].status = SUNK;
    }

    return count;
}


/*
 * Print attack order.
 */
static void printAttackOrder(
    const int order[],
    int count)
{
    int i;

    printf("\nB ATTACK ORDER\n");
    printf("--------------\n");

    if (count == 0)
    {
        printf("No escort is in B attack range.\n");
        return;
    }

    for (i = 0; i < count; i++)
    {
        printf(
            "%d. Escort #%d\n",
            i + 1,
            order[i]
        );
    }
}


/*
 * Save attack order to file.
 */
static void saveAttackOrder(
    FILE *file,
    const int order[],
    int count)
{
    int i;

    fprintf(
        file,
        "\nB ATTACK ORDER\n"
    );

    fprintf(
        file,
        "--------------\n"
    );

    if (count == 0)
    {
        fprintf(
            file,
            "No escort is in B attack range.\n"
        );

        return;
    }

    for (i = 0; i < count; i++)
    {
        fprintf(
            file,
            "%d. Escort #%d\n",
            i + 1,
            order[i]
        );
    }
}


/*
 * One Part 2-B round.
 *
 * E ships can fire repeatedly.
 * B fires every TB seconds.
 *
 * impactMode = 0
 *     Part 1-A / Part 1-B
 *
 * impactMode = 1
 *     Part 1-C
 */
static void runRound(
    Battlefield *field,
    double minAngle,
    double maxAngle,
    double firingInterval,
    int impactMode,
    double *cumulativeImpact,
    BattleResult *result,
    int order[],
    int *orderCount)
{
    double currentTime = 0.0;
    double nextBShot = 0.0;

    int i;

    result->battleshipSunk = 0;
    result->killerEscortId = -1;
    result->killerTime = 0.0;

    result->hitCount = 0;
    result->duration = 0.0;

    result->cumulativeImpact =
        *cumulativeImpact;


    /*
     * Create the strategic attack order
     * before the battle starts.
     */
    *orderCount =
        createAttackOrder(
            field,
            minAngle,
            maxAngle,
            order
        );


    /*
     * Continue while B is alive.
     *
     * The limit prevents an infinite loop
     * when neither side can attack.
     */
    while (
        field->battleship.status == ALIVE &&
        currentTime <= 10000.0)
    {
        int target;


        /*
         * E ships fire continuously.
         */
        for (i = 0;
             i < field->escortCount;
             i++)
        {
            EscortShip *e =
                &field->escorts[i];

            double velocity;
            double angle;
            double flightTime;


            if (e->status != ALIVE)
            {
                continue;
            }


            /*
             * Fire at time 0, TE, 2TE...
             */
            if (currentTime > 0.0)
            {
                double remainder;

                remainder =
                    currentTime /
                    e->firingInterval;

                if (
                    remainder -
                    (int)remainder >
                    0.000001)
                {
                    continue;
                }
            }


            velocity =
                randomDouble(
                    e->minVelocity,
                    e->maxVelocity
                );


            e->shotsFired++;


            if (canHit(
                    e->position,
                    field->battleship.position,
                    velocity,
                    e->minAngle,
                    e->maxAngle,
                    &angle,
                    &flightTime))
            {
                e->lastShotVelocity =
                    velocity;

                e->lastShotAngle =
                    angle;

                e->lastFlightTime =
                    flightTime;


                /*
                 * Part 1-A / Part 1-B:
                 * one successful hit destroys B.
                 */
                if (!impactMode)
                {
                    field->battleship.health =
                        0.0;

                    field->battleship.status =
                        SUNK;

                    result->battleshipSunk =
                        1;

                    result->killerEscortId =
                        e->id;

                    result->killerTime =
                        currentTime +
                        flightTime;

                    result->duration =
                        result->killerTime;

                    printf(
                        "Escort #%d sank B.\n",
                        e->id
                    );

                    return;
                }


                /*
                 * Part 1-C:
                 * Add cumulative damage.
                 */
                *cumulativeImpact +=
                    e->impactPower;

                field->battleship.health =
                    1.0 -
                    *cumulativeImpact;

                if (field->battleship.health < 0.0)
                {
                    field->battleship.health =
                        0.0;
                }


                if (*cumulativeImpact >= 1.0)
                {
                    field->battleship.status =
                        SUNK;

                    result->battleshipSunk =
                        1;

                    result->killerEscortId =
                        e->id;

                    result->killerTime =
                        currentTime +
                        flightTime;

                    result->duration =
                        result->killerTime;

                    return;
                }
            }
        }


        /*
         * B fires every TB seconds.
         */
        if (currentTime >= nextBShot)
        {
            target =
                selectTarget(
                    field,
                    minAngle,
                    maxAngle
                );


            if (target != -1)
            {
                double velocity;
                double angle;
                double flightTime;


                velocity =
                    randomDouble(
                        0.0,
                        field->battleship.maxVelocity
                    );


                field->battleship.shotsFired++;


                if (canHit(
                        field->battleship.position,
                        field->escorts[target].position,
                        velocity,
                        minAngle,
                        maxAngle,
                        &angle,
                        &flightTime))
                {
                    field->battleship.lastShotVelocity =
                        velocity;

                    field->battleship.lastShotAngle =
                        angle;

                    field->battleship.lastFlightTime =
                        flightTime;


                    field->escorts[target].status =
                        SUNK;

                    field->escorts[target].health =
                        0.0;


                    if (result->hitCount <
                        MAX_ESCORTS)
                    {
                        result->hitIds[
                            result->hitCount] =
                            field->escorts[target].id;

                        result->hitTimes[
                            result->hitCount] =
                            currentTime +
                            flightTime;

                        result->hitCount++;
                    }


                    if (currentTime +
                        flightTime >
                        result->duration)
                    {
                        result->duration =
                            currentTime +
                            flightTime;
                    }


                    printf(
                        "B destroyed Escort #%d.\n",
                        field->escorts[target].id
                    );
                }
            }


            nextBShot +=
                firingInterval;
        }


        /*
         * If there are no reachable escorts,
         * this simulation step is finished.
         */
        if (selectTarget(
                field,
                minAngle,
                maxAngle) == -1)
        {
            break;
        }


        currentTime += 1.0;
    }


    result->cumulativeImpact =
        *cumulativeImpact;
}


/*
 * Save one simulation result.
 */
static void saveResult(
    const Battlefield *field,
    const BattleResult *result,
    const int order[],
    int orderCount,
    const char *name,
    double firingInterval,
    double minAngle)
{
    char filename[150];
    FILE *file;
    int i;


    sprintf(
        filename,
        "part2B_%s.txt",
        name
    );


    file =
        fopen(filename, "w");


    if (file == NULL)
    {
        printf(
            "Cannot create %s\n",
            filename
        );

        return;
    }


    fprintf(
        file,
        "PART 2-B - %s\n\n",
        name
    );


    fprintf(
        file,
        "B firing interval TB: %.2f seconds\n",
        firingInterval
    );


    fprintf(
        file,
        "B angle range: %.2f - %.2f degrees\n",
        minAngle,
        90.0
    );


    fprintf(
        file,
        "\nESCORT FIRING INTERVALS\n"
    );


    for (i = 0;
         i < field->escortCount;
         i++)
    {
        fprintf(
            file,
            "Escort #%d (%s): TE = %.2f seconds\n",
            field->escorts[i].id,
            getEscortTypeName(
                field->escorts[i].type
            ),
            field->escorts[i].firingInterval
        );
    }


    saveAttackOrder(
        file,
        order,
        orderCount
    );


    fprintf(
        file,
        "\nBATTLE RESULT\n"
    );


    fprintf(
        file,
        "Battleship Status: %s\n",

        field->battleship.status == ALIVE
            ? "SURVIVED"
            : "SUNK"
    );


    fprintf(
        file,
        "Battleship Health: %.2f%%\n",
        field->battleship.health * 100.0
    );


    fprintf(
        file,
        "Total Battle Time: %.2f seconds\n",
        result->duration
    );


    fprintf(
        file,
        "Escorts Destroyed: %d\n",
        result->hitCount
    );


    fprintf(
        file,
        "Cumulative Impact: %.2f%%\n",
        result->cumulativeImpact * 100.0
    );


    fprintf(
        file,
        "\nESCORT STATUS\n"
    );


    for (i = 0;
         i < field->escortCount;
         i++)
    {
        fprintf(
            file,
            "Escort #%d: %s\n",
            field->escorts[i].id,

            field->escorts[i].status == ALIVE
                ? "ALIVE"
                : "SUNK"
        );
    }


    fclose(file);
}


/*
 * Run Part 2-B Part 1-A simulation.
 */
static void runPart2B_1A(
    const Battlefield *initial,
    double firingInterval,
    unsigned int seed)
{
    Battlefield field;

    BattleResult result;

    int order[MAX_ESCORTS];
    int orderCount;

    double cumulativeImpact = 0.0;


    resetBattlefield(
        &field,
        initial
    );


    srand(seed);


    printf(
        "\n========================================\n"
    );

    printf(
        "PART 2-B -> PART 1-A\n"
    );

    printf(
        "========================================\n"
    );


    runRound(
        &field,
        0.0,
        90.0,
        firingInterval,
        0,
        &cumulativeImpact,
        &result,
        order,
        &orderCount
    );


    printAttackOrder(
        order,
        orderCount
    );


    printf(
        "\nBattleship Status : %s\n",

        field.battleship.status == ALIVE
            ? "SURVIVED"
            : "SUNK"
    );


    printf(
        "Battle Time       : %.2f seconds\n",
        result.duration
    );


    saveResult(
        &field,
        &result,
        order,
        orderCount,
        "PART1A",
        firingInterval,
        0.0
    );
}


/*
 * Run one Part 1-B path simulation.
 */
static void runPart2BPath(
    const Battlefield *initial,
    const Point path[],
    int points,
    int jamIteration,
    double jamAngle,
    double firingInterval,
    int simulationNumber,
    unsigned int seed)
{
    Battlefield field;

    BattleResult result;

    int order[MAX_ESCORTS];

    int orderCount;

    double cumulativeImpact = 0.0;

    int i;


    resetBattlefield(
        &field,
        initial
    );


    srand(seed);


    for (i = 0; i < points; i++)
    {
        char name[100];

        field.battleship.position =
            path[i];


        if (field.battleship.status == SUNK)
        {
            break;
        }


        /*
         * After t, B gun is jammed.
         */
        if (i + 1 > jamIteration)
        {
            sprintf(
                name,
                "PART1B_SIM%d_STEP%d_JAM",
                simulationNumber,
                i + 1
            );

            runRound(
                &field,
                jamAngle,
                90.0,
                firingInterval,
                0,
                &cumulativeImpact,
                &result,
                order,
                &orderCount
            );
        }
        else
        {
            sprintf(
                name,
                "PART1B_SIM%d_STEP%d",
                simulationNumber,
                i + 1
            );

            runRound(
                &field,
                0.0,
                90.0,
                firingInterval,
                0,
                &cumulativeImpact,
                &result,
                order,
                &orderCount
            );
        }


        saveResult(
            &field,
            &result,
            order,
            orderCount,
            name,
            firingInterval,
            i + 1 > jamIteration
                ? jamAngle
                : 0.0
        );


        if (field.battleship.status == SUNK)
        {
            break;
        }
    }
}


/*
 * Run Part 1-C path simulation.
 */
static void runPart2BCPath(
    const Battlefield *initial,
    const Point path[],
    int points,
    int jamIteration,
    double jamAngle,
    double firingInterval,
    int simulationNumber,
    unsigned int seed)
{
    Battlefield field;

    BattleResult result;

    int order[MAX_ESCORTS];

    int orderCount;

    double cumulativeImpact = 0.0;

    int i;


    resetBattlefield(
        &field,
        initial
    );


    srand(seed);


    for (i = 0; i < points; i++)
    {
        char name[100];

        field.battleship.position =
            path[i];


        if (field.battleship.status == SUNK)
        {
            break;
        }


        if (i + 1 > jamIteration)
        {
            sprintf(
                name,
                "PART1C_SIM%d_STEP%d_JAM",
                simulationNumber,
                i + 1
            );

            runRound(
                &field,
                jamAngle,
                90.0,
                firingInterval,
                1,
                &cumulativeImpact,
                &result,
                order,
                &orderCount
            );
        }
        else
        {
            sprintf(
                name,
                "PART1C_SIM%d_STEP%d",
                simulationNumber,
                i + 1
            );

            runRound(
                &field,
                0.0,
                90.0,
                firingInterval,
                1,
                &cumulativeImpact,
                &result,
                order,
                &orderCount
            );
        }


        saveResult(
            &field,
            &result,
            order,
            orderCount,
            name,
            firingInterval,
            i + 1 > jamIteration
                ? jamAngle
                : 0.0
        );


        if (field.battleship.status == SUNK)
        {
            break;
        }
    }
}


/*
 * Complete Part 2-B.
 */
void runPart2B(
    Battlefield *field)
{
    Battlefield initial;

    Point path[MAX_PATH_POINTS];

    double firingInterval;

    int points;
    int jamIteration;

    double jamAngle;

    unsigned int seed;


    initial = *field;


    printf(
        "\n========================================\n"
    );

    printf(
        "              PART 2-B\n"
    );

    printf(
        "========================================\n"
    );


    printf(
        "\nPart 2-B uses the Part 2-A strategy,\n"
    );

    printf(
        "but Escort ships can fire continuously.\n"
    );


    printf(
        "\nStrategy:\n"
    );

    printf(
        "1. Higher Impact first\n"
    );

    printf(
        "2. Shorter Distance next\n"
    );

    printf(
        "3. Smaller ID if tied\n"
    );


    printf(
        "\nEscort firing intervals:\n"
    );

    printf(
        "EA = 8 sec | EB = 10 sec | "
        "EC = 12 sec | ED = 15 sec | EE = 18 sec\n"
    );


    /*
     * B firing interval.
     */
    do
    {
        printf(
            "\nEnter B firing interval TB: "
        );

        scanf(
            "%lf",
            &firingInterval
        );

    } while (firingInterval <= 0.0);


    /*
     * Number of path points.
     */
    do
    {
        printf(
            "Enter number of path points "
            "(1-%d): ",
            MAX_PATH_POINTS
        );

        scanf(
            "%d",
            &points
        );

    } while (
        points < 1 ||
        points > MAX_PATH_POINTS
    );


    /*
     * Gun jam starts after t.
     */
    do
    {
        printf(
            "Enter gun jam iteration t "
            "(0 < t < %d): ",
            points
        );

        scanf(
            "%d",
            &jamIteration
        );

    } while (
        jamIteration <= 0 ||
        jamIteration >= points
    );


    /*
     * Jam angle.
     */
    do
    {
        printf(
            "Enter theta_min "
            "(0 < theta_min < 30): "
        );

        scanf(
            "%lf",
            &jamAngle
        );

    } while (
        jamAngle <= 0.0 ||
        jamAngle >= 30.0
    );


    /*
     * Same initial path for B simulations.
     */
    seed =
        (unsigned int)rand();

    srand(seed);


    generatePath(
        path,
        points,
        field->size
    );


    printf(
        "\nPART 2-B PATH\n"
    );

    printf(
        "--------------\n"
    );


    {
        int i;

        for (i = 0; i < points; i++)
        {
            printf(
                "Point %d : (%.2f, %.2f)\n",
                i + 1,
                path[i].x,
                path[i].y
            );
        }
    }


    /*
     * Part 2-B -> Part 1-A
     */
    runPart2B_1A(
        &initial,
        firingInterval,
        seed
    );


    /*
     * Part 2-B -> Part 1-B
     */
    runPart2BPath(
        &initial,
        path,
        points,
        jamIteration,
        jamAngle,
        firingInterval,
        1,
        seed
    );


    runPart2BPath(
        &initial,
        path,
        points,
        jamIteration,
        jamAngle,
        firingInterval,
        2,
        seed
    );


    /*
     * Create another path for Part 1-C.
     */
    seed =
        (unsigned int)rand();

    srand(seed);


    generatePath(
        path,
        points,
        field->size
    );


    /*
     * Part 2-B -> Part 1-C
     */
    runPart2BCPath(
        &initial,
        path,
        points,
        jamIteration,
        jamAngle,
        firingInterval,
        1,
        seed
    );


    runPart2BCPath(
        &initial,
        path,
        points,
        jamIteration,
        jamAngle,
        firingInterval,
        2,
        seed
    );


    printf(
        "\n========================================\n"
    );

    printf(
        "PART 2-B COMPLETED\n"
    );

    printf(
        "Part 1-A, Part 1-B and Part 1-C "
        "simulations completed using Part 2-B rules.\n"
    );

    printf(
        "========================================\n"
    );
}
