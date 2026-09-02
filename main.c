#include <stdio.h>
#include "setup.h"
#include "part1A.h"

int main(void)
{
    Battlefield field;

    initializeBattlefield(&field);
    displayBattlefield(&field);

    runPart1A(&field);

    return 0;
}
