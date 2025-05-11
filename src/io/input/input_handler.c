#include "input_handler.h"

#include "../../logging/logger.h"
#include "../../logging/ringbuffer.h"
#include "../../thread/thread_handler.h"
#include "../io_handler.h"// Include io_handler.h to access global nc

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
    #include <unistd.h>// for usleep
#endif

// Define platform-specific key event type
#ifdef __APPLE__
    #define KEY_EVENT NCTYPE_PRESS
#else
    #define KEY_EVENT NCTYPE_UNKNOWN
#endif

// Use the global nc from io_handler.c - no need to redeclare it here

// The input buffer to store events between threads
static ring_buffer_t input_buffer;

// Flag to control the input thread
static volatile bool input_thread_running = false;

// Forward declarations
static void input_thread_function(void);

bool init_input_handler(struct notcurses* notcurses_ptr) {
    if (!notcurses_ptr) {
        log_msg(ERROR, "input_handler", "Null Notcurses instance provided");
        return false;
    }

    // Assign to the global variable
    nc = notcurses_ptr;
    log_msg(INFO, "input_handler", "Set nc pointer to %p", (void*) nc);

    // Initialize the input buffer
    if (init_ringbuffer(&input_buffer) != 0) {
        log_msg(ERROR, "input_handler", "Failed to initialize input buffer");
        return false;
    }

    // Start the input thread
    input_thread_running = true;
    start_simple_thread(input_thread_function);

#ifdef __APPLE__
    log_msg(INFO, "input_handler", "Input handler initialized for macOS");
#else
    log_msg(INFO, "input_handler", "Input handler initialized for Linux/Windows");
#endif

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
                // Create a simple string representation of the input event
                // Format: "TYPE:ID:MODIFIERS:EVTYPE"
                char event_str[64];
                snprintf(event_str, sizeof(event_str), "%d:%d:%d:%d",
                         (int) type, (int) raw_input.id, (int) raw_input.modifiers, (int) raw_input.evtype);

                // Add to buffer
                write_to_ringbuffer(&input_buffer, event_str);
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

    // Initialize the event with INPUT_NONE
    event->type = INPUT_NONE;
    memset(&event->raw_input, 0, sizeof(ncinput));

    // Wait for an input event from the buffer
    char event_str[64];
    while (input_thread_running) {
        if (read_from_ringbuffer(&input_buffer, event_str) > 0) {
            // Parse the event string
            int type, id, modifiers, evtype;
            if (sscanf(event_str, "%d:%d:%d:%d", &type, &id, &modifiers, &evtype) == 4) {
                event->type = (input_t) type;
                event->raw_input.id = id;
                event->raw_input.modifiers = modifiers;
                event->raw_input.evtype = evtype;
                return true;
            }
        }

// Small sleep to avoid busy-waiting
#ifdef _WIN32
        Sleep(10);
#else
        usleep(10000);
#endif
    }

    return false;
}

bool get_input_nonblocking(input_event_t* event) {
    if (!event || !nc) {
        log_msg(ERROR, "input_handler", "Null event pointer or uninitialized handler");
        return false;
    }

    // Initialize the event with INPUT_NONE
    event->type = INPUT_NONE;
    memset(&event->raw_input, 0, sizeof(ncinput));

    // Check for an input event from the buffer
    char event_str[64];
    if (read_from_ringbuffer(&input_buffer, event_str) > 0) {
        // Parse the event string
        int type, id, modifiers, evtype;
        if (sscanf(event_str, "%d:%d:%d:%d", &type, &id, &modifiers, &evtype) == 4) {
            event->type = (input_t) type;
            event->raw_input.id = id;
            event->raw_input.modifiers = modifiers;
            event->raw_input.evtype = evtype;
            return true;
        }
    }

    return false;
}

// Using the inline function from the header file

void shutdown_input_handler(void) {
    // Stop the input thread
    input_thread_running = false;

// Allow the thread some time to clean up
#ifdef _WIN32
    Sleep(100);
#else
    usleep(100000);
#endif

    // Free the input buffer
    free_ringbuffer(&input_buffer);

    nc = NULL;
    log_msg(INFO, "input_handler", "Input handler shut down");
}