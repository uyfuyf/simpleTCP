#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
 
#define BUFFSIZE 1024
 
 
int main(int argc, char **argv) {
    int client_socket, msg_length;
    char client_buffer[BUFFSIZE];
    struct sockaddr_in server_addr;
 
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    bzero(&(server_addr.sin_zero), 8);

    while ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("Socket创建失败");
        sleep(1);
    }
 
    while (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        printf("连接失败\n");
        sleep(1);
    }
    printf("连接成功\n");
 
    msg_length = read(client_socket, client_buffer, BUFFSIZE);
    client_buffer[msg_length] = '\0';
    printf("服务端发送数据:%s\n", client_buffer);
 
    while (1) {
        printf("输入:\n");
        scanf("%s", client_buffer);
        write(client_socket, client_buffer, strlen(client_buffer));
        msg_length = read(client_socket, client_buffer, BUFFSIZE);
        client_buffer[msg_length] = '\0';
        printf("服务端发送数据:%s\n", client_buffer);
    }
 
    close(client_socket);
    return 0;
}