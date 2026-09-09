#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "setup.h"
#include "menu.h"


int main(void)
{
    Battlefield field;
    //start random number generator
    srand((unsigned int)time(NULL));
    //create the initial battlefield
    initializeBattlefield(&field);
    //show initial battlefield
    displayBattlefield(&field);
    //start the main menu
    runMenu(&field);

    return 0;
}
