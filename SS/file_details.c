
#include "ss1.h"
#include <pwd.h>  // For getpwuid()
#include <time.h> //localtime
#include <grp.h>  //to get group

// Regular text
#define BLK "\e[0;30m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YEL "\e[0;35m"
#define BLU "\e[0;34m"
#define MAG "\e[0;33m"
#define CYN "\e[0;36m"
#define WHT "\e[0;37m"
#define GREY "\x1B[90m"
#define COL_RESET "\x1B[0m"

#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <errno.h>

char *getFormattedFileInfo(const struct stat *file_stat, struct passwd *owner, struct group *group)
{
    static char info_str[1024]; // Adjust the size as needed

    char permissions[11];
    snprintf(permissions, sizeof(permissions), "%s%s%s%s%s%s%s%s%s%s",
             (S_ISDIR(file_stat->st_mode)) ? "d" : "-",
             (file_stat->st_mode & S_IRUSR) ? "r" : "-",
             (file_stat->st_mode & S_IWUSR) ? "w" : "-",
             (file_stat->st_mode & S_IXUSR) ? "x" : "-",
             (file_stat->st_mode & S_IRGRP) ? "r" : "-",
             (file_stat->st_mode & S_IWGRP) ? "w" : "-",
             (file_stat->st_mode & S_IXGRP) ? "x" : "-",
             (file_stat->st_mode & S_IROTH) ? "r" : "-",
             (file_stat->st_mode & S_IWOTH) ? "w" : "-",
             (file_stat->st_mode & S_IXOTH) ? "x" : "-");

    struct tm *tm_info;
    char time_buf[30];
    tm_info = localtime(&(file_stat->st_mtime));
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", tm_info);

    snprintf(info_str, sizeof(info_str), "\033[1;34m%s\033[0m \033[1;35m%lu\033[0m \033[1;36m%s\033[0m \033[1;32m%6ld\033[0m \033[1;31m%s\033[0m",
             permissions, (unsigned long)file_stat->st_nlink, owner->pw_name, (long)file_stat->st_size, time_buf);

    return info_str;
}

char *file_details(char *filename)
{
    // const char *filename = "dir1/";  // Replace with the path to your file

    struct stat file_info;
    struct passwd *owner;
    struct group *group;
    char result[4096];

    // Use the stat function to get information about the file
    if (stat(filename, &file_info) == 0)
    {
        owner = getpwuid(file_info.st_uid); // Get owner's name
        group = getgrgid(file_info.st_gid); // Get group's name

        snprintf(result, sizeof(result), "File Info: %s\n", getFormattedFileInfo(&file_info, owner, group));
        printf("%s\n",result);
    }
    else
    {
        perror("stat");
        strncpy(result, strerror(errno), sizeof(result) - 1);
        result[sizeof(result) - 1] = '\0';
        printf("%s\n",result);
    }

    return result;
}

void write_file(int sock, char *path)
{
    int fileDescriptor;
    fileDescriptor = open(path, O_WRONLY | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);

    if (fileDescriptor == -1) {
        perror("Error opening file");
        return;
    } //dont send start ack

    int array_size = (1024 / CHUNK_SIZE - 1) + 1000;

    chunk **chunk_array = (chunk **)malloc(sizeof(chunk *) * array_size);

    for (int i = 0; i < array_size; i++)
    {
      chunk_array[i] = (chunk *)malloc(sizeof(chunk));
      // to know this chunk not yet received
    }

    int no_received = 0;
    int chunk_no = array_size;
    int i=0;
    while (no_received < chunk_no)
    {
      chunk *received_packet = (chunk *)malloc(sizeof(chunk));

      recv(sock, received_packet, sizeof(chunk), 0);
      printf("[+]Data received: %s\n", received_packet->chunk_buffer);
      strncpy(chunk_array[i++]->chunk_buffer,received_packet->chunk_buffer,sizeof(received_packet->chunk_buffer));
      chunk_no = received_packet->chunk_no; // set the correct no of chunks

    


      ssize_t bytesWritten = write(fileDescriptor, received_packet->chunk_buffer, strlen(received_packet->chunk_buffer));

        if (bytesWritten == -1) {
            perror("Error writing to file");
            close(fileDescriptor);
            break;
        }

          int ack = received_packet->seq;
      send(sock, &ack, sizeof(ack), 0);
      printf("[+]Ack sent: %d\n",ack);
      no_received++;
      
    }

    close(fileDescriptor);
}