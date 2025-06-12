/**
 * @file main_menu_local.h
 * @brief Exposes functionality for the localization of the main menu.
 */
#ifndef MAIN_MENU_LOCAL_H
#define MAIN_MENU_LOCAL_H

enum main_menu_index {
    NEW_GAME_STR,
    CONTINUE_STR,
    SAVE_GAME_STR,
    LOAD_GAME_STR,
    CHANGE_LANGUAGE_STR,
    EXIT_STR,
    QUESTION_CONTINUE,
    QUESTION_EXIT,

    MAX_MAIN_MENU_STRINGS
};

extern char** main_menu_strings;

/**
 * @brief Updates the localized strings for the main menu.
 *
 * This function retrieves the localized strings for the main menu
 * and updates the corresponding entries in the main menu strings array.
 */
void update_main_menu_local(void);

#endif//MAIN_MENU_LOCAL_H
