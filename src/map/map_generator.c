#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#include "map_generator.h"
#include "map.h"
#include "map_mode.h"
#include "map_populator.h"

// map array to store the maze
int visited[WIDTH][HEIGHT];


// Shuffle array using Fisher-Yates algorithm
void shuffle(Vector2D *dir, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Vector2D tmp = dir[j];
        dir[j] = dir[i];
        dir[i] = tmp;
    }
}

// Check if cell is within bounds of the map
int is_in_bounds(int x, int y) {
    return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT;
}

// Check if a cell is a valid cell to visit (in bounds and not visited)
int is_valid_cell(int x, int y) {
    return is_in_bounds(x, y) && !visited[x][y];
}

int validate_exit_position(int exit_edge, int x, int y) {
    switch (exit_edge) {
        case TOP:
            return map[x][y + 1] == FLOOR;
        case BOTTOM:
            return map[x][y - 1] == FLOOR;
        case LEFT:
            return map[x + 1][y] == FLOOR;
        case RIGHT:
            return map[x - 1][y] == FLOOR;
        default:
            //TODO log error
            return 0;
    }
}

// Recursive backtracking algorithm to generate map (based on dfs)
void carve_passages(int x, int y) {
    visited[x][y] = 1;
    map[x][y] = FLOOR;

    // Create a copy of directions and shuffle them
    Vector2D shuffled_dirs[4];
    for (int i = 0; i < 4; i++) {
        shuffled_dirs[i] = directions[i];
    }
    shuffle(shuffled_dirs, 4);

    // Try each direction in random order
    for (int i = 0; i < 4; i++) {
        int nx = x + shuffled_dirs[i].dx * 2; // Move two cells in the direction
        int ny = y + shuffled_dirs[i].dy * 2;

        if (is_valid_cell(nx, ny)) {
            // Carve passage by setting the cell between current and next to FLOOR
            int wall_x = x + shuffled_dirs[i].dx;
            int wall_y = y + shuffled_dirs[i].dy;

            // make the wall a floor and continue the path
            map[wall_x][wall_y] = FLOOR;
            carve_passages(nx, ny);
        }
    }
}

// Add loops to the map by knocking down some walls
void add_loops(int num_loops) {
    int count = 0;
    int max_attempts = num_loops * 10; // Limit the number of attempts

    while (count < num_loops && max_attempts > 0) {
        // Pick a random cell
        int x = 1 + rand() % (WIDTH - 2);
        int y = 1 + rand() % (HEIGHT - 2);

        // If the wall has exactly 2 opposing floor neighbors, knock it down to create a loop
        if (map[x][y] == WALL) {
            int neighbor_directions[] = {0, 0, 0, 0};
            // Count neighboring floor cells
            int floor_count = 0;

            for (int i = 0; i < 4; i++) {
                int dx = x + directions[i].dx;
                int dy = y + directions[i].dy;

                if (map[dx][dy] == FLOOR) {
                    floor_count++;
                    neighbor_directions[i] = 1;
                }
            }

            // check if the wall has exactly 2 opposing floor neighbors
            if (floor_count == 2) {
                if ((neighbor_directions[TOP] && neighbor_directions[BOTTOM]) || (
                        neighbor_directions[LEFT] && neighbor_directions[RIGHT])) {
                    map[x][y] = FLOOR;
                    count++;
                }
            }
        }

        max_attempts--;
    }
}

// Place the exit on a random edge of the map, ensuring there's a path to it
void place_exit(int start_edge, int *exit_x, int *exit_y) {
    // get a random exit edge that is different from the start edge
    int exit_edge = start_edge;
    while (exit_edge == start_edge) {
        exit_edge = rand() % 4;
    }

    do {
        switch (exit_edge) {
            case TOP:
                *exit_x = 1 + 2 * (rand() % ((WIDTH - 2) / 2));
                *exit_y = 0;
                break;
            case BOTTOM:
                *exit_x = 1 + 2 * (rand() % ((WIDTH - 2) / 2));
                *exit_y = HEIGHT - 1;
                break;
            case LEFT:
                *exit_x = 0;
                *exit_y = 1 + 2 * (rand() % ((HEIGHT - 2) / 2));
                break;
            case RIGHT:
                *exit_x = WIDTH - 1;
                *exit_y = 1 + 2 * (rand() % ((HEIGHT - 2) / 2));
                break;
            default:
                //TODO log error
                return;
        }
    } while (!validate_exit_position(exit_edge, *exit_x, *exit_y));

    map[*exit_x][*exit_y] = EXIT_DOOR;
}


// Initialize the map with walls
void initialize_map() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            map[x][y] = WALL;
            visited[x][y] = 0;
        }
    }
}

// Generate a new map
void generate_maze(int start_x, int start_y) {
    // Make sure start position is valid for dfs (odd coordinates)
    // relevant if we want to implement the starting position differently
    if (start_x % 2 == 0) {
        start_x++;
    }
    if (start_y % 2 == 0) {
        start_y++;
    }

    // Generate the map using recursive backtracking
    carve_passages(start_x, start_y);

    // Add some loops to the map
    int num_loops = (WIDTH * HEIGHT) / 100 + 1; // Use fewer loops to prevent overflow
    add_loops(num_loops);
}

// set the start position on the map
void set_start_position(int start_edge, int *start_x, int *start_y) {
    switch (start_edge) {
        case TOP:
            *start_x = 3 + 2 * (rand() % ((WIDTH - 5) / 2));
            *start_y = 1;
            map[*start_x][0] = START_DOOR;
            break;
        case RIGHT:
            *start_x = WIDTH - 2;
            *start_y = 3 + 2 * (rand() % ((HEIGHT - 5) / 2));
            map[WIDTH - 1][*start_y] = START_DOOR;
            break;
        case BOTTOM:
            *start_x = 3 + 2 * (rand() % ((WIDTH - 5) / 2));
            *start_y = HEIGHT - 2;
            map[*start_x][HEIGHT - 1] = START_DOOR;
            break;
        case LEFT:
            *start_x = 1;
            *start_y = 3 + 2 * (rand() % ((HEIGHT - 5) / 2));
            map[0][*start_y] = START_DOOR;
            break;
        default:
            // TODO log error
            return;
    }
}

// generate the map and populate it with keys, enemies, and the exit
void generate_map() {
    // Better random seed using a combination of time and process info
    unsigned int seed = (unsigned int) time(nullptr);
    // XOR with address of a stack variable to add more entropy
    int stack_var;
    seed ^= (uintptr_t) &stack_var;
    srand(seed);

    // Initialize the map with walls
    initialize_map();

    // Generate map with a random start position (at the start_edge)
    // Start position must be an odd coordinate (for dfs) and should be at least 3 cells away from other edges (not start_edge)
    int start_x, start_y;
    int start_edge = rand() % 4;
    set_start_position(start_edge, &start_x, &start_y);

    generate_maze(start_x, start_y);

    int exit_x, exit_y;
    place_exit(start_edge, &exit_x, &exit_y);

    populate_map();

    set_start(start_x, start_y);
}
