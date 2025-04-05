#include "map.h"

map_tile map[WIDTH][HEIGHT];
map_tile revealed_map[WIDTH][HEIGHT];

Vector2D directions[4] = {
    {0, -1}, // up
    {0, 1}, // down
    {-1, 0}, // left
    {1, 0} // right
};