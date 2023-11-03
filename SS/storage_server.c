#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>



#define MAX_LENGTH_ACC_PATHS_ONE_SS 100000

typedef struct send_nm_init{
    char accessible_paths[MAX_LENGTH_ACC_PATHS_ONE_SS];
    int port_nm;
    int port_client;
    char ip[40];  
}send_nm_init;



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
    printf("Disconnected from the server.\n");

    













    // int serverPort = 12345;  // Set your desired port number
    // int serverSocket;
    // struct sockaddr_in serverAddr;

    // // Create a socket
    // serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    // if (serverSocket == -1) {
    //     perror("Socket creation failed");
    //     exit(1);
    // }

    // serverAddr.sin_family = AF_INET;
    // serverAddr.sin_port = htons(serverPort);
    // serverAddr.sin_addr.s_addr = INADDR_ANY;

    // // Bind the socket to the address
    // if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
    //     perror("Binding failed");
    //     exit(1);
    // }

    // // Listen for incoming connections
    // if (listen(serverSocket, 10) == 0) {
    //     printf("Listening...\n");
    // } else {
    //     perror("Listening failed");
    //     exit(1);
    // }

    // // Get the server's IP address and port number
    // struct sockaddr_in myAddr;
    // socklen_t addrLen = sizeof(myAddr);
    // getsockname(serverSocket, (struct sockaddr*)&myAddr, &addrLen);

    // printf("Storage server is running on IP: %s, Port: %d\n", inet_ntoa(myAddr.sin_addr), ntohs(myAddr.sin_port));

    // // Handle incoming connections and data storage logic here

    // close(serverSocket);
    // return 0;
}

