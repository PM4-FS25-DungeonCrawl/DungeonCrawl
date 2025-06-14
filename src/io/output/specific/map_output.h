/**
 * @file map_output.h
 * @brief Exposes functions for outputing to the screen while in the map mode.
 */
#ifndef MAP_OUTPUT_H
#define MAP_OUTPUT_H

#include "../../../common.h"
#include "../../../map/map.h"

/**
 * @brief Draws the map mode UI based on the given parameters.
 *
 * @param arr The map array to be drawn
 * @param height The height of the map
 * @param width The width of the map
 * @param anchor The anchor position of the map mode, defined as the top left corner
 * @param player_pos The position of the player
 *
 * @note This function checks makes different checks to ensure the given parameters are valid.
 * The checks are done in the following order:
 * - check if the array is NULL
 * - check if the height and width are greater than 0
 * - check if the anchor position is greater or equal 0
 * - check if the player position is within the bounds of the map
 * If any of the checks fail, an error message is logged and the function returns.
 */
void draw_map_mode(const map_tile_t* arr, int height, int width, vector2d_t anchor, vector2d_t player_pos);

/**
 * @brief Draws the player information for the map mode.
 *
 * This function renders player-related information such as health, position, and available commands.
 *
 * @param x The x position of the player info to be drawn
 * @param y The y position of the player info to be drawn
 * @param player_pos The current player position
 */
void draw_player_info(int x, int y, vector2d_t player_pos);

/**
 * @brief Draws the transition screen when the player moves to a new floor.
 *
 * This function displays an animation and waits for user input before proceeding.
 */
void draw_transition_screen(void);

#endif// MAP_OUTPUT_H
