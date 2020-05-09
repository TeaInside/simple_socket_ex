/* 
 * First Written : 2020 05 08
 * CLIENT C
 * Written by    : Aviezab
 * MIT License
 * Clang-format
 * Convention    : using memset instead of bzero, bzero is not standard for C lib
*/

// STD Lib
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
// Custom Library
#include <data_structure.h>
// Define SA as sockaddr struct
#define SA struct sockaddr

uint16_t PORT = 9090;

int evcltloop(int sockfd)
{
    // This declaration below
    packet *pkt = (packet *)&data_arena;
    for (;;)
    {
    awal:
        printf("Enter the message : ");
        fgets(pkt->data, 1024, stdin);
        pkt->len = strlen(pkt->data) - 1;
        pkt->data[pkt->len] = '\0';
        size_t akhirb = strlen(pkt->data);
        if (akhirb == sizeof(data_arena))
        {
            printf("This version is only able to pass %lu of message.", sizeof(data_arena));
            goto awal;
        }
        if (akhirb == 0)
        {
            printf("No single char passed. Input again!");
            goto awal;
        }
        pkt->data[pkt->len] = '\0';
        write(sockfd, pkt->data, pkt->len);
        // Reuse to be reply message from server
        memset(pkt->data, 0, pkt->len);
        read(sockfd, pkt->data, pkt->len + 4);
        printf("From Server (%d) : %s\n", pkt->len + 4, pkt->data);
        if ((strncmp(pkt->data, "q", 1)) == 0)
        {
            printf("Received terminate signal from server. Client Exit...\n");
            break;
        }
        memset(pkt->data, 0, pkt->len);
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
    evcltloop(sockfd);
    close(sockfd);
    // close the socket
    // if (atexit(close(sockfd)) != 0)
    // {
    //     perror("Error closing socket ...");
    // }
}
