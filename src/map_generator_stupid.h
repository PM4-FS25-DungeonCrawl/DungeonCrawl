#ifndef MAP_GENERATOR_STUPID_H
#define MAP_GENERATOR_STUPID_H

#define WIDTH 80
#define HEIGHT 18

enum map_tile {
    WALL = 0,
    FLOOR = 1
};

void generate_map();

#endif //MAP_GENERATOR_STUPID_H
