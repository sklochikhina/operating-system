#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    int pid = fork();

    if (pid == 0) {
        int child = fork();

        if (child == 0) {
            sleep(5);
            printf("\nDaughter became an orphan\n");
            printf("\nDaughter PID:            %d", getpid());
            printf("\nDaughter parent PID:     %d", getppid());
        }
        else {
            sleep(1);

            printf("\nMum PID:            %d", getpid());
            printf("\nMum parent PID:     %d", getppid());
            printf("\n\nMum became a zombi\n");

            exit(5);
        }
    }
    else {
        printf("Grand mum PID:          %d\n", getpid());
        sleep(10);
    }

    return EXIT_SUCCESS;
}