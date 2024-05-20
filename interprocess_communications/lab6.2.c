#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define ERROR (-1)
#define SUCCESS 0
#define SIZE 128

int main(int argc, char* argv[]) {
    int pid;
    int pipefd[2];
    int err;

    err = pipe(pipefd);
    if (err == -1) {
        perror("pipe() failed!\n");
        return ERROR;
    }

    pid = fork();

    if (pid == 0) {
        char buff[SIZE];
        close(pipefd[1]);
        while(1) {
            read(pipefd[0], buff, sizeof(buff));
            printf("Child received: \"%s\"\n", buff);
            sleep(1);
        }
        close(pipefd[0]);
    }
    else if (pid > 0) {
        char buff[] = "Hello from parent!\n";
        close(pipefd[0]);
        while(1) {
            printf("Parent sending: \"%s\"\n", buff);
            write(pipefd[1], buff, sizeof(buff));
            sleep(1);
        }
        close(pipefd[1]);
    }
    else {
        perror("fork() failed!\n");
        return ERROR;
    }

    return SUCCESS;
}