
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{

  // to storage server
  char *ip = "127.0.0.1";
  int port = 1234;

  int sock;
  struct sockaddr_in addr;
  socklen_t addr_size;
  char buffer[1024];
  int n;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
  {
    perror("[-]Socket error");
    exit(1);
  }

  printf("[+]TCP server socket created.\n");

  memset(&addr, '\0', sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = port;
  addr.sin_addr.s_addr = inet_addr(ip);

  char *path = "dir1/";

  if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) >= 0)
  {
    printf("Connected to the server.\n");

    bzero(buffer, 1024);
    strcpy(buffer, "HELLO, THIS IS CLIENT.");
    printf("Client: %s\n", buffer);
    send(sock, buffer, strlen(buffer), 0);

    bzero(buffer, 1024);
    recv(sock, buffer, sizeof(buffer), 0);
    printf("Storage Server: %s\n", buffer);

    char c = 'f';
    send(sock, &c, sizeof(c), 0);

    char ack_start[10];
    recv(sock, ack_start, sizeof(ack_start), 0);
    if (strcmp(ack_start, "START") == 0)
      printf("START ack received\n");

    send(sock, path, strlen(path), 0);

    char ack_stop[10];
    recv(sock, ack_stop, sizeof(ack_stop), 0);
    if (strcmp(ack_stop, "STOP") == 0)
      printf("STOP ack received\n");

    close(sock);
    printf("Disconnected from the server.\n");
  }

  // to nfs_server

  port = 5566;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
  {
    perror("[-]Socket error");
    exit(1);
  }

  printf("[+]TCP server socket created.\n");

  memset(&addr, '\0', sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = port;
  addr.sin_addr.s_addr = inet_addr(ip);

  if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) >= 0)
  {
    printf("Connected to the server.\n");

    int ss_or_client = 2;
    send(sock, &ss_or_client, sizeof(ss_or_client), 0);
  }

  return 0;
}