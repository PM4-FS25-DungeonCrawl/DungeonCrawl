/**
 * @file map.c
 * @brief Implementations for the games map.
 */
#include "map.h"

map_tile_t map[WIDTH][HEIGHT];
map_tile_t revealed_map[WIDTH][HEIGHT];


vector2d_t directions[4] = {
        {0, -1},// up
        {0, 1}, // down
        {-1, 0},// left
        {1, 0}  // right
};
