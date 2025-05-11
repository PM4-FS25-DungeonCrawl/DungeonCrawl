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
 * @brief IO system states
 *
 * These states track what the IO system is currently doing
 */
typedef enum {
    IO_STATE_GAME,       // Normal game operation
    IO_STATE_LOADING,    // Showing loading screen
    IO_STATE_LAUNCH      // Showing launch screen
} io_state_t;

/**
 * @brief Process input and update IO state
 *
 * This function is called from the main game loop to process
 * any pending input and update the IO system state.
 *
 * @param game_state The current game state
 * @return The new game state after input processing
 */
int process_io_events(int game_state);

/**
 * @brief Show a loading screen while performing a background operation
 *
 * This function starts an asynchronous loading screen and executes
 * the provided callback in a background thread. The IO system will
 * show a loading animation until the operation completes.
 *
 * @param loading_text Text to display on the loading screen
 * @param callback Function to execute in the background
 * @return true if the operation started successfully, false otherwise
 */
bool show_loading_screen(const char* loading_text, void (*callback)(void));

/**
 * @brief Show the game launch screen
 *
 * Displays a launch screen with the game logo and title.
 * This screen is shown for a few seconds before transitioning to the main menu.
 *
 * @param duration_ms How long to show the launch screen in milliseconds
 * @return true if the launch screen was shown successfully, false otherwise
 */
bool show_launch_screen(int duration_ms);

/**
 * @brief Shutdown the IO handler
 * 
 * Cleans up all input and output subsystems.
 */
void shutdown_io_handler(void);

#endif // IO_HANDLER_H