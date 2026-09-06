#include <stdio.h>
#include <stdlib.h>

#include "part1B.h"
#include "part1A.h"
#include "common.h"

static void generatePath(
    Point path[],
    int count,
    double size)
{
    int i;

    for (i = 0; i < count; i++)
        path[i] = generateRandomPosition(size);
}

static void printPath(
    const Point path[],
    int count)
{
    int i;

    printf("\nGENERATED BATTLESHIP PATH\n");
    printf("-------------------------\n");

    for (i = 0; i < count; i++)
        printf("Point %d : (%.2f, %.2f)\n",
               i + 1, path[i].x, path[i].y);
}

static void resetBattlefield(
    Battlefield *field,
    const Battlefield *initial)
{
    int i;

    *field = *initial;

    field->battleship.status = ALIVE;
    field->battleship.health = 1.0;
    field->battleship.shotsFired = 0;

    for (i = 0; i < field->escortCount; i++)
    {
        field->escorts[i].status = ALIVE;
        field->escorts[i].health = 1.0;
        field->escorts[i].shotsFired = 0;
        field->escorts[i].lastFlightTime = 0.0;
    }
}

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

    sprintf(filename,
            "part1B_%s_step%d.txt",
            simulation,
            step);

    file = fopen(filename, "w");

    if (file == NULL)
    {
        printf("Error: Cannot create %s\n", filename);
        return;
    }

    fprintf(file,
            "PART 1-B %s - STEP %d\n\n",
            simulation, step);

    fprintf(file,
            "Battleship position: (%.2f, %.2f)\n",
            position.x, position.y);

    fprintf(file,
            "B firing angle: %.2f - 90.00 degrees\n",
            minimumAngle);

    fprintf(file, "Battleship status: %s\n",
            field->battleship.status == ALIVE
                ? "ALIVE" : "SUNK");

    if (result->battleshipSunk)
    {
        fprintf(file,
                "Sunk by Escort: #%d\n",
                result->killerEscortId);

        fprintf(file,
                "Time to hit: %.2f seconds\n",
                result->killerTime);
    }
    else
    {
        fprintf(file,
                "\nEscorts hit by Battleship: %d\n",
                result->hitCount);

        for (i = 0; i < result->hitCount; i++)
        {
            fprintf(file,
                    "Escort #%d - %.2f seconds\n",
                    result->hitIds[i],
                    result->hitTimes[i]);
        }

        fprintf(file,
                "Battle duration: %.2f seconds\n",
                result->duration);
    }

    fprintf(file, "\nESCORT STATUS\n");

    for (i = 0; i < field->escortCount; i++)
    {
        fprintf(file,
                "Escort #%d: %s\n",
                field->escorts[i].id,
                field->escorts[i].status == ALIVE
                    ? "ALIVE" : "SUNK");
    }

    fclose(file);
}

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
    BattleResult result;

    resetBattlefield(field, initial);
    srand(seed);

    printf("\n========================================\n");
    printf("PART 1-B - %s\n", name);
    printf("========================================\n");

    for (i = 0; i < count; i++)
    {
        field->battleship.position = path[i];

        minimumAngle = 0.0;

        if (useJam && i + 1 > jamIteration)
            minimumAngle = jamAngle;

        printf("\n--- %s : Step %d ---\n",
               name, i + 1);

        printf("Battleship position: (%.2f, %.2f)\n",
               path[i].x, path[i].y);

        if (useJam && i + 1 > jamIteration)
            printf("Gun status: JAMMED\n");
        else
            printf("Gun status: NORMAL\n");

        printf("B angle range: %.2f - 90.00 degrees\n",
               minimumAngle);

        runPart1ARound(
            field,
            minimumAngle,
            90.0,
            &result);

        saveStep(
            field,
            &result,
            path[i],
            i + 1,
            name,
            minimumAngle);

        if (result.battleshipSunk)
        {
            printf("Battleship destroyed at step %d.\n",
                   i + 1);
            break;
        }

        printf("Battleship survived step %d.\n",
               i + 1);
    }
}

void runPart1B(Battlefield *field)
{
    Battlefield initial;
    Battlefield simulation;

    Point path[MAX_PATH_POINTS];

    int count;
    int jamIteration;
    double jamAngle;
    unsigned int seed;

    initial = *field;

    printf("\n========================================\n");
    printf("PART 1-B SETUP\n");
    printf("========================================\n");

    do
    {
        printf("Enter number of path points (1-%d): ",
               MAX_PATH_POINTS);
        scanf("%d", &count);
    }
    while (count < 1 || count > MAX_PATH_POINTS);

    do
    {
        printf("Enter gun jam iteration t (0 < t < %d): ",
               count);
        scanf("%d", &jamIteration);
    }
    while (jamIteration <= 0 || jamIteration >= count);

    do
    {
        printf("Enter jam minimum angle theta_min "
               "(0 < theta_min < 30): ");
        scanf("%lf", &jamAngle);
    }
    while (jamAngle <= 0.0 || jamAngle >= 30.0);

    seed = (unsigned int)rand();

    srand(seed);
    generatePath(path, count, field->size);
    printPath(path, count);

    runSimulation(
        &simulation,
        &initial,
        path,
        count,
        jamIteration,
        jamAngle,
        0,
        seed,
        "SIMULATION 1");

    runSimulation(
        &simulation,
        &initial,
        path,
        count,
        jamIteration,
        jamAngle,
        1,
        seed,
        "SIMULATION 2");

    printf("\n========================================\n");
    printf("PART 1-B COMPLETED\n");
    printf("========================================\n");
}
