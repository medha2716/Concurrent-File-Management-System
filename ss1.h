#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/types.h>



#define PATH_MAX 1024
#define BUFFER_SIZE 1024

#define CHUNK_SIZE 10

typedef struct chunk
{
    int chunk_no; // total number of chunks in total
    char chunk_buffer[CHUNK_SIZE];
    uint32_t seq; // chunk no i have sent right now
                  // int ack; // the byte number that the receiver expects to receive next
} chunk;

int directoryExists( char *path);
void get_accessible_path_present( char *relativePath);

int create_file(char* relativePath);
int create_dirs(char *relativePath);

int delete_file(char *relativePath);
int delete_dir(char *relativePath);

int ss1_copy(int sock);
void copy_ss2(int client_sock);

int self_copy(char *srcPath, char *destPath);

char* file_details(char *filename);

int read_file(int sock, char* path);
void write_file(int sock,char* path);

#define MAX_LENGTH_ACC_PATHS_ONE_SS 100000

#define MAX_CLIENTS 20 // as mentioned in doubts doc

