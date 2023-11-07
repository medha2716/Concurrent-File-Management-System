#include "ss.h"

int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
        exit(1);
    }

    const char *srcPath = argv[1];
    const char *destPath = argv[2];

    char *ip = "127.0.0.1";
    int port1 = 2344;
    int ack;

    int sock1; // copy from
    struct sockaddr_in addr1;
    socklen_t addr_size;
    char buffer[1024];
    int n;
    sock1 = socket(AF_INET, SOCK_STREAM, 0);
    if (sock1 < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    memset(&addr1, '\0', sizeof(addr1));
    addr1.sin_family = AF_INET;
    addr1.sin_port = port1;
    addr1.sin_addr.s_addr = inet_addr(ip);

    connect(sock1, (struct sockaddr *)&addr1, sizeof(addr1));
    printf("Connected to the storage server 1.\n");

    int port2 = 2345; // copy 2
    int sock2;
    struct sockaddr_in addr2;

  
    sock2 = socket(AF_INET, SOCK_STREAM, 0);
    if (sock2 < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    memset(&addr2, '\0', sizeof(addr2));
    addr2.sin_family = AF_INET;
    addr2.sin_port = port2;
    addr2.sin_addr.s_addr = inet_addr(ip);

    connect(sock2, (struct sockaddr *)&addr2, sizeof(addr2));
    printf("Connected to the storage server 2.\n");

    // start
    bzero(buffer, 1024);
    strcpy(buffer, srcPath);
    printf("Sent: %s\n",buffer);
    send(sock1, &buffer, sizeof(buffer), 0);
    recv(sock1, &ack, sizeof(ack), 0);

    bzero(buffer, 1024);
    strcpy(buffer, destPath);
    printf("Sent: %s\n",buffer);
    send(sock1, &buffer, sizeof(buffer), 0);
    recv(sock1, &ack, sizeof(ack), 0);

    while (1)
    {
        char c;
        recv(sock1, &c, sizeof(c), 0);
        printf("Received: %c\n", c);

        send(sock2, &c, sizeof(c), 0);
        recv(sock2, &ack, sizeof(ack), 0); // till server2 doesnt receive c we dont go ahead

        ack = 1;
        send(sock1, &ack, sizeof(ack), 0); // send ack to sock1 only after receving ack from sock2

        if (c == 'F')
        {
            bzero(buffer, 1024);
            recv(sock1, buffer, sizeof(buffer), 0); // filename

            send(sock2, buffer, strlen(buffer), 0);
            recv(sock2, &ack, sizeof(ack), 0);

            ack = 1;
            send(sock1, &ack, sizeof(ack), 0); // filename sent ack sent to sock1

            // now actual file content
            while (1)
            {
                bzero(buffer, 1024);
                recv(sock1, buffer, sizeof(buffer), 0);
                send(sock2, buffer, strlen(buffer), 0);
                recv(sock2, &ack, sizeof(ack), 0);

                ack = 1;
                send(sock1, &ack, sizeof(ack), 0);

                printf("Received: %s\n", buffer);
                if (strcmp(buffer, "end") == 0)
                    break;
            }
        }
        else if (c == 'D')
        {
            bzero(buffer, 1024);
            recv(sock1, buffer, sizeof(buffer), 0);
            send(sock2, buffer, strlen(buffer), 0);
            recv(sock2, &ack, sizeof(ack), 0); // dirname
            printf("Dir name: %s\n", buffer);

            ack = 1;
            send(sock1, &ack, sizeof(ack), 0);
        }
        else if (c == 'E')
        {
            break;
        }
    }
    close(sock1);
    close(sock2);
}
