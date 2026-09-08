#include <stdio.h>

#include "menu.h"
#include "part1A.h"
#include "part1B.h"
#include "part1C.h"
#include "part2A.h"


/* Display the main menu */
void displayMenu(void)
{
    printf("\n");
    printf("========================================\n");
    printf("   ADVANCED NAVAL BATTLE SIMULATOR\n");
    printf("========================================\n");

    printf("1. Run Part 1-A only\n");
    printf("2. Run Part 1-B only\n");
    printf("3. Run Part 1-C only\n");
    printf("4. Run Part 2-A only\n");
    printf("5. Run Part 1-A + Part 1-B + Part 1-C + Part 2-A\n");
    printf("6. Exit\n");

    printf("========================================\n");
}


/* Run the selected menu option */
void runMenu(Battlefield *field)
{
    int choice;

    while (1)
    {
        displayMenu();

        printf("Enter your choice: ");
        scanf("%d", &choice);


        if (choice == 1)
        {
            /* Run Part 1-A only */
            Battlefield part1AField;

            part1AField = *field;

            runPart1A(&part1AField);
        }


        else if (choice == 2)
        {
            /* Run Part 1-B only */
            Battlefield part1BField;

            part1BField = *field;

            runPart1B(&part1BField);
        }


        else if (choice == 3)
        {
            /* Run Part 1-C only */
            Battlefield part1CField;

            part1CField = *field;

            runPart1C(&part1CField);
        }


        else if (choice == 4)
        {
            /* Run Part 2-A only */
            Battlefield part2AField;

            part2AField = *field;

            runPart2A(&part2AField);
        }


        else if (choice == 5)
        {
            Battlefield part1AField;
            Battlefield part1BField;
            Battlefield part1CField;
            Battlefield part2AField;


            /*
             * Give every part
             * the same initial battlefield.
             */
            part1AField = *field;
            part1BField = *field;
            part1CField = *field;
            part2AField = *field;


            /* Run all parts */
            runPart1A(&part1AField);

            runPart1B(&part1BField);

            runPart1C(&part1CField);

            runPart2A(&part2AField);
        }


        else if (choice == 6)
        {
            printf("\n");
            printf("Program ended.\n");

            break;
        }


        else
        {
            printf("\n");
            printf(
                "Invalid choice. "
                "Please enter 1-6.\n");
        }
    }
}
