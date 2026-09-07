#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "setup.h"
#include "part1A.h"
#include "part1B.h"
#include "part1C.h"


int main(void)
{
    Battlefield field;
    Battlefield part1BField;
    Battlefield part1CField;

    srand((unsigned int)time(NULL));

    /*
     * Create initial battlefield.
     */
    initializeBattlefield(&field);

    displayBattlefield(&field);

    /*
     * Keep original battlefield
     * for each part.
     */
    part1BField = field;
    part1CField = field;

    /*
     * Part 1-A
     */
    runPart1A(&field);

    /*
     * Part 1-B
     */
    runPart1B(&part1BField);

    /*
     * Part 1-C
     */
    runPart1C(&part1CField);

    return 0;
}
