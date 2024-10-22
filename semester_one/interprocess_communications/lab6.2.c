#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define ERROR (-1)
#define SUCCESS 0
#define SIZE 128

int main(int argc, char* argv[]) {
    int pid;
    int pipefd[2];
    unsigned int val = 0;

    if (pipe(pipefd) == -1) {
        perror("pipe() failed!\n");
        return ERROR;
    }

    pid = fork();

    if (pid == 0) {
        unsigned int vals[SIZE];
        close(pipefd[1]);
        while(1) {
            for (int i = 0; i < SIZE; i++) {
                read(pipefd[0], vals, sizeof(vals));
                printf("Child received: '%u'\n", vals[i]);
                sleep(1);
            }
        }
        close(pipefd[0]);
    }
    else if (pid > 0) {
        unsigned int vals[SIZE];
        close(pipefd[0]);
        while(1) {
            for (int i = 0; i < SIZE; i++) {
                vals[i] = val++;
                printf("Parent sending: '%u'\n", vals[i]);
                write(pipefd[1], vals, sizeof(vals));
                sleep(1);
            }
        }
        close(pipefd[1]);
    }
    else {
        perror("fork() failed!\n");
        return ERROR;
    }

    return SUCCESS;
}