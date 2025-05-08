#include "common_output.h"
#include "../../../logging/logger.h"

#include <string.h>

// Global variables to store Notcurses state
static struct notcurses* notcurses_instance = NULL;
static struct ncplane* standard_plane = NULL;

bool init_output_handler(struct notcurses* nc, struct ncplane* stdplane) {
    if (!nc || !stdplane) {
        log_msg(ERROR, "output_handler", "Null Notcurses instance or standard plane provided");
        return false;
    }
    
    notcurses_instance = nc;
    standard_plane = stdplane;
    
    log_msg(INFO, "output_handler", "Output handler initialized");
    return true;
}

bool clear_screen(uint32_t bg_color) {
    if (!standard_plane) {
        log_msg(ERROR, "output_handler", "Output handler not initialized");
        return false;
    }
    
    // Set the background color
    uint64_t channels = 0;
    ncchannels_set_bg_rgb8(&channels, 
                         COLOR_R(bg_color),
                         COLOR_G(bg_color),
                         COLOR_B(bg_color));
    
    // Clear the plane with the specified color
    ncplane_set_base(standard_plane, " ", 0, channels);
    ncplane_erase(standard_plane);
    
    return true;
}

bool print_text(int y, int x, const char* text, uint32_t fg_color, uint32_t bg_color) {
    if (!standard_plane || !text) {
        log_msg(ERROR, "output_handler", "Output handler not initialized or null text");
        return false;
    }
    
    // Create channels with the specified colors
    uint64_t channels = make_channels(fg_color, bg_color);
    
    // Set the channels and print the text
    ncplane_set_channels(standard_plane, channels);
    int ret = ncplane_putstr_yx(standard_plane, y, x, text);
    
    return ret >= 0;
}

bool print_text_default(int y, int x, const char* text) {
    return print_text(y, x, text, COLOR_DEFAULT_FG, COLOR_DEFAULT_BG);
}

bool print_menu(const char* title, const char** options, int option_count, 
                int selected_index, int y, int x,
                uint32_t title_fg, uint32_t title_bg,
                uint32_t option_fg, uint32_t option_bg,
                uint32_t selected_fg, uint32_t selected_bg) {
    if (!standard_plane || !title || !options || option_count <= 0) {
        log_msg(ERROR, "output_handler", "Invalid menu parameters");
        return false;
    }
    
    // Print title
    print_text(y, x, title, title_fg, title_bg);
    
    // Print options
    for (int i = 0; i < option_count; i++) {
        if (i == selected_index) {
            // Highlight selected option
            print_text(y + i + 1, x, options[i], selected_fg, selected_bg);
        } else {
            print_text(y + i + 1, x, options[i], option_fg, option_bg);
        }
    }
    
    return true;
}

bool print_menu_default(const char* title, const char** options, int option_count,
                        int selected_index, int y, int x) {
    return print_menu(title, options, option_count, selected_index, y, x,
                     COLOR_UI_HIGHLIGHT, COLOR_UI_BG,
                     COLOR_UI_FG, COLOR_UI_BG,
                     COLOR_UI_HIGHLIGHT, COLOR_UI_BG);
}

bool draw_box(int y, int x, int height, int width, const char* title,
              uint32_t fg_color, uint32_t bg_color) {
    if (!standard_plane || height <= 0 || width <= 0) {
        log_msg(ERROR, "output_handler", "Invalid box parameters");
        return false;
    }
    
    uint64_t channels = make_channels(fg_color, bg_color);
    ncplane_set_channels(standard_plane, channels);
    
    // Draw the box
    ncplane_cursor_move_yx(standard_plane, y, x);
    ncplane_box(standard_plane, NULL, NULL, height, width, 0);
    
    // Draw the title if provided
    if (title && strlen(title) > 0) {
        // Center the title
        int title_x = x + (width - strlen(title)) / 2;
        if (title_x < x) title_x = x;
        
        // Print the title
        print_text(y, title_x, title, fg_color, bg_color);
    }
    
    return true;
}

bool draw_box_default(int y, int x, int height, int width, const char* title) {
    return draw_box(y, x, height, width, title, COLOR_UI_FG, COLOR_UI_BG);
}

bool render_frame(void) {
    if (!notcurses_instance) {
        log_msg(ERROR, "output_handler", "Output handler not initialized");
        return false;
    }
    
    // Render all changes
    int ret = notcurses_render(notcurses_instance);
    return ret >= 0;
}

bool get_screen_dimensions(int* width, int* height) {
    if (!standard_plane || !width || !height) {
        log_msg(ERROR, "output_handler", "Invalid parameters for get_screen_dimensions");
        return false;
    }
    
    // Get the dimensions of the standard plane
    *width = ncplane_dim_x(standard_plane);
    *height = ncplane_dim_y(standard_plane);
    
    return true;
}

void shutdown_output_handler(void) {
    // No resources to free, just reset the globals
    notcurses_instance = NULL;
    standard_plane = NULL;
    
    log_msg(INFO, "output_handler", "Output handler shut down");
}