#ifndef TASK_H
#define TASK_H

#include <cstdint>

typedef enum {
    TASK_READY,
    TASK_RUNNING,
    TASK_BLOCKED,
    TASK_SUSPENDED
} TaskState;

typedef void (*TaskFunction)(void);

typedef struct {
    TaskFunction taskFunction;  // Task function pointer
    uint32_t *stackPointer;     // Pointer to the task's stack
    TaskState state;            // Current state of the task
    uint8_t priority;           // Task priority
    uint32_t delay;             // Delay for task to unblock (for time-delayed tasks)
    char taskName[16];          // Task name for debugging
} TaskControlBlock;

#define MAX_TASKS 10
#define STACK_SIZE 1024

#endif