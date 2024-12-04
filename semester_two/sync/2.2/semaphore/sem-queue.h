#ifndef OS_2ND_H_
#define OS_2ND_H_

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>

typedef struct _QueueNode {
	int val;
	struct _QueueNode* next;
} qnode_t;

typedef struct _Queue {
	qnode_t* first;
	qnode_t* last;

	pthread_t qmonitor_tid;
	pthread_mutex_t lock;
	sem_t empty;
	sem_t full;

	int count;
	int max_count;

	// queue statistics
	long add_attempts;
	long get_attempts;
	long add_count;
	long get_count;
} sem_queue_t;

sem_queue_t* sem_queue_init(int max_count);
void sem_queue_destroy(sem_queue_t *q);
int sem_queue_add(sem_queue_t *q, int val);
int sem_queue_get(sem_queue_t *q, int *val);
void queue_print_stats(sem_queue_t *q);

#endif	// OS_2ND_H_
