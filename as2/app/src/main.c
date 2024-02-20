#include <stdio.h>
#include <stdlib.h>
#include "light_sampler.h"
#include "periodTimer.h"


int main ()
{
    printf("Starting to sample data...\n");
    Sampler_init(); 
    Sampler_cleanup();
    return 0;
}


