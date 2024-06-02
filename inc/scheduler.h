#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "task.h"
#include "rtos_status.h"
#include <string.h>

RTOSStatus init_scheduler();
RTOSStatus start_scheduler();
RTOSStatus create_task(TaskFunction taskFunction, uint8_t priority, const char *name);

#endif