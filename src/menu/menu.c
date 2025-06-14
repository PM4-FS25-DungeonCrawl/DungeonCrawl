/**
 * @file menu.c
 * @brief Implementation of helper functions for menus.
 */
#include "menu.h"

#include "../common.h"
#include "../io/input/input_handler.h"
#include "../io/output/common/output_handler.h"
#include "../io/output/common/text_output.h"
#include "../local/local_handler.h"
#include "../logging/logger.h"
#include "local/save_menu_local.h"

#include <string.h>

void draw_menu(const char** menu_options, int menu_count, int selected_index) {
    // Clear the screen
    clear_screen();

    // Draw title
    char* title = get_local_string("LAUNCH.TITLE");
    print_text(MENU_START_Y, MENU_START_X, title, RED_TEXT_COLORS);
    free(title);

    // Draw menu options
    int y = MENU_START_Y + 3;
    for (int i = 0; i < menu_count; i++) {
        if (i == selected_index) {
            // Highlight selected option
            print_text(y, MENU_START_X, menu_options[i], INVERTED_COLORS);
        } else {
            // Normal option
            print_text_default(y, MENU_START_X, menu_options[i]);
        }
        y += MENU_ITEM_SPACING;
    }

    // Render the frame
    render_frame();
}

bool show_confirmation(const char* message) {
    // Clear the screen
    clear_screen();

    // Print confirmation messages
    print_text_default(MENU_START_Y, MENU_START_X, save_menu_strings[WARNING_LOST_PROGRESS]);
    print_text_default(MENU_START_Y + 2, MENU_START_X, message);
    char* yes_no = get_local_string("CONFIRM.YES_NO");
    print_text_default(MENU_START_Y + 4, MENU_START_X, yes_no);
    free(yes_no);

    // Render the frame
    render_frame();

    // Wait for Y/N input
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
