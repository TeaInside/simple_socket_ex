
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com>
 * @license MIT
 *
 * Simple socket server.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "data_structure.h"

int server_socket(char *server_ip, uint16_t server_port);

int main(int argc, char *argv[])
{
  
  if (argc != 3) {
    printf("Invalid argument!\n");
    printf("Usage: %s <bind_ip> <bind_port>\n", argv[0]);
    return 1;
  }

  return server_socket(argv[1], atoi(argv[2]));
}

int server_socket(char *server_ip, uint16_t server_port)
{
  ssize_t rwbytes;
  int net_fd, client_fd;
  char client_ip[32], data_arena[65538]; /* Why should we use heap if we can use stack? */
  packet *pkt = (packet *)&data_arena;
  struct sockaddr_in server_addr, client_addr;
  socklen_t rlen = sizeof(struct sockaddr_in);

  /**
   * Prepare server bind address data.
   */
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(server_port);
  server_addr.sin_addr.s_addr = inet_addr(server_ip);

  /**
   * Prepare client address data.
   */
  memset(&client_addr, 0, sizeof(client_addr));

  /**
   * Create TCP socket.
   */
  if ((net_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Socket creation failed");
    return 1; /* Don't need to close socket fd since it fails to create. */
  }

  /**
   * Bind socket to address.
   */
  if (bind(net_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("Bind failed");
    goto close_server;
  }

  /**
   * Listen
   */
  if (listen(net_fd, 3) < 0) {
    perror("Listen failed");
    goto close_server;
  }

  printf("Listening on %s:%d...\n", server_ip, server_port);

  while (1) {
    /**
     * Accepting client connection.
     */
    client_fd = accept(net_fd, (struct sockaddr *)&client_addr, &rlen);

    printf(
      "Accepting client (%s:%d)...\n",
      inet_ntoa(client_addr.sin_addr),
      ntohs(client_addr.sin_port)
    );
    printf("Waiting for client data...\n");

    /**
     * Read data length.
     */
    rwbytes = recv(client_fd, &(pkt->len), sizeof(uint16_t), 0);
    if (rwbytes < 0) {
      perror("Read error");
      goto close_client;
    }

    /**
     * Read data.
     */
    rwbytes = recv(client_fd, &(pkt->data), pkt->len, 0);
    if (rwbytes < 0) {
      perror("Read error");
      goto close_client;
    }

    /* Add null terminator */
    pkt->data[pkt->len] = '\0';
    printf("Received data \"%s\" (%d bytes).\n", pkt->data, pkt->len);


    // Reverse the data and wrap with '##'
    {
      register int i;
      register int j;
      register char k;

      pkt->data[pkt->len + 2] = '#';
      pkt->data[pkt->len + 3] = '#';
      pkt->data[pkt->len + 4] = '\0';
      for (j = 0, i = pkt->len - 1; i > 1; i--, j++) {
        k = pkt->data[i + 2];
        pkt->data[i + 2] = pkt->data[j];
        pkt->data[j] = k;
      }
      pkt->data[0] = '#';
      pkt->data[1] = '#';
    }

    /* Add wrap chars length */
    pkt->len += 4;

    printf("Sending response \"%s\" (%d bytes) to the client...\n", pkt->data, pkt->len);

    {
      size_t wtotal = 0;

      /* Send data length */
      rwbytes = send(client_fd, &(pkt->len), sizeof(uint16_t), 0);
      if (rwbytes < 0) {
        perror("Write error");
        goto close_client;
      }
      wtotal += rwbytes;

      /* Send data */
      rwbytes = send(client_fd, pkt->data, pkt->len, 0);
      if (rwbytes < 0) {
        perror("Write error");
        goto close_client;
      }
      wtotal += rwbytes;
      printf("%ld bytes sent!\n", wtotal);
    }

  close_client:
    close(client_fd);
    printf("\n");
  }

close_server:
  close(net_fd);
  return 1;
}
