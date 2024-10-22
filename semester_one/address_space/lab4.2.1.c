#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
    printf("pid: %d\n", getpid());
    sleep(1);
    execv(argv[0], argv);

    printf("!!!!!!!!!!\n");

    return 0;
}