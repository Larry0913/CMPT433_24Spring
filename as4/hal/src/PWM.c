#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

#include "threadController.h"
#include "PWM.h"

static pthread_t PWM_id;

volatile SoundType currentSound = SOUND_NONE;
volatile requestNewSound = false;
static flag = 0;

static int setPWMFrequency(int frequency);

static void playHitSound();
static void playMissSound();

void PWM_init()
{
    //printf("PWM Buzzer starting!\n");
    runCommand("config-pin P9_22 pwm");
    pthread_create(&PWM_id, NULL, PWMBuzzerThread, NULL);
}

void PWM_wait()
{
    pthread_join(PWM_id, NULL);
}

void PWM_cleanup(void)
{
    writeTOFile(PWM_FILE_ENABLE, 0);
}

void *PWMBuzzerThread(void *args)
{
    (void)args;
    while (running_flag) {
        if (requestNewSound) {
            if (currentSound == SOUND_HIT) 
            {
                requestNewSound = false;
                playHitSound();
            }
            else if (currentSound == SOUND_MISS) 
            {
                requestNewSound = false;
                playMissSound();
            }
             
        }
        sleepForMs(10);
    }
    pthread_exit(NULL);
    return NULL;
}

void turnOffSound()
{
    requestNewSound = true;
    currentSound = SOUND_NONE;
    flag = 1;
}

void playSound(int frequency, int durationMs) {
    if (requestNewSound) 
    { 
        if(flag == 0)
        {
            printf("New sound comes in, stop previous sound now!\n");
            return;
        }
        else if(flag == 1) 
        {
            printf("Shut down program and stop current sound!\n");
            flag = 2;
        }
    }
    else 
    {
        //printf("normal, count is %d\n", count);
        setPWMFrequency(frequency); 
        sleepForMs(durationMs);
    }
}

void setHitSoundPlay()
{
    currentSound = SOUND_HIT;
    requestNewSound = true;
}

static void playHitSound()
{
    playSound(C4, 200);
    playSound(E4, 100);
    playSound(G4, 200);
    playSound(C5, 100);
    playSound(G4, 200);
    writeTOFile(PWM_FILE_DUTYCYCLE, 0); 
    writeTOFile(PWM_FILE_ENABLE, 0); 
}

void setMissSoundPlay()
{
    currentSound = SOUND_MISS;
    requestNewSound = true;
}

static void playMissSound()
{
    playSound(F4, 300);
    playSound(B4, 300);
    playSound(A4, 300);
    playSound(G4, 300);
    playSound(F4, 300);
    playSound(E4, 300);
    writeTOFile(PWM_FILE_DUTYCYCLE, 0); 
    writeTOFile(PWM_FILE_ENABLE, 0); 
}

static int setPWMFrequency(int frequency)
{
    //runCommand("cd /dev/bone/pwm/0/b");
    // Calculate the period in ns (1/frequency in seconds, converted to ns)
    // frequency is in Hz, so the period in seconds is 1/frequency
    // Then convert the period to nanoseconds
    
    long period_ns = 10; 
    if(frequency != 0)
    {
        period_ns = 1000000000L / frequency;
    }
    
    //set duty_cycle = 0
    writeTOFile(PWM_FILE_DUTYCYCLE, 0);
    
    // Set the period
    writeTOFile(PWM_FILE_PERIOD, period_ns);
    
    // Set the duty cycle to half of the period for a 50% duty cycle
    long dutyCycle_ns = period_ns / 2;

    // Set the duty cycle
    writeTOFile(PWM_FILE_DUTYCYCLE, dutyCycle_ns);
    //printf("current dutycycle_ns is %ld\n", dutyCycle_ns);

    // Enable the PWM
    writeTOFile(PWM_FILE_ENABLE, 1);

    return 0;
}
