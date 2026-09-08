#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "part2A.h"
#include "common.h"


/*
 * Generate a random path for B.
 */
static void generatePath(
    Point path[],
    int numberOfPoints,
    double battlefieldSize)
{
    int i;

    for (i = 0; i < numberOfPoints; i++)
    {
        path[i] =
            generateRandomPosition(
                battlefieldSize);
    }
}


/*
 * Reset the battlefield to the
 * original condition.
 */
static void resetBattlefield(
    Battlefield *field,
    const Battlefield *initial)
{
    int i;

    *field = *initial;

    field->battleship.status = ALIVE;
    field->battleship.health = 1.0;
    field->battleship.shotsFired = 0;

    field->battleship.lastShotVelocity = 0.0;
    field->battleship.lastShotAngle = 0.0;
    field->battleship.lastFlightTime = 0.0;

    for (i = 0; i < field->escortCount; i++)
    {
        field->escorts[i].status = ALIVE;
        field->escorts[i].health = 1.0;
        field->escorts[i].shotsFired = 0;

        field->escorts[i].lastShotVelocity = 0.0;
        field->escorts[i].lastShotAngle = 0.0;
        field->escorts[i].lastFlightTime = 0.0;
    }
}


/*
 * Check whether an Escort is inside
 * B's possible attacking range.
 *
 * Maximum B velocity is used here.
 */
static int isInBattleshipRange(
    const Battlefield *field,
    int escortIndex,
    double minimumAngle,
    double maximumAngle)
{
    double angle;
    double time;

    return canHit(
        field->battleship.position,
        field->escorts[escortIndex].position,
        field->battleship.maxVelocity,
        minimumAngle,
        maximumAngle,
        &angle,
        &time);
}


/*
 * Compare two Escorts according
 * to our custom strategy.
 *
 * Priority:
 * 1. Higher impact power
 * 2. Shorter distance
 * 3. Smaller ID
 */
static int shouldComeFirst(
    const Battlefield *field,
    int first,
    int second)
{
    double firstDistance;
    double secondDistance;

    firstDistance =
        calculateDistance(
            field->battleship.position,
            field->escorts[first].position);

    secondDistance =
        calculateDistance(
            field->battleship.position,
            field->escorts[second].position);


    /*
     * Priority 1:
     * Higher impact power first.
     */
    if (field->escorts[first].impactPower >
        field->escorts[second].impactPower)
    {
        return 1;
    }

    if (field->escorts[first].impactPower <
        field->escorts[second].impactPower)
    {
        return 0;
    }


    /*
     * Priority 2:
     * If impact is equal,
     * shorter distance first.
     */
    if (firstDistance < secondDistance)
    {
        return 1;
    }

    if (firstDistance > secondDistance)
    {
        return 0;
    }


    /*
     * Priority 3:
     * If impact and distance are equal,
     * smaller ID comes first.
     */
    return field->escorts[first].id <
           field->escorts[second].id;
}


/*
 * Create the custom attack order.
 *
 * Only alive Escorts inside
 * B's attacking range are included.
 */
static int createAttackOrder(
    const Battlefield *field,
    double minimumAngle,
    double maximumAngle,
    int order[])
{
    int i;
    int j;
    int count;

    count = 0;


    /*
     * Find possible targets.
     */
    for (i = 0; i < field->escortCount; i++)
    {
        if (field->escorts[i].status != ALIVE)
        {
            continue;
        }

        if (isInBattleshipRange(
                field,
                i,
                minimumAngle,
                maximumAngle))
        {
            order[count] = i;
            count++;
        }
    }


    /*
     * Sort targets using
     * our custom strategy.
     */
    for (i = 0; i < count - 1; i++)
    {
        for (j = i + 1; j < count; j++)
        {
            if (shouldComeFirst(
                    field,
                    order[j],
                    order[i]))
            {
                int temp;

                temp = order[i];
                order[i] = order[j];
                order[j] = temp;
            }
        }
    }

    return count;
}


/*
 * Print the attack order.
 */
static void printAttackOrder(
    const Battlefield *field,
    const int order[],
    int orderCount)
{
    int i;

    printf("\n");
    printf("ATTACK ORDER\n");
    printf("------------\n");

    if (orderCount == 0)
    {
        printf(
            "No Escort is in B's attacking range.\n");

        return;
    }

    for (i = 0; i < orderCount; i++)
    {
        printf(
            "%d. Escort #%d (%s)\n",
            i + 1,
            field->escorts[order[i]].id,
            getEscortTypeName(
                field->escorts[order[i]].type));
    }
}


