#ifndef DRAW_MAP_H
#define DRAW_MAP_H

#include "../map.h"

void draw_map(const map_tile_t* arr, const int height, const int width, const vector2d_t anchor, const vector2d_t player_pos);
void draw_map_ui(const int x, const int y, const vector2d_t player_pos);

#endif //DRAW_MAP_H
