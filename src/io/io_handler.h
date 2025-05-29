/**
 * @file io_handler.h
 * @brief Exposes functions for the IO-Handler.
 */
#ifndef IO_HANDLER_H
#define IO_HANDLER_H

#include "../common.h"// Added for COMMON_SUCCESS
#include "input/input_handler.h"
#include "input/input_types.h"
#include "output/common/output_handler.h"
#include "output/media/media_output.h"

#include <notcurses/notcurses.h>
#include <stdbool.h>

/**
 * Supported platform types for platform-specific handling
 */
typedef enum platform_type {
    PLATFORM_TYPE_UNKNOWN,
    PLATFORM_TYPE_WINDOWS,
    PLATFORM_TYPE_MACOS,
    PLATFORM_TYPE_LINUX
} platform_type_t;

/**
 * Main IO handler structure for Dungeon Crawl
 * Encapsulates the notcurses context and provides interface to input/output handlers
 */
typedef struct io_handler {
    struct notcurses* nc;        // Main notcurses context
    struct ncplane* stdplane;    // Standard plane for basic operations
    bool initialized;            // Flag to track initialization state
    platform_type_t platform;    // Current platform
} io_handler_t;

// Make the init_done flag accessible from other files
extern volatile int init_done;

// Global notcurses instance and standard plane
// These are used directly for rendering and are no longer part of a separate output thread
extern struct notcurses* nc;
extern struct ncplane* stdplane;

// Global io_handler instance
extern io_handler_t* g_io_handler;

/**
 * @brief Initialize the IO handler
 *
 * Sets up all input and output subsystems.
 * @return COMMON_SUCCESS on success, non-zero value on failure
 */
int init_io_handler(void);

/**
 * @brief Initialize the IO handler with a new io_handler_t structure
 * 
 * @return Pointer to the initialized io_handler_t, or NULL on failure
 */
io_handler_t* io_handler_init(void);

/**
 * @brief Initialize the IO handler with custom flags
 * 
 * @param flags Notcurses initialization flags
 * @return Pointer to the initialized io_handler_t, or NULL on failure
 */
io_handler_t* io_handler_init_with_flags(uint64_t flags);

/**
 * @brief Render the current frame using the io_handler
 * 
 * @param handler Pointer to the io_handler_t
 * @return true on success, false on failure
 */
bool io_handler_render(io_handler_t* handler);

/**
 * @brief Shutdown and cleanup the io_handler
 * 
 * @param handler Pointer to the io_handler_t to shutdown
 */
void io_handler_shutdown(io_handler_t* handler);

/**
 * @brief Get the current platform type
 * 
 * @return The detected platform type
 */
platform_type_t io_handler_get_platform(void);


/**
 * @brief Execute a function in a background thread
 *
 * This function executes the provided callback in a background thread.
 *
 * @param callback Function to execute in the background
 * @return true if the thread started successfully, false otherwise
 */
bool run_background_task(void (*callback)(void));

/**
 * @brief Shutdown the IO handler
 *
 * Cleans up all input and output subsystems.
 */
void shutdown_io_handler(void);

#endif// IO_HANDLER_H
