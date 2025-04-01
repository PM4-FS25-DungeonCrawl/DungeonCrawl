#include <string.h>

#include "ringbuffer.h"

#ifdef _WIN32
    #define INIT_MUTEX(mutex) InitializeCriticalSection(mutex)
    #define INIT_COND(cond) InitializeConditionVariable(cond)

    #define MUTEX_LOCK(mutex) EnterCriticalSection(mutex)
    #define MUTEX_UNLOCK(mutex) LeaveCriticalSection(mutex)
    #define SIGNAL_COND(cond) WakeConditionVariable(cond)
    #define SIGNAL_WAIT(cond, mutex) SleepConditionVariableCS(cond, mutex, INFINITE)
#else
    #define INIT_MUTEX(mutex) pthread_mutex_init(mutex, NULL)
    #define INIT_COND(cond) pthread_cond_init(cond, NULL)

    #define MUTEX_LOCK(mutex) pthread_mutex_lock(mutex)
    #define MUTEX_UNLOCK(mutex) pthread_mutex_unlock(mutex)
    #define SIGNAL_COND(cond) pthread_cond_signal(cond)
    #define SIGNAL_WAIT(cond, mutex) pthread_cond_wait(cond, mutex)
#endif

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
