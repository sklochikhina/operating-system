#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int global_var = 1;

int main() {
    int local_var = 2;

    printf("Parent PID:         %d\n", getpid());
    printf("Parent global_var:  %p %d\n", &global_var, global_var);
    printf("Parent local_var:   %p %d\n", &local_var, local_var);

    int pid = fork();

    if (pid == 0) {
        printf("Child PID:          %d\n", getpid());
        printf("Child PPID:         %d\n", getppid());

        printf("Child global_var:   %p %d\n", &global_var, global_var);
        printf("Child local_var:    %p %d\n", &local_var, local_var);

        global_var = 3;
        local_var = 4;
        printf("Child global_var:   %p %d\n", &global_var, global_var);
        printf("Child local_var:    %p %d\n", &local_var, local_var);

        return 5;
    }
    else if (pid > 0){
        printf("Parent global_var:  %p %d\n", &global_var, global_var);
        printf("Parent local_var:   %p %d\n", &local_var, local_var);

        printf("Child process became a zombi\n");
        sleep(10);
    }
    else {
        perror("fork");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}