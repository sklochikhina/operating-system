#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/mman.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define SERVER_PORT 54321
#define BUF_SIZE 1024
#define SIZE sizeof(struct sockaddr_in)
#define CLIENTS_COUNT 5
#define FAIL (-1)

int newsockfd;

int main() {
    int sockfd;
    char buffer[BUF_SIZE];

    struct sockaddr_in server = {AF_INET, SERVER_PORT, INADDR_ANY};

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == FAIL) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if (bind(sockfd, (struct sockaddr *)&server, SIZE) == FAIL) {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, CLIENTS_COUNT) == FAIL) {
        perror("listen");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("TCP server is running and waiting for clients...\n");

    int shm_fd = shm_open("/myshm", O_RDWR | O_CREAT, 0666);
    if (shm_fd == FAIL) {
        perror("shm_open");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd, sizeof(int)) == FAIL) {
        perror("ftruncate");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    int* connections = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    *connections = 0;

    while (1) {
        if (*connections >= CLIENTS_COUNT) {
            if ((newsockfd = accept(sockfd, NULL, NULL)) != FAIL){
                perror("The queue is full! I can't let you here");
                close(newsockfd);
                continue;
            }
        }
        if ((newsockfd = accept(sockfd, NULL, NULL)) == FAIL) {
            perror("accept");
            continue;
        }
        printf("got new connection = %d\n", ++(*connections));
        int pid = fork();
        if (pid == 0) break;
        close(newsockfd);
    }

    close(sockfd);

    while (1) {
        ssize_t recv_len = recv(newsockfd, buffer, BUF_SIZE, 0);
        if (recv_len == 0) {
            --(*connections);
            perror("Connection is broken");
            close(newsockfd);
            exit(EXIT_SUCCESS);
        }
        else if (recv_len == FAIL) {
            --(*connections);
            perror("recv");
            close(newsockfd);
            exit(EXIT_FAILURE);
        }

        ssize_t send_len = send(newsockfd, buffer, BUF_SIZE, 0);
        if (send_len == 0) {
            --(*connections);
            perror("Connection is broken");
            close(newsockfd);
            exit(EXIT_SUCCESS);
        }
        else if (send_len == FAIL) {
            --(*connections);
            perror("send");
            close(newsockfd);
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
