#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "threadControl.h"
#include "PWM.h"
#include "light_sampler.h"

static pthread_t PWM_id;
static int currentFrequency;
pthread_mutex_t POTMutex = PTHREAD_MUTEX_INITIALIZER;

void PWM_init()
{
    //printf("PWM an LED starting!\n");
    runCommand("config-pin P9_21 pwm");
    pthread_create(&PWM_id, NULL, PWMLEDThread, NULL);
}

void PWM_wait()
{
    pthread_join(PWM_id, NULL);
}

void PWM_cleanup(void)
{
    writeTOFile(PWM_FILE_ENABLE, 0);
}

void *PWMLEDThread(void *args)
{
    (void)args;
    while (running_flag) 
    {
        int potValue = getPotVoltageReading(); // Read potentiometer value (0 to 4095)
        int desiredFrequency = potValue / 40; // Calculate desired PWM frequency
        
        currentFrequency = -1; // Track the current PWM frequency
        if (desiredFrequency != currentFrequency) {
            pthread_mutex_lock(&POTMutex);
            {
                //setPWMFrequency(desiredFrequency);
                if(setPWMFrequency(desiredFrequency) != 0)
                {
                    printf("set LED frequency failed\n");
                    exit(-1);
                }
                currentFrequency = desiredFrequency;
            }
            pthread_mutex_unlock(&POTMutex);
        }
        
        // Assuming a 100ms delay for ten times per second reading
        sleepForMs(100); // Delay in milliseconds
    }
    
    pthread_exit(NULL);
    return NULL;
}

int getPotHz()
{
    pthread_mutex_lock(&POTMutex);
    int portFreq = currentFrequency;
    pthread_mutex_unlock(&POTMutex);
    return portFreq;
}

int setPWMFrequency(int frequency)
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

int getPotVoltageReading()
{
    // Open file
    FILE *f = fopen(A2D_FILE_VOLTAGE0, "r");
    if (!f) {
        printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
        printf(" Check /boot/uEnv.txt for correct options.\n");
        exit(-1);
    }
    // Get reading
    int a2dReading = 0;
    int itemsRead = fscanf(f, "%d", &a2dReading);
    if (itemsRead <= 0) {
        printf("ERROR: Unable to read values from voltage input file.\n");
        exit(-1);
    }
    // Close file
    fclose(f);
    return a2dReading;
}

int runCommand(char* command)
{
    // Execute the shell command (output into pipe)
    FILE *pipe = popen(command, "r");
    // Ignore output of the command; but consume it
    // so we don't get an error when closing the pipe.
    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe)) {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL)
            break;
    // printf("--> %s", buffer); // Uncomment for debugging
    }
    // Get the exit code from the pipe; non-zero is an error:
    int exitCode = WEXITSTATUS(pclose(pipe));
    // if (exitCode != 0) {
    //     perror("Unable to execute command:");
    //     printf(" command: %s\n", command);
    //     printf(" exit code: %d\n", exitCode);
    // }
    return exitCode;
}

void writeTOFile(char *path, long value)
{
    //read file
    FILE *pFile = fopen(path, "w");
    if (pFile == NULL) {
        printf("ERROR OPENING %s.\n", path);
        exit(-1);
    }
    // Write to data to the file using fprintf():
    fprintf(pFile, "%ld", value);
    // Close the file using fclose():
    fclose(pFile);
}