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
#include <stdbool.h>

#define PATH_MAX 1024
#define BUFFER_SIZE 1024
#define ALPHABET_SIZE 94

#define CHUNK_SIZE 10

typedef struct chunk
{
    int chunk_no; // total number of chunks in total
    char chunk_buffer[CHUNK_SIZE];
    uint32_t seq; // chunk no i have sent right now
                  // int ack; // the byte number that the receiver expects to receive next
} chunk;

#define MAX_LENGTH_ACC_PATHS_ONE_SS 100000
#define MAX_NO_PATHS 50
#define MAX_CLIENTS 20 // as mentioned in doubts doc



extern int storage_servers_connected;


int     directoryExists(char *path);
void    get_accessible_path_present(char *relativePath);

int create_file(char *relativePath);
int create_dirs(char *relativePath);

int delete_file(char *relativePath);
int delete_dir(char *relativePath);

int   ss1_copy(int sock);
void  copy_ss2(int client_sock);

int self_copy(char *srcPath, char *destPath);

char *file_details(char *filename);

int  read_file(int sock, char *path);
void write_file(int sock, char *path);



#define RST "\033[0m"
#define BLK "\033[30m"
#define RED "\033[31m"
#define GRN "\033[32m"
#define YEL "\033[33m"
#define BLU "\033[34m"
#define MAG "\033[35m"
#define CYN "\033[36m"
#define WHT "\033[37m"

// Custom color codes
#define CSTM1 "\033[38;5;200m"
#define CSTM2 "\033[38;5;220m"

struct TrieNode
{
    struct TrieNode *children[94];
    bool end;
    int endnum;
};



struct TrieNode*   createNode();
void               insertPath(struct TrieNode *root, const char *path, int s);
int                searchPath(struct TrieNode *root, const char *path);
bool               isEmpty(struct TrieNode *root);
void               deleteAllNodes(struct TrieNode *node);
void               deleteNodesStartingFrom(struct TrieNode *node);
struct TrieNode*   removee(struct TrieNode *root, const char *key, int depth);

struct store
{
    char *stringvalues[5];
    int ss_num[5];
    int index;
};

int check(const char *path, struct store *lru);

