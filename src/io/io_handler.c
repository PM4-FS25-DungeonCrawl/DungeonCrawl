#include "io_handler.h"
#include "../logging/logger.h"

bool init_io_handler(struct notcurses* nc, struct ncplane* stdplane) {
    if (!nc || !stdplane) {
        log_msg(ERROR, "io_handler", "Null Notcurses instance or standard plane provided");
        return false;
    }
    
    // Initialize input handler
    if (!init_input_handler(nc)) {
        log_msg(ERROR, "io_handler", "Failed to initialize input handler");
        return false;
    }
    
    // Initialize common output handler
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
    
    log_msg(INFO, "io_handler", "IO handler initialized successfully");
    return true;
}

// Placeholder for the event processing function that will be implemented
// when integrating with the game engine
int process_io_events(int game_state) {
    // This is a stub and will be implemented during integration
    // It would handle input events based on the current game state
    // and dispatch them to the appropriate handlers
    return game_state;
}

void shutdown_io_handler(void) {
    // Shutdown all subsystems in reverse order of initialization
    shutdown_map_output();
    shutdown_media_output();
    shutdown_output_handler();
    shutdown_input_handler();
    
    log_msg(INFO, "io_handler", "IO handler shut down");
}