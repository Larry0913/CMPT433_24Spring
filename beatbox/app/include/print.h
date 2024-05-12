//This module is used to print data

#ifndef _PRINT_H_
#define _PRINT_H_

void print_init();

//join thread
void print_wait();

//thread function
void *printDataThread(void *args);

long long getTimeInMs(void);


#endif