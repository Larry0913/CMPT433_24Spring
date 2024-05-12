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
#include "sharedDataStruct.h"
#include "randomGen.h"
#include "sharedMem-Linux.h"

static pthread_t accel_id;
static int i2cFileDesc;

float accel[2];
//unsigned char buff[7];

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
        //int16_t z = (buff[5] << 8) | (buff[4]);

	    //accel[0] = x >> 4;
	    //accel[1] = y >> 4;
	    //accel[2] = z >> 4;

        accel[0] = (float) x / 16000;
	    accel[1] = (float) y / 16000;
	    //accel[2] = z / 20000;

        int color = xPixelShow(accel[0]);
        yPixelShow(accel[1], color);

        //printf("X:  %.1f, Y  %.1f\n", accel[0] ,accel[1]);
        sleepForMs(50);
    }
    for (int i = 0; i < STR_LEN; i++)
    {
        setPixelColor(i, 0, true); // turn off all pixels.
    }
    pthread_exit(NULL);
    return NULL;
}

float *getAccel()
{
    //static int i = 0;
    //printf("get accel %d\n",i++);
    return accel;
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




