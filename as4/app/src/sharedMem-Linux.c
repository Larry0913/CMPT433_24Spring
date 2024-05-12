#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <math.h>

#include "sharedMem-Linux.h"
#include "sharedDataStruct.h"
#include "joystick.h"
#include "randomGen.h"
#include "threadController.h"

static volatile sharedMemStruct_t* pSharedPru0 = NULL;

void neoPixel_init(volatile sharedMemStruct_t* pSharedPru0_temp)
{
    //init pixel pruout
    runCommand("config-pin P8.11 pruout");

    //get pru value
    pSharedPru0 = pSharedPru0_temp;
}

// Return the address of the PRU's base memory
volatile void* getPruMmapAddr(void) {
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        perror("ERROR: could not open /dev/mem");
        exit(EXIT_FAILURE);
    }

    // Points to start of PRU memory.
    volatile void* pPruBase = mmap(0, PRU_LEN, PROT_READ | PROT_WRITE, 
        MAP_SHARED, fd, PRU_ADDR);
    if (pPruBase == MAP_FAILED) {
        perror("ERROR: could not map memory");
        exit(EXIT_FAILURE);
    }
    close(fd);
    return pPruBase;
}

void freePruMmapAddr(volatile void* pPruBase){
    if (pPruBase != NULL) {
        if (munmap((void*) pPruBase, PRU_LEN)) {
            perror("PRU munmap failed");
            exit(EXIT_FAILURE);
        }
        pPruBase = NULL; 
    }
}

void setPixelColor(int index, int color, bool flag) {
    if(index >= 0 && index < STR_LEN)
    {
        if(color == 0)
        {
            pSharedPru0->neoPixels[index] = OFF_LED;
        }
        else if (flag)
        {
            if (color == 1)
            {
                pSharedPru0->neoPixels[index] = BlueBright;
            }
            else if (color == 2)
            {
                pSharedPru0->neoPixels[index] = GreenBright;
            }
            else if(color == 3)
            {
                pSharedPru0->neoPixels[index] = RedBright;
            }
        }
        else 
        {
            if (color == 1)
            {
                pSharedPru0->neoPixels[index] = Blue;
            }
            else if (color == 2)
            {
                pSharedPru0->neoPixels[index] = Green;
            }
            else if (color == 3)
            {
                pSharedPru0->neoPixels[index] = Red;
            }
        }
    }
    
}

int xPixelShow(float x)
{
    // x axis control the color of the pixel
    //0:blue/ 1:green/ 2:red 
    float dotX = getDotX();
    float minXValue = dotX - 0.05;
    float maxXValue = dotX + 0.05;

    //printf("current dotX is %.1f\n", dotX);

    if (minXValue <= x && x <= maxXValue)
    {
        //blue
        return 1;
    }
    else if (x < minXValue)
    {   
        //green
        return 2;
    }
    else if (x > maxXValue)
    {
        //red
        return 3;
    }
    else 
    {
        //off
        return 0; 
    }
}

void yPixelShow(float y, int color)
{
    clearStrip();
    // y axis control which pixel will show
    float dotY = getDotY();
    float minYValue = dotY - 0.05;
    float maxYvalue = dotY + 0.05;

    //printf("current dotY is %.1f\n", dotY);

    //show all pixels if the target is on the correct way
    if (minYValue <= y && y <= maxYvalue)
    {
        // on the target way, then show all pixel up
        // Bright for all pixels.
        for (int i = 0; i < STR_LEN; i++)
        {
            setPixelColor(i, color, true); 
        }
    }
    else 
    {
        // Scale factor to map accelerometer values to LED indices
        // This may need adjustment to match the physical setup and desired behavior
        float scaleFactor = STR_LEN/ (2 * 0.5);

        // Calculate how much y is off from dotY
        float delta = y - dotY;

        // Calculate target index, initially assuming y is centered at 0
        int targetMidIndex = (STR_LEN / 2) - 1; // For an 8 LED strip, this centers at index 3

        // Adjust index based on the delta and scale factor
        targetMidIndex += round(delta * scaleFactor);
        
        //printf("current mid index is %d\n", targetMidIndex);

        if(targetMidIndex >=0 && targetMidIndex <= 7)
        {
            // If aiming way below the point
            setPixelColor(targetMidIndex - 1, color, false); 
            setPixelColor(targetMidIndex, color, true); 
            setPixelColor(targetMidIndex + 1, color, false); 
        }
        else if (targetMidIndex < 0)
        {
            setPixelColor(0, color, false);
        }
        else
        {
            setPixelColor(STR_LEN - 1, color, false);
        }
    }
}

void clearStrip()
{
    for (int i = 0; i < STR_LEN; i++)
    {
        pSharedPru0->neoPixels[i] = OFF_LED;
    }

}

bool isPressed(enum joystick direction) 
{
    if(direction == DOWN && pSharedPru0->pruJoystickDown == 0)
    {
        printf("joystick DOWN\n");
        return true;
    }
    else if(direction == RIGHT && pSharedPru0->pruJoystickRight == 0)
    {
        printf("joystick RIGHT\n");
        return true;
    }
    else{
        return false;
    }
}

