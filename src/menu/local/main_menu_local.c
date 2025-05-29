/**
 * @file main_menu_local.c
 * @brief Loads and updates localized strings for the main menu UI.
 */
#include "main_menu_local.h"

#include "../../local/local_handler.h"

#include <stdlib.h>

char** main_menu_strings = NULL;

void update_main_menu_local(void) {
    for (int i = 0; i < MAX_MAIN_MENU_STRINGS; i++) {
        if (main_menu_strings[i] != NULL) {
            free(main_menu_strings[i]);
        }
    }

    main_menu_strings[NEW_GAME_STR] = get_local_string("MAIN_MENU.NEW_GAME");
    main_menu_strings[CONTINUE_STR] = get_local_string("MAIN_MENU.CONTINUE");
    main_menu_strings[SAVE_GAME_STR] = get_local_string("MAIN_MENU.SAVE_GAME");
    main_menu_strings[LOAD_GAME_STR] = get_local_string("MAIN_MENU.LOAD_GAME");
    main_menu_strings[CHANGE_LANGUAGE_STR] = get_local_string("MAIN_MENU.CHANGE_LANGUAGE");
    main_menu_strings[EXIT_STR] = get_local_string("MAIN_MENU.EXIT");
    main_menu_strings[QUESTION_CONTINUE] = get_local_string("MAIN_MENU.QUESTION.CONTINUE");
    main_menu_strings[QUESTION_EXIT] = get_local_string("MAIN_MENU.QUESTION.EXIT");
}
