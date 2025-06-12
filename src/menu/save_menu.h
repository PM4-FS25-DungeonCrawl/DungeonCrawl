/**
 * @file save_menu.h
 * @brief Exposes functions for the save menu.
 */
#ifndef SAVE_MENU_H
#define SAVE_MENU_H

#include "menu.h"

// Global variables to store menu state
extern int selected_save_file_id;
extern char last_save_name[50];

/**
 * @brief Initialize the save menu with the needed data.
 */
int init_save_menu(void);

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

/**
 * Show save game interface to get save name from user
 * @return MENU_SAVE_GAME if a name was entered, MENU_CONTINUE if canceled
 */
menu_result_t show_save_game_menu(void);

/**
 * Show load game interface to select a save file
 * @param game_in_progress indicates whether there's an active game that might need confirmation to discard
 * @return MENU_LOAD_GAME if a save was selected, MENU_CONTINUE if canceled
 */
menu_result_t show_load_game_menu(bool game_in_progress);

/**
 * @brief Shuts down the save menu and frees associated data.
 */
void shutdown_save_menu(void);

#endif// SAVE_MENU_H
