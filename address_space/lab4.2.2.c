#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>

#define PAGE_SIZE 4096

void sigsegv_handler(int signal){
    perror("Received SIGSEGV signal\n");
    exit(signal);
}

void allocate_on_stack() {
    char buff[PAGE_SIZE];
    sleep(1);
    allocate_on_stack();
}

void allocate_on_heap(int count) {
    if (count == 1) return;
    char* buff = (char*)malloc(PAGE_SIZE * sizeof(char));
    sleep(1);
    allocate_on_heap(--count);
    free(buff);
}

void allocate_address_region() {
    char* addr_region = mmap(NULL, 10 * PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    printf("Created address region: %p\n", &addr_region[0]);
    sleep(10);

    mprotect(addr_region, 10 * PAGE_SIZE, PROT_NONE);
    printf("Changed memory protection: PROT_NONE\n");
    //printf("%c\n", addr_region[0]);
    //*addr_region = 'a';
    signal(SIGSEGV, sigsegv_handler);
    sleep(10);

    munmap(addr_region + 3 * PAGE_SIZE, 2 * PAGE_SIZE);
    printf("Deallocated address region\n");
    sleep(10);
}


int main(int argc, char **argv) {
    printf("pid: %d\n", getpid());
    sleep(10);

    allocate_on_stack();
    //allocate_on_heap(20);
    //allocate_address_region();

    return EXIT_SUCCESS;
}
