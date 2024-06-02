#ifndef QUEUE_H
#define QUEUE_H

#include <cstdint>
#include <stdbool.h>

typedef struct {
    uint8_t *buffer;
    uint32_t head;
    uint32_t tail;
    uint32_t maxLen;
    bool full;
} Queue;

void init_queue(Queue *q, uint8_t *buffer, uint32_t size);
bool enqueue(Queue *q, uint8_t data);
bool dequeue(Queue *q, uint8_t *data);

#endif