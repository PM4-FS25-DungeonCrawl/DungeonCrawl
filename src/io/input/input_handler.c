/**
 * @file input_handler.c
 * @brief Implements functionality for handling input.
 */
#include "input_handler.h"

#include "../../logging/logger.h"
#include "../io_handler.h"                  // Include io_handler.h to access global nc
#include "../output/common/output_handler.h"// For handle_screen_resize

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef CLOCK_MONOTONIC
    #define CLOCK_MONOTONIC 1// Fallback for platforms without CLOCK_MONOTONIC
#endif

// Define platform-specific key event type
#ifdef __APPLE__
    #define KEY_EVENT NCTYPE_PRESS
#else
    #define KEY_EVENT NCTYPE_UNKNOWN
#endif

// Define the minimum time between key presses (milliseconds)
#define KEY_DEBOUNCE_TIME_MS 100

// Structure to track input timing for debouncing
typedef struct {
    struct timespec last_key_time;
    bool first_key;
} input_timing_t;

// Global input timing tracker
static input_timing_t input_timing = {
        .first_key = true};

// Track screen dimensions for resize detection
static int last_screen_width = 0;
static int last_screen_height = 0;
static bool screen_size_initialized = false;

/**
 * @brief Calculate time difference in milliseconds
 *
 * Helper function that computes the difference between two timespec structures
 * and returns the result in milliseconds.
 *
 * @param start Pointer to the start timestamp
 * @param end Pointer to the end timestamp
 * @return Time difference in milliseconds
 */
static long get_time_diff_ms(struct timespec* start, struct timespec* end) {
    return (end->tv_sec - start->tv_sec) * 1000 +
           (end->tv_nsec - start->tv_nsec) / 1000000;
}

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

    // Check for resize event (in older notcurses versions, resize might be detected differently)
    // For now, we'll handle resize through other means since NCTYPE_RESIZE may not be available

    // Check if this is a key event (allow both NCTYPE_UNKNOWN and NCTYPE_PRESS)
    if (raw_input->evtype == NCTYPE_PRESS || raw_input->evtype == NCTYPE_UNKNOWN) {
        // Arrow keys for navigation
        if (raw_input->id == NCKEY_UP) return INPUT_UP;
        if (raw_input->id == NCKEY_DOWN) return INPUT_DOWN;
        if (raw_input->id == NCKEY_LEFT) return INPUT_LEFT;
        if (raw_input->id == NCKEY_RIGHT) return INPUT_RIGHT;

        // Enter key for confirmation
        if (raw_input->id == NCKEY_ENTER || raw_input->id == ' ') return INPUT_CONFIRM;

        // Cancel key (C)
        if (raw_input->id == 'c' || raw_input->id == 'C') return INPUT_CANCEL;

        // Menu key (M)
        if (raw_input->id == 'm' || raw_input->id == 'M') return INPUT_MENU;

        // Stats key (L)
        if (raw_input->id == 'l' || raw_input->id == 'L') return INPUT_STATS;

        // Inventory key (I)
        if (raw_input->id == 'i' || raw_input->id == 'I') return INPUT_INVENTORY;
    }

    // If we didn't match anything, return INPUT_NONE
    return INPUT_NONE;
}

/**
 * @brief Check if the screen has been resized
 * 
 * @return true if resize was detected and handled, false otherwise
 */
static bool check_and_handle_resize(void) {
    int current_width, current_height;
    if (!get_screen_dimensions(&current_width, &current_height)) {
        return false;
    }

    if (!screen_size_initialized) {
        last_screen_width = current_width;
        last_screen_height = current_height;
        screen_size_initialized = true;
        return false;
    }

    if (current_width != last_screen_width || current_height != last_screen_height) {
        log_msg(DEBUG, "input_handler", "Screen resize detected: %dx%d -> %dx%d",
                last_screen_width, last_screen_height, current_width, current_height);

        last_screen_width = current_width;
        last_screen_height = current_height;

        if (!handle_screen_resize()) {
            log_msg(ERROR, "input_handler", "Failed to handle screen resize");
            return false;
        }

        return true;
    }

    return false;
}

bool init_input_handler(struct notcurses* notcurses_ptr) {
    if (!notcurses_ptr) {
        log_msg(ERROR, "input_handler", "Null Notcurses instance provided");
        return false;
    }

    // Initialize input timing
    input_timing.first_key = true;

    return true;
}

/**
 * @brief Check if enough time has passed since the last key press
 *
 * This function implements key debouncing to prevent input overflow.
 * It checks if enough time has passed since the last key press to
 * consider the current key press valid. This helps prevent rapid
 * key repetition that could cause performance issues.
 *
 * @return true if the key should be processed, false if it should be ignored
 */
static bool should_process_key() {
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);

    if (input_timing.first_key) {
        input_timing.first_key = false;
        input_timing.last_key_time = current_time;
        return true;
    }

    long time_diff = get_time_diff_ms(&input_timing.last_key_time, &current_time);

    if (time_diff < KEY_DEBOUNCE_TIME_MS) {
        // Not enough time has passed, ignore this key
        return false;
    }

    // Update the last key time
    input_timing.last_key_time = current_time;
    return true;
}

bool get_input_blocking(input_event_t* event) {
    if (!event || !gio || !gio->nc) {
        log_msg(ERROR, "input_handler", "Null event pointer or uninitialized handler");
        return false;
    }

    // Get input directly using notcurses
    ncinput raw_input;
    memset(&raw_input, 0, sizeof(ncinput));

    // Set default values in case we return early
    event->type = INPUT_NONE;
    memset(&event->raw_input, 0, sizeof(ncinput));

    // Loop until we get a valid input or notcurses returns an error
    while (true) {
        // Check for resize before getting input
        if (check_and_handle_resize()) {
            // Return a resize event
            event->type = INPUT_RESIZE;
            memset(&event->raw_input, 0, sizeof(ncinput));
            return true;
        }

        uint32_t ret = notcurses_get_blocking(gio->nc, &raw_input);
        if (ret <= 0) {
            // Error or no input
            return false;
        }

        // Debounce - if we're getting keys too fast, ignore some
        if (!should_process_key()) {
            continue;
        }

        // Translate and fill the event structure
        event->type = translate_input(&raw_input);
        event->raw_input = raw_input;

        return true;
    }
}

bool get_input_nonblocking(input_event_t* event) {
    if (!event || !gio || !gio->nc) {
        log_msg(ERROR, "input_handler", "Null event pointer or uninitialized handler");
        return false;
    }

    // Get input directly using notcurses
    ncinput raw_input;
    memset(&raw_input, 0, sizeof(ncinput));

    // Set default values in case we return early
    event->type = INPUT_NONE;
    memset(&event->raw_input, 0, sizeof(ncinput));

    // Check for resize first
    if (check_and_handle_resize()) {
        // Return a resize event
        event->type = INPUT_RESIZE;
        memset(&event->raw_input, 0, sizeof(ncinput));
        return true;
    }

    uint32_t ret = notcurses_get_nblock(gio->nc, &raw_input);
    if (ret <= 0) {
        // No input available
        return false;
    }

    // Debounce - if we're getting keys too fast, ignore some
    if (!should_process_key()) {
        return false;
    }

    // Translate and fill the event structure
    event->type = translate_input(&raw_input);
    event->raw_input = raw_input;

    return true;
}
