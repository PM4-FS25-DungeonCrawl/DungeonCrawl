#include "main_menu.h"

#include "../database/database.h"
#include "../logging/logger.h"
#include "notcurses/nckeys.h"
#include "save_menu.h"

#include <notcurses/notcurses.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>// For nanosleep
//
#ifdef __APPLE__
    #define KEY_EVENT NCTYPE_PRESS
#else
    #define KEY_EVENT NCTYPE_UNKNOWN
#endif /* ifdef __APPLE__ */


// External reference to notcurses context
extern struct notcurses* nc;
extern struct ncplane* stdplane;

bool menu_active;
menu_result_t active_menu_state;

// Color channels
#define RED_ON_BLACK NCCHANNELS_INITIALIZER(255, 0, 0, 0, 0, 0)
#define WHITE_ON_BLACK NCCHANNELS_INITIALIZER(255, 255, 255, 0, 0, 0)
#define BLACK_ON_WHITE NCCHANNELS_INITIALIZER(0, 0, 0, 255, 255, 255)

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

        ncinput input;
        memset(&input, 0, sizeof(input));
        uint32_t ret = notcurses_get_nblock(nc, &input);

        if(input.evtype == NCTYPE_UNKNOWN || input.evtype == NCTYPE_PRESS) { continue;}

        switch (input.id) {
            case NCKEY_UP:
                selected_index = (selected_index - 1 + menu_count) % menu_count;
                break;
            case NCKEY_DOWN:
                selected_index = (selected_index + 1) % menu_count;
                break;
            case NCKEY_ENTER: {
                // Get the selected menu option
                const char* selected_option = menu_options[selected_index];
                select_menu_option(selected_option, game_in_progress);
                break;
            }
            case TB_KEY_CTRL_C:
                active_menu_state = MENU_EXIT;
                menu_active = false;
                break;
            case NCKEY_ESC:
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
