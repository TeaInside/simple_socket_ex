/* 
 * First Written : 2020 05 08
 * SERVER C
 * Written by    : Aviezab
 * MIT License
 * Clang-format
 * Convention    : using memset instead of bzero, bzero is not standard for C lib
*/

// STD Lib
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
// Custom Library
#include <data_structure.h>

// Change SA to sockaddr to be more readable
#define SA struct sockaddr

// Default PORT 9090
uint16_t PORT = 9090;
int sockfd, connfd;
unsigned int len;
struct sockaddr_in serveraddr, cli;
// handling ctrl c and exit
struct sigaction act;

// Event loop to be used to communicate between server and client
int evsvrloop(int sockfd)
{
    // Initiate buffer and loop variable
    char buff[sizeof(data_arena)];
    //printf("DATA ARENA SIZE: %ld\n", sizeof(data_arena));
    packet *pkt = (packet *)&data_arena;
    int i;
    // infinite loop
    for (;;)
    {
        // Read the message from client and copy it in packet struct
        memset(buff, 0, strlen(buff));
        read(sockfd, buff, sizeof(buff));
        printf("Received data \"%s\"\t (%ld bytes)\n", buff, strlen(buff));

        // if msg contains "q" then server exit and chat ended.
        if (strncmp("q", buff, 1) == 0)
        {
            printf("Exiting Server & send exit command to Client ...\n");
            write(sockfd, "q", 1);
            break;
        }
        size_t len_cltmsg = strlen(buff);
        // Empty to reuse
        for (int i = 0; i < len_cltmsg; i++)
        {
            pkt->data[i] = '\0';
        }
        pkt->len = len_cltmsg + 4;
        pkt->data[pkt->len - 2] = '#';
        pkt->data[pkt->len - 1] = '#';
        pkt->data[pkt->len] = '\0';
        pkt->data[0] = '#';
        pkt->data[1] = '#';
        // Reuse pkt->data to reply Client
        for (i = 0; i < len_cltmsg; i++)
        {
            //printf("i: %d %c\n", i, buff[i]);
            pkt->data[i + 2] = buff[len_cltmsg - 1 - i];
        }
        write(sockfd, pkt->data, pkt->len);
    }
    return 0;
}

// Main Program
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        perror("Argument is too few. Exiting...");
        return 1;
    }
    if (argc == 2)
    {
        printf("Port is not defined, using default port %d", PORT);
    }
    if (argc == 3)
    {
        printf("Server is running on %s %s\n", argv[1], argv[2]);
        PORT = atoi(argv[2]);
    }
    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("Socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");

    memset(&serveraddr, 0, sizeof(serveraddr));

    // assign IP, PORT
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
    serveraddr.sin_port = htons(PORT);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA *)&serveraddr, sizeof(serveraddr))) != 0)
    {
        printf("Socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);

    // Accept the data packet from client and verification
    connfd = accept(sockfd, (SA *)&cli, &len);
    if (connfd < 0)
    {
        printf("Server acccept failed...\n");
        exit(0);
    }
    else
        printf("Server acccepts the Client...\n");

    // Function for chatting between client and server
    evsvrloop(connfd);
    close(sockfd);
}
