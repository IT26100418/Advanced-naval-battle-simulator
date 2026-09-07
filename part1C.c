#include <stdio.h>
#include <stdlib.h>

#include "part1C.h"
#include "part1A.h"
#include "common.h"


/* Generate B path */
static void generatePath(
    Point path[],
    int numberOfPoints,
    double battlefieldSize)
{
    int i;

    for (i = 0;
         i < numberOfPoints;
         i++)
    {
        path[i] =
            generateRandomPosition(
                battlefieldSize);
    }
}


/* Reset battlefield */
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

    for (i = 0;
         i < field->escortCount;
         i++)
    {
        field->escorts[i].status = ALIVE;
        field->escorts[i].health = 1.0;
        field->escorts[i].shotsFired = 0;

        field->escorts[i].lastShotVelocity = 0.0;
        field->escorts[i].lastShotAngle = 0.0;
        field->escorts[i].lastFlightTime = 0.0;
    }
}


/* Save one simulation step */
static void saveStep(
    const Battlefield *field,
    const BattleResult *result,
    Point position,
    int step,
    const char *simulation,
    double minimumAngle)
{
    char filename[100];
    FILE *file;
    int i;

    sprintf(
        filename,
        "part1C_%s_step%d.txt",
        simulation,
        step);

    file = fopen(filename, "w");

    if (file == NULL)
    {
        printf(
            "Error: Cannot create %s\n",
            filename);
        return;
    }

    fprintf(
        file,
        "PART 1-C %s - STEP %d\n\n",
        simulation,
        step);

    fprintf(
        file,
        "Battleship position: "
        "(%.2f, %.2f)\n",
        position.x,
        position.y);

    fprintf(
        file,
        "B firing angle: %.2f - 90.00 degrees\n",
        minimumAngle);

    fprintf(
        file,
        "Battleship status: %s\n",
        field->battleship.status == ALIVE
            ? "ALIVE" : "SUNK");

    fprintf(
        file,
        "Battleship health: %.2f%%\n",
        field->battleship.health * 100.0);

    fprintf(
        file,
        "Cumulative impact: %.2f%%\n",
        result->cumulativeImpact * 100.0);

    if (result->battleshipSunk)
    {
        fprintf(
            file,
            "Final contributing Escort: #%d\n",
            result->killerEscortId);

        fprintf(
            file,
            "Time to final impact: %.2f seconds\n",
            result->killerTime);
    }
    else
    {
        fprintf(
            file,
            "\nEscorts hit by Battleship: %d\n",
            result->hitCount);

        for (i = 0;
             i < result->hitCount;
             i++)
        {
            fprintf(
                file,
                "Escort #%d - %.2f seconds\n",
                result->hitIds[i],
                result->hitTimes[i]);
        }

        fprintf(
            file,
            "Battle duration: %.2f seconds\n",
            result->duration);
    }

    fprintf(
        file,
        "\nESCORT STATUS\n");

    for (i = 0;
         i < field->escortCount;
         i++)
    {
        fprintf(
            file,
            "Escort #%d: %s\n",
            field->escorts[i].id,
            field->escorts[i].status == ALIVE
                ? "ALIVE" : "SUNK");
    }

    fclose(file);
}


/* Save final summary */
static void saveSummary(
    const Battlefield *field,
    const BattleResult *result,
    double totalTime,
    const char *simulation)
{
    char filename[100];
    FILE *file;
    int i;
    int destroyed = 0;

    sprintf(
        filename,
        "part1C_%s_summary.txt",
        simulation);

    file = fopen(filename, "w");

    if (file == NULL)
    {
        printf(
            "Error: Cannot create %s\n",
            filename);
        return;
    }

    for (i = 0;
         i < field->escortCount;
         i++)
    {
        if (field->escorts[i].status == SUNK)
        {
            destroyed++;
        }
    }

    fprintf(
        file,
        "PART 1-C %s SUMMARY\n\n",
        simulation);

    fprintf(
        file,
        "Battleship Status: %s\n",
        field->battleship.status == ALIVE
            ? "SURVIVED" : "SUNK");

    fprintf(
        file,
        "Total Battle Time: %.2f seconds\n",
        totalTime);

    fprintf(
        file,
        "Total Impact: %.2f%%\n",
        result->cumulativeImpact * 100.0);

    fprintf(
        file,
        "Escorts Destroyed: %d\n",
        destroyed);

    fprintf(
        file,
        "Escorts Remaining: %d\n",
        field->escortCount - destroyed);

    fclose(file);
}


/*
 * Run one Part 1-C battle round.
 *
 * E impact continues from
 * previous path points.
 */
static void runImpactRound(
    Battlefield *field,
    double minimumBAngle,
    double maximumBAngle,
    BattleResult *result)
{
    int i;

    int lastImpactEscortId = -1;
    double lastImpactTime = 0.0;

    result->battleshipSunk = 0;
    result->killerEscortId = -1;
    result->killerTime = 0.0;

    result->hitCount = 0;
    result->duration = 0.0;

    /*
     * Each alive E gets one chance.
     */
    for (i = 0;
         i < field->escortCount;
         i++)
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
             * Add E impact to B.
             */
            result->cumulativeImpact +=
                field->escorts[i].impactPower;

            /*
             * Remember the latest E
             * that contributed impact.
             */
            lastImpactEscortId =
                field->escorts[i].id;

            lastImpactTime = time;

            printf(
                "Escort #%d hit Battleship.\n",
                field->escorts[i].id);

            printf(
                "Impact power: %.2f%%\n",
                field->escorts[i].impactPower *
                100.0);
        }
    }

    /*
     * Calculate B health.
     */
    field->battleship.health =
        1.0 - result->cumulativeImpact;

    if (field->battleship.health < 0.0)
    {
        field->battleship.health = 0.0;
    }

    /*
     * B is destroyed when
     * cumulative impact reaches 100%.
     */
    if (result->cumulativeImpact >= 1.0)
    {
        field->battleship.status = SUNK;

        result->battleshipSunk = 1;

        result->killerEscortId =
            lastImpactEscortId;

        result->killerTime =
            lastImpactTime;

        printf(
            "Battleship was destroyed by "
            "cumulative Escort impact.\n");

        return;
    }

    /*
     * B survived, so B attacks.
     */
    runBattleshipAttack(
        field,
        minimumBAngle,
        maximumBAngle,
        result);
}


