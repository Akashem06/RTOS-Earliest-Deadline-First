#ifndef MUTEX_H
#define MUTEX_H

#include <stdbool.h>
#include <stddef.h>

#include "queue.h"
#include "rtos_status.h"
#include "task.h"

typedef struct {
    bool locked;
    uint8_t priority;
    TaskControlBlock* owner;
    Queue waiting_tasks;
} Mutex;

RTOSStatus init_mutex(Mutex* mutex);
RTOSStatus lock_mutex(Mutex* mutex);
RTOSStatus unlock_mutex(Mutex* mutex);

#endif