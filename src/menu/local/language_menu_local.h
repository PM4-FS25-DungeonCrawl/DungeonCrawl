/**
 * @file language_menu_local.h
 * @brief Exposes functionality to manage the localization of the language menu.
 */
#ifndef LANGUAGE_MENU_LOCAL_H
#define LANGUAGE_MENU_LOCAL_H

enum language_menu_index {
    LANGUAGE_ENGLISH,
    LANGUAGE_GERMAN,

    MAX_LANGUAGE_MENU_STRINGS
};

extern char** language_menu_strings;

/**
 * @brief Updates the localized strings for the language menu.
 *
 * This function retrieves the localized strings for the language menu
 * and updates the corresponding entries in the language menu strings array.
 */
void update_language_menu_local(void);

#endif//LANGUAGE_MENU_LOCAL_H
