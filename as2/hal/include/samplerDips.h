//samplerDips.h count for light dips 

#ifndef _SAMPLERDIPS_H_
#define _SAMPLERDIPS_H_

#define THRESHOLD 0.1
#define HYSTERESIS 0.03

void Dip_init();
void Dip_wait();
void Dip_cleanup(void);
void *DipCounterThread(void *args);
int getDipCount();
void printData();

#endif