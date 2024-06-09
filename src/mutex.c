#include "mutex.h"

uint8_t mutex_buffer[MAX_TASKS];

RTOSStatus init_mutex(Mutex* mutex) {
    mutex->locked = false;
    mutex->owner = NULL;
    status_ok_or_return(init_queue(&mutex->waiting_tasks, mutex_buffer, MAX_TASKS));
    return RTOS_OK;
}

RTOSStatus lock_mutex(Mutex* mutex) {
    if (!mutex->locked) {
        mutex->locked = true;
        mutex->owner = &task_list[current_task_index];
        // Include HW Interrupt disable (TASK ENTER CRITICAL SECTION)
        return RTOS_OK;
    } else {
        // Earlier deadline task requests the mutex

        // Handle task requesting mutex

        // Add the current index of the task to the waiting list and block it
        enqueue(&mutex->waiting_tasks, current_task_index);
        task_list[current_task_index].state = TASK_BLOCKED;
    }

    return RTOS_OK;
}

RTOSStatus unlock_mutex(Mutex* mutex) { return RTOS_OK; }
