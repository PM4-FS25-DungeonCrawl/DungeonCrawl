#include "main_menu.h"

#include "../../include/termbox2.h"
#include "../logging/logger.h"
#include <stdbool.h>

// Menu options
#define NEW_GAME_OPTION "New Game"
#define CONTINUE_OPTION "Continue"
#define SAVE_GAME_OPTION "Save Game"
#define LOAD_GAME_OPTION "Load Game"
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
static const char* SAVE_GAME = SAVE_GAME_OPTION;
static const char* LOAD_GAME = LOAD_GAME_OPTION;
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
    const char* menu_options[5]; // Increased size for new options
    int menu_count;
    
    // Always include New Game, Load Game and Exit
    menu_options[0] = NEW_GAME;
    
    if (game_in_progress) {
        // If game is in progress, show Continue and Save Game options
        menu_options[1] = CONTINUE;
        menu_options[2] = SAVE_GAME;
        menu_options[3] = LOAD_GAME;
        menu_options[4] = EXIT;
        menu_count = 5;
    } else {
        // If no game in progress, don't show Continue and Save Game options
        menu_options[1] = LOAD_GAME;
        menu_options[2] = EXIT;
        menu_count = 3;
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
                } else if (menu_options[selected_index] == SAVE_GAME) {
                    // Prompt user for save file name
                    tb_clear();
                    tb_print(MENU_START_X, MENU_START_Y, TB_WHITE, TB_DEFAULT, "Enter name for save file (placeholder):");
                    tb_present();
                    
                    // Wait for any key press to simulate accepting input
                    struct tb_event save_event;
                    tb_poll_event(&save_event);
                    
                    log_msg(INFO, "Menu", "Save Game selected (placeholder)");
                    result = MENU_SAVE_GAME;
                    menu_active = false;
                } else if (menu_options[selected_index] == LOAD_GAME) {
                    if (game_in_progress) {
                        // Warn user about unsaved progress
                        tb_clear();
                        tb_print(MENU_START_X, MENU_START_Y, TB_WHITE, TB_DEFAULT, "Warning: All unsaved progress will be lost!");
                        tb_print(MENU_START_X, MENU_START_Y + 2, TB_WHITE, TB_DEFAULT, "Do you want to continue? (Y/N)");
                        tb_present();
                        
                        // Wait for Y or N
                        bool waiting_for_confirmation = true;
                        while (waiting_for_confirmation) {
                            struct tb_event confirm_event;
                            tb_poll_event(&confirm_event);
                            
                            if (confirm_event.ch == 'y' || confirm_event.ch == 'Y') {
                                waiting_for_confirmation = false;
                                log_msg(INFO, "Menu", "Load Game confirmed (placeholder)");
                                result = MENU_LOAD_GAME;
                                menu_active = false;
                            } else if (confirm_event.ch == 'n' || confirm_event.ch == 'N') {
                                waiting_for_confirmation = false;
                                // Return to main menu
                                draw_menu(menu_options, menu_count, selected_index);
                            }
                        }
                    } else {
                        // No game in progress, load directly
                        log_msg(INFO, "Menu", "Load Game selected (placeholder)");
                        result = MENU_LOAD_GAME;
                        menu_active = false;
                    }
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