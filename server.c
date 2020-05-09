/* 
 * First Written: 2020 05 08
 * SERVER
 * Written by   : Aviezab
 * MIT License
 * Clang-format
 * Convention    : using memset instead of bzero, bzero is not standard for C lib
*/

#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>

#define MAX 1024 // Maximum buffer size
// Change SA to sockaddr to be more readable
#define SA struct sockaddr

// Default PORT 9090
int PORT = 9090;
int sockfd, connfd, len;
struct sockaddr_in serveraddr, cli;
// handling ctrl c and exit
struct sigaction act;

typedef struct
{
    uint16_t len; // the length of data to be sent
    char data[1]; // must be "struct hack"
} packet;

// Event loop to be used to communicate between server and client
int evxloop(int sockfd)
{
    // Initiate buffer and loop variable
    char buff[MAX];
    int n;
    // infinite loop
    for (;;)
    {
        // Fill buffer with zeroes
        memset(buff, 0, strlen(buff));
        // read the message from client and copy it in buffer
        read(sockfd, buff, sizeof(buff));

        printf("Received data \"%s\"\t (%ld bytes)\n", buff, strlen(buff));
        n = 0;
        size_t bufflen = strlen(buff);
        printf("size_t: %d", bufflen);
        char buffx[bufflen + 3];
        int i;

        for (i = 0; i < bufflen; i++)
        {
            printf("i: %d %c\n", i, buff[i]);
            buffx[i + 2] = buff[bufflen - 1 - i];
            // if (i == bufflen - 1)
            // {
            //     buffx[i + 2] = buff[0];
            // }
            // else
            // {
            //     buffx[i + 2] = buff[bufflen - 1 - i];
            // }
        }
        buffx[0] = '#';
        buffx[1] = '#';
        printf("Reversed: %s", buffx);
        break;
        // // Reverse string, and give ##
        // size_t pjg = strlen(buff) + 1;
        // char buff2[pjg - 1];
        // int i;
        // for (i = 0; i < pjg - 2; i++)
        // {
        //     buff2[i] = buff[pjg - 2 - i];
        // }
        // buff2[pjg] = buff[0];

        // printf("%s\n\n", buff2);
        // char buff3[pjg + 4];
        // buff3[pjg + 4] = '\0';
        // sprintf(buff3, "##%s##", buff2);
        // // and send that buffer to client
        // write(sockfd, buff3, sizeof(buff3));
        // memset(buff, 0, strlen(buff));
        // memset(buff2, 0, strlen(buff2));
        // memset(buff3, 0, strlen(buff3));
        // // if msg contains ":q" then server exit and chat ended.
        // if (strncmp(":q", buff, 2) == 0)
        // {
        //     printf("Exitting server ...\n");
        //     break;
        // }
    }
    return 0;
}

// Main Program
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Argument is too few. Exiting...");
        return 1;
    }
    if (argc == 2)
    {
        printf("Port is not defined, using default port %d", PORT);
        return 1;
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
        printf("socket creation failed...\n");
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
        printf("socket bind failed...\n");
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
        printf("server acccept failed...\n");
        exit(0);
    }
    else
        printf("server acccept the client...\n");

    // Function for chatting between client and server
    evxloop(connfd);
    close(sockfd);
    // close the socket
    // if (atexit(close(sockfd)) != 0)
    // {
    //     perror("Error closing socket ...");
    // }
    // else
    // {
    //     printf("Server socket successfully closed");
    // }
}
