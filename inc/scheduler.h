#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdbool.h>
#include <string.h>

#include "rtos_status.h"
#include "task.h"

#define RUN_FIRST_TASK_SVC 2
#define STOP_TASK_SVC 3

/**
 * These are functions native to the ISR system in ARM architecture
 */
void edf_schedule(void);
void SVC_Handler_Main(unsigned int *svc_args);
void PendSV_Handler(void);
void SysTick_Handler(void);

RTOSStatus init_scheduler();
RTOSStatus create_task(TaskFunction task_function, void *args, uint32_t period, uint32_t deadline, const char *name);
RTOSStatus start_scheduler();

#endif