#include <stdio.h>

#include "menu.h"
#include "part1A.h"
#include "part1B.h"
#include "part1C.h"


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
    printf("4. Run Part 1-A + Part 1-B + Part 1-C\n");
    printf("5. Exit\n");

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
            runPart1A(field);
        }
        else if (choice == 2)
        {
            Battlefield part1BField;

            /* Use a fresh copy for Part 1-B */
            part1BField = *field;

            runPart1B(&part1BField);
        }
        else if (choice == 3)
        {
            Battlefield part1CField;

            /* Use a fresh copy for Part 1-C */
            part1CField = *field;

            runPart1C(&part1CField);
        }
        else if (choice == 4)
        {
            Battlefield part1AField;
            Battlefield part1BField;
            Battlefield part1CField;

            /*
             * Give every part
             * the same initial battlefield.
             */
            part1AField = *field;
            part1BField = *field;
            part1CField = *field;

            /* Run all three parts */
            runPart1A(&part1AField);
            runPart1B(&part1BField);
            runPart1C(&part1CField);
        }
        else if (choice == 5)
        {
            printf("\n");
            printf("Program ended.\n");
            break;
        }
        else
        {
            printf("\n");
            printf("Invalid choice. Please enter 1-5.\n");
        }
    }
}
