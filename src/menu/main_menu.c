#include "main_menu.h"

#include "../../include/termbox2.h"
#include "../logging/logger.h"

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

static const char* menu_options[] = {
    NEW_GAME_OPTION,
    CONTINUE_OPTION,
    EXIT_OPTION
};

static const int menu_count = sizeof(menu_options) / sizeof(menu_options[0]);

void init_main_menu(void) {
    log_msg(INFO, "Menu", "Initializing main menu");
}

void draw_menu(int selected_index) {
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

menu_result_t show_main_menu(void) {
    int selected_index = 0;
    menu_result_t result = MENU_CONTINUE;
    bool menu_active = true;
    
    while (menu_active) {
        draw_menu(selected_index);
        
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
                switch (selected_index) {
                    case 0: // New Game
                        result = MENU_START_GAME;
                        menu_active = false;
                        break;
                    case 1: // Continue
                        result = MENU_CONTINUE;
                        menu_active = false;
                        break;
                    case 2: // Exit
                        result = MENU_EXIT;
                        menu_active = false;
                        break;
                }
            } else if (event.key == TB_KEY_CTRL_C || event.key == TB_KEY_ESC) {
                result = MENU_EXIT;
                menu_active = false;
            }
        }
    }
    
    return result;
}