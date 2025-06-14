/**
 * @file rinbuffer.c
 * @brief Provides a ringbuffer for easier memory management.
 */

#include "ringbuffer.h"

#include <stdio.h>
#include <stdlib.h>

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


int init_ringbuffer(ring_buffer_t* buffer) {
    buffer->head = 0;
    buffer->tail = 0;
    buffer->count = 0;


    // allocate pointer array for messages
    buffer->messages = (char**) malloc(BUFFER_SIZE * sizeof(char*));
    if (!buffer->messages) {
        return 1;
    }

    // allocate memory for messages (at each pointer)
    for (int i = 0; i < BUFFER_SIZE; i++) {
        buffer->messages[i] = (char*) malloc(MAX_MSG_LENGTH);
        if (!buffer->messages[i]) {
            // if malloc fails, free up all the previous allocated memory
            for (int j = 0; j < i; j++) {
                free(buffer->messages[j]);
            }
            free(buffer->messages);
            return 1;
        }
    }

    INIT_MUTEX(&buffer->mutex);
    INIT_COND(&buffer->cond);

    return 0;
}

void free_ringbuffer(const ring_buffer_t* buffer) {
    if (buffer->messages) {
        for (int i = 0; i < BUFFER_SIZE; i++) {
            free(buffer->messages[i]);
        }
        free(buffer->messages);
    }
}

void write_to_ringbuffer(ring_buffer_t* buffer, const char* message) {
    MUTEX_LOCK(&buffer->mutex);
    if (buffer->count < BUFFER_SIZE) {
        snprintf(buffer->messages[buffer->tail], MAX_MSG_LENGTH, "%s", message);
        buffer->tail = (buffer->tail + 1) % BUFFER_SIZE;
        buffer->count++;
        SIGNAL_COND(&buffer->cond);
    }
    MUTEX_UNLOCK(&buffer->mutex);
}


int read_from_ringbuffer(ring_buffer_t* buffer, char* message) {
    MUTEX_LOCK(&buffer->mutex);
    while (buffer->count == 0) {
        SIGNAL_WAIT(&buffer->cond, &buffer->mutex);
    }
    snprintf(message, MAX_MSG_LENGTH, "%s", buffer->messages[buffer->head]);
    buffer->head = (buffer->head + 1) % BUFFER_SIZE;
    buffer->count--;
    MUTEX_UNLOCK(&buffer->mutex);
    return 0;
}
