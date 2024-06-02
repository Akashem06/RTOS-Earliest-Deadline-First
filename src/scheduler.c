#include "scheduler.h"

TaskControlBlock task_list[MAX_TASKS];
uint8_t task_stacks[MAX_TASKS][STACK_SIZE];

void init_scheduler() {
    for (int i = 0; i < MAX_TASKS; i++) {
        task_list[i].state = TASK_SUSPENDED;
        task_list[i].stackPointer = &task_stacks[i][STACK_SIZE - 1];
    }
}

void create_task(TaskFunction task_function, uint8_t priority, const char *name) {
    for (int i = 0; i < MAX_TASKS; i++) {
        if (task_list[i].state == TASK_SUSPENDED) {
            task_list[i].taskFunction = task_function;
            task_list[i].priority = priority;
            task_list[i].state = TASK_READY;
            // Copies over name into taskName and states string end using \0
            strncpy(task_list[i].taskName, name, sizeof(task_list[i].taskName));
            task_list[i].taskName[sizeof(task_list[i].taskName) - 1] = '\0';

            task_list[i].stackPointer = &task_stacks[i][STACK_SIZE - 1];

            break;
        }
    }
}
