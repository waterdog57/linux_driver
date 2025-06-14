#include   <signal.h>
#include   <stdio.h>
#include   <stdlib.h>
#include   <string.h>
#include   <unistd.h>
#include   <arpa/inet.h>
#include   <errno.h>

#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

int sock;
uint8_t message[] = "Hello, Client!";

void handle_sigint(int sig) {
    printf("\nCaught SIGINT, closing socket...\n");
    close(sock);
    exit(0);
}

int main(){
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sock);
        return EXIT_FAILURE;
    }

    signal(SIGINT, handle_sigint);

    if(listen(sock, 5) < 0) {
        perror("Listen failed");
        close(sock);
        return EXIT_FAILURE;
    }
    printf("Server listening on port %d\n", SERVER_PORT);
    while(1) {
        int client_sock = accept(sock, NULL, NULL);
        if (client_sock < 0) {
            perror("Accept failed");
            continue;
        }
        printf("Client connected\n");

        ssize_t bytes_received = read(client_sock, buffer, BUFFER_SIZE - 1);
        if (bytes_received < 0) {
            perror("Receive failed");
            close(client_sock);
            continue;
        }
        buffer[bytes_received] = '\0'; // Null-terminate the received data
        printf("Received: %s\n", buffer);

        send(client_sock, "Hello, Client!", sizeof(message), 0);
        printf("Response sent to client\n");

        close(client_sock);
    }
    printf("here");
    close(sock);
    return EXIT_SUCCESS;

}