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

    #define INIT_MUTEX(mutex) InitializeCriticalSection(mutex)
    #define INIT_COND(cond) InitializeConditionVariable(cond)

    #define MUTEX_LOCK(mutex) EnterCriticalSection(mutex)
    #define MUTEX_UNLOCK(mutex) LeaveCriticalSection(mutex)
    #define SIGNAL_COND(cond) WakeConditionVariable(cond)
    #define SIGNAL_WAIT(cond, mutex) SleepConditionVariableCS(cond, mutex, INFINITE)
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

    #define INIT_MUTEX(mutex) pthread_mutex_init(mutex, nullptr)
    #define INIT_COND(cond) pthread_cond_init(cond, nullptr)

    #define MUTEX_LOCK(mutex) pthread_mutex_lock(mutex)
    #define MUTEX_UNLOCK(mutex) pthread_mutex_unlock(mutex)
    #define SIGNAL_COND(cond) pthread_cond_signal(cond)
    #define SIGNAL_WAIT(cond, mutex) pthread_cond_wait(cond, mutex)
#endif

void init_ring_buffer(RingBuffer *buffer);

void write_to_ring_buffer(RingBuffer *buffer, const char *message);

int read_from_ring_buffer(RingBuffer *buffer, char *message);

#endif //RINGBUFFER_H
