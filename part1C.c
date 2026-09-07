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


/* Save one Part 1-C step */
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


/*
 * One Part 1-C battle round.
 *
 * Impact continues from previous steps.
 */
static void runImpactRound(
    Battlefield *field,
    double minimumBAngle,
    double maximumBAngle,
    BattleResult *result)
{
    int i;

    result->battleshipSunk = 0;
    result->killerEscortId = -1;
    result->killerTime = 0.0;

    result->hitCount = 0;
    result->duration = 0.0;

    /*
     * Every alive E gets one chance.
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
     * Update B health.
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

        /*
         * The last contributing E
         * is stored for the result.
         */
        for (i = field->escortCount - 1;
             i >= 0;
             i--)
        {
            if (field->escorts[i].status == ALIVE &&
                field->escorts[i].lastFlightTime > 0.0)
            {
                result->killerEscortId =
                    field->escorts[i].id;

                result->killerTime =
                    field->escorts[i].lastFlightTime;

                break;
            }
        }

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
     * Same path for both simulations.
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
     * ----------------------------
     * Simulation 1
     * ----------------------------
     */

    resetBattlefield(
        &simulation,
        &initial);

    srand(randomSeed);

    {
        BattleResult result;
        double totalTime = 0.0;
        int i;

        result.cumulativeImpact = 0.0;

        printf("\n");
        printf("========================================\n");
        printf("PART 1-C - SIMULATION1\n");
        printf("========================================\n");

        for (i = 0;
             i < numberOfPoints;
             i++)
        {
            simulation.battleship.position =
                path[i];

            printf(
                "\n--- SIMULATION1 : Step %d ---\n",
                i + 1);

            printf(
                "Battleship position: "
                "(%.2f, %.2f)\n",
                path[i].x,
                path[i].y);

            runImpactRound(
                &simulation,
                0.0,
                90.0,
                &result);

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
                simulation.battleship.health *
                100.0);

            saveStep(
                &simulation,
                &result,
                path[i],
                i + 1,
                "SIMULATION1",
                0.0);

            if (result.battleshipSunk)
            {
                break;
            }
        }

        printf(
            "\n========== BATTLE SUMMARY ==========\n");

        if (simulation.battleship.status == SUNK)
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
            result.cumulativeImpact *
            100.0);

        {
            int destroyed = 0;

            for (i = 0;
                 i < simulation.escortCount;
                 i++)
            {
                if (simulation.escorts[i].status ==
                    SUNK)
                {
                    destroyed++;
                }
            }

            printf(
                "Escorts Destroyed : %d\n",
                destroyed);

            printf(
                "Escorts Remaining : %d\n",
                simulation.escortCount -
                destroyed);
        }

        printf(
            "====================================\n");
    }


    /*
     * ----------------------------
     * Simulation 2
     * ----------------------------
     */

    resetBattlefield(
        &simulation,
        &initial);

    srand(randomSeed);

    {
        BattleResult result;
        double totalTime = 0.0;
        int i;

        result.cumulativeImpact = 0.0;

        printf("\n");
        printf("========================================\n");
        printf("PART 1-C - SIMULATION2\n");
        printf("========================================\n");

        for (i = 0;
             i < numberOfPoints;
             i++)
        {
            double minimumAngle = 0.0;

            simulation.battleship.position =
                path[i];

            if (i + 1 > jamIteration)
            {
                minimumAngle =
                    jamMinAngle;
            }

            printf(
                "\n--- SIMULATION2 : Step %d ---\n",
                i + 1);

            printf(
                "Battleship position: "
                "(%.2f, %.2f)\n",
                path[i].x,
                path[i].y);

            if (minimumAngle > 0.0)
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
                &simulation,
                minimumAngle,
                90.0,
                &result);

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
                simulation.battleship.health *
                100.0);

            saveStep(
                &simulation,
                &result,
                path[i],
                i + 1,
                "SIMULATION2",
                minimumAngle);

            if (result.battleshipSunk)
            {
                break;
            }
        }

        printf(
            "\n========== BATTLE SUMMARY ==========\n");

        if (simulation.battleship.status == SUNK)
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
            result.cumulativeImpact *
            100.0);

        {
            int destroyed = 0;

            for (i = 0;
                 i < simulation.escortCount;
                 i++)
            {
                if (simulation.escorts[i].status ==
                    SUNK)
                {
                    destroyed++;
                }
            }

            printf(
                "Escorts Destroyed : %d\n",
                destroyed);

            printf(
                "Escorts Remaining : %d\n",
                simulation.escortCount -
                destroyed);
        }

        printf(
            "====================================\n");
    }

    printf("\n");
    printf("========================================\n");
    printf("PART 1-C COMPLETED\n");
    printf("========================================\n");
}
