#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "setup.h"
#include "menu.h"


int main(void)
{
    Battlefield field;

    /* Start random number generator */
    srand((unsigned int)time(NULL));

    /* Create the initial battlefield */
    initializeBattlefield(&field);

    /* Show initial battlefield */
    displayBattlefield(&field);

    /* Start the main menu */
    runMenu(&field);

    return 0;
}
