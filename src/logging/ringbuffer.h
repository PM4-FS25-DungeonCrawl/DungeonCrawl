/**
 * @file rinbuffer.h
 * @brief Header file for the ringbuffer.
 */

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#define BUFFER_SIZE 20
#define MAX_MSG_LENGTH 1024

#ifdef _WIN32
    #include <windows.h>

typedef struct {
    char** messages;
    int head;
    int tail;
    int count;
    CRITICAL_SECTION mutex;
    CONDITION_VARIABLE cond;
} ring_buffer_t;
#else
    #include <pthread.h>

typedef struct {
    char** messages;
    int head;
    int tail;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} ring_buffer_t;
#endif

/**
 * @brief Initialize the ringbuffer.
 *
 * @param buffer The ringbuffer to initialize.
 * @return 0 if initialization was successfully or 1 if not
 */
int init_ringbuffer(ring_buffer_t* buffer);
/**
 * @brief Free the passed in ringbuffer.
 *
 * @param buffer The buffer to be cleaned up.
 */
void free_ringbuffer(const ring_buffer_t* buffer);
/**
 * @brief Write a message to the ringbuffer.
 *
 * @param buffer The buffer to write to.
 * @param message The message to write.
 */
void write_to_ringbuffer(ring_buffer_t* buffer, const char* message);
/**
 * @brief Read a message from the ringbuffer.
 *
 * @param buffer The buffer to read from
 * @param message A char buffer to read the message into.
 * @return 0 on successfully reading the message :
 */
int read_from_ringbuffer(ring_buffer_t* buffer, char* message);

#endif//RINGBUFFER_H
