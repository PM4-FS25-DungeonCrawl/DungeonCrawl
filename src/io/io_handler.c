/**
 * @file io_handler.c
 * @brief Implementation of the handler for handling Input/Output.
 */
#include "io_handler.h"

#include "../common.h"// Added for COMMON_SUCCESS
#include "../logging/logger.h"
#include "../thread/thread_handler.h"
#include "input/input_handler.h"
#include "output/common/output_handler.h"
#include "output/media/media_output.h"
#include "output/media/media_output_handler.h"
#include "output/specific/wait_output.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef _WIN32
    #include <unistd.h>// for usleep
#endif

// Global notcurses instance and standard plane
struct notcurses* nc = NULL;
struct ncplane* stdplane = NULL;

// Global io_handler instance
io_handler_t* g_io_handler = NULL;

/**
 * @brief Detect the current platform
 * 
 * @return The detected platform type
 */
platform_type_t io_handler_get_platform(void) {
#ifdef _WIN32
    return PLATFORM_TYPE_WINDOWS;
#elif defined(__APPLE__)
    return PLATFORM_TYPE_MACOS;
#elif defined(__linux__)
    return PLATFORM_TYPE_LINUX;
#else
    return PLATFORM_TYPE_UNKNOWN;
#endif
}

io_handler_t* io_handler_init(void) {
    io_handler_t* handler = (io_handler_t*)malloc(sizeof(io_handler_t));
    if (!handler) {
        log_msg(ERROR, "io_handler", "Failed to allocate memory for io_handler");
        return NULL;
    }
    
    // Zero-initialize the handler
    memset(handler, 0, sizeof(io_handler_t));
    
    // Detect the platform
    handler->platform = io_handler_get_platform();
    
    // Initialize notcurses
    notcurses_options ncopt;
    memset(&ncopt, 0, sizeof(ncopt));
    
    handler->nc = notcurses_init(&ncopt, stdout);
    if (!handler->nc) {
        log_msg(ERROR, "io_handler", "Failed to initialize notcurses");
        free(handler);
        return NULL;
    }
    
    handler->stdplane = notcurses_stdplane(handler->nc);
    if (!handler->stdplane) {
        log_msg(ERROR, "io_handler", "Failed to get standard plane");
        notcurses_stop(handler->nc);
        free(handler);
        return NULL;
    }
    
    // Set background color
    ncplane_set_bg_rgb(handler->stdplane, 0x281D10);
    
    // Mark as initialized
    handler->initialized = true;
    
    return handler;
}

io_handler_t* io_handler_init_with_flags(uint64_t flags) {
    io_handler_t* handler = (io_handler_t*)malloc(sizeof(io_handler_t));
    if (!handler) {
        log_msg(ERROR, "io_handler", "Failed to allocate memory for io_handler");
        return NULL;
    }
    
    // Zero-initialize the handler
    memset(handler, 0, sizeof(io_handler_t));
    
    // Detect the platform
    handler->platform = io_handler_get_platform();
    
    // Initialize notcurses with custom flags
    notcurses_options ncopt;
    memset(&ncopt, 0, sizeof(ncopt));
    ncopt.flags = flags;
    
    handler->nc = notcurses_init(&ncopt, stdout);
    if (!handler->nc) {
        log_msg(ERROR, "io_handler", "Failed to initialize notcurses");
        free(handler);
        return NULL;
    }
    
    handler->stdplane = notcurses_stdplane(handler->nc);
    if (!handler->stdplane) {
        log_msg(ERROR, "io_handler", "Failed to get standard plane");
        notcurses_stop(handler->nc);
        free(handler);
        return NULL;
    }
    
    // Set background color
    ncplane_set_bg_rgb(handler->stdplane, 0x281D10);
    
    // Mark as initialized
    handler->initialized = true;
    
    return handler;
}

bool io_handler_render(io_handler_t* handler) {
    if (!handler || !handler->nc || !handler->initialized) {
        return false;
    }
    
    return notcurses_render(handler->nc) >= 0;
}

void io_handler_shutdown(io_handler_t* handler) {
    if (!handler) {
        return;
    }
    
    if (handler->nc) {
        notcurses_stop(handler->nc);
        handler->nc = NULL;
        handler->stdplane = NULL;
    }
    
    handler->initialized = false;
    free(handler);
}


int init_io_handler(void) {
    // Use the new structured approach
    g_io_handler = io_handler_init();
    if (!g_io_handler) {
        return 1; // Error code
    }
    
    // Update global references for backward compatibility
    nc = g_io_handler->nc;
    stdplane = g_io_handler->stdplane;
    
    // Initialize input handler (which starts its own thread)
    if (!init_input_handler(nc)) {
        log_msg(ERROR, "io_handler", "Failed to initialize input handler");
        io_handler_shutdown(g_io_handler);
        g_io_handler = NULL;
        nc = NULL;
        stdplane = NULL;
        return 3;// Error code
    }

    // Initialize common output handler
    if (!init_output_handler()) {
        log_msg(ERROR, "io_handler", "Failed to initialize output handler");
        shutdown_input_handler();
        io_handler_shutdown(g_io_handler);
        g_io_handler = NULL;
        nc = NULL;
        stdplane = NULL;
        return 4;// Error code
    }

    // Initialize media output handler
    if (!init_media_output()) {
        log_msg(ERROR, "io_handler", "Failed to initialize media output handler");
        // Continue without media support, not a fatal error
    }

    return COMMON_SUCCESS;// 0
}

// Execute a callback in a background thread
bool run_background_task(void (*callback)(void)) {
    if (!callback) {
        log_msg(ERROR, "io_handler", "Invalid callback for background task");
        return false;
    }

    // Start a thread to execute the callback
    start_simple_thread(callback);
    return true;
}

void shutdown_io_handler(void) {
    // Shutdown subsystems in reverse order of initialization
    shutdown_output_handler();
    shutdown_input_handler();

    // Shutdown the global io_handler
    if (g_io_handler) {
        io_handler_shutdown(g_io_handler);
        g_io_handler = NULL;
        nc = NULL;
        stdplane = NULL;
    } else if (nc) {
        // Fallback for backward compatibility
        notcurses_stop(nc);
        nc = NULL;
        stdplane = NULL;
    }
}
