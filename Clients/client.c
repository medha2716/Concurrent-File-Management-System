
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "ss1.h"
#define MAX_FILE_READ 4096

int waitForAck(int sock, int expectedAck, int timeoutSeconds)
{
    fd_set readSet;
    struct timeval timeout;

    FD_ZERO(&readSet);
    FD_SET(sock, &readSet);

    timeout.tv_sec = timeoutSeconds;
    timeout.tv_usec = 0;

    int ready = select(sock + 1, &readSet, NULL, NULL, &timeout);

    if (ready == -1)
    {
        perror("select");
        return -1;
    }
    else if (ready == 0)
    {
        // Timeout occurred
        printf("Timeout: No acknowledgment received within %d seconds. Terminating communication.\n", timeoutSeconds);
        return -1;
    }
    else
    {
        // Data is ready to be read
        int ack;
        if (recv(sock, &ack, sizeof(ack), 0) < 0)
        {
            perror("error receiving ack from client");
            return -1;
        }

        if (ack == expectedAck)
        {
            printf("Ack for chunk %d received\n", ack);
            return 0;
        }
        else
        {
            printf("Unexpected Ack received. Terminating communication.\n");
            return -1;
        }
    }
    return 0;
}

void write_file_client(char *buffer, int sock)
{
    int array_size = (1024 / CHUNK_SIZE - 1) + 1000;

    chunk **chunk_array = (chunk **)malloc(sizeof(chunk *) * array_size);

    for (int i = 0; i < array_size; i++)
    {
        chunk_array[i] = (chunk *)malloc(sizeof(chunk));
        chunk_array[i]->chunk_no = 0; // tells client number of chunks it has to receive
        chunk_array[i]->seq = i;
    }

    int file_descriptor;
   

    int i = 0;
    int j = 0;
    while (buffer[i] != '\0')
    {
        for (int k = 0; k < CHUNK_SIZE && buffer[i] != '\0'; k++)
        {
            chunk_array[j]->chunk_buffer[k] = buffer[i];
            i++;
        }
        chunk_array[j]->chunk_buffer[strlen(chunk_array[j]->chunk_buffer)] = '\0';
        printf("%s\n",chunk_array[j]->chunk_buffer);
        j++;
    }

    int chunk_no = j;

    printf("%d\n",chunk_no);

    chunk *send_packet;

    for (int i = 0; i < chunk_no; i++)
    {
        chunk_array[i]->chunk_no = chunk_no;
        send_packet = chunk_array[i];
        if (send(sock, (chunk *)send_packet, sizeof(chunk), 0) < 0)
            perror("sending message to storage server");
        printf("[+]Data sent: %s\n", send_packet->chunk_buffer);

        int check = waitForAck(sock, chunk_array[i]->seq, 10);
        if (check < 0)
        {
            printf("hi");
            break;
        }
    }
}

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

  char *path = "file.txt";

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

    char c = 'w'; ///eeeeee - f,r,w
    send(sock, &c, sizeof(c), 0);

    char ack_start[10];
    recv(sock, ack_start, sizeof(ack_start), 0);
    if (strcmp(ack_start, "START") == 0)
      printf("START ack received\n");

    send(sock, path, strlen(path), 0);

    // for file details

    // char result[4096];
    // bzero(result,4096);
    // recv(sock,result,sizeof(result),0);

    // printf("%s\n",result);

    // int ack = 1;
    // send(sock, &ack, sizeof(ack), 0);
    // printf("Sent ack\n");

    // for reading file

    // int array_size = (1024 / CHUNK_SIZE - 1) + 1000;

    // chunk **chunk_array = (chunk **)malloc(sizeof(chunk *) * array_size);

    // for (int i = 0; i < array_size; i++)
    // {
    //   chunk_array[i] = (chunk *)malloc(sizeof(chunk));
    //   // to know this chunk not yet received
    // }

    // int no_received = 0;
    // int chunk_no = array_size;
    // int i=0;
    // while (no_received < chunk_no)
    // {
    //   chunk *received_packet = (chunk *)malloc(sizeof(chunk));

    //   recv(sock, received_packet, sizeof(chunk), 0);
    //   printf("[+]Data received: %s\n", received_packet->chunk_buffer);
    //   strncpy(chunk_array[i++]->chunk_buffer,received_packet->chunk_buffer,sizeof(received_packet->chunk_buffer));
    //   chunk_no = received_packet->chunk_no; // set the correct no of chunks

    //   int ack = received_packet->seq;
    //   send(sock, &ack, sizeof(ack), 0);
    //   printf("[+]Ack sent: %d\n",ack);
    //   no_received++;
      
    // }

    // for writing file

  

    printf("Enter the content you want to write in the file: (press enter twice once you are done typing)\n");
    char texts[MAX_FILE_READ];
    char temp[MAX_FILE_READ];

    bzero(texts,MAX_FILE_READ);
    bzero(temp,MAX_FILE_READ);

    while (1)
    {
        // Read a line from the user
        if (fgets(temp, sizeof(temp), stdin) == NULL)
        {
            // Error or end of input
            break;
        }

        // append the line to texts
        strncat(texts, temp, sizeof(texts) - strlen(texts) - 1);

        // check if the user pressed Enter without typing anything (empty line to finish)
        if (strcmp(temp, "\n") == 0)
        {
            break;
        }
    }

    // remove the trailing newline character, if present
    size_t len = strlen(texts);
    if (len > 0 && texts[len - 1] == '\n')
    {
        texts[len - 1] = '\0';
    }

    printf("The data you want to write to file:\n%s\n", texts);
   

    //now divide into chunks and send to storage server where it will write into file

    write_file_client(texts,sock);
    

    char ack_stop[10];
    recv(sock, ack_stop, sizeof(ack_stop), 0);
    if (strcmp(ack_stop, "STOP") == 0)
      printf("STOP ack received\n");

    close(sock);
    printf("Disconnected from the server.\n");

    // printf("\e[1;36mFile data read: "); // for reading file
    // for (int i = 0; i < chunk_no; i++)
    // {
    //   printf("%s", chunk_array[i]->chunk_buffer);
    // }
    // printf("\n\033[0m");
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