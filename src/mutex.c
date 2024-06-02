#include "mutex.h"

uint8_t mutex_buffer[MAX_TASKS];

RTOSStatus init_mutex(Mutex* mutex) {
    mutex->locked = false;
    mutex->priority = 0;
    mutex->owner = NULL;
    status_ok_or_return(init_queue(&mutex->waiting_tasks, mutex_buffer, MAX_TASKS));
    return RTOS_OK;
}

RTOSStatus lock_mutex(Mutex* mutex) {
    if (!mutex->locked) {
        mutex->locked = true;
        mutex->owner = &task_list[current_task_index];
        mutex->priority = task_list[current_task_index].priority;
        // Include HW Interrupt disable (TASK ENTER CRITICAL SECTION)
        return RTOS_OK;
    } else {
        // Higher priority task requests the mutex
        if (task_list[current_task_index].priority > mutex->priority) {
            // Priority Inheritance
            mutex->owner->priority = task_list[current_task_index].priority;
        }
        // Handle task requesting mutex

        // Add the current index of the waiting task to the list
        enqueue(&mutex->waiting_tasks, current_task_index);
        task_list[current_task_index].state = TASK_BLOCKED;

        
    }

    return RTOS_OK;
}

RTOSStatus unlock_mutex(Mutex* mutex) {
    return RTOS_OK;
}
