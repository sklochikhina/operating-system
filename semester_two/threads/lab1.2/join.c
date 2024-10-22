#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <malloc.h>

void* mythread_num(void *arg) {
    int* result = (int*)malloc(sizeof(int));
    *result = 42;
    pthread_exit(result);
    return NULL;
}

void* mythread_string(void *arg) {
    char* str = "Hello, World!";
    pthread_exit(str);
    return NULL;
}

int main() {
    pthread_t tid;
    int err;

    int* res;
    char* str;

    err = pthread_create(&tid, NULL, mythread_num, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    err = pthread_join(tid, (void**)&res);
    if (err) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
        return -1;
    }

    printf("main: result = %d\n", *res);
    free(res);

// -------------------------------------------------------------------------------

    err = pthread_create(&tid, NULL, mythread_string, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    err = pthread_join(tid, (void**)&str);
    if (err) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
        return -1;
    }

    printf("main: string = %s\n", str);

    return 0;
}
