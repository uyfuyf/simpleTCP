#include <stdio.h>
#include <arpa/inet.h>  // inet_addr()  sockaddr_in
#include <string.h>     // bzero()
#include <sys/socket.h> // socket
#include <unistd.h>     //
#include <stdlib.h>     //exit()
#include <sys/select.h>
#define BUFFER_SIZE 1024


//select机制实现tcp server
int main(){
    char listen_addr_srt[] = "0.0.0.0";
    size_t listen_addr = inet_addr(listen_addr_srt);
    int port = 8080;
    int server_socket, client_socket;
    struct sockaddr_in server_addr,client_addr;
    socklen_t addr_size;
    char server_buffer[BUFFER_SIZE];

    int msg_length; //传输的消息长度
   
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

    printf("创建TCP服务器成功\n");

    fd_set reads,copy_reads;
    int fd_max,fd_num;
    struct timeval timeout;
    FD_ZERO(&reads);        //初始化清空socket集合
    FD_SET(server_socket,&reads);
    fd_max = server_socket;

    while(1){
        copy_reads = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;

        //无限循环调用select 监视可读事件
        if((fd_num = select(fd_max+1, &copy_reads, 0, 0, &timeout)) == -1){
            perror("select error");
            break;
        }
        if(fd_num == 0){    //如果没有变动的socket
            continue;
        }
        int i=0;
        for(;i<fd_max+1;i++){
            if(FD_ISSET(i,&copy_reads)){
                if(i == server_socket){ //  i==server_socket 创建新连接
                    addr_size = sizeof(client_addr);
                    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
                    printf("%d 连接成功\n",client_socket);
                    char msg[] = "恭喜连接成功";
                    write(client_socket, msg, sizeof(msg));

                    FD_SET(client_socket,&reads);
                    if(fd_max < client_socket){
                        fd_max = client_socket;
                    }
                }else{
                    memset(server_buffer, 0, sizeof(server_buffer));
                    msg_length = read(i, server_buffer, BUFFER_SIZE);
                    if(msg_length == 0){
                        close(i);
                        printf("连接已经关闭: %d\n",i);
                        FD_CLR(i,&reads);   //从read中删除相关信息
                    }else{
                        printf("%d 客户端发送数据%s\n",i,server_buffer);
                        write(i, server_buffer, msg_length);
                    }
                }
            }
        }

        
    }
    return 0;

}