/*
 * Save the attack order to a file.
 */
static void saveAttackOrder(
    FILE *file,
    const Battlefield *field,
    const int order[],
    int orderCount)
{
    int i;

    fprintf(
        file,
        "\nATTACK ORDER\n");

    fprintf(
        file,
        "------------\n");

    if (orderCount == 0)
    {
        fprintf(
            file,
            "No Escort is in B's attacking range.\n");

        return;
    }

    for (i = 0; i < orderCount; i++)
    {
        fprintf(
            file,
            "%d. Escort #%d (%s)\n",
            i + 1,
            field->escorts[order[i]].id,
            getEscortTypeName(
                field->escorts[order[i]].type));
    }
}


/*
 * B attacks Escorts according
 * to the custom attack order.
 *
 * One firing attempt consumes TB
 * seconds of B's firing schedule.
 */
static void runStrategicBattleshipAttack(
    Battlefield *field,
    double minimumAngle,
    double maximumAngle,
    double firingInterval,
    BattleResult *result,
    int order[],
    int *orderCount)
{
    int i;
    int firingAttempts;

    double currentFireTime;
    double lastImpactTime;


    /*
     * Create attack order before
     * B starts firing.
     */
    *orderCount =
        createAttackOrder(
            field,
            minimumAngle,
            maximumAngle,
            order);


    currentFireTime = 0.0;
    lastImpactTime = 0.0;
    firingAttempts = 0;


    /*
     * Fire at targets according
     * to the selected order.
     */
    for (i = 0; i < *orderCount; i++)
    {
        int escortIndex;

        double velocity;
        double angle;
        double flightTime;
        double impactTime;


        escortIndex = order[i];


        /*
         * Skip if the Escort was
         * already destroyed.
         */
        if (field->escorts[escortIndex].status != ALIVE)
        {
            continue;
        }


        /*
         * B fires one shell.
         */
        velocity =
            randomDouble(
                0.0,
                field->battleship.maxVelocity);

        field->battleship.shotsFired++;

        firingAttempts++;


        /*
         * Check whether the shell
         * can hit the Escort.
         */
        if (canHit(
                field->battleship.position,
                field->escorts[escortIndex].position,
                velocity,
                minimumAngle,
                maximumAngle,
                &angle,
                &flightTime))
        {
            field->escorts[escortIndex].health = 0.0;

            field->escorts[escortIndex].status = SUNK;

            field->battleship.lastShotVelocity =
                velocity;

            field->battleship.lastShotAngle =
                angle;

            field->battleship.lastFlightTime =
                flightTime;


            /*
             * Calculate shell arrival time.
             */
            impactTime =
                currentFireTime +
                flightTime;


            result->hitIds[result->hitCount] =
                field->escorts[escortIndex].id;

            result->hitTimes[result->hitCount] =
                impactTime;

            result->hitCount++;


            if (impactTime > lastImpactTime)
            {
                lastImpactTime = impactTime;
            }


            printf(
                "B fired at Escort #%d "
                "and destroyed it.\n",
                field->escorts[escortIndex].id);
        }


        /*
         * Next B firing happens after TB.
         */
        currentFireTime += firingInterval;
    }


    /*
     * TB is the time between
     * consecutive firing attempts.
     *
     * Therefore the firing schedule
     * takes firingAttempts * TB.
     */
    result->duration =
        firingAttempts *
        firingInterval;


    /*
     * If a shell arrives later than
     * the firing schedule, include
     * that flight time.
     */
    if (lastImpactTime > result->duration)
    {
        result->duration = lastImpactTime;
    }
}


/*
 * Run one Part 2-A round.
 *
 * impactMode = 0
 *     Part 1-A / Part 1-B
 *
 * impactMode = 1
 *     Part 1-C
 */
