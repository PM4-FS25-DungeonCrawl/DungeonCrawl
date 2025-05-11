#include "common_output.h"

#include "../../../common.h"
#include "../../../logging/logger.h"
#include "../../../logging/ringbuffer.h"
#include "../../../thread/thread_handler.h"
#include "../../io_handler.h"// Include this to access global nc and stdplane

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
    #include <pthread.h>
    #include <unistd.h>// for usleep
#else
    #include <windows.h>
#endif

// Using the global variables from io_handler.h
// No need to redeclare them here as they're already included through io_handler.h

// Mutex for thread safety with rendering operations
#ifdef _WIN32
static CRITICAL_SECTION output_mutex;
#else
static pthread_mutex_t output_mutex;
#endif

// Ring buffer for output commands from multiple threads
static ring_buffer_t output_buffer;

// Flag to control the output thread
static volatile bool output_thread_running = false;

// Forward declarations
static void output_thread_function(void);

// Helper function to initialize the mutex
static bool init_output_mutex(void) {
#ifdef _WIN32
    InitializeCriticalSection(&output_mutex);
    return true;
#else
    return (pthread_mutex_init(&output_mutex, NULL) == 0);
#endif
}

// Helper function to lock the mutex
static void lock_output_mutex(void) {
#ifdef _WIN32
    EnterCriticalSection(&output_mutex);
#else
    pthread_mutex_lock(&output_mutex);
#endif
}

// Helper function to unlock the mutex
static void unlock_output_mutex(void) {
#ifdef _WIN32
    LeaveCriticalSection(&output_mutex);
#else
    pthread_mutex_unlock(&output_mutex);
#endif
}

// Helper function to destroy the mutex
static void destroy_output_mutex(void) {
#ifdef _WIN32
    DeleteCriticalSection(&output_mutex);
#else
    pthread_mutex_destroy(&output_mutex);
#endif
}

bool init_output_handler() {
    log_msg(INFO, "output_handler", "Starting initialization");

    if (!nc) {
        log_msg(ERROR, "output_handler", "Null Notcurses instance provided");
        return false;
    }

    if (!stdplane) {
        log_msg(ERROR, "output_handler", "Null standard plane provided");
        return false;
    }

    // Initialize the mutex
    if (!init_output_mutex()) {
        log_msg(ERROR, "output_handler", "Failed to initialize output mutex");
        return false;
    }

    // Initialize the output buffer
    if (init_ringbuffer(&output_buffer) != 0) {
        log_msg(ERROR, "output_handler", "Failed to initialize output buffer");
        destroy_output_mutex();
        return false;
    }

    // Start the output thread
    output_thread_running = true;
    start_simple_thread(output_thread_function);

    log_msg(INFO, "output_handler", "Output handler initialized");
    return true;
}

// Output thread function
static void output_thread_function(void) {
    log_msg(INFO, "output_handler", "Output thread started");

    while (output_thread_running) {
        char command[MAX_MSG_LENGTH];

        // Check for commands in the buffer
        // Note: read_from_ringbuffer is blocking, which works for our purpose
        // as this thread is dedicated to rendering
        read_from_ringbuffer(&output_buffer, command);

        // Process the command
        // We don't implement a full command parser here, but in a real implementation,
        // we would parse the command and execute the appropriate rendering function

        // For now, we just render the screen after any command
        lock_output_mutex();
        notcurses_render(nc);
        unlock_output_mutex();
    }

    log_msg(INFO, "output_handler", "Output thread stopped");
}

void clear_screen(void) {
    if (!stdplane) {
        log_msg(ERROR, "output_handler", "Output handler not initialized");
        return;
    }

    lock_output_mutex();

    // Clear the plane with the default colors
    ncplane_set_base(stdplane, " ", 0, DEFAULT_COLORS);
    ncplane_erase(stdplane);

    // Add a command to the buffer to trigger a render
    write_to_ringbuffer(&output_buffer, "CLEAR");

    unlock_output_mutex();
}

void print_text(int y, int x, const char* text, uint64_t ncchannel) {
    if (!stdplane || !text) {
        log_msg(ERROR, "output_handler", "Output handler not initialized or null text");
        return;
    }

    lock_output_mutex();

    // Set the channels and print the text
    ncplane_set_channels(stdplane, ncchannel);
    ncplane_putstr_yx(stdplane, y, x, text);

    // Add a command to the buffer to trigger a render
    char command[64];
    snprintf(command, sizeof(command), "TEXT:%d:%d", y, x);
    write_to_ringbuffer(&output_buffer, command);

    unlock_output_mutex();
}

void print_text_default(int y, int x, const char* text) {
    print_text(y, x, text, DEFAULT_COLORS);
}

