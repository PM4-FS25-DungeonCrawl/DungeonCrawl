#include "input_handler.h"

#include "../../logging/logger.h"
#include "../../thread/thread_handler.h"
#include "../io_handler.h"// Include io_handler.h to access global nc

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
    #include <unistd.h>// for usleep
    #include <pthread.h>
#endif

// Define platform-specific key event type
#ifdef __APPLE__
    #define KEY_EVENT NCTYPE_PRESS
#else
    #define KEY_EVENT NCTYPE_UNKNOWN
#endif

// Use the global nc from io_handler.c - no need to redeclare it here

// We now use a custom circular buffer implementation with proper synchronization

// Flag to control the input thread
static volatile bool input_thread_running = false;

/**
 * @brief Structure to store raw input events directly
 *
 * This structure stores input events in their raw form without string serialization,
 * which is more efficient and avoids potential serialization/parsing errors.
 *
 * @note For "press any key to continue..." prompts, we use notcurses_get_blocking() directly
 * rather than going through the threaded input handler, since any key is acceptable and we
 * want immediate response.
 */
typedef struct {
    input_t type;       // Logical input type (UP, DOWN, etc.)
    uint32_t id;        // Original key ID from notcurses
    uint16_t modifiers; // Key modifiers (CTRL, ALT, etc.)
    uint8_t evtype;     // Event type (press, release, etc.)
} input_raw_data_t;

#define MAX_INPUT_BUFFER_SIZE 32
static input_raw_data_t input_data_buffer[MAX_INPUT_BUFFER_SIZE];
static int input_buffer_head = 0;
static int input_buffer_tail = 0;
static int input_buffer_count = 0;

#ifdef _WIN32
static CRITICAL_SECTION input_buffer_mutex;
static CONDITION_VARIABLE input_buffer_cond;
#define INIT_MUTEX() InitializeCriticalSection(&input_buffer_mutex)
#define LOCK_MUTEX() EnterCriticalSection(&input_buffer_mutex)
#define UNLOCK_MUTEX() LeaveCriticalSection(&input_buffer_mutex)
#define SIGNAL_COND() WakeConditionVariable(&input_buffer_cond)
#define WAIT_COND() SleepConditionVariableCS(&input_buffer_cond, &input_buffer_mutex, INFINITE)
#define DESTROY_MUTEX() DeleteCriticalSection(&input_buffer_mutex)
#else
static pthread_mutex_t input_buffer_mutex;
static pthread_cond_t input_buffer_cond;
#define INIT_MUTEX() pthread_mutex_init(&input_buffer_mutex, NULL)
#define LOCK_MUTEX() pthread_mutex_lock(&input_buffer_mutex)
#define UNLOCK_MUTEX() pthread_mutex_unlock(&input_buffer_mutex)
#define SIGNAL_COND() pthread_cond_signal(&input_buffer_cond)
#define WAIT_COND() pthread_cond_wait(&input_buffer_cond, &input_buffer_mutex)
#define DESTROY_MUTEX() pthread_mutex_destroy(&input_buffer_mutex)
#endif

// Forward declarations
static void input_thread_function(void);

// Add an input event to the buffer
static bool add_input_event(input_t type, const ncinput* raw_input) {
    LOCK_MUTEX();

    bool success = false;

    // Only add if there's space or we can overwrite the oldest entry
    if (input_buffer_count < MAX_INPUT_BUFFER_SIZE) {
        input_raw_data_t* event = &input_data_buffer[input_buffer_tail];
        event->type = type;

        if (raw_input) {
            event->id = raw_input->id;
            event->modifiers = raw_input->modifiers;
            event->evtype = raw_input->evtype;
        } else {
            event->id = 0;
            event->modifiers = 0;
            event->evtype = 0;
        }

        input_buffer_tail = (input_buffer_tail + 1) % MAX_INPUT_BUFFER_SIZE;
        input_buffer_count++;
        success = true;

        // Signal waiting threads that data is available
        SIGNAL_COND();
    }

    UNLOCK_MUTEX();
    return success;
}

// Get an input event from the buffer (non-blocking)
static bool get_input_event(input_event_t* event, bool blocking) {
    if (!event) {
        return false;
    }

    LOCK_MUTEX();

    // If blocking and no data, wait
    while (blocking && input_buffer_count == 0 && input_thread_running) {
        WAIT_COND();
    }

    bool success = false;

    if (input_buffer_count > 0) {
        input_raw_data_t* raw_event = &input_data_buffer[input_buffer_head];

        event->type = raw_event->type;
        event->raw_input.id = raw_event->id;
        event->raw_input.modifiers = raw_event->modifiers;
        event->raw_input.evtype = raw_event->evtype;

        input_buffer_head = (input_buffer_head + 1) % MAX_INPUT_BUFFER_SIZE;
        input_buffer_count--;
        success = true;
    } else {
        // No data available
        event->type = INPUT_NONE;
        memset(&event->raw_input, 0, sizeof(ncinput));
    }

    UNLOCK_MUTEX();
    return success;
}

bool init_input_handler(struct notcurses* notcurses_ptr) {
    if (!notcurses_ptr) {
        log_msg(ERROR, "input_handler", "Null Notcurses instance provided");
        return false;
    }

    // Assign to the global variable
    nc = notcurses_ptr;
    log_msg(INFO, "input_handler", "Set nc pointer to %p", (void*) nc);

    // Initialize input buffer state
    input_buffer_head = 0;
    input_buffer_tail = 0;
    input_buffer_count = 0;

    // Initialize mutex and condition variable
    INIT_MUTEX();

    // Start the input thread
    input_thread_running = true;
    start_simple_thread(input_thread_function);

    log_msg(INFO, "input_handler", "Input handler initialized");
    return true;
}

// Input thread function that continuously polls for input
static void input_thread_function(void) {
    log_msg(INFO, "input_handler", "Input thread started");

    while (input_thread_running) {
        // Check for input
        ncinput raw_input;
        memset(&raw_input, 0, sizeof(ncinput));

        uint32_t ret = notcurses_get_nblock(nc, &raw_input);
        if (ret > 0) {
            // Translate the input
            input_t type = translate_input(&raw_input);

            // If it's a valid input, add it to the buffer
            if (type != INPUT_NONE) {
                add_input_event(type, &raw_input);
                log_msg(DEBUG, "input_handler", "Added input event: type=%d, id=%d",
                       (int)type, (int)raw_input.id);
            }
        }

        // Small sleep to avoid using 100% CPU
        // Notcurses doesn't offer a built-in way to wait for input events
        // without blocking, so we use a polling approach with a small delay
        #ifdef _WIN32
            Sleep(10);// 10 milliseconds
        #else
            usleep(10000);// 10 milliseconds
        #endif
    }

    log_msg(INFO, "input_handler", "Input thread stopped");
}

bool get_input_blocking(input_event_t* event) {
    if (!event || !nc) {
        log_msg(ERROR, "input_handler", "Null event pointer or uninitialized handler");
        return false;
    }

    return get_input_event(event, true);
}

bool get_input_nonblocking(input_event_t* event) {
    if (!event || !nc) {
        log_msg(ERROR, "input_handler", "Null event pointer or uninitialized handler");
        return false;
    }

    return get_input_event(event, false);
}

void shutdown_input_handler(void) {
    // Stop the input thread
    input_thread_running = false;

    // Allow the thread some time to clean up
    #ifdef _WIN32
        Sleep(100);
    #else
        usleep(100000);
    #endif

    // Destroy synchronization primitives
    DESTROY_MUTEX();

    nc = NULL;
    log_msg(INFO, "input_handler", "Input handler shut down");
}