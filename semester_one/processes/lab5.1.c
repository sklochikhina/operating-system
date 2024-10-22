#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int global_var = 1;

int main() {
    int local_var = 2;

    printf("Parent pid:         %d\n", getpid());
    printf("Parent global_var:  %p %d\n", &global_var, global_var);
    printf("Parent local_var:   %p %d\n", &local_var, local_var);

    int pid = fork();

    if (pid == 0) {
        printf("Child pid:          %d\n", getpid());
        printf("Child ppid:         %d\n", getppid());

        printf("Child global_var:   %p %d\n", &global_var, global_var);
        printf("Child local_var:    %p %d\n", &local_var, local_var);

        global_var = 3;
        local_var = 4;
        printf("Child global_var:   %p %d\n", &global_var, global_var);
        printf("Child local_var:    %p %d\n", &local_var, local_var);

        exit(5);
    }
    else if (pid > 0){
        sleep(15);

        printf("Parent global_var:  %p %d\n", &global_var, global_var);
        printf("Parent local_var:   %p %d\n", &local_var, local_var);

        int status;
        wait(&status);

        if (WIFSIGNALED(status))
            printf("Parent: child terminated by signal %d\n\n", WTERMSIG(status));
        printf("Parent: child terminated with code %d\n\n", WEXITSTATUS(status));
    }
    else {
        perror("fork");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}