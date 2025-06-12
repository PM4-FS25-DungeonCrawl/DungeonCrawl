/**
 * @file save_menu_local.c
 * @brief Loads and updates localized strings for the save game menu UI.
 */
#include "save_menu_local.h"

#include "../../local/local_handler.h"

#include <stdlib.h>

char** save_menu_strings = NULL;

void update_save_menu_local(void) {
    for (int i = 0; i < MAX_SAVE_MENU_STRINGS; i++) {
        if (save_menu_strings[i] != NULL) {
            free(save_menu_strings[i]);
        }
    }

    save_menu_strings[SAVE_NAME_REQUEST] = get_local_string("SAVE_MENU.SAVE_NAME.REQUEST");
    save_menu_strings[SAVES_NOT_FOUND] = get_local_string("SAVE_MENU.SAVES.NOT_FOUND");
    save_menu_strings[SELECT_SAVE] = get_local_string("SAVE_MENU.SELECT.SAVE");
    save_menu_strings[SAVING] = get_local_string("SAVE_MENU.SAVING");
    save_menu_strings[NAVIGATE_INSTRUCTIONS] = get_local_string("SAVE_MENU.NAVIGATE.INSTRUCTIONS");
    save_menu_strings[CONFIRM_QUESTION] = get_local_string("SAVE_MENU.QUESTION.CONFIRM");
    save_menu_strings[WARNING_LOST_PROGRESS] = get_local_string("SAVE_MENU.WARNING.PROGRESS.LOST");

    save_menu_strings[PRESS_ENTER_CONFIRM] = get_local_string("PRESS.ENTER.CONFIRM");
    save_menu_strings[PRESS_ANY_RETURN] = get_local_string("PRESS.ANY.RETURN");
}
