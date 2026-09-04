#include <stdio.h>

#include "part1A.h"
#include "common.h"


/*
 * Save initial battlefield conditions.
 */
static void saveInitialConditions(
    const Battlefield *field)
{
    FILE *file;
    int i;


    file = fopen(
        "part1A_initial.txt",
        "w"
    );


    if (file == NULL)
    {
        printf(
            "Error opening "
            "part1A_initial.txt.\n"
        );

        return;
    }


    fprintf(
        file,
        "PART 1-A INITIAL CONDITIONS\n\n"
    );


    fprintf(
        file,
        "Battlefield size: %.2f\n\n",
        field->size
    );


    /*
     * Battleship.
     */
    fprintf(
        file,
        "BATTLESHIP\n"
    );


    fprintf(
        file,
        "Type: %c\n",
        field->battleship.notation
    );


    fprintf(
        file,
        "Position: (%.2f, %.2f)\n",
        field->battleship.position.x,
        field->battleship.position.y
    );


    fprintf(
        file,
        "Maximum velocity: %.2f\n",
        field->battleship.maxVelocity
    );


    fprintf(
        file,
        "Health: %.2f\n",
        field->battleship.health
    );


    fprintf(
        file,
        "\nESCORT SHIPS\n\n"
    );


    /*
     * Escorts.
     */
    for (i = 0;
         i < field->escortCount;
         i++)
    {
        fprintf(
            file,
            "Escort #%d\n",
            field->escorts[i].id
        );


        fprintf(
            file,
            "Type: %s\n",
            getEscortTypeName(
                field->escorts[i].type
            )
        );


        fprintf(
            file,
            "Position: (%.2f, %.2f)\n",
            field->escorts[i].position.x,
            field->escorts[i].position.y
        );


        fprintf(
            file,
            "Minimum velocity: %.2f\n",
            field->escorts[i].minVelocity
        );


        fprintf(
            file,
            "Maximum velocity: %.2f\n",
            field->escorts[i].maxVelocity
        );


        fprintf(
            file,
            "Minimum angle: %.2f\n",
            field->escorts[i].minAngle
        );


        fprintf(
            file,
            "Maximum angle: %.2f\n",
            field->escorts[i].maxAngle
        );


        fprintf(
            file,
            "Impact power: %.2f\n",
            field->escorts[i].impactPower
        );


        fprintf(
            file,
            "Health: %.2f\n\n",
            field->escorts[i].health
        );
    }


    fclose(file);
}


/*
 * Save final battlefield conditions.
 */
static void saveFinalConditions(
    const Battlefield *field,
    int sunkByEscort,
    double timeToHit)
{
    FILE *file;
    int i;


    file = fopen(
        "part1A_final.txt",
        "w"
    );


    if (file == NULL)
    {
        printf(
            "Error opening "
            "part1A_final.txt.\n"
        );

        return;
    }


    fprintf(
        file,
        "PART 1-A FINAL CONDITIONS\n\n"
    );


    fprintf(
        file,
        "Battlefield size: %.2f\n\n",
        field->size
    );


    /*
     * Battleship information.
     */
    fprintf(
        file,
        "BATTLESHIP\n"
    );


    fprintf(
        file,
        "Type: %c\n",
        field->battleship.notation
    );


    fprintf(
        file,
        "Position: (%.2f, %.2f)\n",
        field->battleship.position.x,
        field->battleship.position.y
    );


    fprintf(
        file,
        "Maximum velocity: %.2f\n",
        field->battleship.maxVelocity
    );


    fprintf(
        file,
        "Health: %.2f\n",
        field->battleship.health
    );


    fprintf(
        file,
        "Status: %s\n",
        field->battleship.status == ALIVE
            ? "ALIVE"
            : "SUNK"
    );


    fprintf(
        file,
        "Shots fired: %d\n",
        field->battleship.shotsFired
    );


    fprintf(
        file,
        "Last shot velocity: %.2f\n",
        field->battleship.lastShotVelocity
    );


    fprintf(
        file,
        "Last shot angle: %.2f\n",
        field->battleship.lastShotAngle
    );


    fprintf(
        file,
        "Last flight time: %.2f\n",
        field->battleship.lastFlightTime
    );


    /*
     * If an escort sank B.
     */
    if (sunkByEscort != -1)
    {
        fprintf(
            file,
            "Sunk by Escort: #%d\n",
            sunkByEscort
        );


        fprintf(
            file,
            "Time to hit: %.2f seconds\n",
            timeToHit
        );
    }


    /*
     * Escort information.
     */
    fprintf(
        file,
        "\nESCORT SHIPS\n\n"
    );


    for (i = 0;
         i < field->escortCount;
         i++)
    {
        fprintf(
            file,
            "Escort #%d\n",
            field->escorts[i].id
        );


        fprintf(
            file,
            "Type: %s\n",
            getEscortTypeName(
                field->escorts[i].type
            )
        );


        fprintf(
            file,
            "Position: (%.2f, %.2f)\n",
            field->escorts[i].position.x,
            field->escorts[i].position.y
        );


        fprintf(
            file,
            "Minimum velocity: %.2f\n",
            field->escorts[i].minVelocity
        );


        fprintf(
            file,
            "Maximum velocity: %.2f\n",
            field->escorts[i].maxVelocity
        );


        fprintf(
            file,
            "Minimum angle: %.2f\n",
            field->escorts[i].minAngle
        );


        fprintf(
            file,
            "Maximum angle: %.2f\n",
            field->escorts[i].maxAngle
        );


        fprintf(
            file,
            "Impact power: %.2f\n",
            field->escorts[i].impactPower
        );


        fprintf(
            file,
            "Health: %.2f\n",
            field->escorts[i].health
        );


        fprintf(
            file,
            "Status: %s\n",
            field->escorts[i].status == ALIVE
                ? "ALIVE"
                : "SUNK"
        );


        fprintf(
            file,
            "Shots fired: %d\n",
            field->escorts[i].shotsFired
        );


        fprintf(
            file,
            "Last shot velocity: %.2f\n",
            field->escorts[i].lastShotVelocity
        );


        fprintf(
            file,
            "Last shot angle: %.2f\n",
            field->escorts[i].lastShotAngle
        );


        fprintf(
            file,
            "Last flight time: %.2f\n\n",
            field->escorts[i].lastFlightTime
        );
    }


    fclose(file);
}


