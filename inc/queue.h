#ifndef QUEUE_H
#define QUEUE_H

#include "rtos_status.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t *buffer;
    uint32_t head;
    uint32_t tail;
    uint32_t len;
    bool full;
} Queue;

RTOSStatus init_queue(Queue *queue, uint8_t *buffer, uint32_t size);
RTOSStatus enqueue(Queue *queue, uint8_t data);
RTOSStatus dequeue(Queue *queue, uint8_t *data);

#endif