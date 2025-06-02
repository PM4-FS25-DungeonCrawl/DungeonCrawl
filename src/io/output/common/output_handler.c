/**
 * @file output_handler.c
 * @brief Implements functionality for handing ouptputting to the console.
 */
#include "output_handler.h"

#include "../../../common.h"
#include "../../../logging/logger.h"
#include "../../input/input_handler.h"
#include "../../input/input_types.h"
#include "../../io_handler.h"

#include <notcurses/nckeys.h>
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
    if (!gio || !gio->nc) {
        log_msg(ERROR, "output_handler", "Null Notcurses instance provided");
        return false;
    }

    if (!gio->stdplane) {
        log_msg(ERROR, "output_handler", "Null standard plane provided");
        return false;
    }
    return true;
}

void clear_screen(void) {
    clear_standard_plane();
}

void clear_all_planes(void) {
    if (!gio->nc) {
        log_msg(ERROR, "output_handler", "Output handler not initialized");
        return;
    }

    // Get the standard plane and clear all planes above and including it
    struct ncplane* plane = gio->stdplane;
    while (plane) {
        ncplane_erase(plane);
        plane = ncplane_below(plane);
    }

    // Also clear the standard plane
    clear_standard_plane();
}

void clear_standard_plane(void) {
    if (!gio->stdplane) {
        log_msg(ERROR, "output_handler", "Output handler not initialized");
        return;
    }

    // Clear the plane with the default colors
    ncplane_set_base(gio->stdplane, " ", 0, DEFAULT_COLORS);
    ncplane_erase(gio->stdplane);
}

void clear_specific_plane(struct ncplane* plane) {
    if (!plane) {
        log_msg(ERROR, "output_handler", "Invalid plane provided to clear_specific_plane");
        return;
    }

    // Clear the specific plane
    ncplane_erase(plane);
}

bool render_frame(void) {
    if (!gio->nc) {
        log_msg(ERROR, "output_handler", "Output handler not initialized");
        return false;
    }

    // Render all changes directly
    int ret = notcurses_render(gio->nc);
    return ret >= 0;
}

bool get_screen_dimensions(int* width, int* height) {
    if (!gio->stdplane || !width || !height) {
        log_msg(ERROR, "output_handler", "Invalid parameters for get_screen_dimensions");
        return false;
    }

    // Get the dimensions of the standard plane
    *width = ncplane_dim_x(gio->stdplane);
    *height = ncplane_dim_y(gio->stdplane);

    return true;
}

void shutdown_output_handler(void) {
    // Reset the globals
    gio->nc = NULL;
    gio->stdplane = NULL;
}
