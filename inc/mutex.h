#ifndef MUTEX_H
#define MUTEX_H

#include "task.h"

typedef struct {
    bool locked;
    bool original_polarity;
    TaskControlBlock *owner;
} Mutex;

void init_mutex(Mutex* mutex);
bool lock_mutex(Mutex* mutex);
void unlock_mutex(Mutex* mutex);

#endif