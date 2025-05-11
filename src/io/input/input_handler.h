#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include "input_types.h"
#include <stdbool.h>
#include <notcurses/notcurses.h>

/**
 * @brief Initialize the input handler
 * 
 * Sets up the input handling system.
 * This function must be called before any other input functions.
 * 
 * @param nc The Notcurses instance to use for input handling
 * @return true on success, false on failure
 */
bool init_input_handler(struct notcurses* nc);

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
// Made static to avoid conflicts with implementation in input_handler.c
static inline input_t translate_input(const ncinput* raw_input)
{
    if (!raw_input) {
        return INPUT_NONE;
    }

    // Handle special case for Ctrl+C to quit
    if (raw_input->id == 'c' && (raw_input->modifiers & NCKEY_MOD_CTRL)) {
        return INPUT_QUIT;
    }

    // Check if this is a key event
    if (raw_input->evtype == NCTYPE_PRESS) {
        // Arrow keys for navigation
        if (raw_input->id == NCKEY_UP) return INPUT_UP;
        if (raw_input->id == NCKEY_DOWN) return INPUT_DOWN;
        if (raw_input->id == NCKEY_LEFT) return INPUT_LEFT;
        if (raw_input->id == NCKEY_RIGHT) return INPUT_RIGHT;

        // Enter key for confirmation
        if (raw_input->id == NCKEY_ENTER || raw_input->id == ' ') return INPUT_CONFIRM;

        // Escape key for cancellation
        if (raw_input->id == NCKEY_ESC) return INPUT_CANCEL;

        // Menu key (M)
        if (raw_input->id == 'm' || raw_input->id == 'M') return INPUT_MENU;

        // Stats key (L)
        if (raw_input->id == 'l' || raw_input->id == 'L') return INPUT_STATS;
    }

    // If we didn't match anything, return INPUT_NONE
    return INPUT_NONE;
}

/**
 * @brief Shutdown the input handler
 * 
 * Cleans up resources. This function should be called when shutting down the game.
 */
void shutdown_input_handler(void);

#endif // INPUT_HANDLER_H