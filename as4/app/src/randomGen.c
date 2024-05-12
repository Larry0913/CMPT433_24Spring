#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include "randomGen.h"

int hitCount = 0;

static float dotX = 0.0;
static float dotY = 0.0;

static float randomFloatInRange() 
{
    static bool initialized = false;
    if (!initialized) {
        // Seed the random number generator once.
        srand((unsigned int)time(NULL));
        initialized = true;
    }

    // Generate a random number within [0, RAND_MAX], then normalize it to [0, 1]
    float randomNormalized = (float)rand() / (float)RAND_MAX;

    // Shift the range to [-0.5, 0.5] and return
    return randomNormalized - 0.5f;
}

void randomGen()
{
    dotX = randomFloatInRange();
    dotY = randomFloatInRange();
    printf("New generation point is (%f, %f)\n", dotX, dotY);
}

bool fireDot(float x, float y)
{
    float minX = dotX - 0.05;
    float maxX = dotX + 0.05;

    float minY = dotY - 0.05;
    float maxY = dotY + 0.05;

    if (minX <= x && x <= maxX && minY <= y && y <= maxY)
    {
        hitCount++;
        return true;
    }
    else
    {
        return false;
    }
}

float getDotX()
{
    return dotX;
}

float getDotY()
{
    //printf("get dot Y is %.1f\n", dotY);
    return dotY;
}

int getHitCount()
{
    return hitCount;
}
