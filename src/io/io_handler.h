#ifndef IO_HANDLER_H
#define IO_HANDLER_H

#include "../common.h"// Added for COMMON_SUCCESS
#include "input/input_handler.h"
#include "input/input_types.h"
#include "output/common/common_output.h"
#include "output/media/media_output.h"

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
 * This now directly calls notcurses_render instead of using a thread.
 */
void render_io_frame(void);

/**
 * @brief Process input events and update IO state
 *
 * This function is called from the main game loop to process
 * any pending input and update the IO system state.
 *
 * @param game_state The current game state
 * @return The new game state after input processing
 */
int process_io_events(int game_state);

/**
 * @brief Shutdown the IO handler
 *
 * Cleans up all input and output subsystems.
 */
void shutdown_io_handler(void);

#endif// IO_HANDLER_H