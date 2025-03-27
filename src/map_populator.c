#include <stdlib.h>

#include "map_populator.h"
#include "map.h"

void place_key(int start_edge, int exit_edge) {
    // get a random key edge that is different from the start and exit edges
    int key_edge = start_edge;
    while (exit_edge == start_edge || key_edge == exit_edge) {
        exit_edge = rand() % 4;
    }

    // randomly choose direction of key placement


}

void populate_map(int start_edge, int exit_edge) {
    place_key(start_edge, exit_edge);
}
