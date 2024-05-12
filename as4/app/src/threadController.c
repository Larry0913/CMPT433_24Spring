#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "threadController.h"
#include "randomGen.h"
#include "accelerometer.h"
#include "sharedMem-Linux.h"
#include "sharedDataStruct.h"
#include "I2C.h"
#include "PWM.h"


int running_flag = 1;

volatile void *pPruBase;
volatile sharedMemStruct_t *pSharedPru0;

void startProgram()
{   
    // Get access to shared memory for my uses
    pPruBase = getPruMmapAddr();
    pSharedPru0 = PRU0_MEM_FROM_BASE(pPruBase);

    neoPixel_init(pSharedPru0);
    randomGen();
    accelerometer_init();
    I2C_init();
    joystick_init();
    PWM_init();

    
    PWM_wait();
    accelerometer_wait();
    I2C_wait();
    joystick_wait();
    
    freePruMmapAddr(pPruBase);
}

void stopProgram()
{
    printf("Cleaning up...54321\nDone!\n");
    running_flag = 0;

    // Cleanup
    accelerometer_cleanup();
    I2C_cleanup();
    PWM_cleanup();

}

void runCommand(char* command)
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
    if (exitCode != 0) {
        perror("Unable to execute command:");
        printf(" command: %s\n", command);
        printf(" exit code: %d\n", exitCode);
    }
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

void sleepForMs(long long delayInMs)
{
    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SECOND = 1000000000;
    long long delayNs = delayInMs * NS_PER_MS; 
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}