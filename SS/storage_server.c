#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>



#define MAX_LENGTH_ACC_PATHS_ONE_SS 100000

typedef struct send_nm_init{
    char accessible_paths[MAX_LENGTH_ACC_PATHS_ONE_SS];
    int port_nm;
    int port_client;
    char ip[40];  
}send_nm_init;

void *nm_commands()
{
    char *ip = "127.0.0.1";
    int port = 1235;

    int ss_server_sock, nm_client_sock;
    struct sockaddr_in ss_server_addr, nm_client_addr;
    socklen_t addr_size;
    char buffer[1024];
    int n;

    ss_server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (ss_server_sock < 0){
        perror("[-]TCP Socket (for receiving NM commands) creation error");
        exit(1);
    }
    printf("[+]TCP server socket (for receiving NM commands) created.\n");

    memset(&ss_server_addr, '\0', sizeof(ss_server_addr));
    ss_server_addr.sin_family = AF_INET;
    ss_server_addr.sin_port = port;
    ss_server_addr.sin_addr.s_addr = inet_addr(ip);

    n = bind(ss_server_sock, (struct sockaddr*)&ss_server_addr, sizeof(ss_server_addr));
    if (n < 0){
        perror("[-]Bind socket (for receiving NM commands) error");
        exit(1);
    }
    printf("[+]Bind to the port number: %d (for receiving NM commands) \n", port);

    listen(ss_server_sock, 5);
    printf("Listening for NM server...\n");

    while(1){
        addr_size = sizeof(nm_client_addr);
        nm_client_sock = accept(ss_server_sock, (struct sockaddr*)&nm_client_addr, &addr_size);
        printf("[+]NM server connected.\n");

        bzero(buffer, 1024);
        recv(nm_client_sock, buffer, sizeof(buffer), 0);
        printf("Client: %s\n", buffer);

        bzero(buffer, 1024);
        strcpy(buffer, "HI, THIS IS SERVER. HAVE A NICE DAY!!!");
        printf("Server: %s\n", buffer);
        send(nm_client_sock, buffer, strlen(buffer), 0);

        close(nm_client_sock);
        printf("[+]Client disconnected.\n\n");

    }
    return NULL;
}

void *client_interactions()
{
    return NULL;
}


int main() {

    printf("Initializing Storage Server\n");

    //taking input of accessible paths
    
    char accessible_paths[MAX_LENGTH_ACC_PATHS_ONE_SS];

    printf("Enter (line separated) accessible paths for this stoarage server:\n") ;
    char temp[MAX_LENGTH_ACC_PATHS_ONE_SS];
   
     while (1) {
        // Read a line from the user
        if (fgets(temp, sizeof(temp), stdin) == NULL) {
            // Error or end of input
            break;
        }

        // append the line to accessible_paths 
        strncat(accessible_paths, temp, sizeof(accessible_paths) - strlen(accessible_paths) - 1);

        // check if the user pressed Enter without typing anything (empty line to finish)
        if (strcmp(temp, "\n") == 0) {
            break;
        }
    }

    // remove the trailing newline character, if present
    size_t len = strlen(accessible_paths);
    if (len > 0 && accessible_paths[len - 1] == '\n') {
        accessible_paths[len - 1] = '\0';
    }

    printf("You entered the following accessible paths:\n%s\n", accessible_paths); 

    //done taking user input for accessible paths

    // send struct to nm server

    char *ip = "127.0.0.1";
    int port = 5566;

    int sock;
    struct sockaddr_in addr;
    socklen_t addr_size;
    char buffer[1024];
    int n;
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0){
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created for connecting to Naming Server..\n");

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = port;
    addr.sin_addr.s_addr = inet_addr(ip);

    connect(sock, (struct sockaddr*)&addr, sizeof(addr));
    printf("Connected to the NM server.\n");

    // send to nfs server that it is a storage server 
    int ss_or_client=1;
    send(sock, &ss_or_client, sizeof(ss_or_client), 0);

    
    send_nm_init struct_to_send;

    strcpy(struct_to_send.accessible_paths,accessible_paths);
    strcpy(struct_to_send.ip,ip);
    struct_to_send.port_client=1234; //HOW TO GIVE DIFF PORTS TO EACH SS???
    struct_to_send.port_nm=1235;

    send(sock, &struct_to_send, sizeof(struct_to_send), 0);

    bzero(buffer, 1024);
    recv(sock, buffer, sizeof(buffer), 0);
    printf("Server: %s\n", buffer);

    close(sock);
    printf("Disconnected from the NM server.\n");

    
    pthread_t connection_for_nm_commands;
    pthread_create(&connection_for_nm_commands,NULL,&nm_commands,NULL);

    pthread_t connection_for_client_interactions;
    pthread_create(&connection_for_client_interactions,NULL,&client_interactions,NULL);


    pthread_join(connection_for_nm_commands,NULL);
    pthread_join(connection_for_client_interactions,NULL);
    return 0;
}

