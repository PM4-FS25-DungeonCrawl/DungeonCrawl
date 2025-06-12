/**
 * @file map_populator.c
 * @brief Implements functionality for populating the generated game map.
 */
#include "map_populator.h"

#include "map.h"

#include <stdlib.h>

/**
 * Check if a cell is a dead end (is floor and has only one neighboring non-wall cell)
 * @param x the x coordinate of the cell
 * @param y the y coordinate of the cell
 * @return 1 if the cell is a dead end, 0 otherwise
 */
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


/**
 * @brief Place a key in a dead end that is not the start or exit edge
 */
void place_key() {
    int x;
    int y;

    // place the key in the first dead end found
    do {
        x = rand() % (WIDTH - 2) + 1;
        y = rand() % (HEIGHT - 2) + 1;
    } while (!is_dead_end(x, y));

    map[x][y] = KEY;
}


/**
 * @brief Check if a cell is close to an enemy (based on ENEMY_MIN_DISTANCE)
 * @param x the x coordinate of the cell
 * @param y the y coordinate of the cell
 * @return 1 if the cell is close to an enemy, 0 otherwise
 */
int is_close_to_enemy(int x, int y) {
    for (int i = -ENEMY_MIN_DISTANCE; i <= ENEMY_MIN_DISTANCE + 1; i++) {
        for (int j = -ENEMY_MIN_DISTANCE; j <= ENEMY_MIN_DISTANCE + 1; j++) {
            if (map[x + i][y + j] == GOBLIN || map[x + i][y + j] == START_DOOR) {
                return 1;
            }
        }
    }
    return 0;
}

/**
 * @brief Place enemies in random locations on the map
 */
void place_enemies() {
    for (int i = 0; i < ENEMY_COUNT; i++) {
        int x;
        int y;

        // Place enemies in random locations
        do {
            x = rand() % (WIDTH - 2) + 1;
            y = rand() % (HEIGHT - 2) + 1;
        } while (map[x][y] != FLOOR || is_close_to_enemy(x, y));

        map[x][y] = GOBLIN;
    }
}


/**
 * @brief Place a mana fountain and a life fountain in random dead ends on the map
 */
void place_fountains() {
    int x;
    int y;

    // place the life fountain in the first dead end found
    do {
        x = rand() % (WIDTH - 2) + 1;
        y = rand() % (HEIGHT - 2) + 1;
    } while (!is_dead_end(x, y));

    map[x][y] = LIFE_FOUNTAIN;

    // place the mana fountain in the first dead end found
    do {
        x = rand() % (WIDTH - 2) + 1;
        y = rand() % (HEIGHT - 2) + 1;
    } while (!is_dead_end(x, y));

    map[x][y] = MANA_FOUNTAIN;
}


void populate_map() {
    place_key();
    place_enemies();
    place_fountains();
}
