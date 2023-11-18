#include "ss1.h"

 
int delete_file(char *relativePath)
{
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));

    

    char fullPath[PATH_MAX];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", cwd, relativePath);
    
    struct stat sb;
    int file_exists=0;
    if (stat(fullPath, &sb) == 0)
    {
        if (!S_ISDIR(sb.st_mode))
        {
            printf("File exists\n");
            file_exists=1;
        }
    }
    if(file_exists)
    {
    if (unlink(relativePath) == 0)
        printf("Deleted successfully\n");
    else
        printf("140: Unable to delete the file\n");
    }
    else
        printf("414: File doesn't exist\n");
 
    return 0;
}