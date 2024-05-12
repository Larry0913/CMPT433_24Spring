#ifndef _THREADCONTROLLER_H_
#define _THREADCONTROLLER_H_

extern int running_flag;

void startProgram();
//static void sleepForMs(long long delayInMs);
void stopProgram();

void runCommand(char* command);

void writeTOFile(char *path, long value);

void sleepForMs(long long delayInMs);

#endif