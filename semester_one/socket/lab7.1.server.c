#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_BUFFER_SIZE 1024
#define SERVER_PORT 12345
#define SIZE sizeof(struct sockaddr_in)
#define FAIL (-1)

int main() {
    int sockfd;
    char buffer[MAX_BUFFER_SIZE];

    struct sockaddr_in server = {AF_INET, SERVER_PORT, INADDR_ANY};
    struct sockaddr_in client;
    int client_len = SIZE;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == FAIL) {
        perror("socket");
        exit(1);
    }

    if (bind(sockfd, (struct sockaddr*)&server, SIZE) == FAIL) {
        perror("bind");
        close(sockfd);
        exit(1);
    }

    printf("UDP server is running and waiting for clients...\n");

    while (1) {
        ssize_t recv_len = recvfrom(sockfd, buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr*)&client, &client_len);
        if (recv_len == FAIL) {
            perror("recvfrom");
            close(sockfd);
            exit(1);
        }

        ssize_t send_len = sendto(sockfd, buffer, recv_len, 0, (struct sockaddr*)&client, client_len);
        if (send_len == FAIL) {
            perror("sendto");
            close(sockfd);
            exit(1);
        }
    }

    close(sockfd);
    return 0;
}