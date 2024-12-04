#ifndef OS__2ND__USER_THREADS_H
#define OS__2ND__USER_THREADS_H

#include <ucontext.h>

#define PAGE 4096
#define STACK_SIZE (PAGE * 8)
#define MAX_USER_THREADS 10

typedef struct user_thread_t {
    int        tid;
    ucontext_t ucontext;
    void*      (*routine)(void *);
    void*      arg;
} user_thread_t;

void uthread_startup(user_thread_t* user_thread);
void switch_uthread();
int uthread_create(user_thread_t* user_thread, void* (*routine)(void *), void* arg);
int start_uthread_dispatcher();

#endif //OS__2ND__USER_THREADS_H
