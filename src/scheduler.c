#include "scheduler.h"

#include <stdio.h>

uint32_t earliest_deadline = UINT32_MAX;

uint8_t active_tasks = 0;

TaskControlBlock task_list[MAX_TASKS];
uint32_t task_stacks[MAX_TASKS][STACK_SIZE];
uint8_t current_task_index;
uint32_t system_time;

void edf_schedule(void) {
    uint32_t min_deadline = task_list[current_task_index].deadline;
    uint8_t next_task_index = current_task_index;

    for (uint8_t i = 0; i < active_tasks; i++) {
        if (task_list[i].state == TASK_READY && task_list[i].deadline < min_deadline) {
            min_deadline = task_list[i].deadline;
            next_task_index = i;
        }

        // Deadline updating
        if (task_list[i].deadline == 0) {
            // Prevent dividing by 0
            task_list[i].deadline = (task_list[i].deadline + task_list[i].period) % UINT32_MAX;
        } else if (task_list[i].state == TASK_READY && system_time % task_list[i].deadline == 0) {
            // The deadline has been reached, and the task must run
            task_list[i].deadline = (task_list[i].deadline + task_list[i].period) % UINT32_MAX;
        }
    }

    // Update the task state and switch context
    if (next_task_index != current_task_index) {
        task_list[current_task_index].state = TASK_READY;
        task_list[next_task_index].state = TASK_RUNNING;
        current_task_index = next_task_index;
    }

#ifdef ARM
    // Set the Process Stack Pointer to the stack pointer of the next task
    __set_PSP((uint32_t)task_list[current_task_index].stackPointer);
#elif x86
    task_list[current_task_index].taskFunction(NULL);
#endif
}

// cppcheck-suppress unmatchedSuppression
// cppcheck-suppress ctuArrayIndex
// cppcheck-suppress constParameter
void SVC_Handler_Main(unsigned int *svc_args) {
    unsigned int svc_number = -1;
    uint32_t min_deadline = UINT32_MAX;

#ifdef ARM
    /*
     * Stack contains:
     * r0, r1, r2, r3, r12, r14, LR and xPSR
     * First argument (r0) is svc_args[0]
     */

    // Cast the return address to a char pointer and access the byte before
    // cppcheck-suppress ctuArrayIndex
    uintptr_t return_address = (uintptr_t)svc_args[6];
    svc_number = ((char *)return_address)[-2];

#elif x86
    svc_number = *svc_args;
#endif

    switch (svc_number) {
        case 0:
            printf("Success!\n");
            break;

        case 1:
            printf("Something else!\n");
            break;

        case RUN_FIRST_TASK_SVC:
            for (uint8_t i = 0; i < active_tasks; i++) {
                if (task_list[i].deadline < min_deadline) {
                    min_deadline = task_list[i].deadline;
                    current_task_index = i;
                }
            }
            task_list[current_task_index].state = TASK_RUNNING;
            task_list[current_task_index].deadline = (task_list[current_task_index].deadline + task_list[current_task_index].period) % UINT32_MAX;
#ifdef ARM
            __set_PSP((uint32_t)task_list[current_task_index].stackPointer);
#elif x86
            task_list[current_task_index].taskFunction(NULL);
#endif

            break;

        case STOP_TASK_SVC:
            // Pend an interrupt to do the context switch
            task_list[current_task_index].state = TASK_SUSPENDED;
#ifdef ARM
            _ICSR |= 1 << 28;
            __asm("isb");
#elif x86
// Simulate context switch
#endif

            break;

        default: /* unknown SVC */
            break;
    }
}

void PendSV_Handler(void) {
#ifdef ARM
    __asm volatile(
        "MRS r0, PSP                     \n"  // Get current process stack pointer
        "STMDB r0!, {r4-r11}             \n"  // Save r4-r11 on the process stack
        "LDR r1, =current_task_index     \n"  // Load address of current_task_index
        "LDR r2, [r1]                    \n"  // Load current_task_index value
        "LDR r3, =task_list              \n"  // Load address of task_list array
        "STR r0, [r3, r2, LSL #2]        \n"  // Save PSP in task_list[current_task_index].stackPointer

        // Call the scheduler
        "BL edf_schedule                 \n"  // Call edf_schedule to select the next task

        "LDR r1, =current_task_index     \n"  // Load address of current_task_index
        "LDR r2, [r1]                    \n"  // Load current_task_index value
        "LDR r3, =task_list              \n"  // Load address of task_list array
        "LDR r0, [r3, r2, LSL #2]        \n"  // Load PSP from task_list[current_task_index].stackPointer

        "LDMIA r0!, {r4-r11}             \n"  // Restore r4-r11 from the new task's stack
        "MSR PSP, r0                     \n"  // Set PSP to the new task's stack pointer
        "BX LR                           \n"  // Return from exception
    );
#elif x86

#endif
}

// ISR for hardware timer ticks. Should check if we need context switch
void SysTick_Handler(void) {
    system_time = (system_time + 1) % UINT32_MAX;
    bool context_switch_flag = false;

    // Iterate through tasks to find the earliest deadline task
    for (int i = 0; i < active_tasks; i++) {
        if (task_list[i].deadline < task_list[current_task_index].deadline) {
            context_switch_flag = true;
        }
    }

    // Check and update the currently running task's deadline if necessary
    if (task_list[current_task_index].deadline == system_time) {
        task_list[current_task_index].deadline = (task_list[current_task_index].deadline + task_list[current_task_index].period) % UINT32_MAX;
        context_switch_flag = true;
    }

    // If the next task is different from the current task, switch the task context
    if (context_switch_flag) {
#ifdef ARM
        _ICSR |= 1 << 28;
        __asm("isb");
#elif x86
        edf_schedule();
#endif
    }
}

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

RTOSStatus create_task(TaskFunction task_function, void *args, uint32_t period, uint32_t deadline, const char *name) {
    if (active_tasks > MAX_TASKS - 1) {
        // Subtract 1 because of idle task
        return RTOS_INTERNAL_ERROR;
    }

    for (int i = 0; i < MAX_TASKS; i++) {
        // If the task stack space is not being used yet
        if (task_list[i].state == TASK_SUSPENDED) {
            active_tasks++;
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

            // Initialize R12, R3, R2, R1 to zero
            for (int j = 0; j < 4; j++) {
                *(task_list[i].stackPointer--) = 0x00000000;
            }

            // Initialize R0 to hold arg values
            *(task_list[i].stackPointer--) = (uintptr_t)args;

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
    if (active_tasks == 0) {
        return RTOS_INTERNAL_ERROR;
    }
#ifdef ARM
    __asm("SVC #2");  // Software interrupt with value of 2 passed in
#elif x86
    unsigned int start_scheduler_val = 2;
    SVC_Handler_Main(&start_scheduler_val);
#endif
    return RTOS_OK;
}
