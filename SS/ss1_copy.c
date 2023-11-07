#include "ss.h"

char *ip = "127.0.0.1";
int port = 5566;
int ack;

int sock;
struct sockaddr_in addr;
socklen_t addr_size;
char buffer[1024];
int n;

#define BUFFER_SIZE 1024
#define PATH_MAX 1024

void copyFile(const char *srcPath, const char *destPath)
{
    int src_fd, dest_fd;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    src_fd = open(srcPath, O_RDONLY);
    if (src_fd == -1)
    {
        perror("Failed to open source file");
        exit(1);
    }

    char c = 'F';
    printf("Sent: %c\n", c);
    send(sock, &c, sizeof(c), 0);

 
    recv(sock, &ack, sizeof(ack), 0);

    bzero(buffer, 1024);
    strcpy(buffer, destPath);
    send(sock, buffer, strlen(buffer), 0);
    printf("Sent: %s\n", buffer);

    recv(sock, &ack, sizeof(ack), 0);
  

    bzero(buffer, 1024);
    while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0)
    { // send to other server
        send(sock, buffer, strlen(buffer), 0);
        printf("Sent: %s\n", buffer);


        recv(sock, &ack, sizeof(ack), 0);
    }
    bzero(buffer, 1024);
    strcpy(buffer, "end");
    send(sock, buffer, strlen(buffer), 0);


    recv(sock, &ack, sizeof(ack), 0);

    close(src_fd);
    // close(dest_fd);
    usleep(10);
}

void copyDirectory(const char *srcPath, const char *destPath)
{
    // Create the destination directory if it doesn't exist // ask other server to do this
    char c = 'D';
    printf("Sent: %c\n", c);
    send(sock, &c, sizeof(c), 0);


    recv(sock, &ack, sizeof(ack), 0);

    bzero(buffer, 1024);
    strcpy(buffer, destPath);
    send(sock, buffer, strlen(buffer), 0);
    printf("Sent: %s\n", buffer);


    recv(sock, &ack, sizeof(ack), 0);
    // mkdir(destPath, S_IRWXU | S_IRWXG | S_IRWXO);

    // Traverse the source directory and copy its contents to the destination directory
    DIR *dir;
    struct dirent *entry;

    dir = opendir(srcPath);
    if (dir == NULL)
    {
        perror("Failed to open source directory");
        exit(1);
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        char srcFilePath[PATH_MAX];
        char destFilePath[PATH_MAX];

        snprintf(srcFilePath, PATH_MAX, "%s/%s", srcPath, entry->d_name);
        snprintf(destFilePath, PATH_MAX, "%s/%s", destPath, entry->d_name);

         if(entry->d_name[0]!='.')
        {
            if (entry->d_type == DT_REG)
            {
                copyFile(srcFilePath, destFilePath); // hidden files
            }
            else if (entry->d_type == DT_DIR)
            {
                copyDirectory(srcFilePath, destFilePath);
            }
        }
    }

    closedir(dir);
    usleep(10);
}

int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
        exit(1);
    }

    const char *srcPath = argv[1];
    const char *destPath = argv[2];

    struct stat srcStat;
    if (stat(srcPath, &srcStat) == -1)
    {
        perror("Failed to get source file/directory information");
        exit(1);
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = port;
    addr.sin_addr.s_addr = inet_addr(ip);

    connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    printf("Connected to the server.\n");

    if (S_ISREG(srcStat.st_mode) == 1)
    {
        copyFile(srcPath, destPath);
    }
    else if (S_ISDIR(srcStat.st_mode) == 1)
    {
        copyDirectory(srcPath, destPath);
    }
    else
    {
        fprintf(stderr, "Source is neither a regular file nor a directory.\n");
        exit(1);
    }
    char c='E';
    printf("Sent: %c\n", c);
    send(sock, &c, sizeof(c), 0);

    
    recv(sock, &ack, sizeof(ack), 0);

    close(sock);
    printf("Disconnected from the server.\n");

    printf("Copy completed.\n");
    return 0;

    return 0;
}