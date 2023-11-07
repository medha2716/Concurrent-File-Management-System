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

int storage_servers_connected;

void create_file_dir(int ss_port, char file_or_dir, char *path)
{

    char *ip = "127.0.0.1";
    

    int sock;
    struct sockaddr_in addr;
    char buffer[1024];
    int n;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created to connect to SS with port number %d\n", ss_port);

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = ss_port;
    addr.sin_addr.s_addr = inet_addr(ip);

    connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    printf("Connected to the storage server.\n");

    bzero(buffer, 1024);
    strcpy(buffer, "HELLO, THIS IS NM SERVER.");
    printf("NM server: %s\n", buffer);
    send(sock, buffer, strlen(buffer), 0);

    bzero(buffer, 1024);
    recv(sock, buffer, sizeof(buffer), 0);
    printf("SS server: %s\n", buffer);

    char c = file_or_dir;
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
    printf("Disconnected from the storage server with port %d.\n",ss_port);
}

int main()
{
    char buffer[1024];
    int n;
    storage_servers_connected = 0;

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
    if (listen(server_sock, 80) == 0) // 30 ss, 50 clients?
        printf("Listening...\n");
    else
        printf("Listening Error\n");

    while (1)
    {
        int ss_sock; // have written ss_sock but could be a client!!!
        struct sockaddr_in ss_addr;
        socklen_t addr_size;

        addr_size = sizeof(ss_addr);
        // Extract the first
        // connection in the queue
        ss_sock = accept(server_sock, (struct sockaddr *)&ss_addr, &addr_size);

        int ss_or_client;
        recv(ss_sock, &ss_or_client, sizeof(ss_or_client), 0);

        if (ss_or_client == 1)
        {
            // for each storage server different thread?
            storage_servers_connected++;

            printf("[+]New Storage Server discovered/Change discovered in already stored servers.\n");

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

            // store new ss data ... in such a way that search is easy
            // Efficient Search: Optimize the search process employed by the Naming Server when serving client requests. Avoid linear searches and explore more efficient data structures such as Tries and Hashmaps to swiftly identify the correct Storage Server (SS) for a given request. This optimization enhances response times, especially in systems with a large number of files and folders.
            // LRU Caching: Implement LRU (Least Recently Used) caching for recent searches. By caching recently accessed information, the NM can expedite subsequent requests for the same data, further improving response times and system efficiency.
        }
        else if (ss_or_client == 2) // it is a client
        {
            // for each client we have to have a different thread
            if (!storage_servers_connected)
                continue;

            create_file_dir(1235, 'f', "dir1/dir3/file.txt"); // the last 2 arguments will be user input and sent from client to nm server; get port from the trie/hashmap
            create_file_dir(1235, 'd', "dir1/dir2");

          

            // delete_file_dir();

            // copy_file_dir();
        }
    }

    return 0;
}
