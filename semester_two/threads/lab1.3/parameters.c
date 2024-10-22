#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <malloc.h>

typedef struct check {
    int a;
    char* str;
} check;

void* mythread(check* obj) {
    printf("mythread [%lu]: a = %d, str = \"%s\"\n", pthread_self(), obj->a, obj->str);
    free(obj);
    return NULL;
}

int main() {
    pthread_t tid;
    int err;

    check* obj = (check*)malloc(sizeof(check));
    obj->a = 10;
    obj->str = "Hello!";

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    err = pthread_create(&tid, &attr, (void *(*)(void *)) mythread, obj);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    pthread_attr_destroy(&attr);

    sleep(1);

    return 0;
}
