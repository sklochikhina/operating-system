#define _GNU_SOURCE
#include <stdio.h>
#include <sched.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include "mythreads.h"

int mythread_startup(void* arg) {
    mythread_struct_t* mythread = (mythread_struct_t*)arg;

    printf("thread_startup: start thread function for thread %d\n", mythread->tid);

    mythread->ret = mythread->routine(mythread->arg);
    mythread->finished = 1;

    printf("thread_startup: the thread function finished for the thread %d\n", mythread->tid);

    return 0;
}

int mythread_create(mythread_struct_t* mythread, start_routine_t start_routine, void* arg) {
    static int thread_num = 0;

    printf("mythread_create: creating thread %d\n", thread_num);

    mythread->stack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mythread->stack == MAP_FAILED) {
        perror("mmap");
        return -EXIT_FAILURE;
    }

    mythread->tid = thread_num++;
    mythread->routine = start_routine;
    mythread->arg = arg;
    mythread->finished = 0;

    mythread_struct_t* new_thread = mythread->stack + STACK_SIZE - sizeof(mythread_struct_t);
    *new_thread = *mythread;

    int child_pid = clone(mythread_startup, mythread->stack + STACK_SIZE - sizeof(mythread_struct_t), CLONE_VM | CLONE_SIGHAND | CLONE_THREAD, (void*) new_thread);
    if (child_pid == -1) {
        perror("clone");
        return -EXIT_FAILURE;
    }

    return 0;
}

int mythread_join(mythread_struct_t* mythread, void** ret) {
    mythread_struct_t* new_thread = (mythread_struct_t* )(mythread->stack + STACK_SIZE - sizeof(mythread_struct_t));

    printf("mythread_join: waiting for the thread %d to finish\n", mythread->tid);
    while (!new_thread->finished) {
        usleep(100000);
    }
    printf("mythread_join: the thread %d finished\n", mythread->tid);

    *mythread = *new_thread;
    *ret = mythread->ret;

    int err = munmap(mythread->stack, STACK_SIZE);
    if (err == -1) {
        perror("munmap");
        return -EXIT_FAILURE;
    }

    return 0;
}