

// ports reserved till 1024

// keep track of files/folders in this server
// for each file/dir have a lock
// and while doing each command lock and unlock

// also do what we did for peek at regular intervals to check if any files have been added to server and tell nm server
#include "../ss1.h"

char HOME[1024];

typedef struct send_nm_init
{
    char accessible_paths[MAX_LENGTH_ACC_PATHS_ONE_SS];
    int port_nm;
    int port_client;
    char ip[40];
} send_nm_init;

send_nm_init struct_to_send;

pthread_t client_thread[MAX_CLIENTS];

// input enters: create file or directory, path to where it wants it to be created, and name of file/dir
// void create_file_dir(char *new_name, char *path, char file_or_dir)
// {
//     if (file_or_dir == 'd')
//         ;
// }
void copy_ss2(int client_sock)
{
    int ack;
    char buffer[1024];
    while (1)
    {
        char c;
        recv(client_sock, &c, sizeof(c), 0);
        printf("Received: %c\n", c);

        ack = 1;
        send(client_sock, &ack, sizeof(ack), 0);

        if (c == 'F')
        {
            bzero(buffer, 1024);
            recv(client_sock, buffer, sizeof(buffer), 0); // filename

            printf("File name: %s\n", buffer);

            ack = 1;
            send(client_sock, &ack, sizeof(ack), 0);

            int dest_fd = open(buffer, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

            if (dest_fd == -1)
            {
                perror("106: Failed to open or create destination file");
                // close(src_fd);
                // exit(1);
            }

            while (1)
            {
                bzero(buffer, 1024);
                recv(client_sock, buffer, sizeof(buffer), 0);

                ack = 1;
                send(client_sock, &ack, sizeof(ack), 0);

                printf("Received: %s\n", buffer);
                if (strcmp(buffer, "end") == 0)
                    break;
                if (write(dest_fd, buffer, strlen(buffer)) != strlen(buffer))
                { // other server does this
                    perror("105: Failed to write to destination file");

                    close(dest_fd);
                    exit(1);
                }
            }
            close(dest_fd);
        }
        else if (c == 'D')
        {
            bzero(buffer, 1024);
            recv(client_sock, buffer, sizeof(buffer), 0); // dirname
            printf("Dir name: %s\n", buffer);

            ack = 1;
            send(client_sock, &ack, sizeof(ack), 0);

            mkdir(buffer, S_IRWXU | S_IRWXG | S_IRWXO);
        }
        else if (c == 'E')
            break;
    }
    return;
}

void *client_handle(void *param)
{
    int *client_sock = (int *)param;
    char buffer[1024];
    bzero(buffer, 1024);
    recv(*client_sock, buffer, sizeof(buffer), 0);
    printf("Client: %s\n", buffer);

    bzero(buffer, 1024);
    strcpy(buffer, "HI, THIS IS SERVER. HAVE A NICE DAY!!!");
    printf("Storage Server: %s\n", buffer);
    send(*client_sock, buffer, strlen(buffer), 0);

    char choice;
    recv(*client_sock, &choice, sizeof(choice), 0);

    char *ack_start = "START";
    printf("START ack sent\n");
    send(*client_sock, ack_start, strlen(ack_start), 0);

    int flag_success = 1;

    int ack;
    char result[4096];

    switch (choice)
    {
    case 't':
        bzero(buffer, 1024);
        recv(*client_sock, buffer, sizeof(buffer), 0);
        strcpy(result, file_details(buffer));
        send(*client_sock, result, sizeof(result), 0);
        recv(*client_sock, &ack, sizeof(ack), 0);
        printf("Received ack\n");
        break;
    case 'r':
        bzero(buffer, 1024);
        recv(*client_sock, buffer, sizeof(buffer), 0);
        read_file(*client_sock, buffer);
        break;
    case 'w':
        bzero(buffer, 1024);
        recv(*client_sock, buffer, sizeof(buffer), 0);
        write_file(*client_sock, buffer);
        break;
    default:
        printf("Unsure what client wants to do\n");
        break;
    }

    if (flag_success)
    {
        char *ack_stop = "STOP";
        printf("STOP ack sent\n");
        send(*client_sock, ack_stop, strlen(ack_stop), 0);
    }
    else
    {
        char *ack_stop = "ERROR_STP";
        printf("ERROR_STP ack sent\n");
        send(*client_sock, ack_stop, strlen(ack_stop), 0);
    }

    close(*client_sock);
    printf("[+]Client disconnected.\n\n");
    return NULL;
}

void *client_interactions(void *arg)
{
    char *ip = "127.0.0.1";
    int port = *((int *)arg);
    // int port = 1234;

    int ss_server_sock, client_sock;
    struct sockaddr_in ss_server_addr, client_addr;
    socklen_t addr_size;
    char buffer[1024];
    int n;

    ss_server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (ss_server_sock < 0)
    {
        perror("[-] 104: TCP Socket (for client interaction) creation error");
        exit(1);
    }
    printf("[+]TCP server socket (for client interaction) created.\n");

    memset(&ss_server_addr, '\0', sizeof(ss_server_addr));
    ss_server_addr.sin_family = AF_INET;
    ss_server_addr.sin_port = port;
    ss_server_addr.sin_addr.s_addr = inet_addr(ip);

    n = bind(ss_server_sock, (struct sockaddr *)&ss_server_addr, sizeof(ss_server_addr));
    if (n < 0)
    {
        perror("[-] 103: Bind socket (for client interaction) error");
        exit(1);
    }
    printf("[+]Bind to the port number: %d (for client interaction) \n", port);

    listen(ss_server_sock, 50);
    printf("Listening for clients...\n\n\n");

    int i = 0;

    while (1)
    {
        addr_size = sizeof(client_addr);
        client_sock = accept(ss_server_sock, (struct sockaddr *)&client_addr, &addr_size);
        printf("[+]new client connected.\n");

        pthread_create(&client_thread[i++], NULL, client_handle, &client_sock);

        if (i >= 50)
        {
            // Update i
            i = 0;
            while (i < 50)
            {
                // Suspend execution of
                // the calling thread
                // until the target
                // thread terminates
                pthread_join(client_thread[i++], NULL);
            }
            // Update i
            i = 0;
        }
    }
    return NULL;
}

void *nm_commands(void *arg)
{
    char *ip = "127.0.0.1";
     int port = *((int *)arg);

    int ss_server_sock, nm_client_sock;
    struct sockaddr_in ss_server_addr, nm_client_addr;
    socklen_t addr_size;
    char buffer[1024];
    int n;

    ss_server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (ss_server_sock < 0)
    {
        perror("[-] 102: TCP Socket (for receiving NM commands) creation error");
        exit(1);
    }
    printf("[+]TCP server socket (for receiving NM commands) created.\n");

    memset(&ss_server_addr, '\0', sizeof(ss_server_addr));
    ss_server_addr.sin_family = AF_INET;
    ss_server_addr.sin_port = port;
    ss_server_addr.sin_addr.s_addr = inet_addr(ip);

    n = bind(ss_server_sock, (struct sockaddr *)&ss_server_addr, sizeof(ss_server_addr));
    if (n < 0)
    {
        perror("[-] 101: Bind socket (for receiving NM commands) error");
        exit(1);
    }
    printf("[+]Bind to the port number: %d (for receiving NM commands) \n", port);

    listen(ss_server_sock, 5);
    printf("Listening for NM server...\n\n\n");

    while (1)
    {
        addr_size = sizeof(nm_client_addr);
        nm_client_sock = accept(ss_server_sock, (struct sockaddr *)&nm_client_addr, &addr_size);
        printf("[+]NM server connected.\n");

        bzero(buffer, 1024);
        recv(nm_client_sock, buffer, sizeof(buffer), 0);
        printf("NM server: %s\n", buffer);

        bzero(buffer, 1024);
        strcpy(buffer, "HI, THIS IS STORAGE SERVER!");
        printf("SS server: %s\n", buffer);
        send(nm_client_sock, buffer, strlen(buffer), 0);

        char choice;
        recv(nm_client_sock, &choice, sizeof(choice), 0);

        char *ack_start = "START";
        printf(GRN);
        printf("START ack sent\n");
        printf(RST);
        send(nm_client_sock, ack_start, strlen(ack_start), 0);

        int flag_success = 1;

        char srcPath[PATH_MAX];
        char destPath[PATH_MAX];
        int ack;


        switch (choice)
        {
        case 'f': // for creation of file
            bzero(buffer, 1024);
            recv(nm_client_sock, buffer, sizeof(buffer), 0);
            int h = create_file(buffer);
            break;
        case 'd': // for creation of dir
            bzero(buffer, 1024);
            recv(nm_client_sock, buffer, sizeof(buffer), 0);
            int l = create_dirs(buffer);
            break;
        case 'F': // for deletion of file
            bzero(buffer, 1024);
            recv(nm_client_sock, buffer, sizeof(buffer), 0);
            l = delete_file(buffer);
            break;
        case 'D': // for deletion of dir
            bzero(buffer, 1024);
            recv(nm_client_sock, buffer, sizeof(buffer), 0);
            l = delete_dir(buffer);
            break;
        case 'c': // sends its file/dir for copying
            copy_ss2(nm_client_sock);
            break;
        case 'p': // for server that copies file/dir
            ss1_copy(nm_client_sock);
            break;
        case 's': // copy from self

            recv(nm_client_sock, srcPath, sizeof(srcPath), 0);
            ack = 1;
            send(nm_client_sock, &ack, sizeof(ack), 0);

            recv(nm_client_sock, destPath, sizeof(destPath), 0);
            ack = 1;
            send(nm_client_sock, &ack, sizeof(ack), 0);
            self_copy(srcPath, destPath);
            break;
        default:
            // (when choice is none of the above)
            flag_success = 0;
            printf("Unsure what the NM server wants to do\n");
            break;
        }

        if (flag_success)
        {
            char *ack_stop = "STOP";
            printf("STOP ack sent\n");
            send(nm_client_sock, ack_stop, strlen(ack_stop), 0);
        }
        else
        {
            char *ack_stop = "ERROR_STP";
            printf("ERROR_STP ack sent\n");
            send(nm_client_sock, ack_stop, strlen(ack_stop), 0);
        }

        close(nm_client_sock);
        printf("[+]Client disconnected.\n\n");
    }
    return NULL;
}

void search_directory(const char *dir_path, char *temp_to_store_curr_paths)
{

    DIR *dir = opendir(dir_path);

    if (dir == NULL)
    {
        perror("opendir");
        return;
    }

    struct dirent *entry;

    int c = 0;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        struct stat entry_stat;
        if (stat(full_path, &entry_stat) == -1)
        {
            perror("stat");
            continue;
        }

        if (S_ISDIR(entry_stat.st_mode))
        {
            search_directory(full_path, temp_to_store_curr_paths);
        }
        else
        {
            int target = strlen(HOME) + 1;
            char access_path_sent[PATH_MAX];
            snprintf(access_path_sent, sizeof(access_path_sent), "%s\n", full_path + target);

            const char *ext = strrchr(entry->d_name, '.');
            if ((ext && strcmp(ext, ".c") == 0) || (ext && strcmp(ext, ".h") == 0))
                continue;
            else if ((entry->d_name[0] != '.')) // hidden
            {
                // printf("%s",access_path_sent);
                strcat(temp_to_store_curr_paths, access_path_sent);
                c++;
            }
        }
    }

    if (c == 0) // empty directories
    {
        int target = strlen(HOME) + 1;
        char access_path_sent[PATH_MAX];
        snprintf(access_path_sent, sizeof(access_path_sent), "%s\n", dir_path + target);
        strcat(temp_to_store_curr_paths, access_path_sent);
    }

    closedir(dir);
}

