#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "threadControl.h"
#include "periodTimer.h"
#include "UDP.h"
#include "light_sampler.h"
#include "samplerDips.h"


//global variable
int sockfd;
struct sockaddr_in servaddr, cliaddr;
static pthread_t UDP_id;

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
    char lastCommand[BUFFER_MAX_SIZE] = {0};

    while (running_flag)
    {
        int size = receivePacket(buffer);
        buffer[size] = '\0';

        size_t length = strlen(lastCommand);
        //if only receive enter
        if (strcmp(buffer, "\n") == 0)
        {
            strncpy(buffer, lastCommand, length);
            buffer[length] = '\0';
        }
        else //updade last command
        {
            size_t length = strlen(buffer);
            strncpy(lastCommand, buffer, length);
            lastCommand[length] = '\0';
        }
        
        runUDPCommand(buffer);
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

void runUDPCommand(char *command)
{
    if (strcmp(command, "help\n") == 0 ||strcmp(command, "?\n") == 0)
    {
        printHelp();
    }
    else if (strcmp(command, "count\n") == 0)
    {
        printCount();
    }
    else if (strcmp(command, "length\n") == 0)
    {
        printLength();
    }
    else if (strcmp(command, "dips\n") == 0)
    {
        printDips();
    }
    else if (strcmp(command, "history\n") == 0)
    {
        printHistory();
    }
    else if (strcmp(command, "stop\n") == 0)
    {
        stop();
    }
    else
    {
        char buffer[BUFFER_MAX_SIZE] = {0};
        sprintf(buffer, "Unknown command. Type 'help' for command list.\n");
        sendPacket(buffer);
        printHelp();
    }
}

void printHelp(void)
{
    char buffer[BUFFER_MAX_SIZE] = {0};
    strcat(buffer, "\nAccepted command examples:\n");
    strcat(buffer, "count       -- get the total number of samples taken.\n");
    strcat(buffer, "length      -- get the number of samples taken in the previously completed second.\n");
    strcat(buffer, "dips        -- get the number of dips in the previously completed second.\n");
    strcat(buffer, "history     -- get all the samples in the previously completed second.\n");
    strcat(buffer, "stop        -- cause the server program to end.\n");
    strcat(buffer, "<enter>     -- repeat last command.\n");
    sendPacket(buffer);
}

void printCount(void)
{
    char buffer[BUFFER_MAX_SIZE] = {0};
    sprintf(buffer, "# samples taken total: %lld\n", Sampler_getNumSamplesTaken());
    sendPacket(buffer);
}

void printLength(void)
{
    char buffer[BUFFER_MAX_SIZE] = {0};
    sprintf(buffer, "# samples taken last second: %d\n", Sampler_getHistorySize());
    sendPacket(buffer);
}

void printDips(void)
{
    char buffer[BUFFER_MAX_SIZE] = {0};
    sprintf(buffer, "# Dips: %d\n", getDipCount());
    sendPacket(buffer);
}

void printHistory(void)
{
    int numOfSamples;
    double *current_history = Sampler_getHistory(&numOfSamples);
    char buffer[BUFFER_MAX_SIZE] = {0};
    int buffer_length = 0;

    for (int i = 0; i < numOfSamples; i++)
    {
        // Calculate the remaining space in the buffer
        int remaining_space = BUFFER_MAX_SIZE - buffer_length;
        
        // Append the current sample to the buffer, including comma
        int written = snprintf(buffer + buffer_length, remaining_space, "%.3f, ", current_history[i]);
        
        // Check if snprintf was successful
        if (written < 0 || written >= remaining_space) {
            // Handle error or buffer overflow here
            break;
        }
        
        buffer_length += written; // Update the buffer_length

        // Check if it's time to send the buffer (after every 20 samples)
        if ((i + 1) % 20 == 0 || i == numOfSamples - 1) {
            // Check if we need to append a newline character
            remaining_space = BUFFER_MAX_SIZE - buffer_length;
            if (remaining_space > 0) {
                buffer[buffer_length] = '\n'; // Add newline character
                buffer_length++;
            }
            if (remaining_space > 1) {
                buffer[buffer_length] = '\0'; // Null-terminate the string
            }
            sendPacket(buffer);
            memset(buffer, 0, BUFFER_MAX_SIZE);
            buffer_length = 0; // Reset buffer for the next line
        }
    }

    free(current_history);
}

void stop(void)
{
    char buffer[BUFFER_MAX_SIZE] = {0};
    sprintf(buffer, "Program terminating.\n");
    sendPacket(buffer);
    stopProgram();
    //running_flag = 0;
}