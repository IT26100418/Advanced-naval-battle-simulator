#include <stdio.h>

#include "menu.h"
#include "part1A.h"
#include "part1B.h"
#include "part1C.h"
#include "part2A.h"
#include "part2B.h"
#include "part2C.h"


/*
 * Display the instructions.
 */
static void showInstructions(void)
{
    printf("\n");
    printf("========================================\n");
    printf("          SIMULATOR INSTRUCTIONS\n");
    printf("========================================\n");

    printf("\nHOW TO USE THE SIMULATOR\n");
    printf("------------------------\n");

    printf("1. Select 'Start Simulation' from the main menu.\n");
    printf("2. Select the required simulation from the submenu.\n");
    printf("3. Follow the instructions and enter the required values.\n");
    printf("4. The simulator will run the selected simulation.\n");
    printf("5. Simulation results are displayed on the terminal.\n");
    printf("6. Detailed results are also saved as TXT files.\n");

    printf("\nSIMULATION OPTIONS\n");
    printf("------------------\n");

    printf("1. Part 1-A : Basic stationary battle simulation.\n");
    printf("2. Part 1-B : Moving battleship and gun-jam simulation.\n");
    printf("3. Part 1-C : Impact power and cumulative damage.\n");
    printf("4. Part 2-A : Battleship attack-order strategy.\n");
    printf("5. Part 2-B : Continuous escort firing using TE intervals.\n");
    printf("6. Part 2-C : Gun impact-power degradation using Gamma.\n");
    printf("7. Run all simulations from Part 1-A to Part 2-C.\n");

    printf("\nGENERAL NOTES\n");
    printf("-------------\n");
    printf("- Follow the valid ranges shown by the simulator.\n");
    printf("- Random values are generated automatically where required.\n");
    printf("- User-input and random-generation options are shown where available.\n");
    printf("- Previous simulation results can be viewed using\n");
    printf("  'Simulation Statistics' from the main menu.\n");

    printf("\nPress ENTER to return to the main menu...");

    /*
     * Clear remaining input.
     */
    while (getchar() != '\n')
    {
        /* Clear input buffer */
    }

    getchar();
}


/*
 * Display the Start Simulation submenu.
 */
static void displaySimulationMenu(void)
{
    printf("\n");
    printf("========================================\n");
    printf("           START SIMULATION\n");
    printf("========================================\n");

    printf("1. Run Part 1-A only\n");
    printf("2. Run Part 1-B only\n");
    printf("3. Run Part 1-C only\n");
    printf("4. Run Part 2-A only\n");
    printf("5. Run Part 2-B only\n");
    printf("6. Run Part 2-C only\n");
    printf("7. Run Part 1-A + Part 1-B + Part 1-C +\n");
    printf("   Part 2-A + Part 2-B + Part 2-C\n");
    printf("8. Back to Main Menu\n");

    printf("========================================\n");
}


/*
 * Run the selected simulation.
 */
static void runSimulationMenu(Battlefield *field)
{
    int choice;

    while (1)
    {
        displaySimulationMenu();

        printf("Enter your choice: ");
        scanf("%d", &choice);

        if (choice == 1)
        {
            Battlefield temp = *field;

            runPart1A(&temp);
        }

        else if (choice == 2)
        {
            Battlefield temp = *field;

            runPart1B(&temp);
        }

        else if (choice == 3)
        {
            Battlefield temp = *field;

            runPart1C(&temp);
        }

        else if (choice == 4)
        {
            Battlefield temp = *field;

            runPart2A(&temp);
        }

        else if (choice == 5)
        {
            Battlefield temp = *field;

            runPart2B(&temp);
        }

        else if (choice == 6)
        {
            Battlefield temp = *field;

            runPart2C(&temp);
        }

        else if (choice == 7)
        {
            Battlefield temp = *field;

            runPart1A(&temp);
            runPart1B(&temp);
            runPart1C(&temp);
            runPart2A(&temp);
            runPart2B(&temp);
            runPart2C(&temp);
        }

        else if (choice == 8)
        {
            break;
        }

        else
        {
            printf("\nInvalid choice. Enter 1-8.\n");
        }
    }
}


/*
 * Display the contents of a TXT file.
 */
static void displayTextFile(const char *filename)
{
    FILE *file;
    char line[256];

    file = fopen(filename, "r");

    if (file == NULL)
    {
        printf("\n%s : No result file found.\n", filename);
        return;
    }

    printf("\n");
    printf("========================================\n");
    printf("%s\n", filename);
    printf("========================================\n");

    while (fgets(line, sizeof(line), file) != NULL)
    {
        printf("%s", line);
    }

    fclose(file);

    printf("\n========================================\n");
}


/*
 * Display statistics from previous simulations.
 */
