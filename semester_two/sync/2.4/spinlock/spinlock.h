#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <stdatomic.h>

typedef int spinlock_t;

void spinlock_init(spinlock_t* s);
void spinlock_lock(spinlock_t* s);
void spinlock_unlock(spinlock_t* s);

#endif //SPINLOCK_H
