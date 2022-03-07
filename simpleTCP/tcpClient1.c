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
    int clientSocketFd, sendByteCount;
    char receiveBuff[BUFFSIZE];
    struct sockaddr_in serverAddress;
 
    while ((clientSocketFd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket fail");
        sleep(1);
    }
 
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    bzero(&(serverAddress.sin_zero), 8);
 
    while (connect(clientSocketFd, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr)) == -1) {
        perror("Connect fail");
        sleep(1);
    }
    printf("Connected server]\n");
 
    sendByteCount = recv(clientSocketFd, receiveBuff, BUFFSIZE, 0);
    receiveBuff[sendByteCount] = '\0';
    printf("receive from server:%s\n", receiveBuff);
 
    while (1) {
        printf("Enter something:\n");
        scanf("%s", receiveBuff);
        send(clientSocketFd, receiveBuff, strlen(receiveBuff), 0);
        sendByteCount = recv(clientSocketFd, receiveBuff, BUFFSIZE, 0);
        receiveBuff[sendByteCount] = '\0';
        printf("Receive from server:%s\n", receiveBuff);
    }
 
    close(clientSocketFd);
    return 0;
}