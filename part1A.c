#include <stdio.h>
#include "part1A.h"
#include "common.h"

/* Save the starting battlefield details */
static void saveInitialConditions(const Battlefield *field)
{
    FILE *file;
    int i;

    file = fopen("part1A_initial.txt", "w");

    if (file == NULL)
    {
        printf("Error: Cannot create initial file.\n");
        return;
    }

    fprintf(file, "PART 1-A INITIAL CONDITIONS\n\n");
    fprintf(file, "Battlefield size: %.2f\n\n", field->size);

    fprintf(file, "BATTLESHIP\n");
    fprintf(file, "Type: %c\n", field->battleship.notation);
    fprintf(file, "Position: (%.2f, %.2f)\n",
            field->battleship.position.x,
            field->battleship.position.y);
    fprintf(file, "Maximum velocity: %.2f\n",
            field->battleship.maxVelocity);

    fprintf(file, "\nESCORT SHIPS\n");

    for (i = 0; i < field->escortCount; i++)
    {
        const EscortShip *e = &field->escorts[i];

        fprintf(file, "\nEscort #%d\n", e->id);
        fprintf(file, "Type: %s\n", getEscortTypeName(e->type));
        fprintf(file, "Position: (%.2f, %.2f)\n",
                e->position.x,
                e->position.y);
        fprintf(file, "Velocity: %.2f - %.2f\n",
                e->minVelocity,
                e->maxVelocity);
        fprintf(file, "Angle: %.2f - %.2f\n",
                e->minAngle,
                e->maxAngle);
        fprintf(file, "Impact power: %.2f\n",
                e->impactPower);
    }

    fclose(file);
}

/* Save the final result */
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

    fprintf(file, "PART 1-A FINAL CONDITIONS\n\n");

    fprintf(file, "Battleship position: (%.2f, %.2f)\n",
            field->battleship.position.x,
            field->battleship.position.y);

    fprintf(file, "Battleship status: %s\n",
            field->battleship.status == ALIVE
                ? "ALIVE" : "SUNK");

    if (result->battleshipSunk)
    {
        fprintf(file, "Sunk by Escort: #%d\n",
                result->killerEscortId);

        fprintf(file, "Time to hit: %.2f seconds\n",
                result->killerTime);
    }
    else
    {
        fprintf(file, "\nESCORTS HIT BY BATTLESHIP\n");

        for (i = 0; i < result->hitCount; i++)
        {
            fprintf(file, "Escort #%d - %.2f seconds\n",
                    result->hitIds[i],
                    result->hitTimes[i]);
        }

        fprintf(file, "\nBattle duration: %.2f seconds\n",
                result->duration);
    }

    fprintf(file, "\nESCORT SHIPS\n");

    for (i = 0; i < field->escortCount; i++)
    {
        const EscortShip *e = &field->escorts[i];

        fprintf(file, "Escort #%d: %s\n",
                e->id,
                e->status == ALIVE ? "ALIVE" : "SUNK");
    }

    fclose(file);
}

/* Run one Part 1-A battle round */
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

    /* Each Escort gets one chance to fire */
    for (i = 0; i < field->escortCount; i++)
    {
        double velocity;
        double angle;
        double time;

        if (field->escorts[i].status != ALIVE)
            continue;

        velocity = randomDouble(
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
            field->escorts[i].lastShotVelocity = velocity;
            field->escorts[i].lastShotAngle = angle;
            field->escorts[i].lastFlightTime = time;

            /* Keep the fastest hit */
            if (result->killerEscortId == -1 ||
                time < result->killerTime)
            {
                result->killerEscortId =
                    field->escorts[i].id;

                result->killerTime = time;
            }
        }
    }

    /* First Escort shell to arrive destroys B */
    if (result->killerEscortId != -1)
    {
        field->battleship.health = 0.0;
        field->battleship.status = SUNK;
        result->battleshipSunk = 1;

        printf("Escort #%d sank the Battleship.\n",
               result->killerEscortId);

        printf("Time to hit: %.2f seconds\n",
               result->killerTime);

        return;
    }

    /* Battleship attacks all alive Escorts it can hit */
    for (i = 0; i < field->escortCount; i++)
    {
        double velocity;
        double angle;
        double time;

        if (field->escorts[i].status != ALIVE)
            continue;

        velocity = randomDouble(
            0.0,
            field->battleship.maxVelocity);

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

            field->battleship.shotsFired++;

            field->battleship.lastShotVelocity = velocity;
            field->battleship.lastShotAngle = angle;
            field->battleship.lastFlightTime = time;

            result->hitIds[result->hitCount] =
                field->escorts[i].id;

            result->hitTimes[result->hitCount] =
                time;

            result->hitCount++;

            if (time > result->duration)
                result->duration = time;

            printf("Battleship destroyed Escort #%d.\n",
                   field->escorts[i].id);
        }
    }
}

/* Main Part 1-A function */
void runPart1A(Battlefield *field)
{
    BattleResult result;

    printf("\n----PART 1-A SIMULATION START----\n");

    saveInitialConditions(field);

    runPart1ARound(
        field,
        0.0,
        90.0,
        &result);

    if (result.battleshipSunk)
    {
        saveFinalConditions(field, &result);

        printf("Battleship was destroyed.\n");
        printf("----PART 1-A SIMULATION END----\n");

        return;
    }

    printf("Battleship survived.\n");

    printf("Number of Escort ships hit by Battleship: %d\n",
           result.hitCount);

    if (result.hitCount > 0)
    {
        printf("Battle duration: %.2f seconds\n",
               result.duration);
    }
    else
    {
        printf("No Escort ships were hit.\n");
    }

    saveFinalConditions(field, &result);

    printf("----PART 1-A SIMULATION END----\n");
}
