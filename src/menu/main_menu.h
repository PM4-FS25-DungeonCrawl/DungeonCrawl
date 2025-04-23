#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "menu.h"

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
 * Process a selected menu option
 * @param selected_option The selected menu option string
 * @param game_in_progress indicates whether there's an active game that can be continued
 */
void select_menu_option(const char* selected_option, bool game_in_progress);

#endif// MAIN_MENU_H
