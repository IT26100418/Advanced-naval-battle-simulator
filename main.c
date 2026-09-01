#include <stdio.h>
#include "setup.h"
int main(void)
{
    Battlefield field;
    //create the initial battlefield
    initializeBattlefield(&field);

    //display the battlefield details
    displayBattlefield(&field);

    return 0;
}
