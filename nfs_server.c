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

#define MAX_LENGTH_ACC_PATHS_ONE_SS 20000

typedef struct send_nm_init
{
    int port_nm;
    int port_client;
    char accessible_paths[MAX_LENGTH_ACC_PATHS_ONE_SS];
    char ip[40];
} send_nm_init;

typedef struct paths_src_dest
{
    char ch;
    char path1[PATH_MAX];
    char path2[PATH_MAX];
} paths_src_dest;

#define DEAD 0
#define ALIVE 1

#define READ_ONLY 2
#define ALL 3

typedef struct storage_server_data
{
    int nm_port;
    int client_port;
    int ss_index;
    int backup1_index;
    int backup2_index;
    int status;
    char paths[MAX_LENGTH_ACC_PATHS_ONE_SS];
} storage_server_data;

storage_server_data storage_server_array[67000];
struct TrieNode *ss_root;
struct store *lru;
int storage_servers_connected;

char *ip = "127.0.0.1";
int port = 5566; // SS KNOWS PORT NUMBER OF NM server

int server_sock;
struct sockaddr_in server_addr;
socklen_t addr_size;

pthread_t nm_thread[30];

int create_file_dir(int ss_port, char file_or_dir, char *path)
{

    char *ip = "127.0.0.1";

    int sock;
    struct sockaddr_in addr;
    char buffer[1024];
    int n;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("[-] 40: Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created to connect to SS with port number %d\n", ss_port);

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(ss_port);
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

    char ack_stop[1000];
    int flag_sucess = 0;
    bzero(ack_stop, 1000);
    recv(sock, ack_stop, sizeof(ack_stop), 0);
    printf(LIGHT_PINK);
    if (strcmp(ack_stop, "STOP") == 0)
    {
        printf("STOP ack received\n");
        flag_sucess = 1;
    }
    else
        printf("%s\n", ack_stop);
    printf(RST);

    close(sock);
    printf("Disconnected from the storage server with port %d.\n\n\n", ss_port);
    return flag_sucess;
}

