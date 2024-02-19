#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define NUM_LED 4

enum joystick {UP = 26, DOWN = 46, RIGHT = 47, LEFT = 65};
enum ledIdx {USR0, USR1, USR2, USR3};


static void gameInit(void);
static long long getWaitTime(void);
bool isPressed(enum joystick direction);
enum joystick randomGenerateDirection(void);
static long long getTimeInMs(void);
static void sleepForMs(long long delayInMs);
static void runCommand(char* command);
static void ledOn(enum ledIdx usrIdx);//led on
static void ledOff(enum ledIdx usrIdx);//led off
static void ledTriggerNone(int numLeds);//change all leds trigger mode to none
//static void ledBlink(int numLeds, long long Hz, float delayTime);
static void ledBlinkBase(int numLeds, long long Hz, float delayTime);

int main (int argc, char* args[])
{
    gameInit();
    printf ("Hello embedded world, from Lingjie Li!\n\n");
    printf ("When the LEDs light up, press the joystick in that direction!\n");
    printf ("(Press left or right to exit)\n");
    
    bool quitFlag = false;
    bool wrongInput;
    long long startTime, waitTime, processTimeStamp, processTime;
    enum joystick correctPress, wrongPress;
    long long maxTime = 5000, bestTime = 5000;
    //long long temp1, temp2;
    
    while(!quitFlag)
    {
        if (isPressed(UP) || isPressed(DOWN) || isPressed(RIGHT) || isPressed(LEFT))
        {
            printf("Please let go of joystick\n");
            sleepForMs(1000);
        }
        else
        {
            correctPress = randomGenerateDirection();
            wrongInput = false;

            printf("Get ready...\n");
            ledOn(USR1);
            ledOn(USR2);
            startTime = getTimeInMs();
            waitTime = getWaitTime();

            while ((getTimeInMs() - startTime) < waitTime)
            {
                if (isPressed(LEFT) || isPressed(RIGHT))
                {
                    quitFlag = true;
                    break;
                }
                else if (isPressed(UP) || isPressed(DOWN))
                {
                    printf("Too soon!\n");
                    sleepForMs(500);
                    wrongInput = true;
                    break;
                }
            }

            if (wrongInput)
            {
                continue;
            }

            else if (quitFlag)
            {
                break;
            }

            //Turn off the middle two LEDs
            processTimeStamp = getTimeInMs();
            ledOff(USR1);
            ledOff(USR2);
            if(correctPress == UP)
            {
                printf("Press UP now!\n");
                wrongPress = DOWN;
                //If program chose up, then turn on the top LED (#0)
                ledOn(USR0);
            }
            else
            {
                printf("Press DOWN now!\n");
                wrongPress = UP;
                //If program chose down, then turn on the bottom LED (#3)
                ledOn(USR3);
            }

            while ((processTime = (getTimeInMs() - processTimeStamp)) <= maxTime)
            {
                //printf("process time now is %lld\n", processTime);
                if (isPressed(correctPress))
                {
                    ledOff(0);
                    ledOff(3);
                    printf("Correct!\n");
                    if (processTime < bestTime)
                    {
                        printf("New best time!\n");
                        bestTime = processTime;
                    }
                
                    printf("Your reaction time was %lldms; best so far in game is %lldms.\n", processTime, bestTime);
                    //flash all four LEDs on and off at 4hz for 0.5 seconds (two flashes)
                    //temp1 = getTimeInMs();
                    ledBlinkBase(NUM_LED, 4, 0.5);
                    //temp2 = getTimeInMs() - temp1;
                    //printf("correct blink time is %lld\n", temp2);
                    sleepForMs(500);
                    break;
                }
                else if (isPressed(wrongPress))
                {
                    ledOff(0);
                    ledOff(3);
                    printf("Incorrect!\n");
                    //flash all four LEDs on and off at 10hz for 1 second.
                    //temp1 = getTimeInMs();
                    ledBlinkBase(NUM_LED, 10, 1);
                    //temp2 = getTimeInMs() - temp1;
                    //printf("incorrect blink time is %lld\n", temp2);
                    sleepForMs(500);
                    break;
                }
                else if (isPressed(RIGHT) || isPressed(LEFT))
                {
                    quitFlag = true;
                    break;
                }
            }

            if (processTime >= maxTime){
                printf("No input within 5000ms; quitting!\n");
                ledTriggerNone(NUM_LED);
                for (int i = 0; i < NUM_LED; i++)
                {
                    ledOff(i);
                }
                break;
            }
        }
    }

    //quit game
    if (quitFlag)
    {
        printf("User selected to quit.\n");
        ledTriggerNone(NUM_LED);
        for (int i = 0; i < NUM_LED; i++)
        {
            ledOff(i);
        }
    }

    return 0;
}

static void gameInit(void)
{
    //Your C program must execute config-pin to force the joystick’s pin to be treated as GPIO.
    runCommand("config-pin p8.43 gpio");
    runCommand("config-pin p8.14 gpio");
    runCommand("config-pin p8.15 gpio");
    runCommand("config-pin p8.16 gpio");
    runCommand("config-pin p8.17 gpio");
    runCommand("config-pin p8.18 gpio");
    //set Leds trigger as none
    ledTriggerNone(NUM_LED);
    for (int i = 0; i < NUM_LED; i++)
    {
        ledOff(i);
    }
}

static long long getWaitTime(void)
{
    float min = 0.5;
    float max = 3.0;
    srand(time(0));
    float seconds =  min + ((float) rand() / RAND_MAX) * (max - min);
    return (long long) seconds * 1000;
}

