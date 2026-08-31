#include <stdio.h>
#include <stdlib.h>
#include "setup.h"

//create the battlefield
void initializeBattlefield(Battlefield *field)
{
    printf("Enter battlefield size: ");
    scanf("%lf", &field->size);
    printf("Enter number of escort ships: ");
    scanf("%d", &field->escortCount);

    setupBattleship(&field->battleship);
    setupEscortShips(field->escorts, field->escortCount);

    printf("\nBattlefield setup completed.\n");
}
//battleship details
void setupBattleship(Battleship *battleship)
{
    char type;
    printf("Enter battleship type (U/M/R/S): ");
    scanf(" %c", &type);

    battleship->notation = type;

    //convert the letter into the enum type
    switch(type)
    {
        case 'U':
        case 'u':
            battleship->type=U;
            break;

        case 'M':
        case 'm':
            battleship->type=M;
            break;

        case 'R':
        case 'r':
            battleship->type=R;
            break;

        case 'S':
        case 's':
            battleship->type=S;
            break;

        default:
            battleship->type=U;
            battleship->notation = 'U';
            printf("Invalid type. U is used.\n");
    }

    //starting position of the battleship
    battleship->position.x=400.0;
    battleship->position.y=450.0;

    battleship->maxVelocity=100.0;
    battleship->health=1.0;
    battleship->gamma=0.01;
    battleship->shotsFired=0;
    battleship->status=ALIVE;
}


//create all escort ships
void setupEscortShips(EscortShip escorts[], int count)
{
    int i;
    for(i = 0; i < count; i++)
    {
        escorts[i].id=i + 1;
        //select a random escort type
        escorts[i].type = rand()%5;

        //select a random position
        generateRandomPosition(&escorts[i].position, 1000.0);
        
	setupEscortDetails(&escorts[i]);
    }
}
//generate a random position
void generateRandomPosition(Point *position, double size)
{
    position->x=((double)rand() / RAND_MAX) * size;
    position->y=((double)rand() / RAND_MAX) * size;
}

//set properties of an escort
void setupEscortDetails(EscortShip *escort)
{
    escort->health =1.0;
    escort->gamma= 0.02;
    escort->shotsFired =0;
    escort->status= ALIVE;

    switch(escort->type)
    {
        case EA:
            escort->minVelocity= 10.0;
            escort->maxVelocity =100.0;
            escort->minAngle =0.0;
            escort->maxAngle= 20.0;
            escort->impactPower =0.08;
            break;

        case EB:
            escort->minVelocity = 10.0;
            escort->maxVelocity = 80.0;
            escort->minAngle = 0.0;
            escort->maxAngle = 30.0;
            escort->impactPower = 0.06;
            break;

        case EC:
            escort->minVelocity = 10.0;
            escort->maxVelocity = 80.0;
            escort->minAngle = 0.0;
            escort->maxAngle = 25.0;
            escort->impactPower = 0.07;
            break;

        case ED:
            escort->minVelocity = 10.0;
            escort->maxVelocity = 80.0;
            escort->minAngle = 0.0;
            escort->maxAngle = 50.0;
            escort->impactPower = 0.05;
            break;

        case EE:
            escort->minVelocity = 10.0;
            escort->maxVelocity = 80.0;
            escort->minAngle = 0.0;
            escort->maxAngle = 70.0;
            escort->impactPower = 0.04;
            break;
    }
}
//display the battlefield details
void displayBattlefield(const Battlefield *field)
{
    int i;
    printf("\nBATTLEFIELD\n");
    printf("Size: %.2f\n", field->size);
    printf("Battleship: %c\n", field->battleship.notation);
    printf("Battleship position: (%.2f, %.2f)\n",
           field->battleship.position.x,
           field->battleship.position.y);
    printf("Escort ships: %d\n", field->escortCount);

    for(i = 0; i < field->escortCount; i++)
    {
        printf("E#%d: (%.2f, %.2f)\n",
               field->escorts[i].id,
               field->escorts[i].position.x,
               field->escorts[i].position.y);
    }
}
