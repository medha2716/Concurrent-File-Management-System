
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../ss1.h"
#define MAX_FILE_READ 4096

typedef struct paths_src_dest
{
  char ch;
  char path1[PATH_MAX];
  char path2[PATH_MAX];
} paths_src_dest;

char command_indicator;

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
      perror("201: Error receiving ack from client");
      return -1;
    }

    if (ack == expectedAck)
    {
      printf("Ack for chunk %d received\n", ack);
      return 0;
    }
    else
    {
      printf("202: Unexpected Ack received. Terminating communication.\n");
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
    printf("%s\n", chunk_array[j]->chunk_buffer);
    j++;
  }

  int chunk_no = j;

  printf("%d\n", chunk_no);

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
      // printf("hi");
      break;
    }
  }
}

int execute1()
{

  int array_size = (1024 / CHUNK_SIZE - 1) + 1000;

  chunk **chunk_array = (chunk **)malloc(sizeof(chunk *) * array_size);

  for (int i = 0; i < array_size; i++)
  {
    chunk_array[i] = (chunk *)malloc(sizeof(chunk));
    // to know this chunk not yet received
  }

  int no_received = 0;
  int chunk_no = array_size;

  // char *path = "file.txt";
  char path[PATH_MAX];

  printf(CYN);
  printf("Enter file path: ");
  printf(RST);

  scanf("%s", path);
  printf("\n");

  // to storage server
  char *ip = "127.0.0.1";

  int sock;
  struct sockaddr_in addr;
  socklen_t addr_size;
  char buffer[1024];
  int n;

  // connect to nm server
  int port = 5566;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
  {
    perror("[-] 200: Socket error");
    exit(1);
  }

  printf("[+]TCP server socket created for connecting to NM server.\n");

  memset(&addr, '\0', sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = port;
  addr.sin_addr.s_addr = inet_addr(ip);

  if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) >= 0)
  {
    printf("Connected to the NM server.\n");

    int ss_or_client = 2;
    send(sock, &ss_or_client, sizeof(ss_or_client), 0);
  }

  bzero(buffer, BUFFER_SIZE);
  recv(sock, buffer, sizeof(buffer), 0);

  // printf("NM Server: %s\n", buffer);

  // send(sock, &command_indicator, sizeof(command_indicator), 0);

  paths_src_dest struct_to_send;
  char buffersend[1024];
  struct_to_send.ch = command_indicator;
  strcpy(struct_to_send.path1, path);
  // strcpy(struct_to_send.path2, NULL);

  send(sock, &struct_to_send, sizeof(struct_to_send), 0);

  printf(CSTM2);
  printf("Path sent: %s\n\n", path);
  printf(RST);

  bzero(buffersend, 1024);
  recv(sock, buffersend, sizeof(buffersend), 0);

  if (strcmp(buffersend, "203: Storage server not found\n") == 0)
    return -1;

  int ss_port;
  recv(sock, &ss_port, sizeof(ss_port), 0);

  if (ss_port != 0)
    port = ss_port;
  else
  {
    printf(RED);
    printf("204: File path (storage server with this path) not found!\n");
    printf(RST);
    return -1;
  }

  close(sock);
  printf("Disconnected from the NM server\n");

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
  {
    perror("\n[-] 200: Socket error");
    return -1;
  }

  printf("\n\n[+]TCP server socket created to communicate with SS directly.\n");

  memset(&addr, '\0', sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = port;
  addr.sin_addr.s_addr = inet_addr(ip);

  if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) >= 0)
  {
    printf("Connected to the server.\n");

    bzero(buffer, 1024);
    strcpy(buffer, "THIS IS THE CLIENT.");
    // printf("Client: %s\n", buffer);
    send(sock, buffer, strlen(buffer), 0);

    bzero(buffer, 1024);
    recv(sock, buffer, sizeof(buffer), 0);

    printf("\033[38;2;173;216;230m");
    printf("Storage Server:\n");
    printf(RST);

    printf("%s\n", buffer);

    char c = command_indicator; /// t,r,w
    send(sock, &c, sizeof(c), 0);

    char ack_start[10];
    recv(sock, ack_start, sizeof(ack_start), 0);
    if (strcmp(ack_start, "START") == 0)
      printf("START ack received\n");

    send(sock, path, strlen(path), 0);

    if (c == 't')
    {
      char result[4096];
      bzero(result, 4096);
      recv(sock, result, sizeof(result), 0);

      printf("%s", result);

      int ack = 1;
      send(sock, &ack, sizeof(ack), 0);

      printf(GRN);
      printf("Sent ack\n");
      printf(RST);
    }
    else if (c == 'r')
    {

      int i = 0;
      while (no_received < chunk_no)
      {
        chunk *received_packet = (chunk *)malloc(sizeof(chunk));

        recv(sock, received_packet, sizeof(chunk), 0);
        printf("[+]Data received: %s\n", received_packet->chunk_buffer);
        strncpy(chunk_array[i++]->chunk_buffer, received_packet->chunk_buffer, sizeof(received_packet->chunk_buffer));
        chunk_no = received_packet->chunk_no; // set the correct no of chunks

        int ack = received_packet->seq;
        send(sock, &ack, sizeof(ack), 0);
        printf("[+]Ack sent: %d\n", ack);
        no_received++;
      }
    }
    else if (c == 'w')
    {
      printf("Enter the content you want to write in the file:\n");
      char texts[MAX_FILE_READ];
      // char temp[MAX_FILE_READ];

      bzero(texts, MAX_FILE_READ);
      // bzero(temp, MAX_FILE_READ);

      printf(CSTM2);
      scanf("%s", texts);
      printf(RST);

      // remove the trailing newline character, if present
      size_t len = strlen(texts);
      if (len > 0 && texts[len - 1] == '\n')
      {
        texts[len - 1] = '\0';
      }

      // printf("The data you want to write to file:\n%s\n", texts);

      // now divide into chunks and send to storage server where it will write into file

      write_file_client(texts, sock);

      char ack_stop[10];
      recv(sock, ack_stop, sizeof(ack_stop), 0);
      if (strcmp(ack_stop, "STOP") == 0)
        printf("STOP ack received\n");
    }

    close(sock);

    printf("Disconnected from the server.\n\n");
    if (c == 'r')
    {
      printf("\e[1;36mFile data read: "); // for reading file
      for (int i = 0; i < chunk_no; i++)
      {
        printf("%s\n", chunk_array[i]->chunk_buffer);
      }
      printf("\n\033[0m");
    }
  }
}

