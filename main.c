#include <stdio.h>
#include "types.h"
int main(void)
{
    Battlefield field;
    //set simple test values
    field.size = 1000;
    field.escortCount = 10;

    printf("Battlefield created successfully\n");
    printf("Size: %.2f\n", field.size);
    printf("Escort count: %d\n", field.escortCount);

    return 0;
}
