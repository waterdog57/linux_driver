#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>


#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

int sock;
uint8_t message[] = "Hello, Server!";

void handle_sigint(int sig) {
    printf("\nCaught SIGINT, closing socket...\n");
    close(sock);
    exit(0);
}



int main() {
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    signal(SIGINT, handle_sigint);

    if( connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr))<0) {
        perror("Connect failed");
        close(sock);
        return EXIT_FAILURE;
    }

    send(sock, message, strlen(message), 0);
    printf("Message sent to server\n");


    while(1){
        read(sock, buffer, BUFFER_SIZE);
        printf("Received: %s\n", buffer);
        memset(buffer, 0, BUFFER_SIZE); // Clear the buffer for the next message
        break;
    }

    close(sock);
    return EXIT_SUCCESS;

}