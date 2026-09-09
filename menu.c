#include <stdio.h>

#include "menu.h"
#include "part1A.h"
#include "part1B.h"
#include "part1C.h"
#include "part2A.h"
#include "part2B.h"
#include "part2C.h"


/* Display main menu */
void displayMenu(void)
{
    printf("\n");

    printf(
        "========================================\n"
    );

    printf(
        "   ADVANCED NAVAL BATTLE SIMULATOR\n"
    );

    printf(
        "========================================\n"
    );

    printf(
        "1. Run Part 1-A only\n"
    );

    printf(
        "2. Run Part 1-B only\n"
    );

    printf(
        "3. Run Part 1-C only\n"
    );

    printf(
        "4. Run Part 2-A only\n"
    );

    printf(
        "5. Run Part 2-B only\n"
    );

    printf(
        "6. Run Part 2-C only\n"
    );

    printf(
        "7. Run Part 1-A + Part 1-B + "
        "Part 1-C + Part 2-A + "
        "Part 2-B + Part 2-C\n"
    );

    printf(
        "8. Exit\n"
    );

    printf(
        "========================================\n"
    );
}


/* Run selected option */
void runMenu(Battlefield *field)
{
    int choice;


    while (1)
    {
        displayMenu();


        printf(
            "Enter your choice: "
        );

        scanf(
            "%d",
            &choice
        );


        if (choice == 1)
        {
            Battlefield temp = *field;

            runPart1A(
                &temp
            );
        }


        else if (choice == 2)
        {
            Battlefield temp = *field;

            runPart1B(
                &temp
            );
        }


        else if (choice == 3)
        {
            Battlefield temp = *field;

            runPart1C(
                &temp
            );
        }


        else if (choice == 4)
        {
            Battlefield temp = *field;

            runPart2A(
                &temp
            );
        }


        else if (choice == 5)
        {
            Battlefield temp = *field;

            runPart2B(
                &temp
            );
        }


        else if (choice == 6)
        {
            Battlefield temp = *field;

            runPart2C(
                &temp
            );
        }


        else if (choice == 7)
        {
            Battlefield temp = *field;


            runPart1A(
                &temp
            );

            runPart1B(
                &temp
            );

            runPart1C(
                &temp
            );

            runPart2A(
                &temp
            );

            runPart2B(
                &temp
            );

            runPart2C(
                &temp
            );
        }


        else if (choice == 8)
        {
            printf(
                "\nProgram ended.\n"
            );

            break;
        }


        else
        {
            printf(
                "\nInvalid choice. "
                "Enter 1-8.\n"
            );
        }
    }
}
