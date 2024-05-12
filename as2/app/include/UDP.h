#ifndef _UDP_H_
#define _UDP_H_

#define PORT 12345
#define BUFFER_MAX_SIZE 1000

void UDP_init();
void UDP_wait();
void UDP_cleanup(void);

void *UDPServerThread(void *);
void setupUDPSocket(void);

int receivePacket(char *packet);
int sendPacket(char *packet);

void runUDPCommand(char *command);
void printHelp(void);
void printCount(void);
void printLength(void);
void printDips(void);
void printHistory(void);
void stop(void);


#endif