int execute()
{

  int array_size = (1024 / CHUNK_SIZE - 1) + 1000;

  chunk **chunk_array = (chunk **)malloc(sizeof(chunk *) * array_size);

  for (int i = 0; i < array_size; i++)
  {
    chunk_array[i] = (chunk *)malloc(sizeof(chunk));
    // to know this chunk not yet received
  }

  int no_received = 0;
  int chunk_no = array_size;

  // char *path = "file.txt";
  char path[PATH_MAX];
  char path2[PATH_MAX];

  if (command_indicator != 'c')
  {
    printf(CYN);
    printf("Enter file path: ");
    printf(RST);

    scanf("%s", path);
    printf("\n");
  }
  else
  {
    printf(CYN);
    printf("Enter source and dest paths (space separated): ");
    printf(RST);

    scanf("%s %s", path, path2);
    printf("\n");
  }

  // to storage server
  char *ip = "127.0.0.1";

  int sock;
  struct sockaddr_in addr;
  socklen_t addr_size;
  char buffer[1024];
  int n;

  // connect to nm server
  int port = 5566;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
  {
    perror("[-] 200: Socket error");
    exit(1);
  }

  printf("[+]TCP server socket created for connecting to NM server.\n");

  memset(&addr, '\0', sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = port;
  addr.sin_addr.s_addr = inet_addr(ip);

  if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) >= 0)
  {
    printf("Connected to the NM server.\n");

    int ss_or_client = 2;
    send(sock, &ss_or_client, sizeof(ss_or_client), 0);
  }

  bzero(buffer, BUFFER_SIZE);
  recv(sock, buffer, sizeof(buffer), 0);

  // printf("NM Server: %s\n", buffer);

  paths_src_dest struct_to_send;

  char buffersend[1024];
  if (command_indicator != 'c')
  {
    struct_to_send.ch = command_indicator;
    strcpy(struct_to_send.path1, path);
    // strcpy(struct_to_send.path2, NULL);

    send(sock, &struct_to_send, sizeof(struct_to_send), 0);

    printf(CSTM2);
    printf("Path sent: %s\n\n", struct_to_send.path1);
    printf(RST);

    bzero(buffersend, 1024);
    recv(sock, buffersend, sizeof(buffersend), 0);

    if (strcmp(buffersend, "203: Storage server not found\n") == 0)
      return -1;
  }
  else
  {
    struct_to_send.ch = command_indicator;
    strcpy(struct_to_send.path1, path);
    strcpy(struct_to_send.path2, path2);

    send(sock, &struct_to_send, sizeof(struct_to_send), 0);

    printf(CSTM2);
    printf("Path sent: %s %s\n\n", path, path2);
    printf(RST);

    bzero(buffersend, 1024);
    recv(sock, buffersend, sizeof(buffersend), 0);

    if (strcmp(buffersend, "203: Storage server not found\n") == 0)
      return -1;
  }

  close(sock);
  printf("Disconnected from the NM server\n\n");
}