int delete_file_dir(int ss_port, char file_or_dir, char *path)
{

    char *ip = "127.0.0.1";

    int sock;
    struct sockaddr_in addr;
    char buffer[1024];
    int n;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("[-] 40: Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created to connect to SS with port number %d\n", ss_port);

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(ss_port);
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

    char ack_stop[1000];
    int flag_sucess = 0;
    bzero(ack_stop, 1000);
    recv(sock, ack_stop, sizeof(ack_stop), 0);

    printf(LIGHT_PINK);
    if (strcmp(ack_stop, "STOP") == 0)
    {
        printf("STOP ack received\n");
        flag_sucess = 1;
    }
    else
        printf("%s\n", ack_stop);
    printf(RST);

    close(sock);
    printf("Disconnected from the storage server with port %d.\n\n\n", ss_port);
    return flag_sucess;
}
void copy_file_dir_one(int ss_port, int port2, char *srcPath, char *destPath)
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
    printf("%d %d\n", port1, port2);

    int sock1; // copy from
    struct sockaddr_in addr1;
    socklen_t addr_size;
    char buffer[1024];
    int n;
    sock1 = socket(AF_INET, SOCK_STREAM, 0);
    if (sock1 < 0)
    {
        perror("[-] 40: Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    memset(&addr1, '\0', sizeof(addr1));
    addr1.sin_family = AF_INET;
    addr1.sin_port = htons(port1);
    addr1.sin_addr.s_addr = inet_addr(ip);

    connect(sock1, (struct sockaddr *)&addr1, sizeof(addr1));
    printf("Connected to the storage server 1.\n");

    // int port2 = 2345; // copy 2

    bzero(buffer, 1024);
    strcpy(buffer, "HELLO, THIS IS NM SERVER.");
    printf("NM server: %s\n", buffer);
    send(sock1, buffer, strlen(buffer), 0);

    bzero(buffer, 1024);
    recv(sock1, buffer, sizeof(buffer), 0);
    printf("SS server: %s\n", buffer);

    char c = 'p';
    send(sock1, (char *)&c, sizeof(c), 0);

    char ack_start[10];

    if (recv(sock1, ack_start, sizeof(ack_start), 0) < 0)
        printf("receive start ack error\n");
    else
        printf("START ack received\n");

    bzero(buffer, 1024);
    strcpy(buffer, srcPath);
    printf("Sent: %s\n", buffer);
    send(sock1, (char **)&buffer, sizeof(buffer), 0);
    recv(sock1, &ack, sizeof(ack), 0);
    // else
    //     printf("%s\n",ack_start);

    // if (strcmp(ack_start, "START") == 0)
    //     printf("START ack received\n");
    int sock2;
    struct sockaddr_in addr2;

    memset(&addr2, '\0', sizeof(addr2));
    addr2.sin_family = AF_INET;
    addr2.sin_port = htons(port2);
    addr2.sin_addr.s_addr = inet_addr(ip);

    sock2 = socket(AF_INET, SOCK_STREAM, 0);
    if (sock2 < 0)
    {
        perror("[-] 40: Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    connect(sock2, (struct sockaddr *)&addr2, sizeof(addr2));
    printf("Connected to the storage server 2.\n");

    bzero(buffer, 1024);
    strcpy(buffer, "HELLO, THIS IS NM SERVER.");
    printf("NM server: %s\n", buffer);
    send(sock2, buffer, strlen(buffer), 0);

    bzero(buffer, 1024);
    recv(sock2, buffer, sizeof(buffer), 0);
    printf("SS server: %s\n", buffer);

    c = 'c';
    send(sock2, (char *)&c, sizeof(c), 0);

    if (recv(sock2, ack_start, sizeof(ack_start), 0) < 0)
        printf("receive start ack error\n");
    else
        printf("START ack received\n");

    // start

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

            printf("File_name: %s\n", buffer);

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
            recv(sock1, buffer, sizeof(buffer), 0); // received dir path
            send(sock2, buffer, strlen(buffer), 0); // sent path
            recv(sock2, &ack, sizeof(ack), 0);      // dirname
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
        printf("STOP ack received\n\n\n");

    recv(sock2, ack_stop, sizeof(ack_stop), 0);
    if (strcmp(ack_stop, "STOP") == 0)
        printf("STOP ack received\n\n\n");

    close(sock1);
    close(sock2);
}
void copy_file_dir_nm(int ss_port, int port2, char *srcPath, char *destPath, int ss2_idx)
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
    printf("%d %d\n", port1, port2);

    int sock1; // copy from
    struct sockaddr_in addr1;
    socklen_t addr_size;
    char buffer[1024];
    int n;
    sock1 = socket(AF_INET, SOCK_STREAM, 0);
    if (sock1 < 0)
    {
        perror("[-] 40: Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    memset(&addr1, '\0', sizeof(addr1));
    addr1.sin_family = AF_INET;
    addr1.sin_port = htons(port1);
    addr1.sin_addr.s_addr = inet_addr(ip);

    connect(sock1, (struct sockaddr *)&addr1, sizeof(addr1));
    printf("Connected to the storage server 1.\n");

    // int port2 = 2345; // copy 2

    bzero(buffer, 1024);
    strcpy(buffer, "HELLO, THIS IS NM SERVER.");
    printf("NM server: %s\n", buffer);
    send(sock1, buffer, strlen(buffer), 0);

    bzero(buffer, 1024);
    recv(sock1, buffer, sizeof(buffer), 0);
    printf("SS server: %s\n", buffer);

    char c = 'p';
    send(sock1, (char *)&c, sizeof(c), 0);

    char ack_start[10];

    if (recv(sock1, ack_start, sizeof(ack_start), 0) < 0)
        printf("receive start ack error\n");
    else
        printf("START ack received\n");

    bzero(buffer, 1024);
    strcpy(buffer, srcPath);
    printf("Sent: %s\n", buffer);
    send(sock1, (char **)&buffer, sizeof(buffer), 0);
    recv(sock1, &ack, sizeof(ack), 0);
    // else
    //     printf("%s\n",ack_start);

    // if (strcmp(ack_start, "START") == 0)
    //     printf("START ack received\n");
    int sock2;
    struct sockaddr_in addr2;

    memset(&addr2, '\0', sizeof(addr2));
    addr2.sin_family = AF_INET;
    addr2.sin_port = htons(port2);
    addr2.sin_addr.s_addr = inet_addr(ip);

    sock2 = socket(AF_INET, SOCK_STREAM, 0);
    if (sock2 < 0)
    {
        perror("[-] 40: Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    connect(sock2, (struct sockaddr *)&addr2, sizeof(addr2));
    printf("Connected to the storage server 2.\n");

    bzero(buffer, 1024);
    strcpy(buffer, "HELLO, THIS IS NM SERVER.");
    printf("NM server: %s\n", buffer);
    send(sock2, buffer, strlen(buffer), 0);

    bzero(buffer, 1024);
    recv(sock2, buffer, sizeof(buffer), 0);
    printf("SS server: %s\n", buffer);

    c = 'c';
    send(sock2, (char *)&c, sizeof(c), 0);

    if (recv(sock2, ack_start, sizeof(ack_start), 0) < 0)
        printf("receive start ack error\n");
    else
        printf("START ack received\n");

    // start

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

            printf("File_name: %s\n", buffer);
            int check_if_exists = searchPath(ss_root, buffer); // search first path in accessible paths

            if (check_if_exists < 0)
                insertPath(ss_root, buffer, ss2_idx);

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
            recv(sock1, buffer, sizeof(buffer), 0); // received dir path
            send(sock2, buffer, strlen(buffer), 0); // sent path
            recv(sock2, &ack, sizeof(ack), 0);      // dirname
            printf("Dir name: %s\n", buffer);

            int check_if_exists = searchPath(ss_root, buffer); // search first path in accessible paths

            if (check_if_exists < 0)
                insertPath(ss_root, buffer, ss2_idx);

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
        printf("STOP ack received\n\n\n");

    recv(sock2, ack_stop, sizeof(ack_stop), 0);
    if (strcmp(ack_stop, "STOP") == 0)
        printf("STOP ack received\n\n\n");

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
        perror("[-] 40: Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created to connect to SS with port number %d\n", ss_port);

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(ss_port);
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
    printf("Disconnected from the storage server with port %d.\n\n\n", ss_port);
}

char **tokenize_paths(char *acc_paths_string)
{

    int no_of_tokens = 0;
    int tokens_array_size = MAX_NO_PATHS;                       // initial assumption for no of tokens
    char **tokens = malloc(tokens_array_size * sizeof(char *)); // array of tokens

    char *delimiters = "\n";
    char *token = strtok(acc_paths_string, delimiters);

    while (token != NULL)
    {
        tokens[no_of_tokens] = token;
        no_of_tokens++;

        if (no_of_tokens >= tokens_array_size)
        {
            tokens_array_size = (int)(tokens_array_size * 2);
            tokens = realloc(tokens, tokens_array_size * sizeof(char *));
        }

        token = strtok(NULL, delimiters);
    }

    tokens[no_of_tokens] = NULL;
    return tokens;
}

void store_in_array(int index, int port_nm, int port_client, int backup1_idx, int backup2_idx) // should ideally do 67000 se mod
{
    storage_server_array[index].ss_index = index; // made array 1 indexed
    storage_server_array[index].client_port = port_client;
    storage_server_array[index].nm_port = port_nm;
    storage_server_array[index].backup1_index = backup1_idx;
    storage_server_array[index].backup2_index = backup2_idx;
    storage_server_array[index].status = ALIVE;
    return;
}

void getPreviousDirectory(const char *path, char *previousDir)
{
    // Find the last occurrence of '/'
    const char *lastSlash = strrchr(path, '/');

    if (lastSlash != NULL)
    {
        // Calculate the length of the previous directory
        size_t length = lastSlash - path;

        // Copy the previous directory to the output buffer
        strncpy(previousDir, path, length);
        previousDir[length] = '\0'; // Null-terminate the string
    }
    else
    {
        // No '/' found, the input is likely just a file name
        strcpy(previousDir, ".");
    }
}

void find_ports(int ss_idx, int *permission, int *ss_port_nm)
{

    if (ss_idx > 0)
    {
        printf(GRN);
        printf("Storage server found!\n"); // defie this error in error code
        printf(RST);

        if (storage_server_array[ss_idx].status == ALIVE)
        {

            *ss_port_nm = storage_server_array[ss_idx].nm_port;
        }
        else if (storage_server_array[ss_idx].backup1_index > 0)
        {
            if (storage_server_array[storage_server_array[ss_idx].backup1_index].status == ALIVE)
            {

                *ss_port_nm = storage_server_array[storage_server_array[ss_idx].backup1_index].nm_port;

                *permission = READ_ONLY;
            }
        }
        else if (storage_server_array[ss_idx].backup2_index > 0)
        {
            if (storage_server_array[storage_server_array[ss_idx].backup2_index].status == ALIVE)
            {

                *ss_port_nm = storage_server_array[storage_server_array[ss_idx].backup2_index].nm_port;
                *permission = READ_ONLY;
            }
        }
        else
        {
            printf(RED);
            printf("Storage Server is Down. There are no backups.\n");
            printf(RST);
        }
    }
    return;
}

void copy_one_ss_into_another(int src_idx, int dest_idx)
{

    int i = 0;
    int src_port = storage_server_array[src_idx].nm_port;
    int dest_port = storage_server_array[dest_idx].nm_port;

    printf("%d %d\n", src_port, dest_port); // till here perfection

    printf("%s\n", storage_server_array[src_idx].paths);

    char **accessible_paths_individual = tokenize_paths(storage_server_array[src_idx].paths);

    while (accessible_paths_individual[i] != NULL)
    {
        printf("%s\n", accessible_paths_individual[i]);
        copy_file_dir_one(src_port, dest_port, accessible_paths_individual[i], accessible_paths_individual[i]); // diff function slightly as we dont need to insert into trie
        i++;
    }
}

void *ping_ss(void *arg)
{

    int index = *((int *)arg);
    int ss_port = storage_server_array[index].nm_port;
    while (1)
    {

        char *ip = "127.0.0.1";

        int sock;
        struct sockaddr_in addr;
        char buffer[1024];
        int n;

        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
        {
            perror("[-] 40: Socket error");
            exit(1);
        }

        memset(&addr, '\0', sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(ss_port);
        addr.sin_addr.s_addr = inet_addr(ip);

        if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            printf("\n\n[-] Connect error SS server with port %d\n\n", ss_port);
            storage_server_array[index].status = DEAD;
            close(sock);
            continue; // move to the next iteration if connection fails
        }

        bzero(buffer, 1024);
        strcpy(buffer, "HELLO, THIS IS NM SERVER (PING).");
        send(sock, buffer, strlen(buffer), 0);

        fd_set readfds;
        struct timeval timeout;

        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);

        timeout.tv_sec = 10; // 10 second timeout
        timeout.tv_usec = 0;

        if (select(sock + 1, &readfds, NULL, NULL, &timeout) == 1)
        {
            // Data available for reading
            bzero(buffer, 1024);
            recv(sock, buffer, sizeof(buffer), 0);
            // printf("SS server with port %d: %s\n", ss_port, buffer);
            storage_server_array[index].status = ALIVE;
        }
        else
        {
            // Timeout reached, handle as needed
            printf("\n\nTimeout reached while waiting for response from SS server with port %d\n\n\n", ss_port);
            storage_server_array[index].status = DEAD;
        }

        close(sock);

        sleep(8);
    }

    return NULL;
}

int main()
{
    ss_root = createNode(); // to store servers
    lru = (struct store *)malloc(sizeof(struct store));
    lru->index = 0;
    for (int i = 0; i < 5; i++)
    {
        // bzero(lru->stringvalues[i],1024);
        lru->ss_num[i] = 0;
    }

    char buffer[1024];
    int n;
    storage_servers_connected = 0;

    printf("Initializing Naming Server..\n\n");

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        perror("[-] 40: Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created\n");

    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    n = bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (n < 0)
    {
        perror("[-] 41: Bind error");
        exit(1);
    }
    printf("[+]Bind to the port number: %d\n", port);

    // Listen on the socket,
    // with 100 max connection
    // requests queued

    if (listen(server_sock, 80) == 0) // 30 ss, 50 clients?
        printf("Listening...\n");
    else
        printf("42: Listening Error\n");

    int i = 0;

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

            printf("[+]New Storage Server discovered.\n");

            send_nm_init struct_received;

            int check = recv(ss_sock, &struct_received, sizeof(struct_received) + 1024, 0);

            printf("Struct Received!\n");
            printf("Accessible Paths:\n%s", struct_received.accessible_paths);

            printf("IP port: %s\n", "127.0.0.1");
            printf("Port for NM connection: %d\n", struct_received.port_nm);
            printf("Port for client interaction: %d\n", struct_received.port_client);

            if (check >= 0) // struct received
            {
                bzero(buffer, 1024);
                strcpy(buffer, "HI, THIS IS NM SERVER! SS HAS BEEN RECOGNIZED.");
                printf("NM server sending message: %s ..\n", buffer);
                send(ss_sock, buffer, strlen(buffer), 0);
            }
            else
            {
                printf("[-] 43: Receiving Error.\n");
            }

            close(ss_sock);
            printf("[+]SS disconnected.\n\n");

            // store new ss data ... in such a way that search is easy
            // Efficient Search: Optimize the search process employed by the Naming Server when serving client requests. Avoid linear searches and explore more efficient data structures such as Tries and Hashmaps to swiftly identify the correct Storage Server (SS) for a given request. This optimization enhances response times, especially in systems with a large number of files and folders.
            // LRU Caching: Implement LRU (Least Recently Used) caching for recent searches. By caching recently accessed information, the NM can expedite subsequent requests for the same data, further improving response times and system efficiency.

            // need to get

            char temp_paths[MAX_LENGTH_ACC_PATHS_ONE_SS];
            strcpy(temp_paths, struct_received.accessible_paths); // as we are doing strtok but need to copy accessible paths  string into array struct

            char **accessible_paths_individual = tokenize_paths(struct_received.accessible_paths);
            int ss_num = searchPath(ss_root, accessible_paths_individual[0]); // search first path in accessible paths

            if (ss_num > 0 && struct_received.port_nm == storage_server_array[ss_num].nm_port)
            {
                printf("Storage Server %d is back up!", ss_num); // this is only possible when server comes back up
                storage_server_array[ss_num].status = ALIVE;
            }
            else // new server so add paths to trie
            {
                // if more than 2 ss then store paths in them and send their index
                store_in_array(storage_servers_connected, struct_received.port_nm, struct_received.port_client, -1, -1);
                strcpy(storage_server_array[storage_servers_connected].paths, temp_paths);

                int i = 0;
                while (accessible_paths_individual[i] != NULL)
                {
                    insertPath(ss_root, accessible_paths_individual[i], storage_servers_connected);
                    // printf("ss_num %d path %s port %d\n",searchPath(ss_root,accessible_paths_individual[i]),accessible_paths_individual[i],storage_server_array[searchPath(ss_root,accessible_paths_individual[i])].nm_port);
                    i++;
                }
            }

            // if (storage_servers_connected == 3)
            // {
            //     copy_one_ss_into_another(1, 2);
            // }
            pthread_create(&nm_thread[i++], NULL, &ping_ss, &storage_servers_connected);
        }
        else if (ss_or_client == 3)
        {
            storage_servers_connected++;

            printf("[+]Change discovered in already connected storage servers.\n");

            send_nm_init struct_received;
            int check = recv(ss_sock, &struct_received, sizeof(struct_received), 0);
            printf("Struct Received!\n");
            printf("Accessible Paths:\n%s", struct_received.accessible_paths);
            printf("IP port: %s\n", "127.0.0.1");
            printf("Port for NM connection: %d\n", struct_received.port_nm);
            printf("Port for client interaction: %d\n", struct_received.port_client);

            if (check >= 0) // struct received
            {
                bzero(buffer, 1024);
                strcpy(buffer, "THIS IS NM SERVER, SS META DATA RECEIVED.");
                printf("NM server sending message: %s ..\n", buffer);
                send(ss_sock, buffer, strlen(buffer), 0);
            }
            else
            {
                printf("[-] 43: Receiving Error.\n");
            }

            close(ss_sock);
            printf("[+]SS disconnected.\n\n");
        }
        else if (ss_or_client == 2) // it is a client
        {
            // for each client we have to have a different thread
            char *response = "CLIENT COMMAND RECEIVED";
            send(ss_sock, response, sizeof(response), 0);

            printf("\nNM Server: %s\n", response);

            char ch;
            paths_src_dest struct_received;
            recv(ss_sock, &struct_received, sizeof(struct_received), 0);

            ch = struct_received.ch;

            int ss_port_client = 0; // if not found send 0
            int ss_port_nm = 0;

            // for ch=='c
            int ss1_client = 0;
            int ss2_client = 0;
            int ss1_nm = 0;
            int ss2_nm = 0;

            char buffersend[1024];
            char path1[PATH_MAX];

            int ss_num1;
            int ss_num2;

            // int checksearch = searchPath(ss_root, "main");
            // printf("%d\n", checksearch);
            // checksearch = searchPath(ss_root, "main/dir1");
            // printf("%d\n", checksearch);
            // checksearch = searchPath(ss_root, "main/dir1/file");
            // printf("%d\n", checksearch);

            if (ch != 'c')
            {
                strcpy(path1, struct_received.path1);
                printf(CSTM2);
                printf("Path received: %s\n", path1);
                printf(RST);
                int check13 = 0;
                check13 = check(path1, lru);
                if ((ch != 'f') && (ch != 'd'))
                {

                    if (check13 == 0)
                    {
                        printf("Cache Miss\n");
                        ss_num1 = searchPath(ss_root, path1);
                        insert(path1, lru, ss_num1);
                    }
                    else
                    {
                        ss_num1 = check13;
                        printf("Cache Hit\n");
                    }
                    if (ch == 'F' || ch == 'D')
                    {
                        deletee(path1, lru);
                    }
                }
                else
                {
                    char previousDir[PATH_MAX]; // Adjust the buffer size as needed
                    getPreviousDirectory(path1, previousDir);
                    printf(YEL);
                    printf("Path to search: %s\n", previousDir);
                    printf(RST);

                    check13 = 0;
                    check13 = check(previousDir, lru);
                    if (check13 == 0)
                    {
                        printf("Cache Miss\n");
                        ss_num1 = searchPath(ss_root, previousDir);
                        insert(previousDir, lru, ss_num1);
                    }
                    else
                    {
                        ss_num1 = check13;
                        printf("Cache Hit\n");
                    }
                }
            }
            else
            {
                printf(CSTM2);
                printf("Paths: %s %s\n\n", struct_received.path1, struct_received.path2);
                printf(RST);
                ss_num1 = searchPath(ss_root, struct_received.path1); // can also do whatever i do for f and d
                ss_num2 = searchPath(ss_root, struct_received.path2);
            }

            // searchhhhhh
            int permission = ALL;
            if (ch != 'c')
            {
                if (ss_num1 > 0)
                {
                    printf(GRN);
                    printf("Storage servers found!\n"); // defie this error in error code
                    printf(RST);

                    if (storage_server_array[ss_num1].status == ALIVE)
                    {
                        ss_port_client = storage_server_array[ss_num1].client_port;
                        ss_port_nm = storage_server_array[ss_num1].nm_port;
                    }
                    else if (storage_server_array[ss_num1].backup1_index > 0)
                    {
                        if (storage_server_array[storage_server_array[ss_num1].backup1_index].status == ALIVE)
                        {
                            ss_port_client = storage_server_array[storage_server_array[ss_num1].backup1_index].client_port;
                            ss_port_nm = storage_server_array[storage_server_array[ss_num1].backup1_index].nm_port;

                            permission = READ_ONLY;
                        }
                    }
                    else if (storage_server_array[ss_num1].backup2_index > 0)
                    {
                        if (storage_server_array[storage_server_array[ss_num1].backup2_index].status == ALIVE)
                        {
                            ss_port_client = storage_server_array[storage_server_array[ss_num1].backup2_index].client_port;
                            ss_port_nm = storage_server_array[storage_server_array[ss_num1].backup2_index].nm_port;
                            permission = READ_ONLY;
                        }
                    }
                    else
                    {
                        printf(RED);
                        printf("Storage Server is Down. There are no backups.\n");
                        printf(RST);
                    }
                }
            }
            else
            {
                find_ports(ss_num1, &permission, &ss1_nm); // nm means nm server, num is ss number
                find_ports(ss_num2, &permission, &ss2_nm);
            }

            // ss1_nm = 1235;
            // ss2_nm = 2346; // 2345
            // printf("%d %d\n",ss1_nm,ss2_nm);

            bzero(buffersend, 1024);


            if ((permission == READ_ONLY) && (ch != 'r') && (ch != 't')) 
            {
                printf(RED);
                printf("Storage server found but Read Only Path\n");
                printf(RST);
                strcpy(buffersend, "Storage server found but Read Only Path\n");
                send(ss_sock, buffersend, sizeof(buffersend), 0);
                continue;
            }
            if ((ss_port_nm != 0) && ch != 'c')
            {
                strcpy(buffersend, "Storage server found\n");
                send(ss_sock, buffersend, sizeof(buffersend), 0);
            }
            else if ((ch == 'c') && (ss1_nm != 0) && (ss2_nm != 0))
            {
                strcpy(buffersend, "Storage server found\n");
                send(ss_sock, buffersend, sizeof(buffersend), 0);
            }
            else
            {
                printf(RED);
                printf("Storage server not found\n");
                printf(RST);
                strcpy(buffersend, "Storage server not found\n");
                send(ss_sock, buffersend, sizeof(buffersend), 0);
                continue;
            }

            if (((ss_port_nm != 0) && (ch != 'c')) || ((ch == 'c') && (ss1_nm != 0) && (ss2_nm != 0)))
            {
                if ((ch == 'r') || (ch == 't') || (ch == 'w')) // no change to file structure!! this is why nm didnt need stop ack
                {
                    send(ss_sock, &ss_port_client, sizeof(ss_port_client), 0);
                    printf("Port sent to client for SS connection: %d\n", ss_port_client);
                    printf("\n");
                }
                else // change file structureeeee
                {
                    // receive paths and call functions

                    if ((ch == 'f') || (ch == 'd'))
                    {
                        printf("Creating...\n");
                        if (create_file_dir(ss_port_nm, ch, path1))
                            insertPath(ss_root, path1, ss_num1);
                    }
                    else if ((ch == 'F') || (ch == 'D'))
                    {
                        if (delete_file_dir(ss_port_nm, ch, path1))
                            removee(ss_root, path1, 0);
                    }
                    else if (ch == 'c')
                    {
                        // printf("hi");
                        if (ss1_nm == ss2_nm)
                        {
                            copy_file_dir_nm_self(ss1_nm, struct_received.path1, struct_received.path2);
                        }
                        else
                        {
                            copy_file_dir_nm(ss1_nm, ss2_nm, struct_received.path1, struct_received.path2, ss_num2);
                        }
                    }
                }
            }
        }
    }
    for (int i = 1; i <= storage_servers_connected; i++)
        pthread_join(nm_thread[i], NULL);

    return 0;
}
