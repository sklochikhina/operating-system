#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "user_threads.h"

void* routine1(void* arg) {
    int counter = 0;
    while (1) {
        printf("routine1: %s: Counter %d\n", (char*)arg, counter++);
        usleep(200000);
    }
}

void* routine2(void* arg) {
    int counter = 0;
    while (1) {
        printf("routine1: %s: Counter %d\n", (char*)arg, counter++);
        usleep(300000);
    }
}

void* routine3(void* arg) {
    int counter = 0;
    while (1) {
        printf("routine1: %s: Counter %d\n", (char*)arg, counter++);
        usleep(400000);
    }
}

int main() {
    printf("main: pid %d\n", getpid());

    user_thread_t user_thread1, user_thread2, user_thread3;

    int err = uthread_create(&user_thread1, routine1, "Thread 0");
    if (err == -1) {
        perror("uthread_create");
        return EXIT_FAILURE;
    }
    printf("Created thread %d\n", user_thread1.tid);

    err = uthread_create(&user_thread2, routine2, "Thread 1");
    if (err == -1) {
        perror("uthread_create");
        return EXIT_FAILURE;
    }
    printf("Created thread %d\n", user_thread2.tid);

    err = uthread_create(&user_thread3, routine3, "Thread 2");
    if (err == -1) {
        perror("uthread_create");
        return EXIT_FAILURE;
    }
    printf("Created thread %d\n", user_thread3.tid);

    err = start_uthread_dispatcher();
    if (err == -1) {
        perror("start_uthread_dispatcher");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}