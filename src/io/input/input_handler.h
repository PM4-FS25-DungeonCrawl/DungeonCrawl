/**
 * @file input_handler.h
 * @brief Exposes functions for working with input.
 */
#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include "input_types.h"

#include <notcurses/notcurses.h>

/**
 * @brief Initialize the input handler
 * 
 * Sets up the input handling system.
 * This function must be called before any other input functions.
 * 
 * @param notcurses_ptr Pointer to the Notcurses instance to use for input handling
 * @return true on success, false on failure
 */
bool init_input_handler(struct notcurses* notcurses_ptr);

/**
 * @brief Get the next input event (blocking)
 * 
 * Waits for an input event and translates it to a logical input type.
 * This function blocks until input is received.
 * 
 * @param[out] event Pointer to an input_event_t structure to fill with the input event
 * @return true if an event was retrieved, false on error
 */
bool get_input_blocking(input_event_t* event);

/**
 * @brief Get the next input event (non-blocking)
 * 
 * Checks for an input event and translates it to a logical input type.
 * This function does not block if no input is available.
 * 
 * @param[out] event Pointer to an input_event_t structure to fill with the input event
 * @return true if an event was retrieved, false if no events are available
 */
bool get_input_nonblocking(input_event_t* event);

/**
 * @brief Translate a raw Notcurses input to a logical input type
 *
 * Helper function to convert from hardware-specific to logical inputs.
 *
 * @param raw_input The raw Notcurses input
 * @return The corresponding logical input type
 */
input_t translate_input(const ncinput* raw_input);

/**
 * @brief Shutdown the input handler
 * 
 * Cleans up resources. This function should be called when shutting down the game.
 */
void shutdown_input_handler(void);

#endif// INPUT_HANDLER_H
