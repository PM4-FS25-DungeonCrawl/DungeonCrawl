//
// Created by jilze on 23/03/2025.
//

#ifndef MAP_GENERATOR_FANCY_H
#define MAP_GENERATOR_FANCY_H

//this ??
#define WIDTH 21 // must be odd
#define HEIGHT 17 // must be odd

enum map_tile {
    WALL = 0,
    FLOOR = 1
};

void generate_map();

#endif //MAP_GENERATOR_FANCY_H
