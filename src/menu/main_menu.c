#include "main_menu.h"

#include "../../include/termbox2.h"
#include "../logging/logger.h"
#include <stdbool.h>
#include <string.h>

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

void init_main_menu(void) {
    log_msg(INFO, "Menu", "Initializing main menu");
}

static void draw_menu(const char** menu_options, int menu_count, int selected_index) {
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

static bool show_confirmation(const char* message) {
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

menu_result_t show_main_menu(bool game_in_progress) {
    const char* menu_options[5];
    int menu_count;
    
    // Always include New Game
    menu_options[0] = NEW_GAME_OPTION;
    
    if (game_in_progress) {
        // If game is in progress, show all options
        menu_options[1] = CONTINUE_OPTION;
        menu_options[2] = SAVE_GAME_OPTION;
        menu_options[3] = LOAD_GAME_OPTION;
        menu_options[4] = EXIT_OPTION;
        menu_count = 5;
    } else {
        // If no game in progress, only show New Game, Load Game and Exit
        menu_options[1] = LOAD_GAME_OPTION;
        menu_options[2] = EXIT_OPTION;
        menu_count = 3;
    }
    
    int selected_index = 0;
    menu_result_t result = MENU_CONTINUE;
    bool menu_active = true;
    
    while (menu_active) {
        draw_menu(menu_options, menu_count, selected_index);
        
        struct tb_event event;
        int ret = tb_peek_event(&event, 10);
        
        if (ret == TB_OK) {
            if (event.key == TB_KEY_ARROW_UP) {
                // Move up
                selected_index = (selected_index - 1 + menu_count) % menu_count;
            } else if (event.key == TB_KEY_ARROW_DOWN) {
                // Move down
                selected_index = (selected_index + 1) % menu_count;
            } else if (event.key == TB_KEY_ENTER) {
                if (strcmp(menu_options[selected_index], NEW_GAME_OPTION) == 0) {
                    if (game_in_progress && !show_confirmation("Do you want to continue?")) {
                        // User declined, continue menu loop
                        continue;
                    }
                    result = MENU_START_GAME;
                    menu_active = false;
                } 
                else if (strcmp(menu_options[selected_index], CONTINUE_OPTION) == 0) {
                    result = MENU_CONTINUE;
                    menu_active = false;
                } 
                else if (strcmp(menu_options[selected_index], SAVE_GAME_OPTION) == 0) {
                    char save_name[50] = {0};
                    int name_length = 0;
                    bool input_active = true;
                    
                    // Get save name from user
                    while (input_active && name_length < 49) {
                        tb_clear();
                        tb_print(MENU_START_X, MENU_START_Y, TB_WHITE, TB_DEFAULT, "Enter name for save file:");
                        tb_print(MENU_START_X, MENU_START_Y + 2, TB_WHITE, TB_DEFAULT, save_name);
                        tb_print(MENU_START_X, MENU_START_Y + 4, TB_WHITE, TB_DEFAULT, "Press Enter when done");
                        tb_present();
                        
                        struct tb_event input_event;
                        tb_poll_event(&input_event);
                        
                        if (input_event.key == TB_KEY_ENTER && name_length > 0) {
                            input_active = false;
                        } else if (input_event.key == TB_KEY_BACKSPACE && name_length > 0) {
                            save_name[--name_length] = '\0';
                        } else if (input_event.key == TB_KEY_ESC) {
                            // Cancel save
                            input_active = false;
                            continue;
                        } else if (input_event.ch != 0 && name_length < 49) {
                            save_name[name_length++] = input_event.ch;
                            save_name[name_length] = '\0';
                        }
                    }
                    
                    if (name_length > 0) {
                        // Show saving message
                        tb_clear();
                        tb_print(MENU_START_X, MENU_START_Y, TB_WHITE, TB_DEFAULT, "Saving game...");
                        tb_present();
                        
                        log_msg(INFO, "Menu", "Saving game with name: %s", save_name);
                        // TODO: Pass save_name to save_game_state function when it's implemented
                        // Currently the save_game_state doesn't consider the save name but will in the future
                        
                        result = MENU_SAVE_GAME;
                        menu_active = false;
                    } else {
                        // If user pressed ESC, just stay in the menu
                        continue;
                    }
                } 
                else if (strcmp(menu_options[selected_index], LOAD_GAME_OPTION) == 0) {
                    if (game_in_progress && !show_confirmation("Do you want to continue?")) {
                        // User declined, continue menu loop
                        continue;
                    }

                    tb_clear();
                    tb_print(MENU_START_X, MENU_START_Y, TB_WHITE, TB_DEFAULT, "Currently no files to load... Press any Key to continue.");
                    tb_present();
                    struct tb_event event;
                    tb_poll_event(&event);

                    result = MENU_LOAD_GAME;
                    menu_active = false;
                } 
                else if (strcmp(menu_options[selected_index], EXIT_OPTION) == 0) {
                    if (game_in_progress && !show_confirmation("Do you want to exit?")) {
                        // User declined, continue menu loop
                        continue;
                    }
                    
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