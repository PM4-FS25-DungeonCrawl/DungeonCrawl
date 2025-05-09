#ifndef DRAW_LIGHT_H
#define DRAW_LIGHT_H

#include "../map.h"

void draw_light_on_player(map_tile_t* arr1, map_tile_t* arr2, int height, int width, vector2d_t player,
                          int light_radius);

#endif//DRAW_LIGHT_H
