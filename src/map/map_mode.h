/**
 * @file map_mode.h
 * @brief Defines and manages functions for map exploration, player movement, and map interactions in map mode.
 */
#ifndef MAP_MODE_H
#define MAP_MODE_H

#include "../character/character.h"
#include "../common.h"
#include "../io/input/input_handler.h"

// Notcurses uses RGB channels directly, but we can define our colors for consistency
#define COLOR_FOREGROUND 0xffffff// White
#define COLOR_BACKGROUND 0x000000// Black

#define LIGHT_RADIUS 3

typedef enum {
    CONTINUE,
    QUIT,
    NEXT_FLOOR,
    COMBAT,
    SHOW_MENU,
    SHOW_STATS,
    SHOW_INVENTORY
} map_mode_result_t;

/**
 * @brief Sets the starting position of the player.
 *
 * @param player_x The starting x position of the player.
 * @param player_y The starting y position of the player.
 */
void set_player_start_pos(int player_x, int player_y);

/**
 * @brief Get the current player position
 * @return the player position as a vector2d_t structure
 */
vector2d_t get_player_pos();

/**
 * Updates the player position based on the player's input and redraws the maze.
 * @return CONTINUE (0) if the game continue, QUIT (1) if the player pressed the exit key.
 */
map_mode_result_t map_mode_update(character_t* player);

/**
 * @brief Initializes the map mode
 */
void init_map_mode(void);
/**
 * @brief Frees any resources associated with the map mode.
 */
void shutdown_map_mode(void);

// has to be exposed for testing
/**
 * @brief Functions is only exposed for testing purposes.
 */
map_mode_result_t handle_input(const input_event_t* input_event, character_t* player);
#endif//MAP_MODE_H
