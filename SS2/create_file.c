
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
            perror("132: Cannot create file");
            return 1;
        }
    }

    return 0;
}





