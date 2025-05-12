#include "common_output.h"

#include "../../../common.h"
#include "../../../logging/logger.h"
#include "../../io_handler.h"// Include this to access global nc and stdplane

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
    #include <unistd.h>// for usleep
#else
    #include <windows.h>
#endif

// Using the global variables from io_handler.h
// No need to redeclare them here as they're already included through io_handler.h

bool init_output_handler() {
    log_msg(INFO, "output_handler", "Starting initialization");

    if (!nc) {
        log_msg(ERROR, "output_handler", "Null Notcurses instance provided");
        return false;
    }

    if (!stdplane) {
        log_msg(ERROR, "output_handler", "Null standard plane provided");
        return false;
    }

    log_msg(INFO, "output_handler", "Output handler initialized");
    return true;
}

void clear_screen(void) {
    if (!stdplane) {
        log_msg(ERROR, "output_handler", "Output handler not initialized");
        return;
    }

    // Clear the plane with the default colors
    ncplane_set_base(stdplane, " ", 0, DEFAULT_COLORS);
    ncplane_erase(stdplane);
}

void print_text(int y, int x, const char* text, uint64_t ncchannel) {
    if (!stdplane || !text) {
        log_msg(ERROR, "output_handler", "Output handler not initialized or null text");
        return;
    }

    // Set the channels and print the text
    ncplane_set_channels(stdplane, ncchannel);
    ncplane_putstr_yx(stdplane, y, x, text);
}

void print_text_default(int y, int x, const char* text) {
    print_text(y, x, text, DEFAULT_COLORS);
}

void print_text_multi_line(int y, int x, const char* text, int max_width, uint64_t ncchannel) {
    if (!stdplane || !text || max_width <= 0) {
        log_msg(ERROR, "output_handler", "Invalid parameters for print_text_multi_line");
        return;
    }

    // Set the channels
    ncplane_set_channels(stdplane, ncchannel);

    // Handle line wrapping
    const char* ptr = text;
    int current_y = y;
    int line_len = 0;
    char line_buffer[256];// Assuming max line length

    while (*ptr) {
        // Copy characters until max_width or newline
        line_len = 0;
        while (*ptr && *ptr != '\n' && line_len < max_width) {
            line_buffer[line_len++] = *ptr++;
        }
        line_buffer[line_len] = '\0';

        // Print this line
        ncplane_putstr_yx(stdplane, current_y, x, line_buffer);
        current_y++;

        // Handle newline character
        if (*ptr == '\n') {
            ptr++;
        }
    }
}

void print_text_multi_line_default(int y, int x, const char* text, int max_width) {
    print_text_multi_line(y, x, text, max_width, DEFAULT_COLORS);
}

void print_text_multi_strings(int y, int x, const char* text[], int count, uint64_t ncchannel) {
    if (!stdplane || !text || count <= 0) {
        log_msg(ERROR, "output_handler", "Invalid parameters for print_text_multi_strings");
        return;
    }

    // Set the channels
    ncplane_set_channels(stdplane, ncchannel);

    // Print each string on a new line
    for (int i = 0; i < count; i++) {
        if (text[i]) {
            ncplane_putstr_yx(stdplane, y + i, x, text[i]);
        }
    }
}

void print_text_multi_strings_default(int y, int x, const char* text[], int count) {
    print_text_multi_strings(y, x, text, count, DEFAULT_COLORS);
}

void print_menu(const char* title, const char** options, int option_count,
                int selected_index, int y, int x,
                uint64_t title_channel,
                uint64_t option_channel,
                uint64_t selected_channel) {
    if (!stdplane || !title || !options || option_count <= 0) {
        log_msg(ERROR, "output_handler", "Invalid menu parameters");
        return;
    }

    // Print title
    ncplane_set_channels(stdplane, title_channel);
    ncplane_putstr_yx(stdplane, y, x, title);

    // Print options
    for (int i = 0; i < option_count; i++) {
        if (i == selected_index) {
            // Highlight selected option
            ncplane_set_channels(stdplane, selected_channel);
        } else {
            ncplane_set_channels(stdplane, option_channel);
        }
        ncplane_putstr_yx(stdplane, y + i + 1, x, options[i]);
    }
}

void print_menu_default(const char* title, const char** options, int option_count,
                        int selected_index, int y, int x) {
    print_menu(title, options, option_count, selected_index, y, x,
               DEFAULT_COLORS, DEFAULT_COLORS, INVERTED_COLORS);
}

bool render_frame(void) {
    if (!nc) {
        log_msg(ERROR, "output_handler", "Output handler not initialized");
        return false;
    }

    // Render all changes directly
    int ret = notcurses_render(nc);
    return ret >= 0;
}

bool get_screen_dimensions(int* width, int* height) {
    if (!stdplane || !width || !height) {
        log_msg(ERROR, "output_handler", "Invalid parameters for get_screen_dimensions");
        return false;
    }

    // Get the dimensions of the standard plane
    *width = ncplane_dim_x(stdplane);
    *height = ncplane_dim_y(stdplane);

    return true;
}

void shutdown_output_handler(void) {
    // Reset the globals
    nc = NULL;
    stdplane = NULL;

    log_msg(INFO, "output_handler", "Output handler shut down");
}