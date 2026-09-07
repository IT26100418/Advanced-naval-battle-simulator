#include <stdio.h>
#include <stdlib.h>

#include "part1B.h"
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


/* Print generated path */
static void printPath(
    const Point path[],
    int numberOfPoints)
{
    int i;

    printf("\n");
    printf("GENERATED BATTLESHIP PATH\n");
    printf("-------------------------\n");

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
        "part1B_%s_step%d.txt",
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
        "PART 1-B %s - STEP %d\n\n",
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

    if (result->battleshipSunk)
    {
        fprintf(
            file,
            "Sunk by Escort: #%d\n",
            result->killerEscortId);

        fprintf(
            file,
            "Time to hit: %.2f seconds\n",
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

    fprintf(file, "\nESCORT STATUS\n");

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


/* Save final Part 1-B summary */
static void saveSummary(
    const Battlefield *field,
    double totalTime,
    const char *simulation)
{
    char filename[100];
    FILE *file;
    int i;
    int destroyed = 0;

    sprintf(
        filename,
        "part1B_%s_summary.txt",
        simulation);

    file = fopen(filename, "w");

    if (file == NULL)
    {
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
        "PART 1-B %s SUMMARY\n\n",
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
        "Escorts Destroyed: %d\n",
        destroyed);

    fprintf(
        file,
        "Escorts Remaining: %d\n",
        field->escortCount - destroyed);

    fclose(file);
}


/* Run one Part 1-B simulation */
static void runSimulation(
    Battlefield *field,
    const Battlefield *initial,
    const Point path[],
    int count,
    int jamIteration,
    double jamAngle,
    int useJam,
    unsigned int seed,
    const char *name)
{
    int i;
    double minimumAngle;
    double totalTime = 0.0;
    BattleResult result;

    resetBattlefield(
        field,
        initial);

    srand(seed);

    printf("\n");
    printf("========================================\n");
    printf("PART 1-B - %s\n", name);
    printf("========================================\n");

    for (i = 0;
         i < count;
         i++)
    {
        field->battleship.position =
            path[i];

        minimumAngle = 0.0;

        /*
         * Jam starts after t.
         */
        if (useJam &&
            i + 1 > jamIteration)
        {
            minimumAngle = jamAngle;
        }

        printf(
            "\n--- %s : Step %d ---\n",
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

        runPart1ARound(
            field,
            minimumAngle,
            90.0,
            &result);

        /*
         * Add this step time
         * to total battle time.
         */
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

        printf(
            "Battleship survived "
            "step %d.\n",
            i + 1);
    }

    printf(
        "\n========== BATTLE SUMMARY ==========\n");

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
        "Total Impact      : NOT USED IN PART 1-B\n");

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
            field->escortCount - destroyed);
    }

    printf(
        "====================================\n");

    saveSummary(
        field,
        totalTime,
        name);
}


/* Run complete Part 1-B */
void runPart1B(Battlefield *field)
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
    printf("PART 1-B SETUP\n");
    printf("========================================\n");

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
     * Create one path for both simulations.
     */
    randomSeed =
        (unsigned int)rand();

    srand(randomSeed);

    generatePath(
        path,
        numberOfPoints,
        field->size);

    printPath(
        path,
        numberOfPoints);

    /*
     * Simulation 1.
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
     * Simulation 2.
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
    printf("========================================\n");
    printf("PART 1-B COMPLETED\n");
    printf("========================================\n");
}
