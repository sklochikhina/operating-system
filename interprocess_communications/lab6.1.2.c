#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SIZE 128
#define PAGE_SIZE 4096
#define shared_file "./shared-file"

int main(int argc, char* argv[]) {
    int sh_fd;
    void* addr_region;
    char* buff;
    char val = 0;

    sh_fd = open(shared_file, O_RDWR |O_CREAT, 0660);

    addr_region = mmap(NULL, PAGE_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, sh_fd, 0);
    close(sh_fd);

    buff = (char*) addr_region;
    while (1)
        for (int i = 0; i < SIZE / sizeof(val); i++) {
            printf("%d ", buff[i]);
            fflush(stdout);
            sleep(1);
            if (i != 0 && buff[i] - 1 != buff[i - 1]) {
                perror("\nError: it shouldn't have happened!\n");
                munmap(addr_region, PAGE_SIZE);
                return -1;
            }
        }

    return 0;
}