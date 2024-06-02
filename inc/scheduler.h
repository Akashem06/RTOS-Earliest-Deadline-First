#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "task.h"

void init_scheduler();
void start_scheduler();
void create_task(TaskFunction taskFunction, uint8_t priority, const char *name);

#endif