void print_text_multi_line(int y, int x, const char* text, int max_width, uint64_t ncchannel) {
    if (!stdplane || !text || max_width <= 0) {
        log_msg(ERROR, "output_handler", "Invalid parameters for print_text_multi_line");
        return;
    }

    lock_output_mutex();

    // Set the channels
    ncplane_set_channels(stdplane, ncchannel);

    // Handle line wrapping
    const char* ptr = text;
    int current_y = y;
    int line_len = 0;
    char line_buffer[256];// Assuming max line length

    while (*ptr) {
        // Copy characters until max_width or newline
        line_len = 0;
        while (*ptr && *ptr != '\n' && line_len < max_width) {
            line_buffer[line_len++] = *ptr++;
        }
        line_buffer[line_len] = '\0';

        // Print this line
        ncplane_putstr_yx(stdplane, current_y, x, line_buffer);
        current_y++;

        // Handle newline character
        if (*ptr == '\n') {
            ptr++;
        }
    }

    // Add a command to the buffer to trigger a render
    char command[64];
    snprintf(command, sizeof(command), "MULTILINE:%d:%d", y, x);
    write_to_ringbuffer(&output_buffer, command);

    unlock_output_mutex();
}

void print_text_multi_line_default(int y, int x, const char* text, int max_width) {
    print_text_multi_line(y, x, text, max_width, DEFAULT_COLORS);
}

void print_text_multi_strings(int y, int x, const char* text[], int count, uint64_t ncchannel) {
    if (!stdplane || !text || count <= 0) {
        log_msg(ERROR, "output_handler", "Invalid parameters for print_text_multi_strings");
        return;
    }

    lock_output_mutex();

    // Set the channels
    ncplane_set_channels(stdplane, ncchannel);

    // Print each string on a new line
    for (int i = 0; i < count; i++) {
        if (text[i]) {
            ncplane_putstr_yx(stdplane, y + i, x, text[i]);
        }
    }

    // Add a command to the buffer to trigger a render
    char command[64];
    snprintf(command, sizeof(command), "MULTISTRINGS:%d:%d:%d", y, x, count);
    write_to_ringbuffer(&output_buffer, command);

    unlock_output_mutex();
}

void print_text_multi_strings_default(int y, int x, const char* text[], int count) {
    print_text_multi_strings(y, x, text, count, DEFAULT_COLORS);
}

void print_menu(const char* title, const char** options, int option_count,
                int selected_index, int y, int x,
                uint64_t title_channel,
                uint64_t option_channel,
                uint64_t selected_channel) {
    if (!stdplane || !title || !options || option_count <= 0) {
        log_msg(ERROR, "output_handler", "Invalid menu parameters");
        return;
    }

    lock_output_mutex();

    // Print title
    ncplane_set_channels(stdplane, title_channel);
    ncplane_putstr_yx(stdplane, y, x, title);

    // Print options
    for (int i = 0; i < option_count; i++) {
        if (i == selected_index) {
            // Highlight selected option
            ncplane_set_channels(stdplane, selected_channel);
        } else {
            ncplane_set_channels(stdplane, option_channel);
        }
        ncplane_putstr_yx(stdplane, y + i + 1, x, options[i]);
    }

    // Add a command to the buffer to trigger a render
    char command[64];
    snprintf(command, sizeof(command), "MENU:%d:%d:%d", y, x, selected_index);
    write_to_ringbuffer(&output_buffer, command);

    unlock_output_mutex();
}

void print_menu_default(const char* title, const char** options, int option_count,
                        int selected_index, int y, int x) {
    print_menu(title, options, option_count, selected_index, y, x,
               DEFAULT_COLORS, DEFAULT_COLORS, INVERTED_COLORS);
}

bool render_frame(void) {
    if (!nc) {
        log_msg(ERROR, "output_handler", "Output handler not initialized");
        return false;
    }

    lock_output_mutex();

    // Render all changes
    int ret = notcurses_render(nc);

    // Add a command to the buffer
    write_to_ringbuffer(&output_buffer, "RENDER");

    unlock_output_mutex();

    return ret >= 0;
}

bool get_screen_dimensions(int* width, int* height) {
    if (!stdplane || !width || !height) {
        log_msg(ERROR, "output_handler", "Invalid parameters for get_screen_dimensions");
        return false;
    }

    lock_output_mutex();

    // Get the dimensions of the standard plane
    *width = ncplane_dim_x(stdplane);
    *height = ncplane_dim_y(stdplane);

    unlock_output_mutex();

    return true;
}

void shutdown_output_handler(void) {
    // Stop the output thread
    output_thread_running = false;

// Allow the thread some time to clean up
#ifdef _WIN32
    Sleep(100);
#else
    usleep(100000);
#endif

    // Free the output buffer
    free_ringbuffer(&output_buffer);

    // Destroy the mutex
    destroy_output_mutex();

    // Reset the globals
    nc = NULL;
    stdplane = NULL;

    log_msg(INFO, "output_handler", "Output handler shut down");
}