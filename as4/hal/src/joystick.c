#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

#include "joystick.h"
#include "threadController.h"
#include "sharedMem-Linux.h"
#include "sharedDataStruct.h"
#include "randomGen.h"
#include "accelerometer.h"
#include "PWM.h"

static pthread_t joystick_id;

void joystick_init()
{
    runCommand("config-pin p8.15 pruin"); //right
    runCommand("config-pin p8.16 pruin"); //down

    pthread_create(&joystick_id, NULL, joystickThread, NULL);
}

void joystick_wait()
{
    pthread_join(joystick_id, NULL);
}

void joystick_cleanup(void)
{

}

void *joystickThread(void *args)
{
    (void)args;

    while(running_flag)
    {
        if (isPressed(RIGHT))
        {
            turnOffSound();
            sleepForMs(250);
            printf("Your final grade is %d, Bye!\n", getHitCount());
            stopProgram();
        }
        else if (isPressed(DOWN))
        {
            float *accel = getAccel();
            bool isHit = fireDot(accel[0], accel[1]);
            if (isHit)
            {
                printf("Congrats! you hit! current hit count is %d\n", getHitCount());
                setHitSoundPlay();
                randomGen();
                sleepForMs(300);
            }
            else
            {
                //printf("current point is (%f, %f)\n", accel[0], accel[1]);
                printf("Fail! You miss it...\n");
                setMissSoundPlay();
                sleepForMs(300);
            }
        }

    }
    
    pthread_exit(NULL);
    return NULL;
}