/* Run one complete simulation */
static void runSimulation(
    Battlefield *field,
    const Battlefield *initial,
    const Point path[],
    int numberOfPoints,
    int jamIteration,
    double jamMinAngle,
    int useJam,
    unsigned int randomSeed,
    const char *name)
{
    BattleResult result;
    double totalTime = 0.0;
    double cumulativeImpact = 0.0;

    int i;

    resetBattlefield(
        field,
        initial);

    /*
     * Start the first simulation
     * with the same random seed.
     */
    if (!useJam)
    {
        srand(randomSeed);
    }

    printf("\n");
    printf("========================================\n");
    printf("PART 1-C - %s\n", name);
    printf("========================================\n");

    for (i = 0;
         i < numberOfPoints;
         i++)
    {
        double minimumAngle = 0.0;

        field->battleship.position =
            path[i];

        /*
         * Jam starts after t.
         */
        if (useJam &&
            i + 1 > jamIteration)
        {
            minimumAngle = jamMinAngle;
        }

        /*
         * Keep cumulative impact
         * between path points.
         */
        result.cumulativeImpact =
            cumulativeImpact;

        printf("\n");
        printf(
            "--- %s : Step %d ---\n",
            name,
            i + 1);

        printf(
            "Battleship position: "
            "(%.2f, %.2f)\n",
            path[i].x,
            path[i].y);

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

        printf(
            "B angle range: %.2f - 90.00 degrees\n",
            minimumAngle);

        runImpactRound(
            field,
            minimumAngle,
            90.0,
            &result);

        cumulativeImpact =
            result.cumulativeImpact;

        if (result.battleshipSunk)
        {
            totalTime +=
                result.killerTime;
        }
        else
        {
            totalTime +=
                result.duration;
        }

        printf(
            "Cumulative impact: %.2f%%\n",
            result.cumulativeImpact *
            100.0);

        printf(
            "Battleship health: %.2f%%\n",
            field->battleship.health *
            100.0);

        saveStep(
            field,
            &result,
            path[i],
            i + 1,
            name,
            minimumAngle);

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

    if (field->battleship.status == SUNK)
    {
        printf(
            "Battleship Status : SUNK\n");
    }
    else
    {
        printf(
            "Battleship Status : SURVIVED\n");
    }

    printf(
        "Total Battle Time : %.2f seconds\n",
        totalTime);

    printf(
        "Total Impact      : %.2f%%\n",
        cumulativeImpact * 100.0);

    {
        int destroyed = 0;

        for (i = 0;
             i < field->escortCount;
             i++)
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

    result.cumulativeImpact =
        cumulativeImpact;

    saveSummary(
        field,
        &result,
        totalTime,
        name);
}


/* Run complete Part 1-C */
void runPart1C(Battlefield *field)
{
    Battlefield initial;
    Battlefield simulation;

    Point path[MAX_PATH_POINTS];

    int numberOfPoints;
    int jamIteration;

    double jamMinAngle;

    unsigned int randomSeed;

    initial = *field;

    printf("\n");
    printf("========================================\n");
    printf("PART 1-C SETUP\n");
    printf("========================================\n");

    /* Get number of path points */
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


    /* Get jam iteration */
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


    /* Get jam minimum angle */
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
                "Invalid angle. "
                "It must be between 0 and 30.\n");
        }

    } while (
        jamMinAngle <= 0.0 ||
        jamMinAngle >= 30.0);


    /*
     * Generate one path.
     * Both simulations use this path.
     */
    randomSeed =
        (unsigned int)rand();

    srand(randomSeed);

    generatePath(
        path,
        numberOfPoints,
        field->size);

    printf("\n");
    printf("GENERATED BATTLESHIP PATH\n");
    printf("-------------------------\n");

    {
        int i;

        for (i = 0;
             i < numberOfPoints;
             i++)
        {
            printf(
                "Point %d : (%.2f, %.2f)\n",
                i + 1,
                path[i].x,
                path[i].y);
        }
    }


    /*
     * Simulation 1:
     * B gun is normal.
     */
    runSimulation(
        &simulation,
        &initial,
        path,
        numberOfPoints,
        jamIteration,
        jamMinAngle,
        0,
        randomSeed,
        "SIMULATION1");


    /*
     * Simulation 2:
     * B gun becomes jammed after t.
     *
     * Do not reset the random seed here.
     */
    runSimulation(
        &simulation,
        &initial,
        path,
        numberOfPoints,
        jamIteration,
        jamMinAngle,
        1,
        randomSeed,
        "SIMULATION2");


    printf("\n");
    printf(
        "========================================\n");
    printf(
        "PART 1-C COMPLETED\n");
    printf(
        "========================================\n");
}
