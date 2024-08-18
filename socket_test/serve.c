#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define serverPort 48763


// message buffer
char buf[1024] = {0};



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

    // 將建立的 socket 綁定到 serverAddr 指定的 port
    if (bind(socket_fd, (const struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind socket failed!");
        close(socket_fd);
        exit(0);
    }

    struct sockaddr_in clientAddr;
    int len = sizeof(clientAddr);

    printf("server start\n");

    while (1) {
        // 當有人使用 UDP 協定送資料到 48763 port
        // 會觸發 recvfrom()，並且把來源資料寫入 clientAddr 當中
        if (recvfrom(socket_fd, buf, sizeof(buf), 0, (struct sockaddr *)&clientAddr, &len) < 0) {
            break;
        }
        
        // 收到 exit 指令就關閉 server
        if (strcmp(buf, "exit") == 0) {
            printf("get exit order, closing the server...\n");
            break;
        }
        

        // 顯示資料來源，原本資料，以及修改後的資料
        printf("get message from [%s:%d]: ", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
        
        // 根據 clientAddr 的資訊，回傳至 client 端
        sendto(socket_fd, buf, sizeof(buf), 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr));
        
        // 清空 message buffer
        memset(buf, 0, sizeof(buf));
    }

    // 關閉 socket，並檢查是否關閉成功
    if (close(socket_fd) < 0) {
            perror("close socket failed!");
    }

    return 0;
}
