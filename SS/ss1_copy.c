#include "ss1.h"

char *ip = "127.0.0.1";
int port = 2344;
int ack;

int client_sock, server_sock;
struct sockaddr_in client_addr, server_addr;
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
    send(client_sock, &c, sizeof(c), 0);

    recv(client_sock, &ack, sizeof(ack), 0);

    bzero(buffer, 1024);
    strcpy(buffer, destPath);
    send(client_sock, buffer, strlen(buffer), 0);
    printf("Sent: %s\n", buffer);

    recv(client_sock, &ack, sizeof(ack), 0);

    bzero(buffer, 1024);
    while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0)
    { // send to other server
        send(client_sock, buffer, strlen(buffer), 0);
        printf("Sent: %s\n", buffer);

        recv(client_sock, &ack, sizeof(ack), 0);
    }
    bzero(buffer, 1024);
    strcpy(buffer, "end");
    send(client_sock, buffer, strlen(buffer), 0);

    recv(client_sock, &ack, sizeof(ack), 0);

    close(src_fd);
    // close(dest_fd);
    usleep(10);
}

void copyDirectory(const char *srcPath, const char *destPath)
{
    // Create the destination directory if it doesn't exist // ask other server to do this
    char c = 'D';
    printf("Sent: %c\n", c);
    send(client_sock, &c, sizeof(c), 0);

    recv(client_sock, &ack, sizeof(ack), 0);

    bzero(buffer, 1024);
    strcpy(buffer, destPath);
    send(client_sock, buffer, strlen(buffer), 0);
    printf("Sent: %s\n", buffer);

    recv(client_sock, &ack, sizeof(ack), 0);
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

        if (entry->d_name[0] != '.')
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

int ss1_copy(int sock)
{

        client_sock=sock;
        char srcPath[PATH_MAX];
        char destPath[PATH_MAX];

        recv(client_sock, srcPath, sizeof(srcPath), 0);
        ack = 1;
        send(client_sock, &ack, sizeof(ack), 0);

        recv(client_sock, destPath, sizeof(destPath), 0);
        ack = 1;
        send(client_sock, &ack, sizeof(ack), 0);

        struct stat srcStat;
        printf("%s\n",srcPath);
        if (stat(srcPath, &srcStat) == -1)
        {
            perror("Failed to get source file/directory information");
            exit(1);
        }

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
        char c = 'E';
        printf("Sent: %c\n", c);
        send(client_sock, &c, sizeof(c), 0);

        recv(client_sock, &ack, sizeof(ack), 0);

        close(client_sock);
        printf("Disconnected from the server.\n");

        printf("Copy completed.\n");
  
    return 0;
}

