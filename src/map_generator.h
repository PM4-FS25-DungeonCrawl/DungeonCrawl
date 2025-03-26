//
// Created by jilze on 23/03/2025.
//

#ifndef MAP_GENERATOR_H
#define MAP_GENERATOR_H

#include <stdio.h>

//this ??
#define WIDTH 81 // must be odd
#define HEIGHT 21 // must be odd

enum map_tile {
    WALL = 0,
    FLOOR = 1
};

void generate_map();

#endif //MAP_GENERATOR_H
