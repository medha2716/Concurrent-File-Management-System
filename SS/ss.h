#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

#define PATH_MAX 1024

int directoryExists(const char *path);
void get_accessible_path_present(const char *relativePath);