bool isPressed(enum joystick direction){
    char filePath[128];
    snprintf(filePath,128,"/sys/class/gpio/gpio%d/value", direction);
    //read file
    FILE *pFile = fopen(filePath, "r");
    if (pFile == NULL) {
        printf("ERROR OPENING %s.\n", filePath);
        exit(-1);
    }
    //get value
    char value[128];
    fgets(value, 128, pFile);
    // close file
    fclose(pFile);
    return !atoi(value);
}

enum joystick randomGenerateDirection(void){
    srand(time(0));
    if (rand()%2)
        return UP;
    return DOWN;
}

static long long getTimeInMs(void)
{
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    long long seconds = spec.tv_sec;
    long long nanoSeconds = spec.tv_nsec;
    long long milliSeconds = seconds * 1000 + nanoSeconds / 1000000;
    return milliSeconds;
}

static void sleepForMs(long long delayInMs)
{
    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SECOND = 1000000000;
    long long delayNs = delayInMs * NS_PER_MS; 
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}

static void runCommand(char* command)
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

static void ledTriggerNone(int numLeds)
{
    for(int i = 0 ; i < numLeds; i++){
        char filePath[128];
        snprintf(filePath, 128, "/sys/class/leds/beaglebone:green:usr%d/trigger", i);
        FILE *pLedTriggerFile = fopen(filePath, "w");
        if (pLedTriggerFile == NULL) {
            printf("ERROR OPENING %s.\n", filePath);
            exit(1);
        }
        int charWritten = fprintf(pLedTriggerFile, "none");
        if (charWritten <= 0) {
            printf("ERROR WRITING DATA");
            exit(1);
        }
        fclose(pLedTriggerFile);
    }
}

static void ledOn(enum ledIdx usrIdx){
    char filePath[128];
    snprintf(filePath,128,"/sys/class/leds/beaglebone:green:usr%d/brightness", usrIdx);
    //read file
    FILE *pFile = fopen(filePath, "w");
    if (pFile == NULL) {
        printf("ERROR OPENING %s.\n", filePath);
        exit(-1);
    }
    // Write to data to the file using fprintf():
    fprintf(pFile, "%d", 1);
    // Close the file using fclose():
    fclose(pFile);

}

static void ledOff(enum ledIdx usrIdx){
    char filePath[128];
    snprintf(filePath,128,"/sys/class/leds/beaglebone:green:usr%d/brightness", usrIdx);
    //read file
    FILE *pFile = fopen(filePath, "w");
    if (pFile == NULL) {
        printf("ERROR OPENING %s.\n", filePath);
        exit(-1);
    }
    // Write to data to the file using fprintf():
    fprintf(pFile, "%d", 0);
    // Close the file using fclose():
    fclose(pFile);

}


static void ledBlinkBase(int numLeds, long long Hz, float delayTime)
{
    long long gapTime = 500 / Hz;
    int times = delayTime * Hz; 
    char command[128];

    for(int j = 0; j < times; j ++){

        for (int j = 0; j < numLeds; j++)//Set bink variable: light up/off for gapTime ms,
        {
            snprintf(command, 128, "/sys/class/leds/beaglebone:green:usr%d/brightness", j);
            FILE *pLedOnFile = fopen(command, "w");
            if (pLedOnFile == NULL) {
                printf("ERROR OPENING %s.\n", command);
                exit(1);
            }
            int charWritten = fprintf(pLedOnFile, "%d", 1);
            if (charWritten <= 0) {
                printf("ERROR WRITING DATA");
                exit(1);
            }
            fclose(pLedOnFile);  
        }

        sleepForMs(gapTime);//Total amount of time for blink LEDs:

        for (int j = 0; j < numLeds; j++)//Set bink variable: light up/off for gapTime ms,
        {
            snprintf(command, 128, "/sys/class/leds/beaglebone:green:usr%d/brightness", j);
            FILE *pLedOffFile = fopen(command, "w");
            if (pLedOffFile == NULL) {
                printf("ERROR OPENING %s.\n", command);
                exit(1);
            }
            int charWritten = fprintf(pLedOffFile, "%d", 0);
            if (charWritten <= 0) {
                printf("ERROR WRITING DATA");
                exit(1);
            }
            fclose(pLedOffFile);  
        }
        sleepForMs(gapTime);
    }
}



// static void ledBlink(int numLeds, long long Hz, float delayTime)
// {
//     long long gapTime = 500 / Hz;
//     //int times = delayTime * Hz; 
//     char command[128];

//     for (int i = 0; i < numLeds; i++)//Set trigger mode
//     {
//         snprintf(command, 128, "cd /sys/class/leds/beaglebone:green:usr%d; echo timer > trigger", i);
//         runCommand(command);
//     }
//     sleepForMs(200);// wait for sub file creation for certian trigger mode


//     for (int j = 0; j < numLeds; j++)//Set bink variable: light up/off for gapTime ms,
//     {
//         snprintf(command, 128, "cd /sys/class/leds/beaglebone:green:usr%d; echo  %lld > delay_on; echo %lld > delay_off", j, gapTime, gapTime);
//         runCommand(command);
        
//     }

//     sleepForMs(delayTime * 1000);//Total amount of time for blink LEDs: 

//     for (int j = 0; j < numLeds; j++)
//     {
//         snprintf(command, 128, "cd /sys/class/leds/beaglebone:green:usr%d; echo  none > trigger", j);
//         runCommand(command);
//     }
// }






