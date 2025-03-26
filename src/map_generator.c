#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>

#include "map_generator.h"
#include "map_mode.h"

//TODO: refactor!!

//maze array
enum map_tile maze[WIDTH][HEIGHT];
bool visited[WIDTH][HEIGHT];

typedef struct {
    int dx, dy;
} Direction;

Direction directions[] = {
    { -1, 0 },  // left
    { 1, 0 },   // right
    { 0, -1 },  // up
    { 0, 1 }    // down
};

// Shuffle array using Fisher-Yates algorithm
void shuffle(Direction *dir, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Direction tmp = dir[j];
        dir[j] = dir[i];
        dir[i] = tmp;
    }
}

// Check if cell is within bounds of the maze
int is_in_bounds(int x, int y) {
    return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT;
}

// Check if a cell is a valid cell to visit (in bounds and not visited)
int is_valid_cell(int x, int y) {
    return is_in_bounds(x, y) && !visited[x][y];
}

// Recursive backtracking algorithm to generate maze (based on dfs)
void carve_passages(int x, int y) {
    visited[x][y] = true;
    maze[x][y] = FLOOR;
    
    // Create a copy of directions and shuffle them
    Direction shuffled_dirs[4];
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
            
            // Check bounds for the wall cell
            if (is_in_bounds(wall_x, wall_y)) {
                maze[wall_x][wall_y] = FLOOR;
                carve_passages(nx, ny);
            }
        }
    }
}

// Add loops to the maze by knocking down some walls
void add_loops(int num_loops) {
    int count = 0;
    int max_attempts = num_loops * 10; // Limit the number of attempts
    
    while (count < num_loops && max_attempts > 0) {
        // Pick a random wall
        int x = 1 + rand() % (WIDTH - 2);
        int y = 1 + rand() % (HEIGHT - 2);
        
        if (maze[x][y] == WALL) {
            // Count neighboring floor cells
            int floor_count = 0;
            int dx, dy;
            
            for (int i = 0; i < 4; i++) {
                dx = x + directions[i].dx;
                dy = y + directions[i].dy;
                
                if (is_in_bounds(dx, dy) && maze[dx][dy] == FLOOR) {
                    floor_count++;
                }
            }
            
            // If the wall has 2 or more floor neighbors, knock it down to create a loop
            if (floor_count >= 2) {
                maze[x][y] = FLOOR;
                count++;
            }
        }
        
        max_attempts--;
    }
}

// Place the exit on a random edge of the maze, ensuring there's a path to it
void place_exit(int start_x, int start_y, int start_edge, int *exit_x, int *exit_y) {
    // Define the edges where the exit can be placed
    int exit_edge = start_edge;
    while (exit_edge == start_edge) {
        exit_edge = rand() % 4;
    }

    do {
        switch(exit_edge) {
            case 0: // Top exit_edge
                *exit_x = 1 + 2 * (rand() % ((WIDTH - 2) / 2));
                *exit_y = 0;
                maze[*exit_x][0] = FLOOR;
                break;
            case 1: // Right exit_edge
                *exit_x = WIDTH - 1;
                *exit_y = 1 + 2 * (rand() % ((HEIGHT - 2) / 2));
                maze[WIDTH - 1][*exit_y] = FLOOR;
                break;
            case 2: // Bottom exit_edge
                *exit_x = 1 + 2 * (rand() % ((WIDTH - 2) / 2));
                *exit_y = HEIGHT - 1;
                maze[*exit_x][HEIGHT - 1] = FLOOR;
                break;
            case 3: // Left exit_edge
                *exit_x = 0;
                *exit_y = 1 + 2 * (rand() % ((HEIGHT - 2) / 2));
                maze[0][*exit_y] = FLOOR;
                break;
        }
    } while (!is_in_bounds(*exit_x, *exit_y) || !maze[*exit_x][*exit_y] == FLOOR);

    //TODO: Implement a method to validate exit position (also path length from start to exit)
}
    

// Initialize the maze with walls
void initialize_maze() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            maze[x][y] = WALL;
            visited[x][y] = false;
        }
    }
}

// Generate a new maze
void generate_maze(int start_x, int start_y) {
      
    // Make sure start position is valid for dfs (odd coordinates)
    // relevant if we want to implement the starting position differently
    if (start_x % 2 == 0) {
        start_x++;
    }
    if (start_y % 2 == 0) {
        start_y++;
    }
    
    // Ensure start position is within bounds
    start_x = start_x < 1 ? 1 : (start_x >= WIDTH - 1 ? WIDTH - 2 : start_x);
    start_y = start_y < 1 ? 1 : (start_y >= HEIGHT - 1 ? HEIGHT - 2 : start_y);
    
    // Generate the maze using recursive backtracking
    carve_passages(start_x, start_y);
    
    // Add some loops to the maze
    int num_loops = (WIDTH * HEIGHT) / 100 + 1; // Use fewer loops to prevent overflow
    add_loops(num_loops);
    
}

void generate_map() {
    // Better random seed using a combination of time and process info
    unsigned int seed = (unsigned int)time(NULL);
    // XOR with address of a stack variable to add more entropy
    int stack_var;
    seed ^= (uintptr_t)&stack_var;
    srand(seed);

    // Initialize the maze with walls
    initialize_maze();
    
    // Generate maze with a random start position (at the start_edge)
    // Start position must be an odd coordinate (for dfs) and should be at least 3 cells away from other edges (not start_edge)
    int start_x, start_y;
    int start_edge = rand() % 4;

    switch(start_edge) {
        case 0: // Top start_edge
            start_x = 3 + 2 * (rand() % ((WIDTH - 5) / 2));
            start_y = 1;
            maze[start_x][0] = FLOOR;
            break;
        case 1: // Right start_edge
            start_x = WIDTH - 2;
            start_y = 3 + 2 * (rand() % ((HEIGHT - 5) / 2));
            maze[WIDTH - 1][start_y] = FLOOR;
            break;
        case 2: // Bottom start_edge
            start_x = 3 + 2 * (rand() % ((WIDTH - 5) / 2));
            start_y = HEIGHT - 2;
            maze[start_x][HEIGHT - 1] = FLOOR;
            break;
        case 3: // Left start_edge
            start_x = 1;
            start_y = 3 + 2 * (rand() % ((HEIGHT - 5) / 2));
            maze[0][start_y] = FLOOR;
            break;
    }
    generate_maze(start_x, start_y);
    
    // Place exit and ensure there's a path from start to exit
    int exit_x, exit_y;
    place_exit(start_x, start_y, start_edge, &exit_x, &exit_y);
    
    // Set the new map
    setNewMap(&maze, start_x, start_y);
}