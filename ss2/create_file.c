
#include "ss1.h"

int create_file(char* relativePath)
{
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));


    char fullPath[PATH_MAX];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", cwd, relativePath);
    
    struct stat sb;
    if (stat(fullPath, &sb) == 0)
    {
        if (!S_ISDIR(sb.st_mode))
        {
            printf("File exists\n");
            return FILE_EXISTS;
        }
    }
    else
    {
        // Path doesn't exist, so create an empty file
        int fd = open(fullPath, O_CREAT | O_WRONLY, 0666); // Change the permission as needed
        if (fd != -1)
        {
            close(fd);
            printf("Empty file created at: %s\n", fullPath);
        }
        else
        {
            printf("132: Cannot create file");
            return FILE_CREATE_ERROR;
        }
    }

    return 0;
}





