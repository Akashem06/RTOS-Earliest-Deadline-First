#include "queue.h"

RTOSStatus init_queue(Queue *queue, uint8_t *buffer, uint32_t size) {
    queue->buffer = buffer;
    queue->head = 0;
    queue->tail = 0;
    queue->len = size;
    queue->full = false;

    return RTOS_OK;
}

RTOSStatus enqueue(Queue *queue, uint8_t data) {
    if (!queue->buffer) {
        return RTOS_UNINITIALIZED;
    } else if (queue->full) {
        return RTOS_RESOURCE_EXHAUSTED;
    }

    queue->buffer[queue->tail] = data;
    queue->tail = (queue->tail + 1) % queue->len;

    return RTOS_OK;
}


RTOSStatus dequeue(Queue *queue, uint8_t *data) {
    if (!queue->buffer) {
        return RTOS_UNINITIALIZED;
    }

    data = &queue->buffer[queue->head];
    queue->head = (queue->head + 1) %queue->len;

    return RTOS_OK;
}