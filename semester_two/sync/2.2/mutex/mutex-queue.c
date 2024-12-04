#define _GNU_SOURCE
#include <pthread.h>
#include <assert.h>

#include "mutex-queue.h"

void* qmonitor(void* arg) {
	mutex_queue_t* q = (mutex_queue_t*) arg;

	printf("qmonitor: [%d %d %d]\n", getpid(), getppid(), gettid());

	while (1) {
		queue_print_stats(q);
		sleep(1);
	}

	return NULL;
}

mutex_queue_t* mutex_queue_init(int max_count) {
	int err;

	mutex_queue_t* q = malloc(sizeof(mutex_queue_t));
	if (!q) {
		printf("Cannot allocate memory for a queue\n");
		abort();
	}

	q->first = NULL;
	q->last = NULL;
	q->max_count = max_count;
	q->count = 0;

	q->add_attempts = q->get_attempts = 0;
	q->add_count = q->get_count = 0;

	if (pthread_mutex_init(&q->lock, NULL) != 0) {
		printf("Failed to initialize the spinlock\n");
		abort();
	}

	err = pthread_create(&q->qmonitor_tid, NULL, qmonitor, q);
	if (err) {
		printf("mutex_queue_init: pthread_create() failed: %s\n", strerror(err));
		abort();
	}

	return q;
}

void mutex_queue_destroy(mutex_queue_t* q) {
	if (q == NULL) {
		return;
	}

	while (q->first) {
		qnode_t* tmp = q->first;
		q->first = q->first->next;
		free(tmp);
	}

	pthread_cancel(q->qmonitor_tid);
	pthread_join(q->qmonitor_tid, NULL);

	pthread_mutex_destroy(&q->lock);

	free(q);
	q = NULL;
}

int mutex_queue_add(mutex_queue_t* q, int val) {
	q->add_attempts++;

	assert(q->count <= q->max_count);

	pthread_mutex_lock(&q->lock);

	if (q->count == q->max_count) {
		pthread_mutex_unlock(&q->lock);
		return 0;
	}

	qnode_t* new = malloc(sizeof(qnode_t));
	if (!new) {
		printf("Cannot allocate memory for new node\n");
		pthread_mutex_unlock(&q->lock);
		abort();
	}

	new->val = val;
	new->next = NULL;

	if (!q->first)
		q->first = q->last = new;
	else {
		q->last->next = new;
		q->last = q->last->next;
	}

	q->count++;
	q->add_count++;

	pthread_mutex_unlock(&q->lock);

	return 1;
}

int mutex_queue_get(mutex_queue_t* q, int* val) {
	q->get_attempts++;

	assert(q->count >= 0);

	pthread_mutex_lock(&q->lock);

	if (q->count == 0) {
		pthread_mutex_unlock(&q->lock);
		return 0;
	}

	qnode_t* tmp = q->first;

	*val = tmp->val;
	q->first = q->first->next;

	free(tmp);
	q->count--;
	q->get_count++;

	pthread_mutex_unlock(&q->lock);

	return 1;
}

void queue_print_stats(mutex_queue_t* q) {
	printf("queue stats: current size %d; attempts: (%ld %ld %ld); counts (%ld %ld %ld)\n",
		q->count,
		q->add_attempts, q->get_attempts, q->add_attempts - q->get_attempts,
		q->add_count, q->get_count, q->add_count - q->get_count);
}

