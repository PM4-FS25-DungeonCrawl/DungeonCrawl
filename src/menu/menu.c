#include "menu.h"

#include "../common.h"
#include "../logging/logger.h"

#include <notcurses/notcurses.h>
#include <stdio.h>
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
    ncplane_printf_yx(stdplane, MENU_START_Y, MENU_START_X, "Warning: All unsaved progress will be lost!");
    ncplane_printf_yx(stdplane, MENU_START_Y + 2, MENU_START_X, "%s", message);
    ncplane_printf_yx(stdplane, MENU_START_Y + 4, MENU_START_X, "(Y/N)");
    notcurses_render(nc);

    while (1) {
        ncinput input;
        memset(&input, 0, sizeof(input));
        notcurses_get_blocking(nc, &input);
        if (input.id == 'y' || input.id == 'Y') {
            return true;
        } else if (input.id == 'n' || input.id == 'N') {
            return false;
        }
    }
}
