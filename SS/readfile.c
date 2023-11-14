#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "ss1.h"

#define CHUNK_SIZE 1024

typedef struct chunk
{
    int chunk_no; // total number of chunks in total
    char chunk_buffer[CHUNK_SIZE];
    uint32_t seq; // chunk no i have sent right now
                  // int ack; // the byte number that the receiver expects to receive next
} chunk;

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

int main()
{
    int sock;
    // till nm receives stop, client is reading. if at any point client does not send back ack in 1 second, end communication to free file for others.

    // alternative and better is that we dont wait for acks like in tcp from udp, if at

    // initialize array to be sent

    int array_size = (1024 / CHUNK_SIZE - 1) + 1000;

    chunk **chunk_array = (chunk **)malloc(sizeof(chunk *) * array_size);

    for (int i = 0; i < array_size; i++)
    {
        chunk_array[i] = (chunk *)malloc(sizeof(chunk));
        chunk_array[i]->chunk_no = 0; // tells client number of chunks it has to receive
        chunk_array[i]->seq = i;
    }

    int file_descriptor;
    char buffer[CHUNK_SIZE];

    // Open the file in read-only mode
    file_descriptor = open("file.txt", O_RDONLY);

    if (file_descriptor == -1)
    {
        perror("Error opening file");
        return 1;
    }

    ssize_t bytesRead;
    int i = 0;

    printf("File content:");

    while ((bytesRead = read(file_descriptor, buffer, sizeof(buffer))) > 0)
    {
        write(STDOUT_FILENO, buffer, bytesRead);
        strncpy(chunk_array[i]->chunk_buffer, buffer, bytesRead);
        i++;
    }

    if (bytesRead == -1)
    {
        perror("Error reading from file");
    }

    int chunk_no = i;

    chunk *send_packet;

    for (int i = 0; i < chunk_no; i++)
    {
        chunk_array[i]->chunk_no = chunk_no;
        send_packet = chunk_array[i];
        if (send(sock, (chunk *)send_packet, sizeof(chunk), 0) < 0)
            perror("sending message to client");
        printf("[+]Data sent: %s\n", send_packet->chunk_buffer);

        int check = waitForAck(sock, chunk_array[i]->seq, 10);
        if (check < 0)
        {
            printf("hi");
            break;
        }
    }

    // Close the file
    close(file_descriptor);

    return 0;
}
