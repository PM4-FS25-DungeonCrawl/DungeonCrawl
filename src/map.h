#ifndef MAP_H
#define MAP_H

#define WIDTH 39 // must be odd
#define HEIGHT 19 // must be odd

#define     TOP     0
#define     BOTTOM  1
#define     LEFT    2
#define     RIGHT   3

enum map_tile {
    WALL = 0,
    FLOOR = 1,
    START_DOOR = 2,
    EXIT_DOOR = 3
};

//maze array
extern enum map_tile map[WIDTH][HEIGHT];

#endif //MAP_H