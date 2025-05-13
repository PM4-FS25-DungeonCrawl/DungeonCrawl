#include "menu.h"

#include "../common.h"
#include "../io/input/input_handler.h"
#include "../logging/logger.h"
#include "local/save_menu_local.h"

#include <notcurses/notcurses.h>
#include <string.h>

extern struct notcurses* nc;
extern struct ncplane* stdplane;

void draw_menu(const char** menu_options, int menu_count, int selected_index) {
    ncplane_set_channels(stdplane, DEFAULT_COLORS);
    // clear screen
    for (uint i = 0; i < ncplane_dim_x(stdplane); i++) {
        for (uint j = 0; j < ncplane_dim_y(stdplane); j++) {
            ncplane_printf_yx(stdplane, (int) j, (int) i, " ");
        }
    }
    // Draw title
    ncplane_set_channels(stdplane, RED_TEXT_COLORS);
    ncplane_printf_yx(stdplane, MENU_START_Y, MENU_START_X, "%s", TITLE);

    // Draw menu options
    int y = MENU_START_Y + 3;
    for (int i = 0; i < menu_count; i++) {
        if (i == selected_index) {
            ncplane_set_channels(stdplane, INVERTED_COLORS);
            ncplane_printf_yx(stdplane, y, MENU_START_X, "%s", menu_options[i]);
        } else {
            ncplane_set_channels(stdplane, DEFAULT_COLORS);
            ncplane_printf_yx(stdplane, y, MENU_START_X, "%s", menu_options[i]);
        }
        y += MENU_ITEM_SPACING;
    }

    notcurses_render(nc);
}

bool show_confirmation(const char* message) {
    // clear screen
    ncplane_set_channels(stdplane, DEFAULT_COLORS);
    for (uint i = 0; i < ncplane_dim_x(stdplane); i++) {
        for (uint j = 0; j < ncplane_dim_y(stdplane); j++) {
            ncplane_printf_yx(stdplane, (int) j, (int) i, " ");
        }
    }
    ncplane_set_channels(stdplane, DEFAULT_COLORS);
    ncplane_printf_yx(stdplane, MENU_START_Y, MENU_START_X, "%s", save_menu_strings[WARNING_LOST_PROGRESS]);
    ncplane_printf_yx(stdplane, MENU_START_Y + 2, MENU_START_X, "%s", message);
    ncplane_printf_yx(stdplane, MENU_START_Y + 4, MENU_START_X, "(Y/N)");
    notcurses_render(nc);

    while (1) {
        input_event_t input_event;
        if (get_input_blocking(&input_event)) {
            if (input_event.raw_input.id == 'y' || input_event.raw_input.id == 'Y') {
                return true;
            } else if (input_event.raw_input.id == 'n' || input_event.raw_input.id == 'N') {
                return false;
            }
        }
    }
}
