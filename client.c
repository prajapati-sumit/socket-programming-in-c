#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include <sys/types.h>
#include <signal.h>
#include<sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_BUFFER 1024
#define IP_ADDRESS "127.0.0.1"
int PORT;
char buffer[MAX_BUFFER];
struct Socket {
    int fd;
    struct sockaddr_in addr;
};
void find_server_socket(struct Socket* server_socket) {

    memset(&server_socket->addr, '\0', sizeof(server_socket->addr));
    server_socket->addr.sin_family = AF_INET;
    server_socket->addr.sin_port = htons(PORT);
    server_socket->addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);
}
int init_client_socket(struct Socket* client_socket, struct Socket* server_socket) {
    if ((client_socket->fd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return -1;
    }
    if (connect(client_socket->fd, (struct sockaddr*)(&server_socket->addr), sizeof(server_socket->addr)) == -1) {
        printf("connect: line busy!\n");
        exit(1);
    }
    printf("Connected to server \n");
    return 0;
}
int connected(int sock_fd) {
    int error = 0;
    socklen_t len = sizeof (error);
    int retval = getsockopt (sock_fd, SOL_SOCKET, SO_ERROR, &error, &len);
    if (retval != 0) {
        fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
        return 0;
    }

    if (error != 0) {
        fprintf(stderr, "socket error: %s\n", strerror(error));
        return 0;
    }
    return 1;
}
void clear_buffer() {
    for (int i = 0; i < MAX_BUFFER; i++)
        buffer[i] = '\0';

}
int send_to(int sock_fd, char* data) {
    if (!connected(sock_fd))
        return -1;
    clear_buffer();
    strcpy(buffer, data);
    if ( send(sock_fd, buffer, strlen(buffer), 0) == -1) {
        perror("send");
        return -1;
    }
    return 0;
}
int receive_from(int sock_fd) {
    if (!connected(sock_fd))
        return -1;
    clear_buffer();
    if (recv(sock_fd, buffer, MAX_BUFFER, 0) == -1) {
        perror("recv");
        return -1;
    }
    return 0;
}
void usage(int argc, char* argv[]) {
    fprintf(stderr, "Usage: %s <port_number>\n", argv[0]);
}
void handle_signal() {
    printf("\nClosing Connection...\n");
    exit(0);
}

int main(int argc, char* argv[])
{
    signal(SIGINT, handle_signal);
    if (argc != 2) {
        usage(argc, argv);
        exit(1);
    }
    PORT = atoi(argv[1]);

    struct Socket server_socket;
    find_server_socket(&server_socket);

    struct Socket client_socket;
    init_client_socket(&client_socket, &server_socket);
    char data_to_send[MAX_BUFFER];


    while (connected(client_socket.fd)) {
        printf("Message to server: ");
        fgets(data_to_send, MAX_BUFFER, stdin);
        send_to(client_socket.fd, data_to_send);
        receive_from(client_socket.fd);
        printf("Server replied: %s\n", buffer);

    }


    return 0;
}
