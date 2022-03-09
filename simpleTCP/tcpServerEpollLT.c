#include <stdio.h>
#include <arpa/inet.h>  // inet_addr()  sockaddr_in
#include <string.h>     // bzero()
#include <sys/socket.h> // socket
#include <unistd.h>     //
#include <stdlib.h>     //exit()
#include <sys/epoll.h>  //epoll

#define BUFFER_SIZE 1024
#define CLIENT_MAX_SIZE 1024


//Epoll-LT(Level Trigger)实现tcp server
int main(){
    char listen_addr_srt[] = "0.0.0.0";
    size_t listen_addr = inet_addr(listen_addr_srt);
    int port = 8080;
    int server_socket, client_socket;
    struct sockaddr_in server_addr,client_addr;
    socklen_t addr_size;
    char server_buffer[BUFFER_SIZE];    //server缓冲区

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

    struct epoll_event event;   //监听事件
    struct epoll_event wait_event_list[CLIENT_MAX_SIZE];  //监听结果
    int fd[CLIENT_MAX_SIZE];
    // int j = 0;
    int epoll_fd = epoll_create(10);     //?
    if(epoll_fd == -1){
        printf("创建epoll句柄失败\n");
        exit(1);
    }
    event.events = EPOLLIN;
    event.data.fd = server_socket;
    
    int result = epoll_ctl(epoll_fd, EPOLL_CTL_ADD,server_socket,&event);   //将server_socket添加到epoll_fd
    if(result == -1){
        printf("注册epoll事件失败\n");
        exit(1);
    }


    while(1){
        result = epoll_wait(epoll_fd, wait_event_list,CLIENT_MAX_SIZE,-1);//阻塞
        if(result <= 0){
            continue;
        }

        int i=0;
        for(;i<result;i++){
            printf("%d 触发事件 %d\n",wait_event_list[i].data.fd, wait_event_list[i].events);
            
            //server_socket触发事件
            if(wait_event_list[i].data.fd == server_socket 
            && wait_event_list[i].events == EPOLLIN
            && EPOLLIN){
                addr_size = sizeof(client_addr);
                client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
                printf("%d 连接成功\n",client_socket);
                char msg[] = "恭喜连接成功";
                write(client_socket, msg, sizeof(msg));
                
                event.data.fd = client_socket;
                event.events = EPOLLIN;
                result = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &event);
                if(result == -1){
                    printf("注册客户端 epoll 事件失败\n");
                    exit(1);
                }
                continue;
            }

            //客户端触发事件
            if((wait_event_list[i].events & EPOLLIN)       //可读
            || (wait_event_list[i].events & EPOLLERR)){    //或发生错误
                memset(server_buffer, 0, sizeof(server_buffer));
                msg_length = read(wait_event_list[i].data.fd, server_buffer, BUFFER_SIZE);
                if(msg_length == 0){
                        close(wait_event_list[i].data.fd);
                        event.data.fd = wait_event_list[i].data.fd;
                        printf("连接已经关闭: %d\n",wait_event_list[i].data.fd);
                }else{
                    printf("%d 客户端发送数据%s\n",wait_event_list[i].data.fd,server_buffer);
                    write(wait_event_list[i].data.fd, server_buffer, msg_length);
                }
            }
        }
    }
    return 0;

}
