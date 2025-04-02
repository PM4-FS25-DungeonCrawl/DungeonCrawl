#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#define BUFFER_SIZE 20
#define MAX_MSG_LENGTH 1024

#ifdef _WIN32
    #include <windows.h>

    typedef struct {
        char messages[BUFFER_SIZE][MAX_MSG_LENGTH];
        int head;
        int tail;
        int count;
        CRITICAL_SECTION mutex;
        CONDITION_VARIABLE cond;
    } ring_buffer_t;
#else
    #include <pthread.h>

    typedef struct {
        char messages[BUFFER_SIZE][MAX_MSG_LENGTH];
        int head;
        int tail;
        int count;
        pthread_mutex_t mutex;
        pthread_cond_t cond;
    } ring_buffer_t;
#endif

void init_ring_buffer(ring_buffer_t *buffer);

void write_to_ring_buffer(ring_buffer_t *buffer, const char *message);

int read_from_ring_buffer(ring_buffer_t *buffer, char *message);

#endif //RINGBUFFER_H
