/* 
 * First Written: 2020 05 08
 * SERVER
 * Written by   : Aviezab
 * MIT License
 * Clang-format
 * Convention    : using memset instead of bzero, bzero is not standard for C lib
*/

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#define MAX 1024
#define SA struct sockaddr

uint16_t PORT = 8080;

int func(int sockfd)
{
    char buff[MAX];
    int n;
    for (;;)
    {
    awal:
        memset(buff, 0, strlen(buff));
        bzero(buff, sizeof(buff));
        printf("Enter the string : ");
        n = 0;
        while ((buff[n++] = getchar()) != '\n')
            ;
        size_t akhirb = strlen(buff);
        if (akhirb == 1024)
        {
            printf("This version is only able to pass 1MB of message.");
            goto awal;
        }
        if (akhirb == 0)
        {
            printf("No single char passed.");
            goto awal;
        }
        buff[akhirb - 1] = '\0';
        write(sockfd, buff, sizeof(buff));
        memset(buff, 0, akhirb);
        break;
        read(sockfd, buff, sizeof(buff));

        printf("From Server : %s\n\n", buff);
        if ((strncmp(buff, "exit", 4)) == 0)
        {
            printf("Client Exit...\n");
            break;
        }
    }
    return 0;
}

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

    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    // socket create and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");

    memset(&servaddr, 0, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("Connection with the server failed...\n");
        exit(0);
    }
    else
        printf("Connected to the server..\n");

    // function for chat
    func(sockfd);
    close(sockfd);
    // close the socket
    // if (atexit(close(sockfd)) != 0)
    // {
    //     perror("Error closing socket ...");
    // }
}
