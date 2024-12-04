#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "user_threads.h"

user_thread_t user_threads[MAX_USER_THREADS];
int user_threads_count = 0;
int curr_thread_num = 0;

void uthread_startup(user_thread_t* uthread) {
    uthread->routine(uthread->arg);
    free(uthread->ucontext.uc_stack.ss_sp);
}

int uthread_create(user_thread_t* uthread, void* (*routine)(void* ), void* arg) {
    static int uthread_tid = 0;

    if (user_threads_count >= MAX_USER_THREADS) {
        perror("user_thread_create: Too many threads\n");
        return -EXIT_FAILURE;
    }

    void* stack = malloc(STACK_SIZE);
    if (stack == NULL) {
        perror("malloc");
        return -EXIT_FAILURE;
    }

    user_thread_t* new_uthread = (user_thread_t*) (stack + STACK_SIZE - sizeof(user_thread_t));
    new_uthread->tid = uthread_tid++;
    new_uthread->routine = routine;
    new_uthread->arg = arg;

    if (getcontext(&new_uthread->ucontext) == -1) {
        perror("getcontext");
        return -1;
    }

    new_uthread->ucontext.uc_stack.ss_sp = stack;
    new_uthread->ucontext.uc_stack.ss_size = STACK_SIZE - sizeof(user_thread_t);
    
    makecontext(&new_uthread->ucontext, (void (*)()) uthread_startup, 1, new_uthread);

    user_threads[user_threads_count] = *new_uthread;
    user_threads_count++;

    *uthread = *new_uthread;
    return 0;
}

void switch_uthread() {
    ucontext_t* curr_cxt = &user_threads[curr_thread_num].ucontext;
    int curr_tid = user_threads[curr_thread_num].tid;

    curr_thread_num = (curr_thread_num + 1) % user_threads_count;

    ucontext_t* next_cxt = &user_threads[curr_thread_num].ucontext;
    int next_tid = user_threads[curr_thread_num].tid;

    printf("switch_uthread: Switch user thread: From %d to %d\n", curr_tid, next_tid);
    alarm(1);

    int err = swapcontext(curr_cxt, next_cxt);
    if (err == -1) {
        perror("swapcontext");
        exit(1);
    }
}

int start_uthread_dispatcher() {
    if (user_threads_count == 0) {
        return 0;
    }

    struct sigaction sa;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGALRM);
    sa.sa_mask = set;
    sa.sa_handler = switch_uthread;

    int err = sigaction(SIGALRM, &sa, NULL);
    if (err == -1) {
        perror("sigaction");
        return -1;
    }

    printf("start_uthread_dispatcher: Start scheduler\n");
    alarm(1);

    err = setcontext(&user_threads[0].ucontext);
    if (err == -1) {
        perror("setcontext");
        return -1;
    }

    return 0;
}