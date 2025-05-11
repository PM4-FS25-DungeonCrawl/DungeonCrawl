#include "io_handler.h"
#include "../logging/logger.h"
#include "../thread/thread_handler.h"
#include "../common.h"  // Added for COMMON_SUCCESS
#include "input/input_handler.h"
#include "output/common/common_output.h"
#include "output/specific/wait_output.h"

#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#ifndef _WIN32
#include <unistd.h> // for usleep
#endif

// Global notcurses instance and standard plane
struct notcurses* nc = NULL;
struct ncplane* stdplane = NULL;

// Loading screen variables
static char loading_message[256] = "";

int init_io_handler(void) {
    log_msg(INFO, "io_handler", "Starting initialization");

    //initialize the Notcurses instance and standard plane
    notcurses_options ncopt;
    memset(&ncopt, 0, sizeof(ncopt));

    log_msg(INFO, "io_handler", "Initializing notcurses");
    nc = notcurses_init(&ncopt, stdout);
    if (nc == NULL) {
        log_msg(ERROR, "io_handler", "Failed to initialize notcurses");
        return 1; // Error code
    }

    log_msg(INFO, "io_handler", "Getting standard plane");
    stdplane = notcurses_stdplane(nc);
    if (!stdplane) {
        log_msg(ERROR, "io_handler", "Failed to get standard plane");
        notcurses_stop(nc);
        nc = NULL;
        return 2; // Error code
    }

    ncplane_set_bg_rgb(stdplane, 0x281D10);

    // Initialize input handler (which starts its own thread)
    if (!init_input_handler(nc)) {
        log_msg(ERROR, "io_handler", "Failed to initialize input handler");
        return 3; // Error code
    }

    // Initialize common output handler
    if (!init_output_handler()) {
        log_msg(ERROR, "io_handler", "Failed to initialize output handler");
        shutdown_input_handler();
        return 4; // Error code
    }

    log_msg(INFO, "io_handler", "IO handler initialized successfully");
    return COMMON_SUCCESS; // 0
}

// Get next input event (non-blocking)
bool get_next_input_event(input_event_t* event) {
    if (!event) {
        return false;
    }

    return get_input_nonblocking(event);
}



// Execute a callback in a background thread
bool run_background_task(void (*callback)(void)) {
    log_msg(INFO, "io_handler", "Running background task");
    if (!callback) {
        log_msg(ERROR, "io_handler", "Invalid callback for background task");
        return false;
    }

    // Start a thread to execute the callback
    start_simple_thread(callback);
    return true;
}

// Convenience wrapper around render_frame
void render_io_frame(void) {
    render_frame();
}

void shutdown_io_handler(void) {
    // Shutdown subsystems in reverse order of initialization
    shutdown_output_handler();
    shutdown_input_handler();

    // Shutdown notcurses
    if (nc) {
        notcurses_stop(nc);
        nc = NULL;
        stdplane = NULL;
    }

    log_msg(INFO, "io_handler", "IO handler shut down");
}