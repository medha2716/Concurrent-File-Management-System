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

int directoryExists( char *path);
void get_accessible_path_present( char *relativePath);

int create_file(char* relativePath);
int create_dirs(char *relativePath);

int delete_file(char *relativePath);
int delete_dir(char *relativePath);

int ss1_copy(int sock);

#define MAX_LENGTH_ACC_PATHS_ONE_SS 100000

#define MAX_CLIENTS 50

