/**
 * @file menu.h
 * @brief Exposes helper functions to work with menus.
 */
#ifndef MENU_H
#define MENU_H


#include <stdbool.h>

// Menu options strings
#define NEW_GAME_OPTION "New Game"
#define CONTINUE_OPTION "Continue"
#define SAVE_GAME_OPTION "Save Game"
#define LOAD_GAME_OPTION "Load Game"
#define CHANGE_LANGUAGE_OPTION "Change Language"
#define EXIT_OPTION "Exit"

// Menu constants
#define TITLE "DUNGEON CRAWL"
#define TITLE_COLOR TB_RED
#define MENU_START_Y 5
#define MENU_START_X 10
#define MENU_ITEM_SPACING 2

typedef enum {
    MENU_START_GAME,
    MENU_CONTINUE,
    MENU_SAVE_GAME,
    MENU_LOAD_GAME,
    MENU_CHANGE_LANGUAGE,
    MENU_EXIT
} menu_result_t;

/**
 * Shows a confirmation dialog with the given message
 * @param message The message to display
 * @return true if the user confirmed, false otherwise
 */
bool show_confirmation(const char* message);

/**
 * Draws a menu with the given options
 * @param menu_options Array of menu option strings
 * @param menu_count Number of menu options
 * @param selected_index Currently selected menu index
 */
void draw_menu(const char** menu_options, int menu_count, int selected_index);

#endif// MENU_H
