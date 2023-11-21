#include "ss1.h"

int directoryExists(char *path)
{
    DIR *dir = opendir(path);

    if (dir)
    {
        closedir(dir);
        return 1; // Directory exists
    }
    else
    {
        return 0; // Directory does not exist
    }
}

void get_accessible_path_present(char *relativePath)
{

    char base_path[PATH_MAX];

    int input_length = strlen(relativePath);
    int base_path_length = 0;

    // Copy characters to the base_path until a '/' is encountered
    int c = 1;

    for (int i = input_length - 1; i >= 0; i--)
    {
        if (c)
        {
            if ((relativePath[i]) != '/')
                continue;
            else // found first / from last
                c = 0;
            continue;
        }
        base_path[base_path_length++] = relativePath[i];
    }

    base_path[base_path_length] = '\0'; // Null-terminate the base_path string

    for (int i = 0, j = base_path_length - 1; i < j; i++, j--)
    {
        // Swap characters at position i and j
        char temp = base_path[i];
        base_path[i] = base_path[j];
        base_path[j] = temp;
    }

    printf("%s\n", base_path);
}

int create_dirs(char *relativePath)
{
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));

    get_accessible_path_present(relativePath);

    char fullPath[PATH_MAX];

    // Combine current working directory and relative path
    if (directoryExists(relativePath))
    {
        printf("Directory already exists\n");
        return DIR_EXISTS_ALREADY;
    }
    else
    {
        if (snprintf(fullPath, sizeof(fullPath), "%s/%s", cwd, relativePath) < sizeof(fullPath))
        {

            if (mkdir(fullPath, S_IRWXU | S_IRWXG | S_IRWXO) == 0)
            {
                printf("Directory created successfully.\n");
            }
            else
            {
                printf("130: Directory creation failed");
                return DIR_CREATION_FAILED;
            }
        }
        else
        {
            printf("131: Path length too long");
            return MAX_PATH_LENGTH_EXCEEDED;
        }
    }

    return 0;
}
