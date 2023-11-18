//Copy Files/Directories: Storage Servers can copy files or directories from other Storage Servers, with the NM providing the relevant IP addresses for efficient data transfer.


#include "ss1.h"

void copyfile(const char *srcPath, const char *destPath) {
    int src_fd, dest_fd;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    src_fd = open(srcPath, O_RDONLY);
    if (src_fd == -1) {
        perror("120: Failed to open source file");
        exit(1);
    }

    dest_fd = open(destPath, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR); // ask other server to do this
    
    if (dest_fd == -1) {
        perror("121: Failed to open or create destination file");
        close(src_fd);
        exit(1);
    }

    while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) { // send to other server
        if (write(dest_fd, buffer, bytes_read) != bytes_read) { // other server does this
            perror("122: Failed to write to destination file");
            close(src_fd);
            close(dest_fd);
            exit(1);
        }
    }

    close(src_fd);
    close(dest_fd);
}

void copydir(const char *srcPath, const char *destPath) {
    // Create the destination directory if it doesn't exist // ask other server to do this
    mkdir(destPath, S_IRWXU | S_IRWXG | S_IRWXO);

    // Traverse the source directory and copy its contents to the destination directory
    DIR *dir;
    struct dirent *entry;

    dir = opendir(srcPath);
    if (dir == NULL) {
        perror("123: Failed to open source directory");
        exit(1);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char srcFilePath[PATH_MAX];
        char destFilePath[PATH_MAX];

        snprintf(srcFilePath, PATH_MAX, "%s/%s", srcPath, entry->d_name);
        snprintf(destFilePath, PATH_MAX, "%s/%s", destPath, entry->d_name);

        if (entry->d_type == DT_REG) {
            copyfile(srcFilePath, destFilePath);
        } else if (entry->d_type == DT_DIR) {
            copydir(srcFilePath, destFilePath);
        }
    }

    closedir(dir);
}

int self_copy(char *srcPath, char *destPath) {
 
   
    struct stat srcStat;
    if (stat(srcPath, &srcStat) == -1) {
        perror("124: Failed to get source file/directory information");
        exit(1);
    }

    if (S_ISREG(srcStat.st_mode) == 1) {
        copyfile(srcPath, destPath);
    } else if (S_ISDIR(srcStat.st_mode) == 1) {
        copydir(srcPath, destPath);
    } else {
        fprintf(stderr, "125: Source is neither a regular file nor a directory.\n");
        exit(1);
    }

    printf("Copy completed.\n");
    return 0;
}