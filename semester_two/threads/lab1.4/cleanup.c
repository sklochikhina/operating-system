#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <malloc.h>

void exit_func(void* arg) {
    free(arg);
}

void* mythread(void* args) {
    char* str = (char*)malloc(sizeof(char) * 12);
    strcpy(str, "hello world\0");
    pthread_cleanup_push(exit_func, str);
    while(1){
        printf("str = \"%s\"\n", str);
    }
    pthread_cleanup_pop(1);
    return NULL;
}

int main() {
    pthread_t tid;
    int err;

    err = pthread_create(&tid, NULL,  mythread, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    sleep(1);

    pthread_cancel(tid);

    return 0;
}