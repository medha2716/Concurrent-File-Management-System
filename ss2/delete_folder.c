#include "ss1.h"

int deletefolder(char *srcPath)
{
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

        snprintf(srcFilePath, PATH_MAX, "%s/%s", srcPath, entry->d_name);

            if (entry->d_type == DT_REG)
            {
                if (unlink(srcFilePath) == 0)
                {
                    printf("Deleted %s successfully\n", srcFilePath);
                }
                else
                {
                    printf("Unable to delete the file %s\n",srcFilePath);
                    return 1;
                }
            }
            else if (entry->d_type == DT_DIR)
            {
                if(deletefolder(srcFilePath)==0)
                {
                    printf("Unable to delete folder %s\n",srcFilePath);
                    return 0;
                }
            }
        
    }

    closedir(dir);
    if (rmdir(srcPath) != 0)
        return 0;
    return 1;
}

int delete_dir(char *relativePath)
{
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));

    char fullPath[PATH_MAX];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", cwd, relativePath);

    struct stat sb;
    int file_exists = 0;
    if (stat(fullPath, &sb) == 0)
    {
        if (S_ISDIR(sb.st_mode))
        {
            printf("Directory exists\n");
            file_exists = 1;
        }
    }
    if (file_exists)
    {
        if (deletefolder(relativePath) == 1) // rmdir
            printf("Deleted successfully\n");
        else
        {
            printf("142: Unable to delete the directory\n");
            return DIR_DELETION_ERROR;
        }    
    }
    else
    {
        printf("143: Directory doesn't exist\n");
        return DIR_DOES_NOT_EXIST;
    }    

    if (stat(fullPath, &sb) == 0)
    {
        if (S_ISDIR(sb.st_mode))
        {
            printf("Directory exists\n");
            file_exists = 1;
        }
    }

    return 0;
}