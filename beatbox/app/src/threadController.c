#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

#include "threadController.h"
#include "periodTimer.h"
#include "audioGeneration.h"
#include "joystick.h"
#include "accelerometer.h"
#include "UDP.h"
#include "print.h"

int running_flag = 1;

void startProgram()
{
    Period_init();
    Audio_init();
    joystick_init();
    UDP_init();
    accelerometer_init();
    print_init();
    

    Audio_wait();
    joystick_wait();
    UDP_wait();
    accelerometer_wait();
    print_wait();
}

void stopProgram()
{
    running_flag = 0;
    Audio_cleanup();
    UDP_cleanup();
    accelerometer_cleanup();
    Period_cleanup();
}