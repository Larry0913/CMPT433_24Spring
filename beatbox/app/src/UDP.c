#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "threadController.h"
#include "UDP.h"
#include "audioGeneration.h"
#include "audioMixer_template.h"

//global variable
int sockfd;
struct sockaddr_in servaddr, cliaddr;
static pthread_t UDP_id;

static float get_uptime()
{
	float upTime;
	FILE *upTimeFile = fopen("/proc/uptime", "r");
	fscanf(upTimeFile, "%f", &upTime);
	fclose(upTimeFile);
	return upTime;
}

void UDP_init()
{
    setupUDPSocket();
    //printf("set up socket successsfully!\n");
    pthread_create(&UDP_id, NULL, UDPServerThread, NULL);
}

void UDP_wait()
{
    pthread_join(UDP_id, NULL);
}

void UDP_cleanup(void)
{
    close(sockfd);
}

void *UDPServerThread(void *args)
{
    (void)args;
    char buffer[BUFFER_MAX_SIZE];
    char response[BUFFER_MAX_SIZE];

    while (running_flag)
    {
        memset(buffer, 0, BUFFER_MAX_SIZE);  // Clear the buffer
        memset(response, 0, BUFFER_MAX_SIZE);  // Clear the response buffer
        int size = receivePacket(buffer);
        buffer[size] = '\0';

        if (size > 0)
        {
            if (strcmp(buffer, "status\n") == 0)
            {
                sprintf(response, "bbg_allin status uptime=%f mode=%d volume=%d bpm=%d", get_uptime(), getMode(), AudioMixer_getVolume(), getBpmValue());
                sendPacket(response);
            }
            else if (strcmp(buffer, "stop\n") == 0)
            {
                sprintf(response, "bbg_allin stop uptime=%f mode=%d volume=%d bpm=%d", get_uptime(), getMode(), AudioMixer_getVolume(), getBpmValue());
                sendPacket(response);
                stop();
            }
            else if (strcmp(buffer, "play base\n") == 0)
            {
                playSound(0);
                sprintf(response, "bbg_allin status uptime=%f mode=%d volume=%d bpm=%d", get_uptime(), getMode(), AudioMixer_getVolume(), getBpmValue());
                sendPacket(response);
            }
            else if (strcmp(buffer, "play hiHat\n") == 0)
            {
                playSound(1);
                sprintf(response, "bbg_allin status uptime=%f mode=%d volume=%d bpm=%d", get_uptime(), getMode(), AudioMixer_getVolume(), getBpmValue());
                sendPacket(response);
            }
            else if (strcmp(buffer, "play snare\n") == 0)
            {
                playSound(2);
                sprintf(response, "bbg_allin status uptime=%f mode=%d volume=%d bpm=%d", get_uptime(), getMode(), AudioMixer_getVolume(), getBpmValue());
                sendPacket(response);
            }
            else if (strcmp(buffer, "play none\n") == 0)
            {
                setMode(0);
                sprintf(response, "bbg_allin status uptime=%f mode=%d volume=%d bpm=%d", get_uptime(), getMode(), AudioMixer_getVolume(), getBpmValue());
                sendPacket(response);
                
            }
            else if (strcmp(buffer, "play rock\n") == 0)
            {
                setMode(1);
                sprintf(response, "bbg_allin status uptime=%f mode=%d volume=%d bpm=%d", get_uptime(), getMode(), AudioMixer_getVolume(), getBpmValue());
                sendPacket(response);
                
            }
            else if (strcmp(buffer, "play customize\n") == 0)
            {
                setMode(2);
                sprintf(response, "bbg_allin status uptime=%f mode=%d volume=%d bpm=%d", get_uptime(), getMode(), AudioMixer_getVolume(), getBpmValue());
                sendPacket(response);
                
            }
            else if (strcmp(buffer, "volume increases\n") == 0)
            {
                AudioMixer_setVolume(AudioMixer_getVolume() + 5);
                sprintf(response, "bbg_allin status uptime=%f mode=%d volume=%d bpm=%d", get_uptime(), getMode(), AudioMixer_getVolume(), getBpmValue());
                sendPacket(response);
                
            }
            else if (strcmp(buffer, "volume decrease\n") == 0)
            {
                AudioMixer_setVolume(AudioMixer_getVolume() - 5);
                sprintf(response, "bbg_allin status uptime=%f mode=%d volume=%d bpm=%d", get_uptime(), getMode(), AudioMixer_getVolume(), getBpmValue());
                sendPacket(response);
                
            }
            else if (strcmp(buffer, "tempo increase\n") == 0)
            {
                setBpmValue(getBpmValue() + 5);
                sprintf(response, "bbg_allin status uptime=%f mode=%d volume=%d bpm=%d", get_uptime(), getMode(), AudioMixer_getVolume(), getBpmValue());
                sendPacket(response);
            }
            else if (strcmp(buffer, "tempo decrease\n") == 0)
            {
                setBpmValue(getBpmValue() - 5);
                sprintf(response, "bbg_allin status uptime=%f mode=%d volume=%d bpm=%d", get_uptime(), getMode(), AudioMixer_getVolume(), getBpmValue());
                sendPacket(response);
            }
            else
            {
                printf("command is %s", buffer);
                printf("Invalid command!\n");
            }
            
        }
        
    }

    pthread_exit(NULL);
    return NULL;
    
}

void setupUDPSocket(void)
{
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    //create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("Failed to create socket!\n");
        exit(1);
    }

    //bind
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("Binding failed!\n");
        exit(1);
    }
}

int receivePacket(char *packet)
{
    unsigned int sin_len = sizeof(cliaddr);
    int bytesRx = recvfrom(sockfd, packet, BUFFER_MAX_SIZE - 1, 0, (struct sockaddr *) &cliaddr, &sin_len);

    if (bytesRx < 0)
    {
        perror("Failed to receive packet.\n");
        exit(1);
    }
    return bytesRx;
}

int sendPacket(char *packet)
{
    unsigned int sin_len = sizeof(cliaddr);
    int byteTx = sendto(sockfd, (const char *)packet, BUFFER_MAX_SIZE - 1, 0, (const struct sockaddr *) &cliaddr, sin_len); 

    if (byteTx < 0)
    {
        perror("Failed to send packet.\n");
        exit(1);
    }
    return byteTx;
}

void stop()
{
    char buffer[BUFFER_MAX_SIZE] = {0};
    sprintf(buffer, "Program terminating.\n");
    sendPacket(buffer);
    stopProgram();
    //running_flag = 0;
}