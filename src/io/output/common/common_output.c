#include "common_output.h"
#include "../../../logging/logger.h"
#include "../../../thread/thread_handler.h"
#include "../../../logging/ringbuffer.h"

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef _WIN32
#include <unistd.h> // for usleep
#include <pthread.h>
#else
#include <windows.h>
#endif

// Global variables to store Notcurses state
static struct notcurses* notcurses_instance = NULL;
static struct ncplane* standard_plane = NULL;

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

bool init_output_handler(struct notcurses* nc, struct ncplane* stdplane) {
    if (!nc || !stdplane) {
        log_msg(ERROR, "output_handler", "Null Notcurses instance or standard plane provided");
        return false;
    }
    
    notcurses_instance = nc;
    standard_plane = stdplane;
    
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
        if (read_from_ringbuffer(&output_buffer, command) > 0) {
            // Process the command
            // We don't implement a full command parser here, but in a real implementation,
            // we would parse the command and execute the appropriate rendering function
            
            // For now, we just render the screen after any command
            lock_output_mutex();
            notcurses_render(notcurses_instance);
            unlock_output_mutex();
        }
        
        // Small sleep to avoid using 100% CPU
        #ifdef _WIN32
            Sleep(10); // 10 milliseconds
        #else
            usleep(10000); // 10 milliseconds
        #endif
    }
    
    log_msg(INFO, "output_handler", "Output thread stopped");
}

bool clear_screen(uint32_t bg_color) {
    if (!standard_plane) {
        log_msg(ERROR, "output_handler", "Output handler not initialized");
        return false;
    }
    
    lock_output_mutex();
    
    // Set the background color
    uint64_t channels = 0;
    ncchannels_set_bg_rgb8(&channels, 
                         COLOR_R(bg_color),
                         COLOR_G(bg_color),
                         COLOR_B(bg_color));
    
    // Clear the plane with the specified color
    ncplane_set_base(standard_plane, " ", 0, channels);
    ncplane_erase(standard_plane);
    
    // Add a command to the buffer to trigger a render
    char command[32];
    snprintf(command, sizeof(command), "CLEAR:%06X", bg_color);
    write_to_ringbuffer(&output_buffer, command);
    
    unlock_output_mutex();
    
    return true;
}

bool print_text(int y, int x, const char* text, uint32_t fg_color, uint32_t bg_color) {
    if (!standard_plane || !text) {
        log_msg(ERROR, "output_handler", "Output handler not initialized or null text");
        return false;
    }
    
    lock_output_mutex();
    
    // Create channels with the specified colors
    uint64_t channels = make_channels(fg_color, bg_color);
    
    // Set the channels and print the text
    ncplane_set_channels(standard_plane, channels);
    int ret = ncplane_putstr_yx(standard_plane, y, x, text);
    
    // Add a command to the buffer to trigger a render
    char command[64];
    snprintf(command, sizeof(command), "TEXT:%d:%d:%06X:%06X", y, x, fg_color, bg_color);
    write_to_ringbuffer(&output_buffer, command);
    
    unlock_output_mutex();
    
    return ret >= 0;
}

bool print_text_default(int y, int x, const char* text) {
    return print_text(y, x, text, COLOR_DEFAULT_FG, COLOR_DEFAULT_BG);
}

bool print_menu(const char* title, const char** options, int option_count, 
                int selected_index, int y, int x,
                uint32_t title_fg, uint32_t title_bg,
                uint32_t option_fg, uint32_t option_bg,
                uint32_t selected_fg, uint32_t selected_bg) {
    if (!standard_plane || !title || !options || option_count <= 0) {
        log_msg(ERROR, "output_handler", "Invalid menu parameters");
        return false;
    }
    
    lock_output_mutex();
    
    // Print title
    print_text(y, x, title, title_fg, title_bg);
    
    // Print options
    for (int i = 0; i < option_count; i++) {
        if (i == selected_index) {
            // Highlight selected option
            print_text(y + i + 1, x, options[i], selected_fg, selected_bg);
        } else {
            print_text(y + i + 1, x, options[i], option_fg, option_bg);
        }
    }
    
    // Add a command to the buffer to trigger a render
    char command[64];
    snprintf(command, sizeof(command), "MENU:%d:%d:%d", y, x, selected_index);
    write_to_ringbuffer(&output_buffer, command);
    
    unlock_output_mutex();
    
    return true;
}

bool print_menu_default(const char* title, const char** options, int option_count,
                        int selected_index, int y, int x) {
    return print_menu(title, options, option_count, selected_index, y, x,
                     COLOR_UI_HIGHLIGHT, COLOR_UI_BG,
                     COLOR_UI_FG, COLOR_UI_BG,
                     COLOR_UI_HIGHLIGHT, COLOR_UI_BG);
}

bool draw_box(int y, int x, int height, int width, const char* title,
              uint32_t fg_color, uint32_t bg_color) {
    if (!standard_plane || height <= 0 || width <= 0) {
        log_msg(ERROR, "output_handler", "Invalid box parameters");
        return false;
    }
    
    lock_output_mutex();
    
    uint64_t channels = make_channels(fg_color, bg_color);
    ncplane_set_channels(standard_plane, channels);
    
    // Draw the box
    ncplane_cursor_move_yx(standard_plane, y, x);
    ncplane_box(standard_plane, NULL, NULL, height, width, 0);
    
    // Draw the title if provided
    if (title && strlen(title) > 0) {
        // Center the title
        int title_x = x + (width - strlen(title)) / 2;
        if (title_x < x) title_x = x;
        
        // Print the title
        print_text(y, title_x, title, fg_color, bg_color);
    }
    
    // Add a command to the buffer to trigger a render
    char command[64];
    snprintf(command, sizeof(command), "BOX:%d:%d:%d:%d:%06X", y, x, height, width, fg_color);
    write_to_ringbuffer(&output_buffer, command);
    
    unlock_output_mutex();
    
    return true;
}

bool draw_box_default(int y, int x, int height, int width, const char* title) {
    return draw_box(y, x, height, width, title, COLOR_UI_FG, COLOR_UI_BG);
}

bool render_frame(void) {
    if (!notcurses_instance) {
        log_msg(ERROR, "output_handler", "Output handler not initialized");
        return false;
    }
    
    lock_output_mutex();
    
    // Render all changes
    int ret = notcurses_render(notcurses_instance);
    
    // Add a command to the buffer
    write_to_ringbuffer(&output_buffer, "RENDER");
    
    unlock_output_mutex();
    
    return ret >= 0;
}

bool get_screen_dimensions(int* width, int* height) {
    if (!standard_plane || !width || !height) {
        log_msg(ERROR, "output_handler", "Invalid parameters for get_screen_dimensions");
        return false;
    }
    
    lock_output_mutex();
    
    // Get the dimensions of the standard plane
    *width = ncplane_dim_x(standard_plane);
    *height = ncplane_dim_y(standard_plane);
    
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
    notcurses_instance = NULL;
    standard_plane = NULL;
    
    log_msg(INFO, "output_handler", "Output handler shut down");
}