void *update_file_structure_nm()
{

    getcwd(HOME, 1024);

    while (1)
    {
        char temp_to_store_current_paths[MAX_LENGTH_ACC_PATHS_ONE_SS];

        search_directory(HOME, temp_to_store_current_paths);

        // printf("%s\n", temp_to_store_current_paths);

        if (strcmp(struct_to_send.accessible_paths, temp_to_store_current_paths) != 0)
        {
            printf(CSTM1);
            printf("Change in File structure detected!\n");
            printf(RST);
            printf("%s\n", temp_to_store_current_paths);

            char *ip = "127.0.0.1";
            int port = 5566;

            int sock;
            struct sockaddr_in addr;
            socklen_t addr_size;
            char buffer[1024];
            int n;

            sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock < 0)
            {
                perror("[-] 100: Socket error");
                exit(1);
            }
            printf(CSTM2);
            printf("[+]TCP server socket created for sending accessible paths to Naming Server..\n");
            printf(RST);

            memset(&addr, '\0', sizeof(addr));
            addr.sin_family = AF_INET;
            addr.sin_port = port;
            addr.sin_addr.s_addr = inet_addr(ip);

            connect(sock, (struct sockaddr *)&addr, sizeof(addr));
            printf("Connected to the NM server.\n");

            // send to nfs server that it is a storage server that wants to update its file structure
            int ss_or_client = 3;
            send(sock, &ss_or_client, sizeof(ss_or_client), 0);

            strcpy(struct_to_send.accessible_paths, temp_to_store_current_paths);

            send(sock, &struct_to_send, sizeof(struct_to_send), 0);

            bzero(buffer, 1024);
            recv(sock, buffer, sizeof(buffer), 0);
            printf(CSTM2);
            printf("Server: %s\n", buffer);
            printf(RST);

            close(sock);
            printf("Disconnected from the NM server.\n\n\n");
        }
        bzero(temp_to_store_current_paths, MAX_LENGTH_ACC_PATHS_ONE_SS);
        sleep(20);
    }
}

