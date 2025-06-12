/**
 * @file save_menu_local.h
 * @brief Declares functions and data for localized save menu UI.
 */
#ifndef SAVE_MENU_LOCAL_H
#define SAVE_MENU_LOCAL_H

enum save_menu_index {
    SAVE_NAME_REQUEST,
    SAVES_NOT_FOUND,
    SELECT_SAVE,
    SAVING,
    NAVIGATE_INSTRUCTIONS,
    CONFIRM_QUESTION,
    WARNING_LOST_PROGRESS,

    PRESS_ENTER_CONFIRM,
    PRESS_ANY_RETURN,

    MAX_SAVE_MENU_STRINGS
};

extern char** save_menu_strings;

/**
 * @brief Updates the localized strings for the save menu.
 *
 * This function retrieves the localized strings for the save menu
 * and updates the corresponding entries in the save menu strings array.
 */
void update_save_menu_local(void);

#endif//SAVE_MENU_LOCAL_H