static void runPart2ARound(
    Battlefield *field,
    double minimumAngle,
    double maximumAngle,
    double firingInterval,
    int impactMode,
    double *cumulativeImpact,
    BattleResult *result,
    int order[],
    int *orderCount)
{
    int i;

    int killerEscortId;
    int lastImpactEscortId;

    double killerTime;


    result->battleshipSunk = 0;
    result->killerEscortId = -1;
    result->killerTime = 0.0;

    result->hitCount = 0;
    result->duration = 0.0;

    result->cumulativeImpact =
        *cumulativeImpact;


    killerEscortId = -1;
    killerTime = 0.0;

    lastImpactEscortId = -1;


    /*
     * Every alive Escort gets
     * one firing chance.
     */
    for (i = 0; i < field->escortCount; i++)
    {
        double velocity;
        double angle;
        double time;

        if (field->escorts[i].status != ALIVE)
        {
            continue;
        }


        velocity =
            randomDouble(
                field->escorts[i].minVelocity,
                field->escorts[i].maxVelocity);

        field->escorts[i].shotsFired++;


        if (canHit(
                field->escorts[i].position,
                field->battleship.position,
                velocity,
                field->escorts[i].minAngle,
                field->escorts[i].maxAngle,
                &angle,
                &time))
        {
            field->escorts[i].lastShotVelocity =
                velocity;

            field->escorts[i].lastShotAngle =
                angle;

            field->escorts[i].lastFlightTime =
                time;


            /*
             * Part 1-A and Part 1-B:
             * one successful E hit
             * destroys B.
             */
            if (!impactMode)
            {
                if (killerEscortId == -1 ||
                    time < killerTime)
                {
                    killerEscortId =
                        field->escorts[i].id;

                    killerTime = time;
                }
            }
            else
            {
                /*
                 * Part 1-C:
                 * add impact power.
                 */
                *cumulativeImpact +=
                    field->escorts[i].impactPower;

                lastImpactEscortId =
                    field->escorts[i].id;

                printf(
                    "Escort #%d hit B "
                    "(impact %.2f%%).\n",
                    field->escorts[i].id,
                    field->escorts[i].impactPower *
                    100.0);
            }
        }
    }


    /*
     * Part 1-A / Part 1-B:
     * one E hit is enough to sink B.
     */
    if (!impactMode &&
        killerEscortId != -1)
    {
        field->battleship.health = 0.0;
        field->battleship.status = SUNK;

        result->battleshipSunk = 1;

        result->killerEscortId =
            killerEscortId;

        result->killerTime =
            killerTime;

        result->duration =
            killerTime;

        *orderCount = 0;


        printf(
            "Escort #%d sank the Battleship.\n",
            killerEscortId);

        printf(
            "Time to hit: %.2f seconds\n",
            killerTime);

        return;
    }


    /*
     * Part 1-C:
     * calculate remaining B health.
     */
    if (impactMode)
    {
        field->battleship.health =
            1.0 - *cumulativeImpact;

        if (field->battleship.health < 0.0)
        {
            field->battleship.health = 0.0;
        }


        /*
         * If cumulative impact reaches
         * 100%, B is destroyed.
         */
        if (*cumulativeImpact >= 1.0)
        {
            field->battleship.status = SUNK;

            result->battleshipSunk = 1;

            result->killerEscortId =
                lastImpactEscortId;

            result->killerTime = 0.0;

            result->duration = 0.0;

            *orderCount = 0;


            printf(
                "Battleship destroyed by "
                "cumulative impact.\n");

            return;
        }
    }


    /*
     * B survived, so B attacks.
     */
    runStrategicBattleshipAttack(
        field,
        minimumAngle,
        maximumAngle,
        firingInterval,
        result,
        order,
        orderCount);


    result->cumulativeImpact =
        *cumulativeImpact;
}


/*
 * Save one Part 2-A result.
 */
