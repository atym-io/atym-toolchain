// ipv4_tcp_echo_server_send_recv.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include "wasi_socket_ext.h"

#define PORT 8000
#define BUFFER_SIZE 1024
#define GREETING "Hello, world!"
#define GREETING_LEN 13

void handle_client(int client_fd, struct sockaddr_in *client_addr)
{
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr->sin_addr), client_ip, INET_ADDRSTRLEN);
    printf("Client connected from %s:%d\n", client_ip, ntohs(client_addr->sin_port));
    fflush(stdout);

    if (send(client_fd, GREETING, GREETING_LEN, 0) == -1)
        perror("send");

    while ((bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0)) > 0)
    {
        printf("Received %zd bytes.\n", bytes_received);
        fflush(stdout);
        if (send(client_fd, buffer, bytes_received, 0) == -1)
        {
            perror("send");
            break;
        }
    }

    if (bytes_received < 0)
        perror("recv");

    close(client_fd);
}

int main(void)
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    printf("Echo server starting.\n");
    fflush(stdout);

    server_fd = socket(AF_INET, SOCK_STREAM, 0); // IPv4 + TCP
    if (server_fd < 0)
    {
        perror("socket");
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 1) < 0)
    {
        perror("listen");
        close(server_fd);
        return 1;
    }

    printf("Echo server listening on port %d...\n", PORT);
    fflush(stdout);

    while (1)
    {
        client_len = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0)
        {
            perror("accept");
            continue;
        }
        handle_client(client_fd, &client_addr);
    }

    close(server_fd);
    return 0;
}
