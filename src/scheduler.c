#include "scheduler.h"

uint32_t earliest_deadline = UINT32_MAX;
TaskControlBlock task_list[MAX_TASKS];
uint32_t task_stacks[MAX_TASKS][STACK_SIZE];
uint8_t current_task_index;
uint32_t system_time;

RTOSStatus init_scheduler() {
    current_task_index = 0;
    earliest_deadline = UINT32_MAX;
    system_time = 0;

    for (int i = 0; i < MAX_TASKS; i++) {
        task_list[i].state = TASK_SUSPENDED;
        task_list[i].stackPointer = &task_stacks[i][STACK_SIZE - 1];
        task_list[i].deadline = UINT32_MAX;
        task_list[i].period = 0;
    }

    // Start tick counter

    return RTOS_OK;
}

RTOSStatus create_task(TaskFunction task_function, uint32_t period, uint32_t deadline, const char *name) {
    for (int i = 0; i < MAX_TASKS; i++) {
        // If the task stack space is not being used yet
        if (task_list[i].state == TASK_SUSPENDED) {
            task_list[i].taskFunction = task_function;
            task_list[i].period = period;
            task_list[i].deadline = deadline;
            task_list[i].state = TASK_READY;

            // Copies over name into taskName and states string end using \0
            strncpy(task_list[i].taskName, name, sizeof(task_list[i].taskName));
            task_list[i].taskName[sizeof(task_list[i].taskName) - 1] = '\0';

            // Initialize stack pointer to the top of the stack
            task_list[i].stackPointer = &task_stacks[i][STACK_SIZE - 1];

            // ARM Cortex-M specific: Simulate the stack frame as if an interrupt had occurred
            // Resource: https://interrupt.memfault.com/blog/cortex-m-rtos-context-switching#cortex-m-arm-mcu-features

            // Initial xPSR value
            *(task_list[i].stackPointer--) = 0x01000000;  // Default xPSR value (Thumb state)
            // Initial PC value (Program Coutner register which holds next instruction to run)
            *(task_list[i].stackPointer--) = (uintptr_t)task_function;
            // Initial LR value (Return address)
            *(task_list[i].stackPointer--) = 0xFFFFFFFD;  // Special value indicating task return

            // Initialize R12, R3, R2, R1, and R0 to zero
            for (int j = 0; j < 5; j++) {
                *(task_list[i].stackPointer--) = 0x00000000;
            }

            // Initialize R11-R4 to zero (not necessary, but often done)
            for (int j = 0; j < 8; j++) {
                *(task_list[i].stackPointer--) = 0x00000000;
            }

            return RTOS_OK;
        }
    }
    return RTOS_INTERNAL_ERROR;
}

RTOSStatus start_scheduler() {
    for (int i = 0; i < MAX_TASKS; i++) {
        if (task_list[i].state == TASK_READY && task_list[i].deadline < earliest_deadline) {
            current_task_index = i;
            earliest_deadline = task_list[i].deadline;
            break;
        }
    }

    if (earliest_deadline != UINT32_MAX) {
        task_list[current_task_index].state = TASK_RUNNING;
        // Directly call the task function for simulation
        task_list[current_task_index].taskFunction();
    }

    return RTOS_OK;
}

// ISR for hardware timer ticks
RTOSStatus tick_handler() {
    system_time = (system_time + 1) % UINT32_MAX;
    uint8_t next_task_index = current_task_index;

    for (int i = 0; i < MAX_TASKS; i++) {
        if (task_list[i].state == TASK_READY && task_list[i].deadline < earliest_deadline) {
            earliest_deadline = task_list[i].deadline;
            next_task_index = i;
        }
    }

    if (next_task_index != current_task_index) {
        task_list[current_task_index].state = TASK_READY;
        task_list[next_task_index].state = TASK_RUNNING;

        current_task_index = next_task_index;

        task_list[current_task_index].taskFunction();
    }
    return RTOS_OK;
}

RTOSStatus update_task_deadlines() {
    for (int i = 0; i < MAX_TASKS; i++) {
        if (task_list[i].state == TASK_READY || task_list[i].state == TASK_RUNNING) {
            // Update the deadline if the task has a period set
            if (task_list[i].period > 0) {
                task_list[i].deadline = system_time + task_list[i].period;
            }
        }
    }

    return RTOS_OK;
}
