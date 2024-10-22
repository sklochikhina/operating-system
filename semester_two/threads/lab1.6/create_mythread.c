#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include "mythreads.h"

void* mythread(void* arg) {
    int a = 2;
    int b = *(int*)arg;

    int* sum = (int*)malloc(sizeof(int));
    *sum = a + b;
    sleep(2);

    return (void*)sum;
}

int main() {
    int err;
    mythread_struct_t tid1, tid2;

    void* ret;
    int b1 = 4, b2 = 10;

    // creating first thread
    err = mythread_create(&tid1, mythread, &b1);
    if (err == -1) {
        perror("mythread_create");
        return -EXIT_FAILURE;
    }
    printf("main: created thread %d\n", tid1.tid);

    // creating second thread
    err = mythread_create(&tid2, mythread, &b2);
    if (err == -1) {
        perror("mythread_create");
        return -EXIT_FAILURE;
    }
    printf("main: created thread %d\n", tid2.tid);

    // join for first thread
    err = mythread_join(&tid1, &ret);
    if (err == -1) {
        perror("mythread_join");
        return -EXIT_FAILURE;
    }
    printf("main: thread1 returned value : %d\n", *(int*)ret);
    free(ret);

    // join for second thread
    err = mythread_join(&tid2, &ret);
    if (err == -1) {
        perror("mythread_join");
        return -EXIT_FAILURE;
    }
    printf("main: thread2 returned value : %d\n", *(int*)ret);
    free(ret);

    return 0;
}