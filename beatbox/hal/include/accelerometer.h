//Allow the user to air-drum with the BeagleBone to play audio. For this, when the user 
// moves the BBG and Zen cape in direction, it will trigger a sound.
// Detect significant accelerations in each of the three axis (X: left/right, Y: away/towards, Z: 
// up/down) and have each play a different sound, one for each axis.
// For example, when the user “drums” the BeagleBone vertically (Z), have it play a base 
// drum. For each other axis, use a different sound.
// It must be reasonably possible for a user to get just one play-back of sound per “air-drumming”. Therefore debouncing is likely required.

#define I2CDRV_LINUX_BUS0 "/dev/i2c-0"
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2CDRV_LINUX_BUS2 "/dev/i2c-2"

#define I2C_DEVICE_ADDRESS 0x18 // LIS331DLH address

#define CTRL_REG1 0x20 
#define WHO_AM_I 0x32

#define CTRL_REG1_VALUE 0x27 // power mode 1 0b00100111

#define OUT_X_L 0x28
#define OUT_X_H 0x29
#define OUT_Y_L 0x2A
#define OUT_Y_H 0x2B
#define OUT_Z_L 0x2C
#define OUT_Z_H 0x2D

#ifndef _ACCELEROMETER_H_
#define _ACCELEROMETER_H_

//configure and init
void accelerometer_init();

void accelerometer_wait();

//clean up malloc variable
void accelerometer_cleanup(void);

void *accelThread(void *args);

//get accelerometer value
int16_t *getAccel();

void sleepForMs(long long delayInMs);


#endif