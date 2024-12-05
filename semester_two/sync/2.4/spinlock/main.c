#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include "spinlock.h"

#define COUNT_ITERATION 100000

int counter = 0;

void set_cpu(int n) {
    int err;
    cpu_set_t cpuset;
    pthread_t tid = pthread_self();

    CPU_ZERO(&cpuset);
    CPU_SET(n, &cpuset);

    err = pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpuset);
    if (err) {
        printf("set_cpu [%d %d %d]: pthread_setaffinity failed for cpu %d\n", getpid(), getppid(), gettid(), n);
        return;
    }

    printf("set_cpu [%d %d %d]: set cpu %d\n", getpid(), getppid(), gettid(), n);
}

void* routine1(void* args) {
    set_cpu(2);
    spinlock_t* s = (spinlock_t*) args;
    for (int i = 0; i < COUNT_ITERATION; i++) {
        spinlock_lock(s);
        ++counter;
        spinlock_unlock(s);
        printf("routine1 [%d %d %d]: count iteration %d\n", getpid(), getppid(), gettid(), i);
    }

    return NULL;
}

void* routine2(void* args) {
    set_cpu(1);
    spinlock_t* s = (spinlock_t*) args;
    for (int i = 0; i < COUNT_ITERATION; i++) {
        spinlock_lock(s);
        ++counter;
        spinlock_unlock(s);
        printf("routine2 [%d %d %d]: count iteration %d\n", getpid(), getppid(), gettid(), i);
    }
    return NULL;
}

int main() {
    spinlock_t s;
    spinlock_init(&s);

    pthread_t tid1, tid2;
    int err = pthread_create(&tid1, NULL, routine1, &s);
    if (err) {
        printf("main [%d %d %d]: pthread_create: %s\n", getpid(), getppid(), gettid(), strerror(err));
        return EXIT_FAILURE;
    }

    err = pthread_create(&tid2, NULL, routine2, &s);
    if (err) {
        printf("main [%d %d %d]: pthread_create: %s\n", getpid(), getppid(), gettid(), strerror(err));
        return EXIT_FAILURE;
    }

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    printf("counter %d\n", counter);
    return EXIT_SUCCESS;
}