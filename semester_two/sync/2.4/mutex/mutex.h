#ifndef MUTEX_H
#define MUTEX_H

#include <stdatomic.h>

typedef struct mutex {
    int lock;
    int tid;
} mutex_t;

void mutex_init(mutex_t* m);
void mutex_lock(mutex_t* m);
void mutex_unlock(mutex_t* m);

#endif //MUTEX_H
