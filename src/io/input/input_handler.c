#include "input_handler.h"

#include "../../logging/logger.h"
#include "../io_handler.h"// Include io_handler.h to access global nc

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// Define platform-specific key event type
#ifdef __APPLE__
    #define KEY_EVENT NCTYPE_PRESS
#else
    #define KEY_EVENT NCTYPE_UNKNOWN
#endif

// Use the global nc from io_handler.c - no need to redeclare it here

/**
 * @brief Translate a raw Notcurses input to a logical input type
 *
 * Helper function to convert from hardware-specific to logical inputs.
 *
 * @param raw_input The raw Notcurses input
 * @return The corresponding logical input type
 */
input_t translate_input(const ncinput* raw_input) {
    if (!raw_input) {
        return INPUT_NONE;
    }

    // Handle special case for Ctrl+C to quit
    if (raw_input->id == 'c' && (raw_input->modifiers & NCKEY_MOD_CTRL)) {
        return INPUT_QUIT;
    }

    // Check if this is a key event (allow both NCTYPE_UNKNOWN and NCTYPE_PRESS)
    if (raw_input->evtype == NCTYPE_PRESS || raw_input->evtype == NCTYPE_UNKNOWN) {
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

        // Stats key (I)
        if (raw_input->id == 'i' || raw_input->id == 'I') return INPUT_INVENTORY;
    }

    // If we didn't match anything, return INPUT_NONE
    return INPUT_NONE;
}

bool init_input_handler(struct notcurses* notcurses_ptr) {
    if (!notcurses_ptr) {
        log_msg(ERROR, "input_handler", "Null Notcurses instance provided");
        return false;
    }

    // Assign to the global variable
    nc = notcurses_ptr;
    return true;
}

bool get_input_blocking(input_event_t* event) {
    if (!event || !nc) {
        log_msg(ERROR, "input_handler", "Null event pointer or uninitialized handler");
        return false;
    }

    // Get input directly using notcurses
    ncinput raw_input;
    memset(&raw_input, 0, sizeof(ncinput));

    uint32_t ret = notcurses_get_blocking(nc, &raw_input);
    if (ret > 0) {
        // Translate and fill the event structure
        event->type = translate_input(&raw_input);
        event->raw_input = raw_input;
        return true;
    }

    // No valid input received
    event->type = INPUT_NONE;
    memset(&event->raw_input, 0, sizeof(ncinput));
    return false;
}

bool get_input_nonblocking(input_event_t* event) {
    if (!event || !nc) {
        log_msg(ERROR, "input_handler", "Null event pointer or uninitialized handler");
        return false;
    }

    // Get input directly using notcurses
    ncinput raw_input;
    memset(&raw_input, 0, sizeof(ncinput));

    uint32_t ret = notcurses_get_nblock(nc, &raw_input);
    if (ret > 0) {
        // Translate and fill the event structure
        event->type = translate_input(&raw_input);
        event->raw_input = raw_input;
        return true;
    }

    // No valid input received
    event->type = INPUT_NONE;
    memset(&event->raw_input, 0, sizeof(ncinput));
    return false;
}

void shutdown_input_handler(void) {
    nc = NULL;
}