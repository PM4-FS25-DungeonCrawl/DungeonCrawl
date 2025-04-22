#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "../database/gamestate/gamestate_database.h"

#include <stdbool.h>

typedef enum {
    MENU_START_GAME,
    MENU_CONTINUE,
    MENU_SAVE_GAME,
    MENU_LOAD_GAME,
    MENU_EXIT
} menu_result_t;

/**
 * Initialize the main menu
 */
void init_main_menu(void);

/**
 * Display and handle the main menu
 * @param game_in_progress indicates whether there's an active game that can be continued
 * @return MENU_START_GAME to start a new game, MENU_CONTINUE to continue the current game, 
 *         MENU_SAVE_GAME to save the game, MENU_LOAD_GAME to load a game, MENU_EXIT to exit the game
 */
menu_result_t show_main_menu(bool game_in_progress);

/**
 * Get the ID of the save file selected by the user
 * @return The ID of the selected save file, or -1 if no file was selected
 */
int get_selected_save_file_id(void);

/**
 * Get the name of the latest save entered by the user
 * @return The name of the latest save, or NULL if no save name was entered
 */
const char* get_save_name(void);

#endif// MAIN_MENU_H

