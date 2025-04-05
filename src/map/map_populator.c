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
    int x;
    int y;

    // check for dead ends in a snake pattern and place the key in the first one found
    do {
        x = rand() % (WIDTH - 2) + 1;
        y = rand() % (HEIGHT - 2) + 1;
    } while (!is_dead_end(x, y));

    map[x][y] = KEY;
}

// Check if a cell is close to an enemy (based on ENEMY_MIN_DISTANCE)
int is_close_to_enemy(int x, int y) {
    for (int i = -ENEMY_MIN_DISTANCE; i <= ENEMY_MIN_DISTANCE + 1; i++) {
        for (int j = -ENEMY_MIN_DISTANCE; j <= ENEMY_MIN_DISTANCE + 1; j++) {
            if (map[x + i][y + j] == SKELETON || map[x + i][y + j] == START_DOOR) {
                return 1;
            }
        }
    }
    return 0;
}

// Place enemies in random locations
void place_enemies() {
    for (int i = 0; i < ENEMY_COUNT; i++) {
        int x;
        int y;

        // Place enemies in random locations
        do {
            x = rand() % (WIDTH - 2) + 1;
            y = rand() % (HEIGHT - 2) + 1;
        } while (map[x][y] != FLOOR || is_close_to_enemy(x, y));

        map[x][y] = SKELETON;
    }
}

// Populate the map
void populate_map() {
    place_key();
    place_enemies();
}
