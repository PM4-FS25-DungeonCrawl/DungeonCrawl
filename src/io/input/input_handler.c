#include "input_handler.h"
#include "../../logging/logger.h"

#include <string.h>

// The Notcurses instance to use for input handling
static struct notcurses* notcurses_instance = NULL;

bool init_input_handler(struct notcurses* nc) {
    if (!nc) {
        log_msg(ERROR, "input_handler", "Null Notcurses instance provided");
        return false;
    }
    
    notcurses_instance = nc;
    log_msg(INFO, "input_handler", "Input handler initialized");
    return true;
}

bool get_input_blocking(input_event_t* event) {
    if (!event || !notcurses_instance) {
        log_msg(ERROR, "input_handler", "Null event pointer or uninitialized handler");
        return false;
    }
    
    // Initialize the event with INPUT_NONE
    event->type = INPUT_NONE;
    memset(&event->raw_input, 0, sizeof(ncinput));
    
    // Wait for input
    uint32_t ret = notcurses_get_blocking(notcurses_instance, &event->raw_input);
    if (ret <= 0) {
        log_msg(DEBUG, "input_handler", "No input received from notcurses_get_blocking");
        return false;
    }
    
    // Translate the raw input to a logical input type
    event->type = translate_input(&event->raw_input);
    return true;
}

bool get_input_nonblocking(input_event_t* event) {
    if (!event || !notcurses_instance) {
        log_msg(ERROR, "input_handler", "Null event pointer or uninitialized handler");
        return false;
    }
    
    // Initialize the event with INPUT_NONE
    event->type = INPUT_NONE;
    memset(&event->raw_input, 0, sizeof(ncinput));
    
    // Check for input
    uint32_t ret = notcurses_get_nblock(notcurses_instance, &event->raw_input);
    if (ret <= 0) {
        // No input available, not an error
        return false;
    }
    
    // Translate the raw input to a logical input type
    event->type = translate_input(&event->raw_input);
    return true;
}

input_t translate_input(const ncinput* raw_input) {
    if (!raw_input) {
        return INPUT_NONE;
    }
    
    // Handle special case for Ctrl+C to quit
    if (raw_input->id == 'c' && (raw_input->modifiers & NCKEY_MOD_CTRL)) {
        return INPUT_QUIT;
    }

    // Check for different event types
    if (raw_input->evtype == NCTYPE_UNKNOWN || raw_input->evtype == NCTYPE_PRESS) {
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

void shutdown_input_handler(void) {
    // No resources to clean up, just log that we're shutting down
    notcurses_instance = NULL;
    log_msg(INFO, "input_handler", "Input handler shut down");
}