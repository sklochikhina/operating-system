#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

static int count = 0;

int global = 10;

void* mythread(void* arg) {
    int val1 = 5;
    static int val2 = 7;
    const int val3 = 1;
    count++;
    printf("mythread [%d %d %lu %d]: Hello from mythread%d!\n", getpid(), getppid(), pthread_self(), gettid(), count);
    printf("vars [global: %p, local: %p, static local: %p, const local: %p]\n", &global, &val1, &val2, &val3);
    return NULL;
}

int main() {
    pthread_t tid1, tid2, tid3, tid4, tid5;
    int err;

    printf("main  \t[%d %d %lu %d]: Hello from main!\n", getpid(), getppid(), pthread_self(), gettid());

    err = pthread_create(&tid1, NULL, mythread, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    err = pthread_create(&tid2, NULL, mythread, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    err = pthread_create(&tid3, NULL, mythread, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    err = pthread_create(&tid4, NULL, mythread, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    err = pthread_create(&tid5, NULL, mythread, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    sleep(1);

    printf("main  \t[%lu %lu %lu %lu %lu]", tid1, tid2, tid3, tid4, tid5);

    return 0;
}
