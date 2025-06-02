/**
 * @file game.h
 * @brief Declares core game states, global database connection, and main game control functions.
 */
#ifndef GAME_H
#define GAME_H

#include "database/database.h"

// Define colors with notcurses
#define COLOR_FG_RGB 255, 255, 255// White
#define COLOR_BG_RGB 0, 0, 0      // Black

// Global notcurses instance and plane
extern struct notcurses* nc;
extern struct ncplane* stdplane;

// global floor variable
extern int current_floor;

typedef enum {
    MAIN_MENU,
    MAP_MODE,
    COMBAT_MODE,
    LOOT_MODE,
    INVENTORY_MODE,
    GENERATE_MAP,
    STATS_MODE,
    EXIT
} game_state_t;

extern db_connection_t db_connection;

/**
 * @brief Starts the game loop.
 *
 * This function initializes and runs the main game loop, handling the various
 * game states and transitions until the game is exited.
 */
void run_game();

/**
 * @brief Handles the main menu state of the game.
 *
 * This function manages the interactions and transitions that occur
 * within the main menu, including starting a new game, loading a game,
 * and changing settings.
 */
void main_menu_state();

/**
 * @brief Handles the map mode state of the game.
 *
 * This function manages the interactions and transitions that occur
 * within the map mode, including player movement, map exploration,
 * and interactions with map elements.
 */
void map_mode_state();

/**
 * @brief Handles the inventory mode state of the game.
 *
 * This function manages the interactions and transitions that occur
 * within the inventory mode, including item management, usage,
 * and equipping items.
 */
void inventory_mode_state();

/**
 * @brief Handles the loot mode state of the game.
 *
 * This function manages the interactions and transitions that occur
 * when the player is looting after a combat encounter, including
 * selecting items to take and managing inventory.
 */
void loot_mode_state();

#endif// GAME_H
