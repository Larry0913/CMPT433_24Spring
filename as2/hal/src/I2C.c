#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "threadControl.h"
#include "I2C.h"
#include "PWM.h"
#include "light_sampler.h"
#include "samplerDips.h"

static pthread_t I2C_id;

//0x00
int bottomDigits[10] = {0xd0, 0xc0, 0x98, 0xd8, 0xc8, 0x58, 0x58, 0x02, 0xd8, 0xd8};
//0x01
int topDigits[10] = {0xa1, 0x00, 0x83, 0x03, 0x22, 0x23, 0xa3, 0x05, 0xa3, 0x23};

static int initI2cBus(char* bus, int address);
static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value);
//static unsigned char readI2cReg(int i2cFileDesc, unsigned char regAddr);
static void exportLeftDisplayPin61(void);
static void exportRightDisplayPin44(void);

void I2C_init()
{
    //printf("14-Seg Diaplay starting!\n");
    int output61 = -1;
	int output44 = -1;

    runCommand("config-pin P9_18 i2c");
    runCommand("config-pin P9_17 i2c");
    
	while (output61 != 0 && output44 != 0)
	{
		output61 = runCommand("echo out > /sys/class/gpio/gpio61/direction");
		if (output61 != 0)
        {
            exportLeftDisplayPin61();
        }
			
		output44 = runCommand("echo out > /sys/class/gpio/gpio44/direction");
		if (output44 != 0)
        {
            exportRightDisplayPin44();
        }		
	}
    
    pthread_create(&I2C_id, NULL, I2CDisplayThread, NULL);
}

void I2C_wait()
{
    pthread_join(I2C_id, NULL);
}

void I2C_cleanup(void)
{
    writeTOFile(I2C_LEFT_DIGIT_PATH, OFF);
    writeTOFile(I2C_RIGHT_DIGIT_PATH, OFF);
}

void *I2CDisplayThread(void *args)
{
    (void)args;
    int dipCount;
    int leftDigit;
    int rightDigit;
    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    
    writeI2cReg(i2cFileDesc, REG_DIRA, 0x00);
    writeI2cReg(i2cFileDesc, REG_DIRB, 0x00);
    

    // for (unsigned char i = 0x00; i <= 0xFF; i++) {
        
    //     writeI2cReg(i2cFileDesc, REG_OUTB, i);
    //     writeI2cReg(i2cFileDesc, REG_OUTA, 0x00);
    //     writeTOFile(I2C_LEFT_DIGIT_PATH, ON);
    //     writeTOFile(I2C_RIGHT_DIGIT_PATH, ON);
    //     printf("current i is %d\n", i);
    //     //sleepForMs(1000);
    //     while (getchar() != '\n');
    //     // At 0xFF, adding 1 will cause the loop variable to wrap back to 0x00, breaking the loop
    //     if (i == 0xFF) {
    //         break;
    //     }
    // }

    while(running_flag)
    {
        dipCount = getDipCount();
        if(dipCount > 99)
        {
            dipCount = 99;
        }

        leftDigit = dipCount / 10;
        rightDigit = dipCount % 10;

        // Set both pins off
        writeTOFile(I2C_LEFT_DIGIT_PATH, OFF);
        writeTOFile(I2C_RIGHT_DIGIT_PATH, OFF);

        // Display left digit
		writeI2cReg(i2cFileDesc, REG_OUTA, bottomDigits[leftDigit]);
		writeI2cReg(i2cFileDesc, REG_OUTB, topDigits[leftDigit]);

        // Turn on left digit
		writeTOFile(I2C_LEFT_DIGIT_PATH, ON);

        sleepForMs(5);

        // Set both pins off
		writeTOFile(I2C_LEFT_DIGIT_PATH, OFF);
        writeTOFile(I2C_RIGHT_DIGIT_PATH, OFF);

		// Display right digit
		writeI2cReg(i2cFileDesc, REG_OUTA, bottomDigits[rightDigit]);
		writeI2cReg(i2cFileDesc, REG_OUTB, topDigits[rightDigit]);

		// Turn on right digit
		writeTOFile(I2C_RIGHT_DIGIT_PATH, ON);

		sleepForMs(5);


    }

    writeTOFile(I2C_LEFT_DIGIT_PATH, OFF);
    writeTOFile(I2C_RIGHT_DIGIT_PATH, OFF);

    pthread_exit(NULL);
    return NULL;
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

static void exportLeftDisplayPin61(void)
{
	int exportPin61 = -1;
	while (exportPin61 != 0)
	{
		exportPin61 = runCommand("echo 61 > /sys/class/gpio/export");
	}
}

static void exportRightDisplayPin44(void)
{
	int exportPin44 = -1;
	while (exportPin44 != 0)
	{
		exportPin44 = runCommand("echo 44 > /sys/class/gpio/export");
	}
}

