#include "spinlock.h"

#include <stdbool.h>

#define STATUS_LOCK 0
#define STATUS_UNLOCK 1

void spinlock_init(spinlock_t* s){
    *s = STATUS_UNLOCK;
}

void spinlock_lock(spinlock_t* s) {
    while(1) {
        int expected = STATUS_UNLOCK;
        if (atomic_compare_exchange_strong(s, &expected, STATUS_LOCK))
            break;
    }
}

void spinlock_unlock(spinlock_t* s) {
    int expected = STATUS_LOCK;
    atomic_compare_exchange_strong(s, &expected, STATUS_UNLOCK);
}