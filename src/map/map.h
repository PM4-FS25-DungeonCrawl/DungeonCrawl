#ifndef MAP_H
#define MAP_H

#define WIDTH 39 // must be odd
#define HEIGHT 19 // must be odd

#define ENEMY_COUNT 8
#define ENEMY_MIN_DISTANCE 3

#define     TOP     0
#define     BOTTOM  1
#define     LEFT    2
#define     RIGHT   3

typedef enum {
    WALL = 0,
    FLOOR = 1,
    START_DOOR = 2,
    EXIT_DOOR = 3,
    KEY = 4,
    SKELETON = 5,
    HIDDEN = 99
} map_tile_t;

/**
 * @brief 2-dimensional vector struct
 */
typedef struct {
    int dx, dy;
} vector2d_t;

extern vector2d_t directions[4];

//map array
extern map_tile_t map[WIDTH][HEIGHT];
extern map_tile_t revealed_map[WIDTH][HEIGHT];

#endif //MAP_H
