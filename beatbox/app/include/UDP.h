//Changing the drum-beat mode directly (i.e., jumping from a standard rock beat to no beat).
// Changing the volume.
// Changing the tempo.
// Playing any one of the sounds your drum-beats use.
// Shutting down the program gracefully

#ifndef _UDP_H_
#define _UDP_H_

#define PORT 12345
#define BUFFER_MAX_SIZE 1024

void UDP_init();
void UDP_wait();
void UDP_cleanup(void);

void *UDPServerThread(void *);
void setupUDPSocket(void);

int receivePacket(char *packet);
int sendPacket(char *packet);

void stop();

#endif