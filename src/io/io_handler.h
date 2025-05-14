#ifndef IO_HANDLER_H
#define IO_HANDLER_H

#include "../common.h"// Added for COMMON_SUCCESS
#include "input/input_handler.h"
#include "input/input_types.h"
#include "output/common/common_output.h"

#include <notcurses/notcurses.h>
#include <stdbool.h>

// Make the init_done flag accessible from other files
extern volatile int init_done;

// Global notcurses instance and standard plane
// These are used directly for rendering and are no longer part of a separate output thread
extern struct notcurses* nc;
extern struct ncplane* stdplane;

/**
 * @brief Initialize the IO handler
 *
 * Sets up all input and output subsystems.
 * @return COMMON_SUCCESS on success, non-zero value on failure
 */
int init_io_handler(void);

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