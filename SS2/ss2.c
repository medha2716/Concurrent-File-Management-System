#include "ss.h"

#define MAX_LENGTH_ACC_PATHS_ONE_SS 100000

int main()
{

    char *ip = "127.0.0.1";
    int port = 2345;
    int ack;

    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[1024];
    int n;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    n = bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (n < 0)
    {
        perror("[-]Bind error");
        exit(1);
    }
    printf("[+]Bind to the port number: %d\n", port);

    listen(server_sock, 5);
    printf("Listening...\n");

    while (1)
    {
        addr_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
        printf("[+]Client connected.\n");

        // bzero(buffer, 1024);
        while (1)
        {
            char c;
            recv(client_sock, &c, sizeof(c), 0);
            printf("Received: %c\n", c);

            ack=1;
            send(client_sock, &ack, sizeof(ack), 0);
            

            if (c == 'F')
            {
                bzero(buffer, 1024);
                recv(client_sock, buffer, sizeof(buffer), 0); // filename

                printf("File name: %s\n", buffer);

                ack=1;
                send(client_sock, &ack, sizeof(ack), 0);

                int dest_fd = open(buffer, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

                if (dest_fd == -1)
                {
                    perror("Failed to open or create destination file");
                    // close(src_fd);
                    // exit(1);
                }

                while (1)
                {
                    bzero(buffer, 1024);
                    recv(client_sock, buffer, sizeof(buffer), 0);

                    ack=1;
                    send(client_sock, &ack, sizeof(ack), 0);

                    printf("Received: %s\n", buffer);
                    if (strcmp(buffer, "end") == 0)
                        break;
                    if (write(dest_fd, buffer, strlen(buffer)) != strlen(buffer))
                    { // other server does this
                        perror("Failed to write to destination file");

                        close(dest_fd);
                        exit(1);
                    }
                }
                close(dest_fd);
            }
            else if(c == 'D')
            {
                bzero(buffer, 1024);
                recv(client_sock, buffer, sizeof(buffer), 0); // dirname
                printf("Dir name: %s\n", buffer);

                ack=1;
                send(client_sock, &ack, sizeof(ack), 0);

                mkdir(buffer, S_IRWXU | S_IRWXG | S_IRWXO);
                
            }
            else if(c == 'E')
                break;
        }

        close(client_sock);
        printf("[+]Client disconnected.\n\n");
    }

    return 0;
}