#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

// блокирует получение всех сигналов
void* mythread1(void* args) {
    sigset_t sigset;
    sigfillset(&sigset);
    pthread_sigmask(SIG_BLOCK, &sigset, NULL);

    while (1) {
        sleep(1);
    }

    return NULL;
}

void signal_handler(int signum) {
    printf("Thread 2: Caught signal %d (SIGINT).\n", signum);
}

// принимает сигнал SIGINT при помощи обработчика сигнала
void* mythread2(void* args) {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);

    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sa.sa_mask = set;
    sigaction(SIGINT, &sa, NULL);

    while (1) {
        pause();
    }
    return NULL;
}

// принимает сигнал SIGQUIT при помощи функции sigwait()
void* mythread3(void* args) {
    sigset_t sigset;
    int sig;

    sigemptyset(&sigset);
    sigaddset(&sigset, SIGQUIT);

    if (pthread_sigmask(SIG_BLOCK, &sigset, NULL) != 0) {
        perror("pthread_sigmask");
        pthread_exit(NULL);
    }

    while (1) {
        if (sigwait(&sigset, &sig) != 0) {
            perror("sigwait");
            pthread_exit(NULL);
        }
        printf("Thread 3: Caught signal %d (SIGQUIT).\n", sig);
    }

    return NULL;
}

int main() {
    pthread_t tid1, tid2, tid3;
    int err;

    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGQUIT);
    if (pthread_sigmask(SIG_BLOCK, &sigset, NULL) != 0) {
        perror("pthread_sigmask");
        return -1;
    }

    printf("pid: %d\n", getpid());

    err = pthread_create(&tid1, NULL,  mythread1, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    err = pthread_create(&tid2, NULL,  mythread2, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    err = pthread_create(&tid3, NULL,  mythread3, NULL);
    if (err) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return -1;
    }

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);

    return 0;
}