#ifndef _RANDOMGEN_H_
#define _RANDOMGEN_H_

//This file is used to generate random dot after each hit

#include <stdbool.h>

void randomGen();

bool fireDot(float x, float y);

//get x and y position
float getDotX();
float getDotY();

int getHitCount();

#endif