#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

void* mythread(void* args) {
    int count = 0;
    while(1){
        //printf("mythread [%lu]\n", pthread_self());
        count++;
        pthread_testcancel();
    }
    return NULL;
}

int main() {
    pthread_t tid;
    int err;

    err = pthread_create(&tid, NULL,  mythread, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    sleep(1);

    pthread_cancel(tid);
    pthread_join(tid, NULL);

    return 0;
}
