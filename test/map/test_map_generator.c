#include "../src/map/map.h"
#include "../src/map/map_generator.h"

#include <assert.h>
#include <stdio.h>

int visited_test[WIDTH][HEIGHT];

/**
 * Perform DFS to check if there is a path from start to exit
 * @param x current x coordinate
 * @param y current y coordinate
 * @return 1 if a path to EXIT_DOOR is found, 0 otherwise
 */
int dfs_check_path(int x, int y) {
    // If out of bounds or already visited, return 0
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT || visited_test[x][y]) {
        return 0;
    }

    // Mark the current cell as visited
    visited_test[x][y] = 1;

    // If we reach the EXIT_DOOR, return 1
    if (map[x][y] == EXIT_DOOR) {
        return 1;
    }

    // If the current cell is a wall, return 0
    if (map[x][y] == WALL) {
        return 0;
    }

    // Explore all four directions
    for (int i = 0; i < 4; i++) {
        int nx = x + directions[i].dx;
        int ny = y + directions[i].dy;

        if (dfs_check_path(nx, ny)) {
            return 1;
        }
    }

    return 0;
}

/**
 * Test function to verify if there is a path from START_DOOR to EXIT_DOOR
 */
void test_map_generator_path() {
    // Generate the map
    generate_map();

    // Find the start position
    int start_x = -1, start_y = -1;
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (map[x][y] == START_DOOR) {
                start_x = x;
                start_y = y;
                break;
            }
        }
        if (start_x != -1) break;
    }

    // Ensure the start position was found
    assert(start_x != -1 && start_y != -1);

    // Reset the visited array
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            visited_test[x][y] = 0;
        }
    }

    // Perform DFS to check if a path exists
    int path_exists = dfs_check_path(start_x, start_y);

    // Assert that a path exists
    assert(path_exists == 1);

    printf("Test passed: Path from START_DOOR to EXIT_DOOR exists.\n");
}

int main(void) {
    test_map_generator_path();
    return 0;
}
