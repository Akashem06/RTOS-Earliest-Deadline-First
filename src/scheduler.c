#include "scheduler.h"

RTOSStatus init_scheduler() {
    current_task_index = 0;
    for (int i = 0; i < MAX_TASKS; i++) {
        task_list[i].state = TASK_SUSPENDED;
        task_list[i].stackPointer = &task_stacks[i][STACK_SIZE - 1];
    }

    // Start tick counter

    return RTOS_OK;
}

RTOSStatus create_task(TaskFunction task_function, uint8_t priority, const char *name) {
    for (int i = 0; i < MAX_TASKS; i++) {
        // If the task stack space is not being used yet
        if (task_list[i].state == TASK_SUSPENDED) {
            task_list[i].taskFunction = task_function;
            task_list[i].priority = priority;
            task_list[i].state = TASK_READY;
            // Copies over name into taskName and states string end using \0
            strncpy(task_list[i].taskName, name, sizeof(task_list[i].taskName));
            task_list[i].taskName[sizeof(task_list[i].taskName) - 1] = '\0';

            // Initialize stack pointer to the top of the stack
            task_list[i].stackPointer = &task_stacks[i][STACK_SIZE - 1];

            // ARM Cortex-M specific: Simulate the stack frame as if an interrupt had occurred
            // Resource: https://interrupt.memfault.com/blog/cortex-m-rtos-context-switching#cortex-m-arm-mcu-features
            
            // Initial xPSR value
            *(task_list[i].stackPointer--) = 0x01000000; // Default xPSR value (Thumb state)
            // Initial PC value
            *(task_list[i].stackPointer--) = (uint32_t)task_function;
            // Initial LR value
            *(task_list[i].stackPointer--) = 0xFFFFFFFD; // Special value indicating task return

            // Initialize R12, R3, R2, R1, and R0 to zero
            for (int j = 0; j < 5; j++) {
                *(task_list[i].stackPointer--) = 0x00000000;
            }

            // Initialize R11-R4 to zero (not necessary, but often done)
            for (int j = 0; j < 8; j++) {
                *(task_list[i].stackPointer--) = 0x00000000;
            }
            
            break;
        }
    }
    return RTOS_OK;
}
