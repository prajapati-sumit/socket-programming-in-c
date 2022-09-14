#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>
#include <sys/types.h>
#include<sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_BUFFER 1024
#define IP_ADDRESS "127.0.0.1"
#define BACKLOG 0

int PORT;
char buffer[MAX_BUFFER];
struct Socket {
    int fd;
    struct sockaddr_in addr;
};
int init_server_socket(struct Socket* server_socket) {
    // creating socket
    if ((server_socket->fd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return -1;
    }
    server_socket->addr.sin_family = AF_INET;
    server_socket->addr.sin_port = htons(PORT);
    server_socket->addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);

    //binding the socket to a name
    if (bind(server_socket->fd, (struct sockaddr*)(&server_socket->addr), sizeof(server_socket->addr)) == -1) {
        perror("bind");
        return -1;
    }

    // listening for clients
    if (listen(server_socket->fd, BACKLOG) == -1) {
        perror("listen");
        return -1;
    }

    printf("Listening on port %d\n", PORT);
    return 0;
}
int accept_client(struct Socket* server_socket, struct Socket* client_socket) {
    int cli_addr_size = sizeof(client_socket->addr);
    int new_fd = accept(server_socket->fd, (struct sockaddr*) & (client_socket->addr), &cli_addr_size);
    if (new_fd == -1) {
        perror("accept");
        return -1;
    }
    printf("Connected to client using socket %d\n", new_fd);
    return new_fd;

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
        printf("socket %d disconnected\n", sock_fd);
        close(sock_fd);
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
    if (send(sock_fd, buffer, strlen(buffer), 0) == -1) {
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
int evaluate(char* res) {
    char str[MAX_BUFFER];
    strcpy(str, buffer);
    char * token = strtok(str, "+-*/");
    int a;
    int b;
    int cnt = 0;
    while ( token != NULL ) {
        if (cnt == 0) {
            a = atoi(token);
            cnt++;
        }
        else if (cnt == 1) {
            b = atoi(token);
            cnt++;
        }
        else
            return -1;
        token = strtok(NULL, " ");
    }
    if (cnt < 2)
        return -1;
    char op = '$';
    for (int i = 0; i < strlen(buffer); i++) {
        if (buffer[i] == '+' || buffer[i] == '-' || buffer[i] == '*' || buffer[i] == '/') {
            if (op != '$')
                return -1;
            op = buffer[i];
        }
    }
    switch (op) {
    case '+': sprintf(res, "%d", (a) + (b)); break;
    case '-': sprintf(res, "%d", (a) - (b)); break;
    case '*': sprintf(res, "%d", (a) * (b)); break;
    case '/': if ((b) == 0)
            strcpy(res, "Division by zero error");
        else
            sprintf(res, "%d", (a) / (b));
        break;
    default: break;
    }

    return 0;
}
void handle_request(int sock_fd) {

    char data_to_send[MAX_BUFFER];
    while (1) {
        if (receive_from(sock_fd) == -1)
            break;
        // socket : pipe broken
        if (!strlen(buffer) && send_to(sock_fd, data_to_send) == -1) {
            break;
        }
        buffer[strlen(buffer) - 1] = '\0';
        printf("Client socket %d sent message: %s\n", sock_fd, buffer);
        if (evaluate(data_to_send) == -1)
            strcpy(data_to_send, "error parsing the expression");
        if (send_to(sock_fd, data_to_send) == -1)
            break;
        printf("Sending reply: %s\n", data_to_send);
    }
    printf("Client with socket %d disconnected\n", sock_fd);
    close(sock_fd);
}
void handle_sigint() {
    printf("\nShutting down server...\n");
    exit(0);
}

int main(int argc, char* argv[])
{
    signal(SIGINT, handle_sigint);
    if (argc != 2) {
        usage(argc, argv);
        exit(1);
    }
    PORT = atoi(argv[1]);

    // main socket of the server
    struct Socket server_socket;
    if (init_server_socket(&server_socket) == -1) {
        exit(1);
    }

    struct Socket client_socket;
    int new_sock_fd;
    while (1) {
        new_sock_fd = accept_client(&server_socket, &client_socket);
        if (new_sock_fd == -1) {
            exit(1);
        }
        handle_request(new_sock_fd);
    }

    close(server_socket.fd);
    return 0;
}
