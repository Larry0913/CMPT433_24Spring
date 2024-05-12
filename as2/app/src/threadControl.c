#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#include "periodTimer.h"
#include "threadControl.h"
#include "light_sampler.h"
#include "UDP.h"
#include "samplerDips.h"
#include "PWM.h"
#include "I2C.h"

int running_flag = 1;

void startProgram()
{
    printf("Starting to sample data...\n");
    Period_init();
    Sampler_init();
    UDP_init();
    Dip_init();
    PWM_init();
    I2C_init();

    Sampler_wait();
    UDP_wait();
    Dip_wait();
    PWM_wait();
    I2C_wait();
}

void stopProgram()
{
    running_flag = 0;
    printf("Cleaning up...54321\nDone!\n");
    //sleepForMs(1000);
    Dip_cleanup();
    Sampler_cleanup();
    UDP_cleanup();
    PWM_cleanup();
    I2C_cleanup();
    Period_cleanup();
}