static void showSimulationStatistics(void)
{
    int choice;

    while (1)
    {
        printf("\n");
        printf("========================================\n");
        printf("       SIMULATION STATISTICS\n");
        printf("========================================\n");

        printf("1. Part 1-A Statistics\n");
        printf("2. Part 1-B Statistics\n");
        printf("3. Part 1-C Statistics\n");
        printf("4. Part 2-A Statistics\n");
        printf("5. Part 2-B Statistics\n");
        printf("6. Part 2-C Statistics\n");
        printf("7. Show All Available Statistics\n");
        printf("8. Back to Main Menu\n");

        printf("========================================\n");
        printf("Enter your choice: ");

        scanf("%d", &choice);


        /*
         * Part 1-A
         */
        if (choice == 1)
        {
            displayTextFile("part1A_initial.txt");
            displayTextFile("part1A_final.txt");
        }


        /*
         * Part 1-B
         */
        else if (choice == 2)
        {
            displayTextFile(
                "part1B_SIMULATION1_summary.txt"
            );

            displayTextFile(
                "part1B_SIMULATION2_summary.txt"
            );
        }


        /*
         * Part 1-C
         */
        else if (choice == 3)
        {
            displayTextFile(
                "part1C_SIMULATION1_summary.txt"
            );

            displayTextFile(
                "part1C_SIMULATION2_summary.txt"
            );
        }


        /*
         * Part 2-A
         */
        else if (choice == 4)
        {
            displayTextFile(
                "part2A_PART1A_summary.txt"
            );

            displayTextFile(
                "part2A_PART1B_SIMULATION1_summary.txt"
            );

            displayTextFile(
                "part2A_PART1B_SIMULATION2_summary.txt"
            );

            displayTextFile(
                "part2A_PART1C_SIMULATION1_summary.txt"
            );

            displayTextFile(
                "part2A_PART1C_SIMULATION2_summary.txt"
            );
        }


        /*
         * Part 2-B
         */
        else if (choice == 5)
        {
            displayTextFile(
                "part2B_PART1A_summary.txt"
            );

            displayTextFile(
                "part2B_PART1B_SIMULATION1_summary.txt"
            );

            displayTextFile(
                "part2B_PART1B_SIMULATION2_summary.txt"
            );

            displayTextFile(
                "part2B_PART1C_SIMULATION1_summary.txt"
            );

            displayTextFile(
                "part2B_PART1C_SIMULATION2_summary.txt"
            );
        }


        /*
         * Part 2-C
         */
        else if (choice == 6)
        {
            displayTextFile(
                "part2C_gamma_configuration.txt"
            );

            displayTextFile(
                "part2C_SIMULATION1_summary.txt"
            );

            displayTextFile(
                "part2C_SIMULATION2_summary.txt"
            );
        }


        /*
         * Show all available statistics.
         */
        else if (choice == 7)
        {
            printf("\n");
            printf("========================================\n");
            printf("     ALL AVAILABLE SIMULATION RESULTS\n");
            printf("========================================\n");


            printf("\n--- PART 1-A ---\n");

            displayTextFile(
                "part1A_initial.txt"
            );

            displayTextFile(
                "part1A_final.txt"
            );


            printf("\n--- PART 1-B ---\n");

            displayTextFile(
                "part1B_SIMULATION1_summary.txt"
            );

            displayTextFile(
                "part1B_SIMULATION2_summary.txt"
            );


            printf("\n--- PART 1-C ---\n");

            displayTextFile(
                "part1C_SIMULATION1_summary.txt"
            );

            displayTextFile(
                "part1C_SIMULATION2_summary.txt"
            );


            printf("\n--- PART 2-C ---\n");

            displayTextFile(
                "part2C_gamma_configuration.txt"
            );

            displayTextFile(
                "part2C_SIMULATION1_summary.txt"
            );

            displayTextFile(
                "part2C_SIMULATION2_summary.txt"
            );
        }


        /*
         * Back to main menu.
         */
        else if (choice == 8)
        {
            break;
        }


        else
        {
            printf("\nInvalid choice. Enter 1-8.\n");
        }
    }
}


/*
 * Display the main menu.
 */
void displayMenu(void)
{
    printf("\n");
    printf("========================================\n");
    printf("     ADVANCED NAVAL BATTLE SIMULATOR\n");
    printf("========================================\n");

    printf("1. Start Simulation\n");
    printf("2. View Instructions\n");
    printf("3. Simulation Statistics\n");
    printf("4. Exit\n");

    printf("========================================\n");
}


/*
 * Run the main menu.
 */
void runMenu(Battlefield *field)
{
    int choice;

    while (1)
    {
        displayMenu();

        printf("Enter your choice: ");
        scanf("%d", &choice);


        /*
         * Start Simulation
         */
        if (choice == 1)
        {
            runSimulationMenu(field);
        }


        /*
         * View Instructions
         */
        else if (choice == 2)
        {
            showInstructions();
        }


        /*
         * Simulation Statistics
         */
        else if (choice == 3)
        {
            showSimulationStatistics();
        }


        /*
         * Exit
         */
        else if (choice == 4)
        {
            printf("\nProgram ended.\n");
            break;
        }


        else
        {
            printf("\nInvalid choice. Enter 1-4.\n");
        }
    }
}


