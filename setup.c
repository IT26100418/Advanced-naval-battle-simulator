#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "setup.h"
#include "common.h"


/*
 * Check whether battlefield size is valid.
 */
static int isValidBattlefieldSize(double size)
{
    return size > 0.0;
}


/*
 * Check whether escort count is valid.
 */
static int isValidEscortCount(int count)
{
    return count > 0 &&
           count <= MAX_ESCORTS;
}


/*
 * Check whether two points are the same.
 */
static int isSamePoint(Point a, Point b)
{
    return fabs(a.x - b.x) < 0.000001 &&
           fabs(a.y - b.y) < 0.000001;
}


/*
 * Create the battlefield.
 */
void initializeBattlefield(Battlefield *field)
{
    if (field == NULL)
    {
        return;
    }


    /*
     * Get battlefield size.
     */
    do
    {
        printf("Enter battlefield size: ");

        scanf(
            "%lf",
            &field->size
        );

        if (!isValidBattlefieldSize(
                field->size))
        {
            printf(
                "Invalid battlefield size. "
                "Enter a value greater than 0.\n"
            );
        }

    } while (!isValidBattlefieldSize(
                 field->size));


    /*
     * Get number of escort ships.
     */
    do
    {
        printf(
            "Enter number of escort ships "
            "(1-%d): ",
            MAX_ESCORTS
        );

        scanf(
            "%d",
            &field->escortCount
        );

        if (!isValidEscortCount(
                field->escortCount))
        {
            printf(
                "Invalid number of escort ships.\n"
            );
        }

    } while (!isValidEscortCount(
                 field->escortCount));


    /*
     * Setup battleship.
     */
    setupBattleship(
        &field->battleship,
        field->size
    );


    /*
     * Setup escort ships.
     */
    setupEscortShips(
        field->escorts,
        field->escortCount,
        field->size,
        field->battleship.position
    );


    printf(
        "\nBattlefield setup completed.\n"
    );
}


/*
 * Set battleship details.
 */
void setupBattleship(
    Battleship *battleship,
    double battlefieldSize)
{
    char type;


    /*
     * Get battleship type.
     */
    printf(
        "Enter battleship type (U/M/R/S): "
    );

    scanf(
        " %c",
        &type
    );


    /*
     * Convert character to enum.
     */
    switch (type)
    {
        case 'U':
        case 'u':

            battleship->type = U;
            battleship->notation = 'U';

            break;


        case 'M':
        case 'm':

            battleship->type = M;
            battleship->notation = 'M';

            break;


        case 'R':
        case 'r':

            battleship->type = R;
            battleship->notation = 'R';

            break;


        case 'S':
        case 's':

            battleship->type = S;
            battleship->notation = 'S';

            break;


        default:

            battleship->type = U;
            battleship->notation = 'U';

            printf(
                "Invalid type. "
                "U is used.\n"
            );

            break;
    }


    /*
     * Get battleship X position.
     */
    do
    {
        printf(
            "Enter battleship X position "
            "(0 - %.2f): ",
            battlefieldSize
        );

        scanf(
            "%lf",
            &battleship->position.x
        );

        if (battleship->position.x < 0.0 ||
            battleship->position.x >
            battlefieldSize)
        {
            printf(
                "Invalid X position.\n"
            );
        }

    } while (
        battleship->position.x < 0.0 ||
        battleship->position.x >
        battlefieldSize
    );


    /*
     * Get battleship Y position.
     */
    do
    {
        printf(
            "Enter battleship Y position "
            "(0 - %.2f): ",
            battlefieldSize
        );

        scanf(
            "%lf",
            &battleship->position.y
        );

        if (battleship->position.y < 0.0 ||
            battleship->position.y >
            battlefieldSize)
        {
            printf(
                "Invalid Y position.\n"
            );
        }

    } while (
        battleship->position.y < 0.0 ||
        battleship->position.y >
        battlefieldSize
    );


    /*
     * Maximum shell velocity of B.
     *
     * This is a student-defined maximum,
     * allowed by the assignment.
     */
    battleship->maxVelocity =
        100.0;


    /*
     * Part 1-A starts with full health.
     */
    battleship->health = 1.0;


    /*
     * Gamma is used later in Part 2-C.
     */
    battleship->gamma = 0.01;


    battleship->shotsFired = 0;

    battleship->status = ALIVE;


    /*
     * No shell has been fired yet.
     */
    battleship->lastShotVelocity = 0.0;
    battleship->lastShotAngle = 0.0;
    battleship->lastFlightTime = 0.0;
}


