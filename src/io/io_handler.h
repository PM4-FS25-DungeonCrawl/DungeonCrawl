#ifndef IO_HANDLER_H
#define IO_HANDLER_H

#include "input/input_types.h"
#include "input/input_handler.h"
#include "output/colors.h"
#include "output/common/common_output.h"
#include "output/media/media_output.h"
#include "output/specific/map_output.h"

#include <notcurses/notcurses.h>
#include <stdbool.h>

/**
 * @brief Initialize the IO handler
 * 
 * Sets up all input and output subsystems.
 * 
 * @param nc The Notcurses instance
 * @param stdplane The standard plane
 * @return true on success, false on failure
 */
bool init_io_handler(struct notcurses* nc, struct ncplane* stdplane);

/**
 * @brief Process input and dispatch to appropriate handler
 * 
 * This is the main IO event loop function.
 * Waits for input and dispatches it to the appropriate handler
 * based on the current game state.
 * 
 * @param game_state The current game state
 * @return The next game state after input processing
 */
int process_io_events(int game_state);

/**
 * @brief Shutdown the IO handler
 * 
 * Cleans up all input and output subsystems.
 */
void shutdown_io_handler(void);

#endif // IO_HANDLER_H