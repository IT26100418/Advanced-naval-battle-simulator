#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "setup.h"
#include "common.h"

//check battlefield size
static int isValidBattlefieldSize(double size)
{
    return size > 0.0;
}

//check escort count
static int isValidEscortCount(int count)
{
    return count > 0 &&
           count <= MAX_ESCORTS;
}

//check whether two points are the same.
static int isSamePoint(Point a, Point b)
{
    return fabs(a.x - b.x) < 0.000001 &&
           fabs(a.y - b.y) < 0.000001;
}

//create the battlefield
void initializeBattlefield(Battlefield *field)
{
    if (field == NULL)
    {
        return;
    }

    do
    {
        printf("Enter battlefield size: ");
        scanf("%lf", &field->size);

        if (!isValidBattlefieldSize(field->size))
        {
            printf(
                "Invalid battlefield size. "
                "Enter a value greater than 0.\n"
            );
        }

    } while (!isValidBattlefieldSize(field->size));


    do
    {
        printf(
            "Enter number of escort ships (1-%d): ",
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


    //setup battleship
    setupBattleship(
        &field->battleship,
        field->size
    );


    //setup escort ships
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

//set battleship details

void setupBattleship(
    Battleship *battleship,
    double battlefieldSize)
{
    char type;


    printf(
        "Enter battleship type (U/M/R/S): "
    );

    scanf(
        " %c",
        &type
    );


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
                "Invalid type. U is used.\n"
            );

            break;
    }


    //battleship X position
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


    //battleship Y position
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


    // Maximum B shell velocity
    battleship->maxVelocity = 100.0;

    //Initial health 
    battleship->health = 1.0;
    //gamma
    battleship->gamma = 0.01;
    battleship->shotsFired = 0;
    battleship->status = ALIVE;
    battleship->lastShotVelocity = 0.0;
    battleship->lastShotAngle = 0.0;
    battleship->lastFlightTime = 0.0;
}

//create escort ships
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
        //unique ID 
        escorts[i].id = i + 1;
        //random escort type
        escorts[i].type =
            (EscortType)(rand() % 5);


        //generate unique position
   
        do
        {
            escorts[i].position =
                generateRandomPosition(
                    battlefieldSize
                );

            j = 0;

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


        //set type details
        setupEscortDetails(
            &escorts[i]
        );
    }
}


//set escort properties
void setupEscortDetails(
    EscortShip *escort)
{
    double upperVelocity;
    escort->health = 1.0;
    escort->gamma = 0.02;
    escort->shotsFired = 0;
    escort->status = ALIVE;
    escort->lastShotVelocity = 0.0;
    escort->lastShotAngle = 0.0;
    escort->lastFlightTime = 0.0;


   //Part 2-B different firing interal
    switch (escort->type)
{
    case EA:

        escort->maxAngle = 20.0;
        escort->impactPower = 0.08;
	upperVelocity = 120.0;
        escort->firingInterval = 8.0;
        break;


    case EB:

        escort->maxAngle = 30.0;
        escort->impactPower = 0.06;
	upperVelocity = 90.0;
        escort->firingInterval = 10.0;
        break;


    case EC:

        escort->maxAngle = 25.0;
        escort->impactPower = 0.07;
        upperVelocity = 90.0;
        escort->firingInterval = 12.0;
        break;


    case ED:

        escort->maxAngle = 50.0;
        escort->impactPower = 0.05;
        upperVelocity = 90.0;
        escort->firingInterval = 15.0;
       break;


    case EE:

        escort->maxAngle = 70.0;
        escort->impactPower = 0.04;
        upperVelocity = 90.0;
        escort->firingInterval = 18.0;
       break;


    default:

        escort->maxAngle = 20.0;
        escort->impactPower = 0.05;
       	upperVelocity = 90.0;
        escort->firingInterval = 10.0;
       break;
}

    //random minimum angle
    
    escort->minAngle =
        randomDouble(
            0.0,
            escort->maxAngle
        );


    //random minimum velocity
    
    escort->minVelocity =
        randomDouble(
            10.0,
            30.0
        );

    //random maximum velocity
    escort->maxVelocity =
        randomDouble(
            escort->minVelocity + 10.0,
            upperVelocity
        );
}

//display battlefield details
void displayBattlefield(
    const Battlefield *field)
{
    int i;


    printf("\nBATTLEFIELD\n");


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


        printf(
            "    Impact: %.2f | TE: %.2f sec\n",
            field->escorts[i].impactPower,
            field->escorts[i].firingInterval
        );
    }
}
