#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "setup.h"
#include "part1A.h"


int main(void)
{
    Battlefield field;


    /*
     * Create a different random sequence
     * every time the program starts.
     */
    srand(
        (unsigned int)time(NULL)
    );


    /*
     * Create the battlefield.
     */
    initializeBattlefield(&field);


    /*
     * Display initial battlefield.
     */
    displayBattlefield(&field);


    /*
     * Run Part 1-A.
     */
    runPart1A(&field);


    return 0;
}
