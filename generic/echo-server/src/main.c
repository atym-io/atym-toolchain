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
#define GREETING "\nHi there, I am an echo server! Whatever you type I will echo back to you.\n"
void handle_client(int client_fd, struct sockaddr_in *client_addr)
{
    char buffer[BUFFER_SIZE];
    char client_msg[BUFFER_SIZE];
	ssize_t bytes_received;
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr->sin_addr), client_ip, INET_ADDRSTRLEN);
    printf("Client connected from %s:%d\n", client_ip, ntohs(client_addr->sin_port));
    printf("**********************************************\n");
    fflush(stdout);
	if (send(client_fd, GREETING, strlen(GREETING), 0) == -1)
        perror("send");
    while ((bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0)) > 0)
	{
        memcpy(client_msg, buffer, bytes_received);
        client_msg[bytes_received] = '\0';
        int len = strlen(client_msg);
		while (len > 0 && (client_msg[len-1] == '\n' || client_msg[len-1] == '\r'))
		{
			client_msg[--len] = '\0';
		}
		printf("You said: '%s'\n", client_msg);
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
    printf("\n**********************************************\n");
    printf("Echo server starting...\n");
    printf("Echo server listening on port %d\n", PORT);
    printf("**********************************************\n");
    printf("To connect:\n");
    printf("1. Find this device's IP: net iface\n");
    printf("2. Telnet to the device device: telnet <IP>:8000\n");
    printf("3. Type messages to test the echo server!\n");
    printf("**********************************************\n");
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