static void savePart2AResult(
    const Battlefield *field,
    const BattleResult *result,
    const int order[],
    int orderCount,
    double firingInterval,
    const char *partName,
    const char *simulationName,
    int step,
    double minimumAngle)
{
    char filename[120];

    FILE *file;

    int i;
    int destroyed;


    sprintf(
        filename,
        "part2A_%s_%s",
        partName,
        simulationName);


    if (step > 0)
    {
        sprintf(
            filename + strlen(filename),
            "_step%d.txt",
            step);
    }
    else
    {
        sprintf(
            filename + strlen(filename),
            ".txt");
    }


    file = fopen(
        filename,
        "w");


    if (file == NULL)
    {
        printf(
            "Error: Cannot create %s\n",
            filename);

        return;
    }


    fprintf(
        file,
        "PART 2-A - %s - %s\n\n",
        partName,
        simulationName);


    fprintf(
        file,
        "Battleship firing interval: "
        "%.2f seconds\n",
        firingInterval);


    fprintf(
        file,
        "B firing angle range: "
        "%.2f - 90.00 degrees\n",
        minimumAngle);


    if (step > 0)
    {
        fprintf(
            file,
            "Path step: %d\n",
            step);
    }


    fprintf(
        file,
        "\nBattleship position: "
        "(%.2f, %.2f)\n",
        field->battleship.position.x,
        field->battleship.position.y);


    saveAttackOrder(
        file,
        field,
        order,
        orderCount);


    fprintf(
        file,
        "\nBATTLE RESULT\n");

    fprintf(
        file,
        "-------------\n");


    fprintf(
        file,
        "Battleship Status: %s\n",
        field->battleship.status == ALIVE
            ? "SURVIVED" : "SUNK");


    fprintf(
        file,
        "Battleship Health: %.2f%%\n",
        field->battleship.health * 100.0);


    fprintf(
        file,
        "Total Battle Time: %.2f seconds\n",
        result->duration);


    fprintf(
        file,
        "Escorts Destroyed: %d\n",
        result->hitCount);


    fprintf(
        file,
        "Cumulative Impact: %.2f%%\n",
        result->cumulativeImpact * 100.0);


    if (result->battleshipSunk)
    {
        fprintf(
            file,
            "Final Escort: #%d\n",
            result->killerEscortId);
    }


    fprintf(
        file,
        "\nESCORTS HIT BY B\n");


    for (i = 0; i < result->hitCount; i++)
    {
        fprintf(
            file,
            "Escort #%d - %.2f seconds\n",
            result->hitIds[i],
            result->hitTimes[i]);
    }


    fprintf(
        file,
        "\nESCORT STATUS\n");


    destroyed = 0;


    for (i = 0; i < field->escortCount; i++)
    {
        if (field->escorts[i].status == SUNK)
        {
            destroyed++;
        }

        fprintf(
            file,
            "Escort #%d: %s\n",
            field->escorts[i].id,
            field->escorts[i].status == ALIVE
                ? "ALIVE" : "SUNK");
    }


    fprintf(
        file,
        "\nEscorts Remaining: %d\n",
        field->escortCount - destroyed);


    fclose(file);
}


/*
 * Run Part 2-A with Part 1-A rules.
 */
static void runPart2A_1A(
    const Battlefield *initial,
    double firingInterval,
    unsigned int randomSeed)
{
    Battlefield simulation;

    BattleResult result;

    int order[MAX_ESCORTS];
    int orderCount;

    double cumulativeImpact;


    resetBattlefield(
        &simulation,
        initial);


    srand(randomSeed);


    printf("\n");
    printf("========================================\n");
    printf("PART 2-A - PART 1-A\n");
    printf("========================================\n");


    cumulativeImpact = 0.0;


    runPart2ARound(
        &simulation,
        0.0,
        90.0,
        firingInterval,
        0,
        &cumulativeImpact,
        &result,
        order,
        &orderCount);


    printAttackOrder(
        &simulation,
        order,
        orderCount);


    printf("\n");

    printf(
        "Battleship Status : %s\n",
        simulation.battleship.status == ALIVE
            ? "SURVIVED" : "SUNK");


    printf(
        "Total Battle Time : %.2f seconds\n",
        result.duration);


    printf(
        "Escorts Destroyed : %d\n",
        result.hitCount);


    savePart2AResult(
        &simulation,
        &result,
        order,
        orderCount,
        firingInterval,
        "PART1A",
        "SIMULATION",
        0,
        0.0);
}


/*
 * Run one Part 2-A path simulation.
 */
