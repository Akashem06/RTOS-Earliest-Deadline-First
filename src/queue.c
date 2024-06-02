#include "queue.h"

void init_queue(Queue *q, uint8_t *buffer, uint32_t size) {
    q->buffer = buffer;
    q->maxLen = size;
    q->head = 0;
    q->tail = 0;
    q->full = false;
}

bool enqueue(Queue *q, uint8_t data) {
    if (q->full) {
        return false;
    }

    q->buffer[q->head] = data;
    q->head = (q->head + 1) % q->maxLen;
    q->full = (q->head == q->tail);
}


bool dequeue(Queue *q, uint8_t *data) {
    if (q->head == q->tail && !q->full) {
        enableInterrupts();
        return false;
    }

    *data = q->buffer[q->tail];
    q->tail = (q->tail + 1) % q->maxLen;
    q->full = false;

    return true;
}