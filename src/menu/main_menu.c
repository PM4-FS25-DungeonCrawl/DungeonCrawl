#include "main_menu.h"

#include "../../include/termbox2.h"
#include "../logging/logger.h"
#include "save_menu.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>// For nanosleep

bool menu_active;
menu_result_t active_menu_state;

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
    active_menu_state = MENU_CONTINUE;
    menu_active = true;

    while (menu_active) {
        draw_menu(menu_options, menu_count, selected_index);

        struct tb_event event;
        int ret = tb_peek_event(&event, 10);

        switch (event.key) {
            case TB_KEY_ARROW_UP:
                selected_index = (selected_index - 1 + menu_count) % menu_count;
                break;
            case TB_KEY_ARROW_DOWN:
                selected_index = (selected_index + 1) % menu_count;
                break;
            case TB_KEY_ENTER: {
                // Get the selected menu option
                const char* selected_option = menu_options[selected_index];
                select_menu_option(selected_option, game_in_progress);
                break;
            }
            case TB_KEY_CTRL_C:
                active_menu_state = MENU_EXIT;
                menu_active = false;
                break;
            case TB_KEY_ESC:
                active_menu_state = MENU_CONTINUE;
                menu_active = false;
                break;
            default:
                // do nothing for other keys
                break;
        }
    }

    return active_menu_state;
}

void select_menu_option(const char* selected_option, bool game_in_progress) {
    log_msg(INFO, "Main Menu", "Selected option: %s", selected_option);

    switch (selected_option[0]) {
        case 'N':// New Game (first letter 'N')
            if (!game_in_progress || show_confirmation("Do you want to continue?")) {
                active_menu_state = MENU_START_GAME;
                menu_active = false;
            }
            break;
        case 'C':// Continue (first letter 'C')
            active_menu_state = MENU_CONTINUE;
            menu_active = false;
            break;
        case 'S':// Save Game (first letter 'S')
            log_msg(INFO, "Main Menu", "Calling show_save_game_menu");
            active_menu_state = show_save_game_menu();
            log_msg(INFO, "Main Menu", "Returned from show_save_game_menu with result: %d", active_menu_state);
            if (active_menu_state == MENU_SAVE_GAME) {
                menu_active = false;
            }
            break;
        case 'L':// Load Game (first letter 'L')
            active_menu_state = show_load_game_menu(game_in_progress);
            if (active_menu_state == MENU_LOAD_GAME) {
                menu_active = false;
            }
            break;
        case 'E':// Exit (first letter 'E')
            if (!game_in_progress || show_confirmation("Do you want to exit?")) {
                active_menu_state = MENU_EXIT;
                menu_active = false;
            }
            break;
    }
}