/*
 * Create escort ships.
 */
void setupEscortShips(
    EscortShip escorts[],
    int count,
    double battlefieldSize,
    Point battleshipPosition)
{
    int i;
    int j;


    for (i = 0; i < count; i++)
    {
        /*
         * Unique ID.
         */
        escorts[i].id = i + 1;


        /*
         * Random escort type.
         */
        escorts[i].type =
            (EscortType)(rand() % 5);


        /*
         * Generate a unique random position.
         */
        do
        {
            escorts[i].position =
                generateRandomPosition(
                    battlefieldSize
                );


            j = 0;


            /*
             * Compare with previous escorts.
             */
            while (
                j < i &&
                !isSamePoint(
                    escorts[i].position,
                    escorts[j].position
                )
            )
            {
                j++;
            }

        } while (
            isSamePoint(
                escorts[i].position,
                battleshipPosition
            )
            ||
            j < i
        );


        /*
         * Set properties according
         * to escort type.
         */
        setupEscortDetails(
            &escorts[i]
        );
    }
}


/*
 * Set escort properties.
 */
void setupEscortDetails(
    EscortShip *escort)
{
    double upperVelocity;


    /*
     * Common values.
     */
    escort->health = 1.0;

    escort->gamma = 0.02;

    escort->shotsFired = 0;

    escort->status = ALIVE;


    escort->lastShotVelocity = 0.0;
    escort->lastShotAngle = 0.0;
    escort->lastFlightTime = 0.0;


    /*
     * Each escort type has its own
     * maximum angle and impact power.
     *
     * Minimum angle and velocities are
     * randomly generated.
     */
    switch (escort->type)
    {
        case EA:

            escort->maxAngle = 20.0;

            escort->impactPower = 0.08;

            /*
             * EA can have maximum velocity
             * up to 1.2 * B maximum velocity.
             */
            upperVelocity = 120.0;

            break;


        case EB:

            escort->maxAngle = 30.0;

            escort->impactPower = 0.06;

            /*
             * Less than EA maximum.
             */
            upperVelocity = 90.0;

            break;


        case EC:

            escort->maxAngle = 25.0;

            escort->impactPower = 0.07;

            upperVelocity = 90.0;

            break;


        case ED:

            escort->maxAngle = 50.0;

            escort->impactPower = 0.05;

            upperVelocity = 90.0;

            break;


        case EE:

            escort->maxAngle = 70.0;

            escort->impactPower = 0.04;

            upperVelocity = 90.0;

            break;


        default:

            escort->maxAngle = 20.0;

            escort->impactPower = 0.05;

            upperVelocity = 90.0;

            break;
    }


    /*
     * Generate a random minimum angle.
     *
     * It is always between 0 and the
     * maximum angle.
     */
    escort->minAngle =
        randomDouble(
            0.0,
            escort->maxAngle
        );


    /*
     * Generate random minimum velocity.
     *
     * Minimum is kept below maximum.
     */
    escort->minVelocity =
        randomDouble(
            10.0,
            30.0
        );


    /*
     * Generate maximum velocity above
     * minimum velocity.
     */
    escort->maxVelocity =
        randomDouble(
            escort->minVelocity + 10.0,
            upperVelocity
        );
}


/*
 * Display battlefield details.
 */
void displayBattlefield(
    const Battlefield *field)
{
    int i;


    printf(
        "\nBATTLEFIELD\n"
    );


    printf(
        "Size: %.2f\n",
        field->size
    );


    printf(
        "Battleship: %c\n",
        field->battleship.notation
    );


    printf(
        "Battleship position: "
        "(%.2f, %.2f)\n",
        field->battleship.position.x,
        field->battleship.position.y
    );


    printf(
        "Battleship maximum velocity: "
        "%.2f\n",
        field->battleship.maxVelocity
    );


    printf(
        "Escort ships: %d\n",
        field->escortCount
    );


    for (i = 0;
         i < field->escortCount;
         i++)
    {
        printf(
            "E#%d: "
            "(%.2f, %.2f) - %s\n",

            field->escorts[i].id,

            field->escorts[i].position.x,

            field->escorts[i].position.y,

            getEscortTypeName(
                field->escorts[i].type
            )
        );


        printf(
            "    Velocity: %.2f - %.2f\n",

            field->escorts[i].minVelocity,

            field->escorts[i].maxVelocity
        );


        printf(
            "    Angle: %.2f - %.2f\n",

            field->escorts[i].minAngle,

            field->escorts[i].maxAngle
        );
    }
}
