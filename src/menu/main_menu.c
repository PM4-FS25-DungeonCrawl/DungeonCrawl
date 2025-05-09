#include "main_menu.h"

#include "../common.h"
#include "../local/local.h"
#include "../local/local_strings.h"
#include "../logging/logger.h"
#include "language_menu.h"
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

// === Internal Functions ===
/**
 * Process a selected menu option
 * @param selected_index The selected menu option index
 * @param game_in_progress Indicates whether there's an active game that can be continued
 */
void select_menu_option(int selected_index, bool game_in_progress);

void update_main_menu_local(void);

// === Internal Global Variables ===
bool menu_active;
menu_result_t active_menu_state;

int init_main_menu() {
    // update local once, so the strings are initialized
    update_main_menu_local();
    // add update local function to the observer list
    add_local_observer(update_main_menu_local);

    init_save_menu();
    init_language_menu();
    return 0;
}

menu_result_t show_main_menu(const bool game_in_progress) {
    const char* menu_options[6];
    int menu_count;

    // Always include New Game
    menu_options[0] = &local_strings[mame_new_game_option.idx].characters[0];

    if (game_in_progress) {
        // If game is in progress, show all options
        menu_options[1] = &local_strings[mame_continue_option.idx].characters[0];
        menu_options[2] = &local_strings[mame_save_game_option.idx].characters[0];
        menu_options[3] = &local_strings[mame_load_game_option.idx].characters[0];
        menu_options[4] = &local_strings[mame_change_language_option.idx].characters[0];
        menu_options[5] = &local_strings[mame_exit_option.idx].characters[0];
        menu_count = 6;
    } else {
        // If no game in progress, only show New Game, Load Game and Exit
        menu_options[1] = &local_strings[mame_load_game_option.idx].characters[0];
        menu_options[2] = &local_strings[mame_change_language_option.idx].characters[0];
        menu_options[3] = &local_strings[mame_exit_option.idx].characters[0];
        menu_count = 4;
    }

    int selected_index = 0;
    active_menu_state = MENU_CONTINUE;
    menu_active = true;

    while (menu_active) {
        draw_menu(menu_options, menu_count, selected_index);

        ncinput input;
        memset(&input, 0, sizeof(input));
        notcurses_get_blocking(nc, &input);

        if (!(input.evtype == NCTYPE_UNKNOWN || input.evtype == NCTYPE_PRESS)) { continue; }

        switch (input.id) {
            case NCKEY_UP:
                selected_index = (selected_index - 1 + menu_count) % menu_count;
                break;
            case NCKEY_DOWN:
                selected_index = (selected_index + 1) % menu_count;
                break;
            case NCKEY_ENTER:
                // Get the selected menu option
                const char* selected_option = menu_options[selected_index];
                select_menu_option(selected_index, game_in_progress);
                break;
            case 'c':
                // if only c was pressed and not ctrl-c break. seems the cleanest solution to me
                if (!(input.modifiers & NCKEY_MOD_CTRL)) {
                    break;
                }
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

void select_menu_option(const int selected_index, const bool game_in_progress) {
    //if the game is not in progress and the selected index is bigger than 0, we need to add 2 to the selected index
    const int true_index = selected_index > 0 && !game_in_progress ? selected_index + 2 : selected_index;

    switch (true_index) {
        case 0:// New Game
            if (!game_in_progress || show_confirmation(local_strings[mame_confirm_continue.idx].characters)) {
                active_menu_state = MENU_START_GAME;
                menu_active = false;
            }
            break;
        case 1:// Continue
            active_menu_state = MENU_CONTINUE;
            menu_active = false;
            break;
        case 2:// Save Game
            active_menu_state = show_save_game_menu();
            if (active_menu_state == MENU_SAVE_GAME) {
                menu_active = false;
            }
            break;
        case 3:// Load Game
            active_menu_state = show_load_game_menu(game_in_progress);
            if (active_menu_state == MENU_LOAD_GAME) {
                menu_active = false;
            }
            break;
        case 4:// Change Language
            active_menu_state = show_language_menu();
            if (active_menu_state == MENU_CHANGE_LANGUAGE) {
                menu_active = false;
            }
            break;
        case 5:// Exit
            if (!game_in_progress || show_confirmation(local_strings[mame_confirm_exit.idx].characters)) {
                active_menu_state = MENU_EXIT;
                menu_active = false;
            }
            break;
        default:
            log_msg(WARNING, "Main Menu", "Invalid menu option selected: %d", selected_index);
            break;
    }
}

void update_main_menu_local(void) {
    snprintf(local_strings[mame_new_game_option.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(mame_new_game_option.key));
    snprintf(local_strings[mame_continue_option.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(mame_continue_option.key));
    snprintf(local_strings[mame_save_game_option.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(mame_save_game_option.key));
    snprintf(local_strings[mame_load_game_option.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(mame_load_game_option.key));
    snprintf(local_strings[mame_change_language_option.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(mame_change_language_option.key));
    snprintf(local_strings[mame_exit_option.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(mame_exit_option.key));

    snprintf(local_strings[mame_confirm_continue.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(mame_confirm_continue.key));
    snprintf(local_strings[mame_confirm_exit.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(mame_confirm_exit.key));
}
