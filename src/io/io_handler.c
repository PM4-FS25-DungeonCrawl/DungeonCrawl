#include "io_handler.h"
#include "../logging/logger.h"
#include "../thread/thread_handler.h"

#include <stdbool.h>
#include <string.h>

#ifndef _WIN32
#include <unistd.h> // for usleep
#endif

// Global variables to maintain IO state
static volatile bool io_running = false;
static int current_game_state = 0;

// Forward declarations
static void process_io_events_thread(void);

bool init_io_handler(struct notcurses* nc, struct ncplane* stdplane) {
    if (!nc || !stdplane) {
        log_msg(ERROR, "io_handler", "Null Notcurses instance or standard plane provided");
        return false;
    }
    
    // Initialize input handler (which starts its own thread)
    if (!init_input_handler(nc)) {
        log_msg(ERROR, "io_handler", "Failed to initialize input handler");
        return false;
    }
    
    // Initialize common output handler (which starts its own thread)
    if (!init_output_handler(nc, stdplane)) {
        log_msg(ERROR, "io_handler", "Failed to initialize output handler");
        shutdown_input_handler();
        return false;
    }
    
    // Initialize media output handler
    if (!init_media_output(nc)) {
        log_msg(ERROR, "io_handler", "Failed to initialize media output handler");
        shutdown_output_handler();
        shutdown_input_handler();
        return false;
    }
    
    // Initialize map output handler
    if (!init_map_output()) {
        log_msg(ERROR, "io_handler", "Failed to initialize map output handler");
        shutdown_media_output();
        shutdown_output_handler();
        shutdown_input_handler();
        return false;
    }
    
    // Start the IO event processing thread
    io_running = true;
    start_simple_thread(process_io_events_thread);
    
    log_msg(INFO, "io_handler", "IO handler initialized successfully with multithreading");
    return true;
}

// Main IO event processing thread function
static void process_io_events_thread(void) {
    log_msg(INFO, "io_handler", "IO event processing thread started");
    
    while (io_running) {
        // Get next input event (non-blocking)
        input_event_t event;
        
        // Check for input events
        if (get_input_nonblocking(&event)) {
            // Handle the input event based on the current game state
            switch (current_game_state) {
                // Note: in a real implementation, we would dispatch to different
                // handler functions based on the game state and input type
                
                default:
                    // Default handling for any game state
                    if (event.type == INPUT_QUIT) {
                        // Handle quit event
                        io_running = false;
                    }
                    break;
            }
        }
        
        // Small sleep to avoid using 100% CPU
        #ifdef _WIN32
            Sleep(10); // 10 milliseconds
        #else
            usleep(10000); // 10 milliseconds
        #endif
    }
    
    log_msg(INFO, "io_handler", "IO event processing thread stopped");
}

// Update the game state and return the new state
// This function will be called by the game loop
int process_io_events(int game_state) {
    // Update the current game state
    current_game_state = game_state;
    
    // Return the current game state
    // The actual state changes are handled in the IO event processing thread
    return current_game_state;
}

void shutdown_io_handler(void) {
    // Stop the IO event processing thread
    io_running = false;
    
    // Allow the thread some time to clean up
    #ifdef _WIN32
        Sleep(100);
    #else
        usleep(100000);
    #endif
    
    // Shutdown all subsystems in reverse order of initialization
    shutdown_map_output();
    shutdown_media_output();
    shutdown_output_handler();
    shutdown_input_handler();
    
    log_msg(INFO, "io_handler", "IO handler shut down");
}