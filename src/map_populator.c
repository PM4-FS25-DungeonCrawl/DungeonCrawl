#include <stdlib.h>

#include "map_populator.h"
#include "map.h"

// Check if a cell is a dead end (is floor and has only one neighboring non-wall cell)
int is_dead_end(int x, int y) {
    // Count neighboring non-wall cells
    int neighbor_count = 0;

    for (int i = 0; i < 4; i++) {
        int dx = x + directions[i].dx;
        int dy = y + directions[i].dy;

        if (map[dx][dy] != WALL) {
            neighbor_count++;
        }
    }

    return neighbor_count == 1 && map[x][y] == FLOOR;
}

// Place a key in a dead end that is not the start or exit edge
void place_key() {
    int x, y;

    // check for dead ends in a snake pattern and place the key in the first one found
    do {
        x = rand() % (WIDTH - 2) + 1;
        y = rand() % (HEIGHT - 2) + 1;
    } while (!is_dead_end(x, y));

    map[x][y] = KEY;
}

// Populate the map
void populate_map() {
    place_key();
}
