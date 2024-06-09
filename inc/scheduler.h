#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <string.h>

#include "rtos_status.h"
#include "task.h"

RTOSStatus init_scheduler();
RTOSStatus create_task(TaskFunction task_function, uint32_t period, uint32_t deadline, const char *name);
RTOSStatus start_scheduler();
RTOSStatus tick_handler();
RTOSStatus update_task_deadlines();

#endif