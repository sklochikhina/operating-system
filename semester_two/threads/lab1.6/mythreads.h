#ifndef OS__2ND__MYTHREADS_H
#define OS__2ND__MYTHREADS_H

#define PAGE 4096
#define STACK_SIZE (PAGE * 8)

typedef void*(*start_routine_t)(void*);

typedef struct _mythread {
    int              tid;
    void*            stack;
    start_routine_t  routine;
    void*            arg;
    void*            ret;
    int              finished;
} mythread_struct_t;

int mythread_startup(void* arg);
int mythread_create(mythread_struct_t* mythread, start_routine_t start_routine, void* arg);
int mythread_join(mythread_struct_t* mythread, void** ret);

#endif //OS__2ND__MYTHREADS_H
