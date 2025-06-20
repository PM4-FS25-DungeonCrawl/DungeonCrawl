/**
 * @file main_menu.h
 * @brief Exposes functions for the main menu.
 */
#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "menu.h"


/**
 * Initializes resources for the main menu, including memory allocation
 * and observer registration for localized menu strings.
 *
 * @return 0 if the main menu initializes successfully,
 *         non-zero value otherwise in case of failure.
 */
int init_main_menu(void);

/**
 * Display and handle the main menu
 * @param game_in_progress indicates whether there's an active game that can be continued
 * @return MENU_START_GAME to start a new game, MENU_CONTINUE to continue the current game, 
 *         MENU_SAVE_GAME to save the game, MENU_LOAD_GAME to load a game, MENU_EXIT to exit the game
 */
menu_result_t show_main_menu(bool game_in_progress);

/**
 * @brief Shuts down the main menu and frees associated data.
 */
void shutdown_main_menu(void);

#endif// MAIN_MENU_H