static void runPart2APathSimulation(
    Battlefield *field,
    const Battlefield *initial,
    const Point path[],
    int numberOfPoints,
    int jamIteration,
    double jamMinAngle,
    double firingInterval,
    int impactMode,
    int useJam,
    unsigned int randomSeed,
    const char *partName)
{
    BattleResult result;

    int order[MAX_ESCORTS];
    int orderCount;

    double totalTime;
    double cumulativeImpact;

    int i;


    resetBattlefield(
        field,
        initial);


    /*
     * Simulation 1 starts from
     * the same saved random seed.
     */
    if (!useJam)
    {
        srand(randomSeed);
    }


    totalTime = 0.0;
    cumulativeImpact = 0.0;


    printf("\n");
    printf("========================================\n");

    if (impactMode)
    {
        printf(
            "PART 2-A - PART 1-C - %s\n",
            useJam
                ? "SIMULATION2"
                : "SIMULATION1");
    }
    else
    {
        printf(
            "PART 2-A - PART 1-B - %s\n",
            useJam
                ? "SIMULATION2"
                : "SIMULATION1");
    }

    printf("========================================\n");


    for (i = 0; i < numberOfPoints; i++)
    {
        double minimumAngle;


        /*
         * Move B to the current path point.
         */
        field->battleship.position =
            path[i];


        minimumAngle = 0.0;


        /*
         * After t iterations,
         * B's gun is jammed.
         */
        if (useJam &&
            i + 1 > jamIteration)
        {
            minimumAngle = jamMinAngle;
        }


        result.cumulativeImpact =
            cumulativeImpact;


        printf("\n");
        printf(
            "--- Step %d ---\n",
            i + 1);


        printf(
            "B position: (%.2f, %.2f)\n",
            path[i].x,
            path[i].y);


        printf(
            "B angle range: %.2f - 90.00\n",
            minimumAngle);


        if (useJam &&
            i + 1 > jamIteration)
        {
            printf(
                "Gun status: JAMMED\n");
        }
        else
        {
            printf(
                "Gun status: NORMAL\n");
        }


        runPart2ARound(
            field,
            minimumAngle,
            90.0,
            firingInterval,
            impactMode,
            &cumulativeImpact,
            &result,
            order,
            &orderCount);


        result.cumulativeImpact =
            cumulativeImpact;


        /*
         * Add this step's time.
         */
        totalTime += result.duration;


        /*
         * Show the selected attack order.
         */
        printAttackOrder(
            field,
            order,
            orderCount);


        printf(
            "Step battle time: %.2f seconds\n",
            result.duration);


        if (impactMode)
        {
            printf(
                "Cumulative impact: %.2f%%\n",
                cumulativeImpact * 100.0);


            printf(
                "B health: %.2f%%\n",
                field->battleship.health *
                100.0);
        }


        /*
         * Save this step.
         */
        savePart2AResult(
            field,
            &result,
            order,
            orderCount,
            firingInterval,
            partName,
            useJam
                ? "SIMULATION2"
                : "SIMULATION1",
            i + 1,
            minimumAngle);


        /*
         * Stop when B is destroyed.
         */
        if (result.battleshipSunk)
        {
            printf(
                "Battleship destroyed "
                "at step %d.\n",
                i + 1);

            break;
        }
    }


    printf("\n");
    printf(
        "========== BATTLE SUMMARY ==========\n");


    printf(
        "Battleship Status : %s\n",
        field->battleship.status == ALIVE
            ? "SURVIVED" : "SUNK");


    printf(
        "Total Battle Time : %.2f seconds\n",
        totalTime);


    if (impactMode)
    {
        printf(
            "Total Impact      : %.2f%%\n",
            cumulativeImpact * 100.0);
    }


    {
        int destroyed = 0;

        for (i = 0; i < field->escortCount; i++)
        {
            if (field->escorts[i].status == SUNK)
            {
                destroyed++;
            }
        }


        printf(
            "Escorts Destroyed : %d\n",
            destroyed);


        printf(
            "Escorts Remaining : %d\n",
            field->escortCount -
            destroyed);
    }


    printf(
        "====================================\n");
}


/*
 * Run complete Part 2-A.
 */
