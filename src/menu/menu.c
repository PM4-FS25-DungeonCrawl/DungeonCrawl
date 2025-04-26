#include "menu.h"


void draw_menu(const char** menu_options, int menu_count, int selected_index) {
    tb_clear();

    // Draw title
    tb_print(MENU_START_X, MENU_START_Y, TITLE_COLOR, TB_DEFAULT, TITLE);

    // Draw menu options
    int y = MENU_START_Y + 3;
    for (int i = 0; i < menu_count; i++) {
        if (i == selected_index) {
            tb_print(MENU_START_X, y, TB_BLACK, TB_WHITE, menu_options[i]);
        } else {
            tb_print(MENU_START_X, y, TB_WHITE, TB_DEFAULT, menu_options[i]);
        }
        y += MENU_ITEM_SPACING;
    }

    tb_present();
}

bool show_confirmation(const char* message) {
    tb_clear();
    tb_print(MENU_START_X, MENU_START_Y, TB_WHITE, TB_DEFAULT, "Warning: All unsaved progress will be lost!");
    tb_print(MENU_START_X, MENU_START_Y + 2, TB_WHITE, TB_DEFAULT, message);
    tb_print(MENU_START_X, MENU_START_Y + 4, TB_WHITE, TB_DEFAULT, "(Y/N)");
    tb_present();

    while (1) {
        struct tb_event event;
        tb_poll_event(&event);

        if (event.ch == 'y' || event.ch == 'Y') {
            return true;
        } else if (event.ch == 'n' || event.ch == 'N') {
            return false;
        }
    }
}
