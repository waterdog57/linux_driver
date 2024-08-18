#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define serverPort 48763


// message buffer
char buf[1024] = {0};
char recvbuf[1024] = {0};



int main(void){

    // server 地址
    struct sockaddr_in serverAddr = {
        .sin_family = AF_INET,           
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(serverPort)
    };


    // 建立 socket
    int socket_fd = socket(PF_INET , SOCK_DGRAM , 0);
    if (socket_fd < 0){
        printf("Fail to create a socket.");
    }

    struct sockaddr_in clientAddr;
    int len = sizeof(clientAddr);

    printf("client start\n");

    while (1) {

        printf("Please input your message: ");
        scanf("%s", buf);
        
        sendto(socket_fd, buf, sizeof(buf), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

        if (recvfrom(socket_fd, buf, sizeof(buf), 0, (struct sockaddr *)&serverAddr, &len) < 0) {
            break;
        }
        
        // 收到 exit 指令就關閉 server
        if (strcmp(buf, "exit") == 0) {
            printf("get exit order, closing the server...\n");
            break;
        }
        
        // 等待 server 回傳轉成大寫的資料
        if (recvfrom(socket_fd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&serverAddr, &len) < 0) {
            printf("recvfrom data from %s:%d, failed!", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));
        }

        // 顯示 server 地址，以及收到的資料
        printf("get receive message from [%s:%d]: %s\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port), recvbuf);
        // 清空 recv buffer
        memset(recvbuf, 0, sizeof(recvbuf));
    }

    // 關閉 socket，並檢查是否關閉成功
    if (close(socket_fd) < 0) {
            perror("close socket failed!");
    }

    return 0;
}
