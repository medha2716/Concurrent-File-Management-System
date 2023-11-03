#include "main.h"

// creates "Copies" folder if does not exist already and creates the new file name
// ex: "Copies/newfile_reverse.txt"
char* generate_new_file_name(char* file_name) {
    const char* folder_name = "Copies"; // name of the folder where the new reversed files are to be stored
    mkdir(folder_name, 0755);           // creating the folder with folder_name if the folder does not exist already

    // calculating the length of file name without the extension
    int len = 0;
    for (int i = 0; i < strlen(file_name); i++) {
        if (file_name[i] == '.') break;
        else len++;
    }

    // generating the new file name with "Copies/" attached at the front and "_reverse" appended in the input file name
    char* new_file_name = (char*) malloc(sizeof(char) * (len + 20));
    for (int i = 0; i < 6; i++) {
        new_file_name[i] = folder_name[i];
    }

    new_file_name[6] = '/';

    for (int i = 7; i < 7 + len; i++) {
        new_file_name[i] = file_name[i - 7];
    }

    char* _reverse = "_reverse.txt";
    for (int i = len + 7; i < len + 19; i++) {
        new_file_name[i] = _reverse[i - len - 7];
    }
    
    new_file_name[len + 19] = '\0';
    return new_file_name;
}

void read_in_buffer(int fd, char* buffer, long long int buffer_size) {
    // moving the file pointer backward
    lseek(fd, -buffer_size, SEEK_CUR);
    // reading the file
    read(fd, buffer, buffer_size);
    // again moving the file pointer backward as reading brings it forward
    lseek(fd, -buffer_size, SEEK_CUR);
}

void reverse_buffer(char* buffer, long long int buffer_size) {
    // reversing the bytes in buffer
    for (int i = 0; i < buffer_size / 2; i++) {
        char temp = buffer[i];
        buffer[i] = buffer[buffer_size - i - 1];
        buffer[buffer_size - i - 1] = temp;
    }
}

void read_and_write_remaining_bytes(int fd, char* buffer, int fd2) {
    int no_of_bytes = lseek(fd, 0, SEEK_CUR);       // calculating the number of bytes left to be read
    lseek(fd, 0, SEEK_SET);                         // moving file pointer to the start of the file
    read(fd, buffer, no_of_bytes);                  // reading the remaining bytes

    // reversing the bytes
    for (int i = 0; i < no_of_bytes / 2; i++) {
        char temp = buffer[i];
        buffer[i] = buffer[no_of_bytes - i - 1];
        buffer[no_of_bytes - i - 1] = temp;
    }

    // writing back the reverse bytes
    write(fd2, buffer, no_of_bytes);
}

int main(int argc, char** argv) {

    /*
    You are expected to make use of File Management
    syscalls for this part.
    
    **INPUT**
    Filename will be given as input from the command line.
    */

    long long int buffer_size = 5000000000; // 5GB

    // reading the file name
    char* file_name = argv[1];
    struct stat file_info;

    // stat function is used to retrive the file information including it's permissions and the data is saved in the struct stat file_info
    stat(file_name, &file_info);
    // bit masking to obtain the permission in octal
    int permissions_of_inputted_file = file_info.st_mode & 0777;
    
    char* new_file_name = generate_new_file_name(file_name);
    // opening files to work with
    int fd = open(file_name, O_RDONLY);                         // read from input file
    int fd2; // file pointer for write file
    if (access(new_file_name, F_OK) == 0) { // checking if the file is already present
        // deriving the permissions of the already present file
        struct stat file_info2;
        stat(new_file_name, &file_info2);
        int permissions_of_already_present_file = file_info2.st_mode & 0777;

        // checking if the already present file has the right permissions as required
        if (permissions_of_already_present_file == permissions_of_inputted_file) { // the already present file has the proper permissions
            // do nothing
        } else {
            // delete the already present file as it does not have the proper permissions
            remove(new_file_name);
        }
    } else {
        // do nothing if the file is not already present as we will make it later
    }
    
    // create the file if not exists and open in write mode else create the file with the specified permissions
    fd2 = open(new_file_name, O_CREAT | O_WRONLY | O_TRUNC, permissions_of_inputted_file);
    
    // creating buffer and calculating the total size of the file
    char* buffer = (char*) calloc(buffer_size, sizeof(char));
    long long int number_of_bytes_in_the_file = lseek(fd, 0, SEEK_END);

    // if the file is smaller than 5GB than better use 500MB of buffer_size for optimisation
    if (number_of_bytes_in_the_file < 5000000000) {
        buffer_size = 500000000; // 500 MB
    }

    // while we are able to read full buffer length/size
    while (lseek(fd, 0, SEEK_CUR) - buffer_size >= 0) {
        read_in_buffer(fd, buffer, buffer_size);
        reverse_buffer(buffer, buffer_size);

        // writing onto the new file
        write(fd2, buffer, buffer_size);
    }

    // when the number of bytes left is less than the buffer size
    if (lseek(fd, 0, SEEK_CUR) - buffer_size < 0) {
        read_and_write_remaining_bytes(fd, buffer, fd2);        
    }
    // freeing memory
    free(new_file_name);
    free(buffer);

    // closing the files
    close(fd);
    close(fd2);

    return 0;
}