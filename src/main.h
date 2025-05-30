/**
 * @file main.h
 * @brief Declares application initialization, shutdown, and main entry point.
 */
#ifndef MAIN_H
#define MAIN_H

extern volatile int init_done;

typedef enum {
    SUCCESS,
    FAIL_TB_INIT,
    FAIL_MEM_POOL_INIT,
    FAIL_LOCAL_INIT,
    FAIL_IO_HANDLER_INIT,
    FAIL_MAIN_MENU_INIT,
    FAIL_SAVE_MENU_INIT,
    FAIL_LANGUAGE_INIT,
    FAIL_GAME_MODE_INIT,
    FAIL_INVENTORY_MODE_INIT,
    FAIL_GAME_ENTITY_INIT,
    FAIL_STATS_MODE_INIT,
    FAIL_MAP_MODE_LOCAL_INIT,
    FAIL_ABILITY_LOCAL_INIT,
    FAIL_ERROR,
} exit_code_t;

/**
 * @brief Main entry point for the application.
 *
 * This function initializes the game, runs the main game loop, and performs
 * cleanup before exiting. It handles the overall flow of the application.
 *
 * @return Exit code indicating success or failure.
 */
int main(void);

#endif// MAIN_H