/*
 * Save details of escorts destroyed
 * by the battleship.
 */
static void saveHitEscorts(
    const Battlefield *field,
    int hitCount,
    const int hitIds[],
    const double hitTimes[])
{
    FILE *file;
    int i;


    file = fopen(
        "part1A_hit_escorts.txt",
        "w"
    );


    if (file == NULL)
    {
        printf(
            "Error creating "
            "part1A_hit_escorts.txt.\n"
        );

        return;
    }


    fprintf(
        file,
        "PART 1-A ESCORTS HIT BY BATTLESHIP\n\n"
    );


    fprintf(
        file,
        "Number of escorts hit: %d\n\n",
        hitCount
    );


    for (i = 0;
         i < hitCount;
         i++)
    {
        fprintf(
            file,
            "Escort #%d\n",
            hitIds[i]
        );


        fprintf(
            file,
            "Time to hit: %.2f seconds\n\n",
            hitTimes[i]
        );
    }


    (void)field;

    fclose(file);
}


/*
 * Run Part 1-A simulation.
 */
void runPart1A(Battlefield *field)
{
    int i;

    int hitCount = 0;

    int hitIds[MAX_ESCORTS];

    double hitTimes[MAX_ESCORTS];

    int killerEscort = -1;

    double killerTime = 1e100;


    printf(
        "\n----PART 1-A "
        "SIMULATION START----\n"
    );


    /*
     * Save the original setup before
     * any shell is fired.
     */
    saveInitialConditions(field);


    /*
     * ------------------------------------------------
     * STEP 1
     *
     * Every Escort gets exactly one shot.
     * ------------------------------------------------
     */
    for (i = 0;
         i < field->escortCount;
         i++)
    {
        double velocity;
        double angle;
        double flightTime;


        /*
         * Ignore a sunk escort.
         */
        if (field->escorts[i].status != ALIVE)
        {
            continue;
        }


        /*
         * Generate the actual shell velocity
         * for this one shot.
         *
         * It is inside the E type's
         * allowed velocity range.
         */
        velocity =
            randomDouble(
                field->escorts[i].minVelocity,
                field->escorts[i].maxVelocity
            );


        /*
         * Store the actual shell velocity.
         */
        field->escorts[i].lastShotVelocity =
            velocity;


        /*
         * One E can fire only once
         * in Part 1-A.
         */
        field->escorts[i].shotsFired = 1;


        /*
         * Check whether this actual shell
         * can hit the battleship.
         */
        if (
            canHit(
                field->escorts[i].position,

                field->battleship.position,

                velocity,

                field->escorts[i].minAngle,

                field->escorts[i].maxAngle,

                &angle,

                &flightTime
            )
        )
        {
            /*
             * Save firing information.
             */
            field->escorts[i].lastShotAngle =
                angle;

            field->escorts[i].lastFlightTime =
                flightTime;


            /*
             * If several escorts can hit B,
             * the shell arriving first wins.
             */
            if (
                killerEscort == -1 ||
                flightTime < killerTime
            )
            {
                killerEscort = i;

                killerTime = flightTime;
            }
        }
    }


    /*
     * ------------------------------------------------
     * STEP 2
     *
     * If an Escort shell can hit B,
     * one impact destroys B.
     * ------------------------------------------------
     */
    if (killerEscort != -1)
    {
        field->battleship.health = 0.0;

        field->battleship.status = SUNK;


        printf(
            "Escort #%d sank "
            "the Battleship.\n",

            field->escorts[
                killerEscort
            ].id
        );


        printf(
            "Shell velocity: %.2f m/s\n",

            field->escorts[
                killerEscort
            ].lastShotVelocity
        );


        printf(
            "Firing angle: %.2f degrees\n",

            field->escorts[
                killerEscort
            ].lastShotAngle
        );


        printf(
            "Time to hit: %.2f seconds\n",
            killerTime
        );


        printf(
            "----PART 1-A "
            "SIMULATION END----\n"
        );


        saveFinalConditions(
            field,

            field->escorts[
                killerEscort
            ].id,

            killerTime
        );


        return;
    }


    /*
     * ------------------------------------------------
     * STEP 3
     *
     * Battleship survived.
     *
     * B fires and can destroy all E ships
     * that are inside its attack range.
     * ------------------------------------------------
     */
    field->battleship.shotsFired = 0;


    for (i = 0;
         i < field->escortCount;
         i++)
    {
        double velocity;
        double angle;
        double flightTime;


        /*
         * Ignore already sunk escorts.
         */
        if (field->escorts[i].status != ALIVE)
        {
            continue;
        }


        /*
         * Generate actual shell velocity.
         *
         * B minimum velocity = 0.
         */
        velocity =
            randomDouble(
                0.0,
                field->battleship.maxVelocity
            );


        /*
         * Store actual velocity.
         */
        field->battleship.lastShotVelocity =
            velocity;


        /*
         * B has one gun.
         *
         * In Part 1-A there is no reload time,
         * so it can fire at all targets in range.
         */
        if (
            canHit(
                field->battleship.position,

                field->escorts[i].position,

                velocity,

                0.0,

                90.0,

                &angle,

                &flightTime
            )
        )
        {
            /*
             * Store firing details.
             */
            field->battleship.lastShotAngle =
                angle;

            field->battleship.lastFlightTime =
                flightTime;


            /*
             * Destroy the escort.
             */
            field->escorts[i].health = 0.0;

            field->escorts[i].status = SUNK;


            field->battleship.shotsFired++;


            hitIds[hitCount] =
                field->escorts[i].id;


            hitTimes[hitCount] =
                flightTime;


            hitCount++;


            printf(
                "Battleship destroyed "
                "Escort #%d.\n",

                field->escorts[i].id
            );


            printf(
                "  Shell velocity: %.2f m/s\n",
                velocity
            );


            printf(
                "  Firing angle: %.2f degrees\n",
                angle
            );


            printf(
                "  Time to hit: %.2f seconds\n",
                flightTime
            );
        }
    }


    /*
     * ------------------------------------------------
     * STEP 4
     *
     * Display final result.
     * ------------------------------------------------
     */
    printf(
        "Battleship survived.\n"
    );


    printf(
        "Number of Escort ships hit "
        "by Battleship: %d\n",
        hitCount
    );


    /*
     * Calculate battle duration.
     *
     * Since B attacks all targets in
     * Part 1-A without reload delay,
     * the last impact time is the
     * battle duration.
     */
    if (hitCount > 0)
    {
        double battleTime = 0.0;


        for (i = 0;
             i < hitCount;
             i++)
        {
            if (hitTimes[i] > battleTime)
            {
                battleTime =
                    hitTimes[i];
            }
        }


        printf(
            "Battle duration: %.2f seconds\n",
            battleTime
        );


        saveHitEscorts(
            field,
            hitCount,
            hitIds,
            hitTimes
        );
    }
    else
    {
        printf(
            "No Escort ships were hit.\n"
        );
    }


    printf(
        "----PART 1-A "
        "SIMULATION END----\n"
    );


    /*
     * Save final battlefield.
     */
    saveFinalConditions(
        field,
        -1,
        0.0
    );
}
