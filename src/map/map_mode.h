#ifndef MAP_MODE_H
#define MAP_MODE_H

#include "map.h"

//#define DEBUG_TRUE

#define COLOR_FOREGROUND TB_WHITE
#define COLOR_BACKGROUND TB_BLACK

#define CONTINUE 0
#define QUIT 1

#ifdef DEBUG_TRUE
    #define DEBUG_PRINT(x, y, ...) print_text(x, y, __VA_ARGS__)
#else
    //empty debug print
    #define DEBUG_PRINT(x, y, ...)
#endif

#define LIGHT_RADIUS 3


void set_start(int newPlayerX, int newPlayerY);


void draw_light_on_player(const int* arr1, int* arr2, int height, int width, Vector2D player, int light_radius);

int map_mode_update(void);

int init_map_mode(void);

#endif //MAP_MODE_H
