#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345
#define SIZE sizeof(struct sockaddr_in)
#define FAIL (-1)

int main() {
    int sockfd;
    char buffer[MAX_BUFFER_SIZE];

    struct sockaddr_in client = {AF_INET, INADDR_ANY, INADDR_ANY};
    struct sockaddr_in server = {AF_INET, SERVER_PORT};
    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    int serv_len = SIZE;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == FAIL) {
        perror("socket");
        exit(1);
    }

    if (bind(sockfd, (struct sockaddr*)&client, SIZE) == FAIL) {
        perror("bind");
        close(sockfd);
        exit(1);
    }

    while (1) {
        printf("Enter message (or 'q' for exit): ");
        fgets(buffer, MAX_BUFFER_SIZE, stdin);

        ssize_t send_len = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*)&server, SIZE);
        if (send_len == FAIL) {
            perror("sendto");
            close(sockfd);
            exit(1);
        }

        if (buffer[0] == 'q')
            break;

        ssize_t recv_len = recvfrom(sockfd, buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr*)&server, &serv_len);
        if (recv_len == FAIL) {
            perror("recvfrom");
            close(sockfd);
            exit(1);
        }

        printf("Received from server: %s\n", buffer);
    }

    close(sockfd);

    return 0;
}