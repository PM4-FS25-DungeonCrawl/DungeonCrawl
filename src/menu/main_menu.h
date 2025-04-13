#ifndef MAIN_MENU_H
#define MAIN_MENU_H

typedef enum {
    MENU_START_GAME,
    MENU_CONTINUE,
    MENU_EXIT
} menu_result_t;

/**
 * Initialize the main menu
 */
void init_main_menu(void);

/**
 * Display and handle the main menu
 * @param game_in_progress indicates whether there's an active game that can be continued
 * @return MENU_START_GAME to start a new game, MENU_CONTINUE to continue the current game, MENU_EXIT to exit the game
 */
menu_result_t show_main_menu(bool game_in_progress);

#endif // MAIN_MENU_H