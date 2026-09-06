#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "setup.h"
#include "part1A.h"
#include "part1B.h"

int main(void)
{
    Battlefield field;
    Battlefield part1BField;

    srand((unsigned int)time(NULL));

    initializeBattlefield(&field);
    displayBattlefield(&field);

    part1BField = field;

    runPart1A(&field);
    runPart1B(&part1BField);

    return 0;
}
