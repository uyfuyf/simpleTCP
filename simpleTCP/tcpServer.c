#include <stdio.h>
#include <arpa/inet.h>  // inet_addr()  sockaddr_in
#include <string.h>     // bzero()
#include <sys/socket.h> // socket
#include <unistd.h>     //
#include <stdlib.h>     //exit()
#include <sys/wait.h>   //waitpid()

#define BUFFER_SIZE 1024


//多进程实现tcp server
int main(){
    char listen_addr_srt[] = "0.0.0.0";
    size_t listen_addr = inet_addr(listen_addr_srt);
    int port = 8080;
    int server_socket, client_socket;
    struct sockaddr_in server_addr,client_addr;
    socklen_t addr_size;
    char buffer[BUFFER_SIZE];


    int str_length;
    pid_t pid;
    int status = 0;//初始化状态
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


    while(1){
        addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
        printf("%d 连接成功\n",client_socket);
        char msg[] = "恭喜连接成功";
        write(client_socket, msg, sizeof(msg));
        pid = fork();
        if(pid > 0){    //父进程,进入下次循环，读取客户端连接事件
            sleep(1);
            waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED);
            printf("waitpid end\n");
            if(WIFEXITED(status)){      //正常退出
                printf("status = %d\n",WEXITSTATUS(status));
            }
            if(WIFSIGNALED(status)){    //子进程是被信号结束了
                printf("singal status = %d\n",WTERMSIG(status));
            }
            if(WIFSTOPPED(status)){
                printf("stop sig num = %d\n",WSTOPSIG(status));
            }
            if(WIFCONTINUED(status)){
                printf("continue......\n");
            }

        }
        else if (pid == 0){        //子进程，为客户端提供服务
            while(1){
                memset(buffer, 0, sizeof(buffer));
                str_length = read(client_socket, buffer, BUFFER_SIZE);
                if(str_length == 0){
                    close(client_socket);
                    printf("连接已经关闭: %d\n",client_socket);
                    exit(1);
                }
                else{
                    printf("%d 客户端发送数据%s\n",client_socket,buffer);
                    write(client_socket, buffer, str_length);
                }
            }
            break;
        }
        else{
            printf("创建子进程失败\n");
            exit(1);
        }
        
    }
    return 0;

}
