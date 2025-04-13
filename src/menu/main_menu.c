#include "main_menu.h"

#include "../../include/termbox2.h"
#include "../logging/logger.h"
#include <stdbool.h>

// Menu options
#define NEW_GAME_OPTION "New Game"
#define CONTINUE_OPTION "Continue"
#define EXIT_OPTION "Exit"

// Menu constants
#define TITLE "DUNGEON CRAWL"
#define TITLE_COLOR TB_RED
#define MENU_START_Y 5
#define MENU_START_X 10
#define MENU_ITEM_SPACING 2

// We'll create these dynamically based on game_in_progress
static const char* NEW_GAME = NEW_GAME_OPTION;
static const char* CONTINUE = CONTINUE_OPTION;
static const char* EXIT = EXIT_OPTION;

void init_main_menu(void) {
    log_msg(INFO, "Menu", "Initializing main menu");
}

void draw_menu(const char** menu_options, int menu_count, int selected_index) {
    tb_clear();
    
    // Calculate center position for title
    int width = tb_width();
    int title_len = sizeof(TITLE) - 1; // -1 to exclude null terminator
    int title_x = (width - title_len) / 2;
    
    // Draw title
    tb_print(title_x, MENU_START_Y, TITLE_COLOR, TB_DEFAULT, TITLE);
    
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
    
    // Draw instructions
    y += 2;
    tb_print(MENU_START_X, y, TB_WHITE, TB_DEFAULT, "Use arrow keys to navigate and Enter to select");
    
    tb_present();
}

menu_result_t show_main_menu(bool game_in_progress) {
    // Create dynamic menu options based on game_in_progress flag
    const char* menu_options[3];
    int menu_count;
    
    // Always include New Game and Exit
    menu_options[0] = NEW_GAME;
    
    if (game_in_progress) {
        // If game is in progress, show Continue option
        menu_options[1] = CONTINUE;
        menu_options[2] = EXIT;
        menu_count = 3;
    } else {
        // If no game in progress, don't show Continue option
        menu_options[1] = EXIT;
        menu_count = 2;
    }
    
    int selected_index = 0;
    menu_result_t result = MENU_CONTINUE;
    bool menu_active = true;
    
    while (menu_active) {
        draw_menu(menu_options, menu_count, selected_index);
        
        struct tb_event event;
        const int ret = tb_peek_event(&event, 10);
        
        if (ret == TB_OK) {
            if (event.key == TB_KEY_ARROW_UP) {
                // Move up
                selected_index = (selected_index - 1 + menu_count) % menu_count;
            } else if (event.key == TB_KEY_ARROW_DOWN) {
                // Move down
                selected_index = (selected_index + 1) % menu_count;
            } else if (event.key == TB_KEY_ENTER) {
                // Handle selection
                if (menu_options[selected_index] == NEW_GAME) {
                    result = MENU_START_GAME;
                    menu_active = false;
                } else if (menu_options[selected_index] == CONTINUE) {
                    result = MENU_CONTINUE;
                    menu_active = false;
                } else if (menu_options[selected_index] == EXIT) {
                    result = MENU_EXIT;
                    menu_active = false;
                }
            } else if (event.key == TB_KEY_CTRL_C || event.key == TB_KEY_ESC) {
                result = MENU_EXIT;
                menu_active = false;
            }
        }
    }
    
    return result;
}