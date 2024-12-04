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
	pthread_spinlock_t lock;

	int count;
	int max_count;

	// queue statistics
	long add_attempts;
	long get_attempts;
	long add_count;
	long get_count;
} spinlock_queue_t;

spinlock_queue_t* spinlock_queue_init(int max_count);
void spinlock_queue_destroy(spinlock_queue_t *q);
int spinlock_queue_add(spinlock_queue_t *q, int val);
int spinlock_queue_get(spinlock_queue_t *q, int *val);
void queue_print_stats(spinlock_queue_t *q);

#endif	// OS_2ND_H_
