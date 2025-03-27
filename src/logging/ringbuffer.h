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
    } RingBuffer;
#else
    #include <pthread.h>

    typedef struct {
        char messages[BUFFER_SIZE][MAX_MSG_LENGTH];
        int head;
        int tail;
        int count;
        pthread_mutex_t mutex;
        pthread_cond_t cond;
    } RingBuffer;
#endif

void init_ring_buffer(RingBuffer *buffer);

void write_to_ring_buffer(RingBuffer *buffer, const char *message);

int read_from_ring_buffer(RingBuffer *buffer, char *message);

#endif //RINGBUFFER_H
