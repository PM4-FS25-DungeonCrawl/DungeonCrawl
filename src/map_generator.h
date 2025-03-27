#ifndef MAP_GENERATOR_H
#define MAP_GENERATOR_H

#define WIDTH 39 // must be odd
#define HEIGHT 19 // must be odd

enum map_tile {
    WALL = 0,
    FLOOR = 1,
    START_DOOR = 2,
    EXIT_DOOR = 3
};

void generate_map();

#endif //MAP_GENERATOR_H
