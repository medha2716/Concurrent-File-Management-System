#include "ss.h"

 
int main()
{
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));

    const char *relativePath = "dir2/dir3/";

    char fullPath[PATH_MAX];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", cwd, relativePath);
    
    struct stat sb;
    int file_exists=0;
    if (stat(fullPath, &sb) == 0)
    {
        if (S_ISDIR(sb.st_mode))
        {
            printf("Directory exists\n");
            file_exists=1;
        }
    }
    if(file_exists)
    {
    if (rmdir(relativePath) == 0)
        printf("Deleted successfully\n");
    else
        printf("Unable to delete the directory\n");
    }
    else
        printf("Directory doesn't exist\n");
 
    return 0;
}