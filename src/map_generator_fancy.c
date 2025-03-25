#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "map_generator_fancy.h"
#include "map_mode.h"

enum map_tile maze[HEIGHT][WIDTH];

// randomizing direction and stuff

typedef struct {
    int dx, dy;
} Direction;

Direction directions[] = {
    { -1, 0 },     // left
    { 1, 0 },      // right
    { 0, -1 },     // up
    { 0, 1 }       // down
};

void shuffle(Direction *dir, int n) {
    for (int i = 0; i < n - 1; i++) {
        int j = i + rand() / (RAND_MAX / (n - i) + 1);
        Direction tmp = dir[i];
        dir[i] = dir[j];
        dir[j] = tmp;
    }
}

// check if cell is within bounds
int is_in_bounds(int x, int y) {
    return x > 0 && x < WIDTH-1 && y > 0 && y < HEIGHT-1;
}

void dfs(int x, int y) {
    shuffle(directions, 4);
    for (int i = 0; i < 4; i++) {
        int nx = x + directions[i].dx;
        int ny = y + directions[i].dy;
        if (is_in_bounds(nx, ny)) {
            maze[ny][nx] = FLOOR;
            maze[y + directions[i].dy/2][x + directions[i].dx/2] = ' ';
            dfs(nx, ny);
        }
    }
}

// knowck down some walls so the maze looks better
void knock_down_some_walls(int count) {
    int attempts = 0;
    while (count > 0 && attempts < 10000) {
        int x = rand() % (WIDTH-2) + 1;
        int y = rand() % (HEIGHT-2) + 1;
        if (maze[y][x] == WALL) {
            // Only knock down if surrounded by paths
            int adj_paths = 0;
            if (maze[y-1][x] == FLOOR) adj_paths++;
            if (maze[y+1][x] == FLOOR) adj_paths++;
            if (maze[y][x-1] == FLOOR) adj_paths++;
            if (maze[y][x+1] == FLOOR) adj_paths++;
            if (adj_paths >= 2) {
                maze[y][x] = FLOOR;
                count--;
            }
        }
        attempts++;
    }
}

void generate_maze(int start_x, int start_y) {
  	// just walls so path can be carved out
    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            maze[y][x] = WALL;

    // take predifened start and run dfs to carve path
    maze[start_y][start_x] = FLOOR;
    dfs(start_x, start_y);

    // tweak this for more/less loops
    knock_down_some_walls(20);
}

//place exit & make sure the maze is solveable
void place_exit(int *exit_x, int *exit_y) {
    int max_dist = 0;
    for (int x = 1; x < WIDTH-1; x += 2) {
        if (maze[0][x] == FLOOR) {
            *exit_x = x;
            *exit_y = 0;
            return;
        }
        if (maze[HEIGHT-1][x] == FLOOR) {
            *exit_x = x;
            *exit_y = HEIGHT-1;
            return;
        }
    }
    for (int y = 1; y < HEIGHT-1; y += 2) {
        if (maze[y][0] == FLOOR) {
            *exit_x = 0;
            *exit_y = y;
            return;
        }
        if (maze[y][WIDTH-1] == FLOOR) {
            *exit_x = WIDTH-1;
            *exit_y = y;
            return;
        }
    }
}

void generate_map() {
    //srand(time(NULL)); I will do this later

    // must be odd and inside bounds, so (1,1) for now
    int start_x = 1;
    int start_y = 1;
    generate_maze(start_x, start_y);

    int exit_x, exit_y;
    place_exit(&exit_x, &exit_y);

    //this needs to be adjusted but I don't wanna decide myself on how to do it
    //setNewMap(&maze, start_x, start_y);
}
