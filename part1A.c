#include <stdio.h>

#include "part1A.h"
#include "common.h"


/* Save initial conditions */
static void saveInitialConditions(
    const Battlefield *field)
{
    FILE *file;
    int i;

    file = fopen("part1A_initial.txt", "w");

    if (file == NULL)
    {
        printf("Error: Cannot create initial file.\n");
        return;
    }

    fprintf(file,
            "PART 1-A INITIAL CONDITIONS\n\n");

    fprintf(file,
            "Battlefield size: %.2f\n\n",
            field->size);

    fprintf(file, "BATTLESHIP\n");

    fprintf(file,
            "Type: %c\n",
            field->battleship.notation);

    fprintf(file,
            "Position: (%.2f, %.2f)\n",
            field->battleship.position.x,
            field->battleship.position.y);

    fprintf(file,
            "Maximum velocity: %.2f\n",
            field->battleship.maxVelocity);

    fprintf(file,
            "Health: %.2f\n",
            field->battleship.health);

    fprintf(file, "\nESCORT SHIPS\n");

    for (i = 0; i < field->escortCount; i++)
    {
        const EscortShip *e =
            &field->escorts[i];

        fprintf(file,
                "\nEscort #%d\n",
                e->id);

        fprintf(file,
                "Type: %s\n",
                getEscortTypeName(e->type));

        fprintf(file,
                "Position: (%.2f, %.2f)\n",
                e->position.x,
                e->position.y);

        fprintf(file,
                "Minimum velocity: %.2f\n",
                e->minVelocity);

        fprintf(file,
                "Maximum velocity: %.2f\n",
                e->maxVelocity);

        fprintf(file,
                "Minimum angle: %.2f\n",
                e->minAngle);

        fprintf(file,
                "Maximum angle: %.2f\n",
                e->maxAngle);

        fprintf(file,
                "Impact power: %.2f\n",
                e->impactPower);
    }

    fclose(file);
}


/* Save final conditions */
static void saveFinalConditions(
    const Battlefield *field,
    const BattleResult *result)
{
    FILE *file;
    int i;

    file = fopen("part1A_final.txt", "w");

    if (file == NULL)
    {
        printf("Error: Cannot create final file.\n");
        return;
    }

    fprintf(file,
            "PART 1-A FINAL CONDITIONS\n\n");

    fprintf(file,
            "Battleship position: (%.2f, %.2f)\n",
            field->battleship.position.x,
            field->battleship.position.y);

    fprintf(file,
            "Battleship status: %s\n",
            field->battleship.status == ALIVE
                ? "ALIVE" : "SUNK");

    fprintf(file,
            "Battleship health: %.2f\n",
            field->battleship.health);

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
                "\nESCORTS HIT BY BATTLESHIP\n");

        for (i = 0;
             i < result->hitCount;
             i++)
        {
            fprintf(file,
                    "Escort #%d - %.2f seconds\n",
                    result->hitIds[i],
                    result->hitTimes[i]);
        }

        fprintf(file,
                "\nBattle duration: %.2f seconds\n",
                result->duration);
    }

    fprintf(file, "\nESCORT STATUS\n");

    for (i = 0;
         i < field->escortCount;
         i++)
    {
        fprintf(file,
                "Escort #%d: %s\n",
                field->escorts[i].id,
                field->escorts[i].status == ALIVE
                    ? "ALIVE" : "SUNK");
    }

    fclose(file);
}


/*
 * B attacks all alive Escorts
 * that it can hit.
 */
void runBattleshipAttack(
    Battlefield *field,
    double minimumBAngle,
    double maximumBAngle,
    BattleResult *result)
{
    int i;

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
                0.0,
                field->battleship.maxVelocity);

        field->battleship.shotsFired++;

        if (canHit(
                field->battleship.position,
                field->escorts[i].position,
                velocity,
                minimumBAngle,
                maximumBAngle,
                &angle,
                &time))
        {
            field->escorts[i].health = 0.0;
            field->escorts[i].status = SUNK;

            field->battleship.lastShotVelocity =
                velocity;

            field->battleship.lastShotAngle =
                angle;

            field->battleship.lastFlightTime =
                time;

            result->hitIds[result->hitCount] =
                field->escorts[i].id;

            result->hitTimes[result->hitCount] =
                time;

            result->hitCount++;

            if (time > result->duration)
            {
                result->duration = time;
            }

            printf(
                "Battleship destroyed Escort #%d.\n",
                field->escorts[i].id);
        }
    }
}


/*
 * One Part 1-A battle round.
 */
void runPart1ARound(
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

    result->cumulativeImpact = 0.0;

    /*
     * Each E gets one chance.
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

            if (result->killerEscortId == -1 ||
                time < result->killerTime)
            {
                result->killerEscortId =
                    field->escorts[i].id;

                result->killerTime = time;
            }
        }
    }

    /*
     * One hit destroys B in Part 1-A.
     */
    if (result->killerEscortId != -1)
    {
        field->battleship.health = 0.0;

        field->battleship.status = SUNK;

        result->battleshipSunk = 1;

        printf(
            "Escort #%d sank the Battleship.\n",
            result->killerEscortId);

        printf(
            "Time to hit: %.2f seconds\n",
            result->killerTime);

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


/* Run complete Part 1-A */
void runPart1A(Battlefield *field)
{
    BattleResult result;

    printf(
        "\n----PART 1-A SIMULATION START----\n");

    saveInitialConditions(field);

    runPart1ARound(
        field,
        0.0,
        90.0,
        &result);

    if (result.battleshipSunk)
    {
        printf(
            "\n========== BATTLE SUMMARY ==========\n");

        printf(
            "Battleship Status : SUNK\n");

        printf(
            "Total Battle Time : %.2f seconds\n",
            result.killerTime);

        printf(
            "Total Impact      : NOT USED IN PART 1-A\n");

        printf(
            "Escorts Destroyed : %d\n",
            result.hitCount);

        printf(
            "Escorts Remaining : %d\n",
            field->escortCount -
            result.hitCount);

        printf(
            "====================================\n");

        saveFinalConditions(
            field,
            &result);

        printf(
            "----PART 1-A SIMULATION END----\n");

        return;
    }

    printf("\nBattleship survived.\n");

    printf(
        "\n========== BATTLE SUMMARY ==========\n");

    printf(
        "Battleship Status : SURVIVED\n");

    printf(
        "Total Battle Time : %.2f seconds\n",
        result.duration);

    printf(
        "Total Impact      : NOT USED IN PART 1-A\n");

    printf(
        "Escorts Destroyed : %d\n",
        result.hitCount);

    printf(
        "Escorts Remaining : %d\n",
        field->escortCount -
        result.hitCount);

    printf(
        "====================================\n");

    saveFinalConditions(
        field,
        &result);

    printf(
        "----PART 1-A SIMULATION END----\n");
}
