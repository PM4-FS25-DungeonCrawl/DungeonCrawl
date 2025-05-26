/**
 * @file draw_light.h
 * @brief Exposes functions for drawing light on player.
 */
#ifndef DRAW_LIGHT_H
#define DRAW_LIGHT_H

#include "../map.h"

/**
 * @brief Draws light around the player.
 *
 * @param arr1 The pointer to the 2D array containing all the map tiles (no Hidden tiles)
 * @param arr2 The pointer to the 2D array to reveal the arr1, based on the player's position and light radius
 * @param height The height of the map
 * @param width The width of the map
 * @param player The player's position on the map
 * @param light_radius The radius of the light around the player
 */
void draw_light_on_player(map_tile_t* arr1, map_tile_t* arr2, int height, int width, vector2d_t player,
                          int light_radius);

#endif//DRAW_LIGHT_H
