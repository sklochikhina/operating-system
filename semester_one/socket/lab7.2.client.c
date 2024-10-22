#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SERVER_PORT 54321
#define SERVER_IP "127.0.0.1"
#define BUF_SIZE 1024
#define SIZE sizeof(struct sockaddr_in)
#define FAIL (-1)

int main() {
    int sockfd;
    char buffer[BUF_SIZE];

    struct sockaddr_in server = {AF_INET, SERVER_PORT};
    server.sin_addr.s_addr = inet_addr(SERVER_IP);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == FAIL) {
        perror("socket");
        exit(1);
    }

    if (connect(sockfd, (struct sockaddr*)&server, SIZE) == FAIL) {
        perror("connect");
        exit(1);
    }

    while (1) {
        printf("Enter message: ");
        memset(buffer, 0, BUF_SIZE);
        fgets(buffer, BUF_SIZE, stdin);

        ssize_t send_len = send(sockfd, buffer, BUF_SIZE, 0);
        if (send_len == 0) {
            perror("Connection is broken: send");
            close(sockfd);
            exit(0);
        }
        else if (send_len == FAIL) {
            perror("send");
            close(sockfd);
            exit(1);
        }

        ssize_t recv_len = recv(sockfd, buffer, BUF_SIZE, 0);
        if (recv_len == 0) {
            perror("Connection is broken: recv");
            close(sockfd);
            exit(0);
        }
        else if (recv_len == FAIL) {
            perror("recv");
            close(sockfd);
            exit(1);
        }

        printf("Received from server: %s\n", buffer);
    }

    close(sockfd);

    return 0;
}