int main()
{

    printf("Initializing Storage Server\n");

    // taking input of accessible paths
    char accessible_paths[MAX_LENGTH_ACC_PATHS_ONE_SS];

    printf("Enter (line separated) accessible paths for this stoarage server:\n");
    char temp[MAX_LENGTH_ACC_PATHS_ONE_SS];

    while (1)
    {
        // Read a line from the user
        if (fgets(temp, sizeof(temp), stdin) == NULL)
        {
            // Error or end of input
            break;
        }

        // append the line to accessible_paths
        strncat(accessible_paths, temp, sizeof(accessible_paths) - strlen(accessible_paths) - 1);

        // check if the user pressed Enter without typing anything (empty line to finish)
        if (strcmp(temp, "\n") == 0)
        {
            break;
        }
    }

    // remove the trailing newline character, if present
    size_t len = strlen(accessible_paths);
    if (len > 0 && accessible_paths[len - 1] == '\n')
    {
        accessible_paths[len - 1] = '\0';
    }

    // printf("You entered the following accessible paths:\n%s\n\n", accessible_paths);

    // done taking user input for accessible paths
    int port_nm;
    printf("Enter port for NM connection: ");
    scanf("%d", &port_nm); // 1235 for now

    int port_client;
    printf("Enter port for client interactions: ");
    scanf("%d", &port_client); // 1234 for now

    // send struct to nm server

    char *ip = "127.0.0.1";
    int port = 5566; // port of nm

    int sock;
    struct sockaddr_in addr;
    socklen_t addr_size;
    char buffer[1024];
    int n;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("[-] 100: Socket error");
        exit(1);
    }
    printf(GRN);
    printf("\n[+]TCP server socket created for connecting to Naming Server..\n");

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = port;
    addr.sin_addr.s_addr = inet_addr(ip);

    connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    printf("Connected to the NM server.\n");

    // send to nfs server that it is a storage server
    int ss_or_client = 1;
    send(sock, &ss_or_client, sizeof(ss_or_client), 0);

    strcpy(struct_to_send.accessible_paths, accessible_paths);
    strcpy(struct_to_send.ip, ip);
    struct_to_send.port_client = port_client;
    struct_to_send.port_nm = port_nm;

    printf("%s\n", struct_to_send.ip);

    send(sock, &struct_to_send, sizeof(struct_to_send), 0);

    bzero(buffer, 1024);
    recv(sock, buffer, sizeof(buffer), 0);
    printf("Server: %s\n", buffer);

    close(sock);
    printf("Disconnected from the NM server.\n\n\n");
    printf(RST);

    usleep(10); 

    pthread_t connection_for_nm_commands;
    pthread_create(&connection_for_nm_commands, NULL, &nm_commands, &port_nm);

    pthread_t connection_for_client_interactions;
    pthread_create(&connection_for_client_interactions, NULL, &client_interactions, &port_client);

    // pthread_t update_file_dir;
    // pthread_create(&update_file_dir, NULL, &update_file_structure_nm, NULL);

    pthread_join(connection_for_nm_commands, NULL);
    pthread_join(connection_for_client_interactions, NULL);
    // pthread_join(update_file_dir, NULL);

    return 0;
}
