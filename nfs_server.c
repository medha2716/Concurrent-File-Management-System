#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "ss1.h"

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

typedef struct paths_src_dest
{
    char ch;
    char path1[PATH_MAX];
    char path2[PATH_MAX];
} paths_src_dest;



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
    printf("Disconnected from the storage server with port %d.\n", ss_port);
}

void delete_file_dir(int ss_port, char file_or_dir, char *path)
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
    printf("Disconnected from the storage server with port %d.\n", ss_port);
}

void copy_file_dir_nm(int ss_port, int port2, char *srcPath, char *destPath)
{
    //   if (argc != 3)
    // {
    //     fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
    //     exit(1);
    // }

    // const char *srcPath = argv[1];
    // const char *destPath = argv[2];

    char *ip = "127.0.0.1";
    int port1 = ss_port;
    int ack;

    int sock1; // copy from
    struct sockaddr_in addr1;
    socklen_t addr_size;
    char buffer[1024];
    int n;
    sock1 = socket(AF_INET, SOCK_STREAM, 0);
    if (sock1 < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    memset(&addr1, '\0', sizeof(addr1));
    addr1.sin_family = AF_INET;
    addr1.sin_port = port1;
    addr1.sin_addr.s_addr = inet_addr(ip);

    connect(sock1, (struct sockaddr *)&addr1, sizeof(addr1));
    printf("Connected to the storage server 1.\n");

    // int port2 = 2345; // copy 2
    int sock2;
    struct sockaddr_in addr2;

    sock2 = socket(AF_INET, SOCK_STREAM, 0);
    if (sock2 < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    memset(&addr2, '\0', sizeof(addr2));
    addr2.sin_family = AF_INET;
    addr2.sin_port = port2;
    addr2.sin_addr.s_addr = inet_addr(ip);

    connect(sock2, (struct sockaddr *)&addr2, sizeof(addr2));
    printf("Connected to the storage server 2.\n");

    bzero(buffer, 1024);
    strcpy(buffer, "HELLO, THIS IS NM SERVER.");
    printf("NM server: %s\n", buffer);
    send(sock1, buffer, strlen(buffer), 0);

    bzero(buffer, 1024);
    recv(sock1, buffer, sizeof(buffer), 0);
    printf("SS server: %s\n", buffer);

    char c = 'p';
    send(sock1, &c, sizeof(c), 0);

    char ack_start[10];
    recv(sock1, ack_start, sizeof(ack_start), 0);
    if (strcmp(ack_start, "START") == 0)
        printf("START ack received\n");

    bzero(buffer, 1024);
    strcpy(buffer, "HELLO, THIS IS NM SERVER.");
    printf("NM server: %s\n", buffer);
    send(sock2, buffer, strlen(buffer), 0);

    bzero(buffer, 1024);
    recv(sock2, buffer, sizeof(buffer), 0);
    printf("SS server: %s\n", buffer);

    c = 'c';
    send(sock2, &c, sizeof(c), 0);

    recv(sock2, ack_start, sizeof(ack_start), 0);
    if (strcmp(ack_start, "START") == 0)
        printf("START ack received\n");

    // start
    bzero(buffer, 1024);
    strcpy(buffer, srcPath);
    printf("Sent: %s\n", buffer);
    send(sock1, &buffer, sizeof(buffer), 0);
    recv(sock1, &ack, sizeof(ack), 0);

    bzero(buffer, 1024);
    strcpy(buffer, destPath);
    printf("Sent: %s\n", buffer);
    send(sock1, &buffer, sizeof(buffer), 0);
    recv(sock1, &ack, sizeof(ack), 0);

    while (1)
    {
        char c;
        recv(sock1, &c, sizeof(c), 0);
        printf("Received: %c\n", c);

        send(sock2, &c, sizeof(c), 0);
        recv(sock2, &ack, sizeof(ack), 0); // till server2 doesnt receive c we dont go ahead

        ack = 1;
        send(sock1, &ack, sizeof(ack), 0); // send ack to sock1 only after receving ack from sock2

        if (c == 'F')
        {
            bzero(buffer, 1024);
            recv(sock1, buffer, sizeof(buffer), 0); // filename

            send(sock2, buffer, strlen(buffer), 0);
            recv(sock2, &ack, sizeof(ack), 0);

            ack = 1;
            send(sock1, &ack, sizeof(ack), 0); // filename sent ack sent to sock1

            // now actual file content
            while (1)
            {
                bzero(buffer, 1024);
                recv(sock1, buffer, sizeof(buffer), 0);
                send(sock2, buffer, strlen(buffer), 0);
                recv(sock2, &ack, sizeof(ack), 0);

                ack = 1;
                send(sock1, &ack, sizeof(ack), 0);

                printf("Received: %s\n", buffer);
                if (strcmp(buffer, "end") == 0)
                    break;
            }
        }
        else if (c == 'D')
        {
            bzero(buffer, 1024);
            recv(sock1, buffer, sizeof(buffer), 0);
            send(sock2, buffer, strlen(buffer), 0);
            recv(sock2, &ack, sizeof(ack), 0); // dirname
            printf("Dir name: %s\n", buffer);

            ack = 1;
            send(sock1, &ack, sizeof(ack), 0);
        }
        else if (c == 'E')
        {
            break;
        }
    }

    char ack_stop[10];
    recv(sock1, ack_stop, sizeof(ack_stop), 0);
    if (strcmp(ack_stop, "STOP") == 0)
        printf("STOP ack received\n");

    recv(sock2, ack_stop, sizeof(ack_stop), 0);
    if (strcmp(ack_stop, "STOP") == 0)
        printf("STOP ack received\n");

    close(sock1);
    close(sock2);
}

void copy_file_dir_nm_self(int ss_port, char *srcPath, char *destPath)
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

    char c = 's';
    send(sock, &c, sizeof(c), 0);

    char ack_start[10];
    recv(sock, ack_start, sizeof(ack_start), 0);
    if (strcmp(ack_start, "START") == 0)
        printf("START ack received\n");

    int ack;
    bzero(buffer, 1024);
    strcpy(buffer, srcPath);
    printf("Sent: %s\n", buffer);
    send(sock, &buffer, sizeof(buffer), 0);
    recv(sock, &ack, sizeof(ack), 0);

    bzero(buffer, 1024);
    strcpy(buffer, destPath);
    printf("Sent: %s\n", buffer);
    send(sock, &buffer, sizeof(buffer), 0);
    recv(sock, &ack, sizeof(ack), 0);

    char ack_stop[10];
    recv(sock, ack_stop, sizeof(ack_stop), 0);
    if (strcmp(ack_stop, "STOP") == 0)
        printf("STOP ack received\n");

    close(sock);
    printf("Disconnected from the storage server with port %d.\n", ss_port);
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
            char *response = "CLIENT COMMAND RECEIVED";
            send(ss_sock, response, sizeof(response), 0);

            printf("NM Server: %s\n", response);

            char ch;
            paths_src_dest struct_received;
            recv(ss_sock, &struct_received, sizeof(struct_received), 0);

            ch=struct_received.ch;
            

            int ss_port_client = 0; // if not found send 0
            int ss_port_nm = 0;

            // for ch=='c
            int ss1_client = 0;
            int ss2_client = 0;
            int ss1_nm = 0;
            int ss2_nm = 0;

            // if (!storage_servers_connected)
            // {
            //     printf(RED);
            //     printf("No storage servers connected to the NM server at the moment\n"); //define this error in search code
            //     printf(RST);
            // }

            char buffersend[1024];
            char path1[PATH_MAX];

            if (ch != 'c')
            {
                strcpy(path1,struct_received.path1);
                printf(CSTM2);
                printf("Path: %s\n\n", path1);
                printf(RST);
            }
            else
            {
                printf(CSTM2);
                printf("Paths: %s %s\n\n", struct_received.path1, struct_received.path2);
                printf(RST);
            }
            
            //searchhhhhh

            printf(GRN);
            printf("Storage servers found!\n"); // defie this error in error code
            printf(RST);
            ss_port_client = 1234;
            ss_port_nm = 1235;

            ss1_nm = 1234;
            ss2_nm = 1234; // 2345

            bzero(buffersend, 1024);
            if ((ss_port_nm != 0)&&ch!='c')
            {
                strcpy(buffersend, "Storage server found\n");
                send(ss_sock, buffersend, sizeof(buffersend), 0);
            }
            else if((ch=='c')&&(ss1_nm!=0)&&(ss2_nm!=0))
            {
                strcpy(buffersend, "Storage server found\n");
                send(ss_sock, buffersend, sizeof(buffersend), 0);
            }
            else
            {
                strcpy(buffersend, "Storage server not found\n");
                send(ss_sock, buffersend, sizeof(buffersend), 0);
                continue;
            }

            if (ss_port_nm != 0)
            {
                if ((ch == 'r') || (ch == 't') || (ch == 'w'))
                {
                    send(ss_sock, &ss_port_client, sizeof(ss_port_client), 0);
                    printf("Port sent to client for SS connection: %d\n", ss_port_client);
                    printf("\n");
                }
                else
                {
                    // receive paths and call functions

                    if ((ch == 'f') || (ch == 'd'))
                        create_file_dir(ss_port_nm, ch, path1); //"dir1/dir3/file.txt"
                    else if ((ch == 'F') || (ch == 'D'))
                        delete_file_dir(ss_port_nm, ch, path1); //"dir1/dir3/file.txt"
                    else if (ch == 'c')
                    {
                        if (ss1_nm == ss2_nm)
                            copy_file_dir_nm_self(ss1_nm, struct_received.path1, struct_received.path2);
                        else
                            copy_file_dir_nm(ss1_nm, ss2_nm, struct_received.path1, struct_received.path2);
                    }
                }
            }

            // create_file_dir(1235, 'f', "dir1/dir3/file.txt"); // the last 2 arguments will be user input and sent from client to nm server; get port from the trie/hashmap
            // printf("\n");

            // create_file_dir(1235, 'd', "dir1/dir2");
            // printf("\n");

            // delete_file_dir(1235,'F', "main_dir/dir1/file.txt");
            // printf("\n");

            // delete_file_dir(1235,'D', "dir1/dir2");
            // printf("\n");

            // copy_file_dir_nm(1235, "main_dir", "dest");
            // printf("\n");

            // copy_file_dir_nm_self(1235, "main_dir", "dest");
            // printf("\n");

            // copy_file_dir();
        }
    }

    return 0;
}
