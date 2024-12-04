#ifndef OS_2ND_H_
#define OS_2ND_H_

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct _QueueNode {
	int val;
	struct _QueueNode* next;
} qnode_t;

typedef struct _Queue {
	qnode_t* first;
	qnode_t* last;

	pthread_t qmonitor_tid;
	pthread_mutex_t lock;

	int count;
	int max_count;

	// queue statistics
	long add_attempts;
	long get_attempts;
	long add_count;
	long get_count;
} mutex_queue_t;

mutex_queue_t* mutex_queue_init(int max_count);
void mutex_queue_destroy(mutex_queue_t *q);
int mutex_queue_add(mutex_queue_t *q, int val);
int mutex_queue_get(mutex_queue_t *q, int *val);
void queue_print_stats(mutex_queue_t *q);

#endif	// OS_2ND_H_
