#ifndef IO_PAUSE_MENU_H
#define IO_PAUSE_MENU_H

#include "menu.h"

/**
 * @brief Pause menu result codes
 */
typedef enum {
    PAUSE_MENU_RESULT_RESUME = 0,   // Resume the game
    PAUSE_MENU_RESULT_SAVE,         // Save the game
    PAUSE_MENU_RESULT_LOAD,         // Load a saved game
    PAUSE_MENU_RESULT_OPTIONS,      // Open options menu
    PAUSE_MENU_RESULT_MAIN_MENU,    // Return to main menu
    PAUSE_MENU_RESULT_QUIT          // Quit the game
} pause_menu_result_t;

/**
 * @brief Create and display the pause menu
 * 
 * @return The result of the menu selection
 */
pause_menu_result_t show_pause_menu(void);

#endif // IO_PAUSE_MENU_H