
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

int client_socket(char *server_ip, uint16_t server_port);

int main(int argc, char *argv[])
{
  
  if (argc != 3) {
    printf("Invalid argument!\n");
    printf("Usage: %s <bind_ip> <bind_port>\n", argv[0]);
    return 1;
  }

  return client_socket(argv[1], atoi(argv[2]));
}

int client_socket(char *server_ip, uint16_t server_port)
{
  ssize_t rwbytes;
  int net_fd;
  char client_ip[32], data_arena[65538]; /* Why should we use heap if we can use stack? */
  packet *pkt = (packet *)&data_arena;
  struct sockaddr_in server_addr;
  socklen_t rlen = sizeof(struct sockaddr_in);

  /**
   * Prepare server address data.
   */
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(server_port);
  server_addr.sin_addr.s_addr = inet_addr(server_ip);

  /**
   * Create TCP socket.
   */
  if ((net_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Socket creation failed");
    return 1; /* Don't need to close socket fd since it fails to create. */
  }

  /**
   * Conncet to TeaVPN server.
   */
  printf("Connecting to 127.0.0.1:8000...\n");
  if (connect(net_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) < 0) {
    perror("Error on connect");
    goto close_socket;
  }

  printf("Connection established!\n");

  printf("Enter your data: ");
  fgets(pkt->data, 1024, stdin);
  pkt->len = strlen(pkt->data) - 1;
  pkt->data[pkt->len] = '\0';

  {
    size_t wtotal = 0;

    /* Send data length */
    rwbytes = send(net_fd, &(pkt->len), sizeof(uint16_t), 0);
    if (rwbytes < 0) {
      perror("Write error");
      goto close_socket;
    }
    wtotal += rwbytes;

    /* Send data */
    rwbytes = send(net_fd, pkt->data, pkt->len, 0);
    if (rwbytes < 0) {
      perror("Write error");
      goto close_socket;
    }
    wtotal += rwbytes;
    printf("%ld bytes sent!\n", wtotal);
  }

  printf("Waiting for the response...\n");

  /**
   * Read data length.
   */
  rwbytes = recv(net_fd, &(pkt->len), sizeof(uint16_t), 0);
  if (rwbytes < 0) {
    perror("Read error");
    goto close_socket;
  }

  /**
   * Read data.
   */
  rwbytes = recv(net_fd, &(pkt->data), pkt->len, 0);
  if (rwbytes < 0) {
    perror("Read error");
    goto close_socket;
  }

  /* Add null terminator */
  pkt->data[pkt->len] = '\0';
  printf("Received data \"%s\" (%d bytes).\n", pkt->data, pkt->len);

close_socket:
  close(net_fd);
  return 0;
}
