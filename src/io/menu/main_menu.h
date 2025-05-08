#ifndef IO_MAIN_MENU_H
#define IO_MAIN_MENU_H

#include "menu.h"

/**
 * @brief Main menu result codes
 */
typedef enum {
    MAIN_MENU_RESULT_CONTINUE = 0, // Continue the game
    MAIN_MENU_RESULT_NEW_GAME,     // Start a new game
    MAIN_MENU_RESULT_LOAD_GAME,    // Load a saved game
    MAIN_MENU_RESULT_OPTIONS,      // Open options menu
    MAIN_MENU_RESULT_CREDITS,      // Show credits
    MAIN_MENU_RESULT_QUIT          // Quit the game
} main_menu_result_t;

/**
 * @brief Create and display the main menu
 * 
 * @return The result of the menu selection
 */
main_menu_result_t show_main_menu(void);

#endif // IO_MAIN_MENU_H