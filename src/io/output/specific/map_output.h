#ifndef MAP_OUTPUT_H
#define MAP_OUTPUT_H

#include "../../../common.h"
#include "../../../map/map.h"

/**
 * @brief Draws the map mode UI based on the given parameters.
 *
 * This function renders the map view for the game, including the map tiles and player information.
 * It uses the centralized IO handler for all drawing operations.
 *
 * @param arr The map array to be drawn
 * @param height The height of the map
 * @param width The width of the map 
 * @param anchor The anchor position of the map mode, defined as the top left corner
 * @param player_pos The position of the player
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

#endif// MAP_OUTPUT_H