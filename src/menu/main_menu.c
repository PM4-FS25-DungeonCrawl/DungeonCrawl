#include "main_menu.h"
#include "save_menu.h"

#include "../../include/termbox2.h"
#include "../logging/logger.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>// For nanosleep

void init_main_menu(void) {
    log_msg(INFO, "Menu", "Initializing main menu");
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
        log_msg(INFO, "Save Menu", "Polled event: %d, key: %d, ch: %c", ret, event.key, event.ch ? event.ch : ' ');

        switch(event.key) {
            case TB_KEY_ARROW_UP: 
                selected_index = (selected_index - 1 + menu_count) % menu_count;
                break;
            case TB_KEY_ARROW_DOWN:
                selected_index = (selected_index + 1) % menu_count;
                break;
            case TB_KEY_ENTER:
                if (strcmp(menu_options[selected_index], NEW_GAME_OPTION) == 0) {
                    if (game_in_progress && !show_confirmation("Do you want to continue?")) {
                        // User declined, continue menu loop
                        continue;
                    }
                    result = MENU_START_GAME;
                    menu_active = false;
                } else if (strcmp(menu_options[selected_index], CONTINUE_OPTION) == 0) {
                    result = MENU_CONTINUE;
                    menu_active = false;
                } else if (strcmp(menu_options[selected_index], SAVE_GAME_OPTION) == 0) {
                    log_msg(INFO, "Main Menu", "Calling show_save_game_menu");
                    result = show_save_game_menu();
                    log_msg(INFO, "Main Menu", "Returned from show_save_game_menu with result: %d", result);
                    if (result == MENU_SAVE_GAME) {
                        menu_active = false;
                    }
                } else if (strcmp(menu_options[selected_index], LOAD_GAME_OPTION) == 0) {
                    result = show_load_game_menu(game_in_progress);
                    if (result == MENU_LOAD_GAME) {
                        menu_active = false;
                    }
                } else if (strcmp(menu_options[selected_index], EXIT_OPTION) == 0) {
                    if (game_in_progress && !show_confirmation("Do you want to exit?")) {
                        // User declined, continue menu loop
                        continue;
                    }

                    result = MENU_EXIT;
                    menu_active = false;
                }
                break;
            case TB_KEY_CTRL_C:
                result = MENU_EXIT;
                menu_active = false;
            break;
                case TB_KEY_ESC:
                result = MENU_CONTINUE;
            menu_active = false;
            break;
            default:
            // do nothing for other keys
            break;
        }        
    }

    return result;
}