int main()
{
  int choice;

  do
  {

    printf("\033[1;35m"); // Set text color to bright magenta
    printf("\nFile and Folder Operations Menu\n");
    printf("\033[38;2;255;192;203m"); // Light Pink
    printf("1 Create a File\n");
    printf("\033[38;2;255;182;193m"); // Lighter Pink
    printf("2 Read a File\n");
    printf("\033[38;2;255;105;180m"); // Pink
    printf("3 Write a File\n");
    printf("4 Delete a File\n");
    printf("\033[38;2;255;20;147m"); // Dark Pink
    printf("5 Create a Folder\n");
    printf("\033[38;2;219;112;147m"); // Darker Pink
    printf("6 Delete a Folder\n");
    printf("\033[38;2;199;21;133m"); // Deepest Pink
    printf("7 File/Folder Details\n");
    printf("8 Copy files or directories between SS\n");
    printf("\033[0m"); // Reset text color
    printf("0 Exit\n");

    // copy-p(other),s(self)

    // Get user choice
    printf(CYN);
    printf("\nEnter your choice: ");
    printf(RST);

    scanf("%d", &choice);

    // Perform the chosen operation
    switch (choice)
    {
    case 1:
      command_indicator = 'f'; // create file
      execute();
      break;
    case 2:
      command_indicator = 'r';
      execute1();
      break;
    case 3:
      command_indicator = 'w';
      execute1();
      break;
    case 4:
      command_indicator = 'F'; // delete file
      execute();
      break;
    case 5:
      command_indicator = 'd'; // create dir
      execute();
      break;
    case 6:
      command_indicator = 'D'; // delete dir
      execute();
      break;
    case 7:
      command_indicator = 't'; // file details
      execute1();
      break;
    case 8:
      command_indicator = 'c'; // copy
      execute();
      break;
    case 0:
      printf("\033[38;2;255;20;147m"); // Dark Pink
      printf("Exiting program. Goodbye!\n");
      printf("\033[0m"); // Reset text color
      break;
    default:
      printf("Invalid choice. Please try again.\n");
    }

    // Pause before clearing the screen for better readability
    printf("Press Enter to continue...");
    while (getchar() != '\n')
      ;        // Clear the input buffer
    getchar(); // Wait for Enter key

  } while (choice != 0);

  return 0;
}
