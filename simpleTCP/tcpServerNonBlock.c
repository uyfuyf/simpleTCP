#include <stdio.h>
#include <arpa/inet.h>  // inet_addr()  sockaddr_in
#include <string.h>     // bzero()
#include <sys/socket.h> // socket
#include <unistd.h>     //
#include <stdlib.h>     //exit()
// #include <sys/wait.h>   //waitpid()
#include <fcntl.h>      //非阻塞

#define BUFFER_SIZE 1024

int set_non_block(int socket){
    int flags = fcntl(socket, F_GETFL, 0);
    flags |= O_NONBLOCK;
    return fcntl(socket, F_SETFL, flags);
}

//单进程非阻塞式tcp server
int main(){
    char listen_addr_srt[] = "0.0.0.0";
    size_t listen_addr = inet_addr(listen_addr_srt);
    int port = 8080;
    int server_socket, client_socket;
    struct sockaddr_in server_addr,client_addr;
    socklen_t addr_size;
    char buffer[BUFFER_SIZE];

    size_t client_arr[100];     //存储客户端数组
    int client_length = 0;      //记录客户端数量

    int str_length;

    server_socket = socket(PF_INET, SOCK_STREAM, 0);//创建socket套接字
    __bzero(&server_addr, sizeof(server_addr));     //初始化
    server_addr.sin_family = INADDR_ANY;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = listen_addr;

    if(bind(server_socket, (struct sockaddr *)&server_addr,sizeof(server_addr)) == -1){
        printf("绑定失败\n");
        exit(1);
    }
    if(listen(server_socket, 5) == -1){
        printf("监听失败\n");
        exit(1);
    }
    if(set_non_block(server_socket) == -1){
        printf("设置非阻塞失败\n");
        exit(1);
    }

    printf("创建TCP服务器成功\n");


    while(1){
        addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
        printf("after accept\n");
        if(client_socket > 0){
            client_arr[client_length] = client_socket;
            client_length++;
            if(set_non_block(client_socket) == -1){
                printf("设置非阻塞失败\n");
                exit(1);
            }
            printf("%d 连接成功\n",client_socket);
            char msg[] = "恭喜连接成功";
            write(client_socket, msg, sizeof(msg));
        }
        int i=0;
        for(i=0;i<client_length;++i){
            if(client_arr[i] == 0){
                continue;
            }
            memset(&buffer, 0, sizeof(buffer));
            str_length = read(client_arr[i], buffer, BUFFER_SIZE);
            if (str_length == -1){//非阻塞下,无法读取返回-1
                continue;
            }
            if(str_length == 0){    //读取数据完毕关闭套接字
                close(client_arr[i]);
                printf("连接已被关闭: %d\n",client_arr[i]);
                client_arr[i] = 0;
                break;
            } else{
                printf("客户端发送数据: %s\n",buffer);
                write(client_arr[i],buffer,str_length);
            }
        }
        sleep(10);
    }
    return 0;

}
