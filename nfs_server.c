#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

// 1 for ss (ss_or_client)
// 2 for client

#define MAX_LENGTH_ACC_PATHS_ONE_SS 100000

typedef struct send_nm_init
{
    char accessible_paths[MAX_LENGTH_ACC_PATHS_ONE_SS];
    int port_nm;
    int port_ss;
    char ip[40];
} send_nm_init;

char *ip = "127.0.0.1";
int port = 5566; // SS KNOWS PORT NUMBER OF NM server

int server_sock;
struct sockaddr_in server_addr;
socklen_t addr_size;

int main()
{
    char buffer[1024];
    int n;

    printf("Initializing Naming Server..\n\n");


    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created\n");

    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    n = bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (n < 0)
    {
        perror("[-]Bind error");
        exit(1);
    }
    printf("[+]Bind to the port number: %d\n", port);

    // Listen on the socket,
    // with 100 max connection
    // requests queued
    if (listen(server_sock, 100) == 0)
        printf("Listening...\n");
    else
        printf("Listening Error\n");

     while (1)
    {
        int ss_sock;
        struct sockaddr_in ss_addr;
        socklen_t addr_size;

        addr_size = sizeof(ss_addr);
        // Extract the first
        // connection in the queue
        ss_sock = accept(server_sock, (struct sockaddr *)&ss_addr, &addr_size);

        int ss_or_client;
        recv(ss_sock, &ss_or_client, sizeof(ss_or_client), 0);
        if(ss_or_client == 1)
        {
            printf("[+]New Storage Server discovered.\n");

            send_nm_init struct_received;
            int check = recv(ss_sock, &struct_received, sizeof(struct_received), 0);
            printf("Struct Received!\n");
            printf("Accessible Paths:\n%s", struct_received.accessible_paths);
            printf("IP port: %s\n", struct_received.ip);
            printf("Port for NM connection: %d\n", struct_received.port_nm);
            printf("Port for ss connection: %d\n", struct_received.port_ss);

            if (check >= 0) // struct received
            {
                bzero(buffer, 1024);
                strcpy(buffer, "HI, THIS IS NM SERVER! SS HAS BEEN RECOGNIZED.");
                printf("NM server sending message: %s ..\n", buffer);
                send(ss_sock, buffer, strlen(buffer), 0);
            }
            else
            {
                printf("[-]Receiving Error.\n");
            }

            close(ss_sock);
            printf("[+]SS disconnected.\n\n");
        }
    }

    

    return 0;
}
