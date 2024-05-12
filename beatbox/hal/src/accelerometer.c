#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "accelerometer.h"
#include "threadController.h"
#include "periodTimer.h"

static pthread_t accel_id;
static int i2cFileDesc;

int16_t accel[3];
//unsigned char buff[7];

static void runCommand(char* command);
static int initI2cBus(char* bus, int address);
static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value);
//static unsigned char readI2cReg(int i2cFileDesc, unsigned char regAddr);


void accelerometer_init()
{
    // Configure i2c
    runCommand("config-pin P9_18 i2c");
    runCommand("config-pin P9_17 i2c");

    runCommand("echo in > /sys/class/gpio/gpio4/direction");
    runCommand("echo in > /sys/class/gpio/gpio5/direction");

    sleepForMs(330);

    // Initialize I2C bus
    i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);

    // Enable the acceleromete
    writeI2cReg(i2cFileDesc, CTRL_REG1, CTRL_REG1_VALUE);

    pthread_create(&accel_id, NULL, accelThread, NULL);

    

    // int16_t start_addr = OUT_X_L | 0x80;
    // writeI2cReg(i2cFileDesc, start_addr, 1);

    // if (read(i2cFileDesc, buff, sizeof(buff)) != sizeof(buff)) {
    //     perror("Failed to read acceleration data");
    //     close(i2cFileDesc);
    //     exit(1);
    // }
}

void accelerometer_wait()
{
    pthread_join(accel_id, NULL);
}

void accelerometer_cleanup(void)
{
    close(i2cFileDesc);
}

void *accelThread(void *args)
{
    (void)args;
    unsigned char buff[6];
    int16_t start_addr = OUT_X_L | 0x80; // Use the auto-increment feature
    //int16_t start_addr = 0x80;

    while (running_flag)
    {
        Period_markEvent(PERIOD_EVENT_SAMPLE_ACCELEROMETER);

        if (write(i2cFileDesc, &start_addr, 1) != 1) {
            perror("Failed to send start address for reading");
            close(i2cFileDesc);
            exit(1);
        }

        if (read(i2cFileDesc, buff, sizeof(buff)) != sizeof(buff)) {
            perror("Failed to read acceleration data");
            close(i2cFileDesc);
            exit(1);
        }
        
        int16_t x = (buff[1] << 8) | (buff[0]);
        int16_t y = (buff[3] << 8) | (buff[2]);
        int16_t z = (buff[5] << 8) | (buff[4]);

	    //accel[0] = x >> 4;
	    //accel[1] = y >> 4;
	    //accel[2] = z >> 4;

        accel[0] = x / 10000;
	    accel[1] = y / 10000;
	    accel[2] = z / 20000;

        //printf("X:  %d, Y  %d, Z  %d\n", accel[0] ,accel[1] ,accel[2]);
        sleepForMs(10);
    }
    pthread_exit(NULL);
    return NULL;
}

int16_t *getAccel()
{
    //static int i = 0;
    //printf("get accel %d\n",i++);
    return accel;
}

// int16_t *getAccel()
// {
//     Period_markEvent(PERIOD_EVENT_SAMPLE_ACCELEROMETER);

//     unsigned char buff[7];

//     int16_t start_addr = OUT_X_L | 0x80;
//     writeI2cReg(i2cFileDesc, start_addr, 1);

//     if (read(i2cFileDesc, buff, sizeof(buff)) != sizeof(buff)) {
//         perror("Failed to read acceleration data");
//         close(i2cFileDesc);
//         exit(1);
//     }

//     int16_t x = (buff[REG_XMSB] << 8) | (buff[REG_XLSB]);
//     int16_t y = (buff[REG_YMSB] << 8) | (buff[REG_YLSB]);
//     int16_t z = (buff[REG_ZMSB] << 8) | (buff[REG_ZLSB]);

// 	accel[0] = x / 10000;
// 	accel[1] = y / 10000;
// 	accel[2] = z / 10000;

//     return accel;
// }

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

static int initI2cBus(char* bus, int address)
{
    int i2cFileDesc = open(bus, O_RDWR);
    int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
    if (result < 0) {
        perror("I2C: Unable to set I2C device to slave address.");
        exit(1);
    }
    return i2cFileDesc;
}

static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value)
{
    unsigned char buff[2];
    buff[0] = regAddr;
    buff[1] = value;
    int res = write(i2cFileDesc, buff, 2);
    if (res != 2) {
        perror("I2C: Unable to write i2c register.");
        exit(1);
    }
}

// static unsigned char readI2cReg(int i2cFileDesc, unsigned char regAddr)
// {
//     // To read a register, must first write the address
//     int res = write(i2cFileDesc, &regAddr, sizeof(regAddr));
//     if (res != sizeof(regAddr)) {
//         perror("I2C: Unable to write to i2c register.");
//         exit(1);
//     }
//     // Now read the value and return it
//     char value = 0;
//     res = read(i2cFileDesc, &value, sizeof(value));
//     if (res != sizeof(value)) {
//         perror("I2C: Unable to read from i2c register");
//         exit(1);
//     }
//     return value;
// }


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