void runPart2A(Battlefield *field)
{
    Battlefield initial;

    Battlefield simulationB1;
    Battlefield simulationB2;

    Battlefield simulationC1;
    Battlefield simulationC2;

    Point pathB[MAX_PATH_POINTS];
    Point pathC[MAX_PATH_POINTS];

    int numberOfPoints;
    int jamIteration;

    double jamMinAngle;
    double firingInterval;

    unsigned int randomSeed;


    /*
     * Keep the original battlefield.
     */
    initial = *field;


    printf("\n");
    printf("========================================\n");
    printf("PART 2-A SETUP\n");
    printf("========================================\n");


    /*
     * Get B firing interval.
     */
    do
    {
        printf(
            "Enter B firing interval TB "
            "(seconds, > 0): ");

        scanf(
            "%lf",
            &firingInterval);


        if (firingInterval <= 0.0)
        {
            printf(
                "Invalid firing interval.\n");
        }

    } while (firingInterval <= 0.0);


    /*
     * Get number of path points.
     */
    do
    {
        printf(
            "Enter number of path points "
            "(1-%d): ",
            MAX_PATH_POINTS);

        scanf(
            "%d",
            &numberOfPoints);


        if (numberOfPoints < 1 ||
            numberOfPoints > MAX_PATH_POINTS)
        {
            printf(
                "Invalid number of points.\n");
        }

    } while (
        numberOfPoints < 1 ||
        numberOfPoints > MAX_PATH_POINTS);


    /*
     * Get gun jam iteration.
     */
    do
    {
        printf(
            "Enter gun jam iteration t "
            "(0 < t < %d): ",
            numberOfPoints);

        scanf(
            "%d",
            &jamIteration);


        if (jamIteration <= 0 ||
            jamIteration >= numberOfPoints)
        {
            printf(
                "Invalid t. "
                "It must satisfy 0 < t < k.\n");
        }

    } while (
        jamIteration <= 0 ||
        jamIteration >= numberOfPoints);


    /*
     * Get jam minimum angle.
     */
    do
    {
        printf(
            "Enter jam minimum angle "
            "theta_min "
            "(0 < theta_min < 30): ");

        scanf(
            "%lf",
            &jamMinAngle);


        if (jamMinAngle <= 0.0 ||
            jamMinAngle >= 30.0)
        {
            printf(
                "Invalid angle.\n");
        }

    } while (
        jamMinAngle <= 0.0 ||
        jamMinAngle >= 30.0);


    /*
     * Create one random seed.
     */
    randomSeed =
        (unsigned int)rand();

    srand(randomSeed);


    /*
     * Generate one path for Part 1-B.
     *
     * Both simulations use the
     * same path.
     */
    generatePath(
        pathB,
        numberOfPoints,
        field->size);


    printf("\n");
    printf("PART 1-B PATH\n");
    printf("--------------\n");


    {
        int i;

        for (i = 0; i < numberOfPoints; i++)
        {
            printf(
                "Point %d : (%.2f, %.2f)\n",
                i + 1,
                pathB[i].x,
                pathB[i].y);
        }
    }


    /*
     * Run Part 2-A with Part 1-A rules.
     */
    runPart2A_1A(
        &initial,
        firingInterval,
        randomSeed);


    /*
     * Part 1-B Simulation 1.
     */
    runPart2APathSimulation(
        &simulationB1,
        &initial,
        pathB,
        numberOfPoints,
        jamIteration,
        jamMinAngle,
        firingInterval,
        0,
        0,
        randomSeed,
        "PART1B");


    /*
     * Part 1-B Simulation 2.
     *
     * Random seed is not reset.
     */
    runPart2APathSimulation(
        &simulationB2,
        &initial,
        pathB,
        numberOfPoints,
        jamIteration,
        jamMinAngle,
        firingInterval,
        0,
        1,
        randomSeed,
        "PART1B");


    /*
     * Create a separate path for Part 1-C.
     *
     * Both C simulations use
     * the same path.
     */
    randomSeed =
        (unsigned int)rand();

    srand(randomSeed);


    generatePath(
        pathC,
        numberOfPoints,
        field->size);


    printf("\n");
    printf("PART 1-C PATH\n");
    printf("--------------\n");


    {
        int i;

        for (i = 0; i < numberOfPoints; i++)
        {
            printf(
                "Point %d : (%.2f, %.2f)\n",
                i + 1,
                pathC[i].x,
                pathC[i].y);
        }
    }


    /*
     * Part 1-C Simulation 1.
     */
    runPart2APathSimulation(
        &simulationC1,
        &initial,
        pathC,
        numberOfPoints,
        jamIteration,
        jamMinAngle,
        firingInterval,
        1,
        0,
        randomSeed,
        "PART1C");


    /*
     * Part 1-C Simulation 2.
     *
     * Random seed is not reset.
     */
    runPart2APathSimulation(
        &simulationC2,
        &initial,
        pathC,
        numberOfPoints,
        jamIteration,
        jamMinAngle,
        firingInterval,
        1,
        1,
        randomSeed,
        "PART1C");


    printf("\n");
    printf(
        "========================================\n");
    printf(
        "PART 2-A COMPLETED\n");
    printf(
        "========================================\n");
}
