#ifndef IO_HANDLER_H
#define IO_HANDLER_H

#include "input/input_types.h"
#include "input/input_handler.h"
#include "output/common/common_output.h"
#include "../common.h"  // Added for COMMON_SUCCESS

#include <notcurses/notcurses.h>
#include <stdbool.h>

// Global notcurses instance and standard plane
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
 * @brief Get next input event (non-blocking)
 *
 * Gets the next input event from the input buffer without blocking.
 *
 * @param event Pointer to store the input event
 * @return true if an event was retrieved, false if no events were available
 */
bool get_next_input_event(input_event_t* event);

/**
 * @brief Draw a loading screen
 *
 * Draws a loading screen with an animation and the specified message.
 *
 * @param loading_text Text to display on the loading screen
 */
void draw_loading_screen(const char* loading_text);

/**
 * @brief Draw the game launch screen
 *
 * Draws a launch screen with the game logo and title.
 */
void draw_launch_screen(void);

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
 * @brief Render a frame
 *
 * Updates the screen with current content. This is a convenience
 * wrapper around the common_output render_frame function.
 */
void render_io_frame(void);

/**
 * @brief Shutdown the IO handler
 *
 * Cleans up all input and output subsystems.
 */
void shutdown_io_handler(void);

#endif // IO_HANDLER_H