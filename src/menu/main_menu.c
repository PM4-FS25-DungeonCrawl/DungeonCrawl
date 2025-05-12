#include "main_menu.h"

#include "../common.h"
#include "../local/local_strings.h"
#include "../logging/logger.h"
#include "language_menu.h"
#include "local/main_menu_local.h"
#include "notcurses/nckeys.h"
#include "save_menu.h"
#include "src/local/local_handler.h"

#include <notcurses/notcurses.h>
#include <stdbool.h>
#include <stdint.h>
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

// === Internal Global Variables ===
bool menu_active;
menu_result_t active_menu_state;

int init_main_menu() {
    main_menu_strings = (char**) malloc(sizeof(char*) * MAX_MAIN_MENU_STRINGS);
    RETURN_WHEN_NULL(main_menu_strings, 1, "Main Menu", "Failed to allocate memory for main menu strings.");

    for (int i = 0; i < MAX_MAIN_MENU_STRINGS; i++) {
        main_menu_strings[i] = NULL;
    }

    // update local once, so the strings are initialized
    update_main_menu_local();
    // add update local function to the observer list
    observe_local(update_main_menu_local);
    return 0;
}

menu_result_t show_main_menu(const bool game_in_progress) {
    const char* menu_options[6];
    int menu_count;

    // Always include New Game
    menu_options[0] = main_menu_strings[NEW_GAME_STR];

    if (game_in_progress) {
        // If game is in progress, show all options
        menu_options[1] = main_menu_strings[CONTINUE_STR];
        menu_options[2] = main_menu_strings[SAVE_GAME_STR];
        menu_options[3] = main_menu_strings[LOAD_GAME_STR];
        menu_options[4] = main_menu_strings[CHANGE_LANGUAGE_STR];
        menu_options[5] = main_menu_strings[EXIT_STR];
        menu_count = 6;
    } else {
        // If no game in progress, only show New Game, Load Game and Exit
        menu_options[1] = main_menu_strings[LOAD_GAME_STR];
        menu_options[2] = main_menu_strings[CHANGE_LANGUAGE_STR];
        menu_options[3] = main_menu_strings[EXIT_STR];
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
            if (!game_in_progress || show_confirmation(main_menu_strings[QUESTION_CONTINUE])) {
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
            if (!game_in_progress || show_confirmation(main_menu_strings[QUESTION_EXIT])) {
                active_menu_state = MENU_EXIT;
                menu_active = false;
            }
            break;
        default:
            log_msg(WARNING, "Main Menu", "Invalid menu option selected: %d", selected_index);
            break;
    }
}

void shutdown_main_menu(void) {
    if (main_menu_strings != NULL) {
        for (int i = 0; i < MAX_MAIN_MENU_STRINGS; i++) {
            if (main_menu_strings[i] != NULL) {
                free(main_menu_strings[i]);
            }
        }
        free(main_menu_strings);
    }
}
