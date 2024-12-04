#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>

#include "spinlock-queue.h"

#define COUNT_ITERATION 100000

#define RED "\033[41m"
#define NOCOLOR "\033[0m"

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

void* reader(void* arg) {
    int expected = 0;
    spinlock_queue_t *q = (spinlock_queue_t *) arg;
    printf("reader [%d %d %d]\n", getpid(), getppid(), gettid());

    set_cpu(1);

    while (1) {
        int val = -1;
        int ok = spinlock_queue_get(q, &val);
        if (!ok)
            continue;

        if (expected != val)
            printf(RED"ERROR: get value is %d but expected - %d" NOCOLOR "\n", val, expected);

        expected = val + 1;
    }

    return NULL;
}

void* writer(void* arg) {
    int i = 0;
    spinlock_queue_t *q = (spinlock_queue_t *) arg;
    printf("writer [%d %d %d]\n", getpid(), getppid(), gettid());

    set_cpu(2);

    while (1) {
        //usleep(1);
        int ok = spinlock_queue_add(q, i);
        if (!ok)
            continue;
        i++;
    }

    return NULL;
}

int main(int argc, char** argv) {
    pthread_t tid1, tid2;
    spinlock_queue_t *q;
    int err;

    printf("main [%d %d %d]\n", getpid(), getppid(), gettid());

    q = spinlock_queue_init(strtol(argv[1], NULL, 10));

    err = pthread_create(&tid1, NULL, reader, q);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    sched_yield();

    err = pthread_create(&tid2, NULL, writer, q);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    spinlock_queue_destroy(q);

    return 0;
}