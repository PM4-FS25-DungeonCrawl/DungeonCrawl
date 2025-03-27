#include <string.h>

#include "ringbuffer.h"

void init_ring_buffer(RingBuffer *buffer) {
    buffer->head = 0;
    buffer->tail = 0;
    buffer->count = 0;
    INIT_MUTEX(&buffer->mutex);
    INIT_COND(&buffer->cond);
}

void write_to_ring_buffer(RingBuffer *buffer, const char *message) {
    MUTEX_LOCK(&buffer->mutex);
    if (buffer->count < BUFFER_SIZE) {
        strncpy(buffer->messages[buffer->tail], message, MAX_MSG_LENGTH);
        buffer->tail = (buffer->tail + 1) % BUFFER_SIZE;
        buffer->count++;
        SIGNAL_COND(&buffer->cond);
    }
    MUTEX_UNLOCK(&buffer->mutex);
}


int read_from_ring_buffer(RingBuffer *buffer, char *message) {
    MUTEX_LOCK(&buffer->mutex);
    while (buffer->count == 0) {
        SIGNAL_WAIT(&buffer->cond, &buffer->mutex);
    }
    strncpy(message, buffer->messages[buffer->head], MAX_MSG_LENGTH);
    buffer->head = (buffer->head + 1) % BUFFER_SIZE;
    buffer->count--;
    MUTEX_UNLOCK(&buffer->mutex);
    return 1;
}