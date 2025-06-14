/**
 * @file draw_light.c
 * @brief Implements functionality fro drawing light on player.
 */
#include "draw_light.h"

#include <stdlib.h>

map_tile_t* map_arr;
map_tile_t* revealed_map_arr;
int map_height;
int map_width;
vector2d_t player_position;
int radius;

/**
 * Each array row corresponds to the vector in the directions array.
 * A row contains:
 * - the diagonal check vector
 * - the reverse check vector
 */
const vector2d_t checks_vector[4][2] = {
        {{1, 1}, {1, 0}},   // for up
        {{-1, -1}, {-1, 0}},// for down
        {{1, -1}, {0, -1}}, // for left
        {{-1, 1}, {0, 1}},  // for right
};


/**
 * @brief This is a helper function to check if the loop needs to break with this specific edge case.
 * @param x current tile x-coordinates
 * @param y current tile y-coordinates
 * @param dir the direction to check
 * @param j loop counter
 * @param prev_wall_at pointer to variable, which should be written
 * @return 1 if loops must break, 0 when not
 */
int need_loop_break(const int x, const int y, const vector2d_t dir, int j, int* prev_wall_at) {
    if (j == 0) {
        //gets the x or y value of the calculated coordinates
        *prev_wall_at = abs(y * dir.dy + x * dir.dx);
        return 1;
    }
    if (*prev_wall_at == abs(y * dir.dy + x * dir.dx)) {
        //if the previous j-loop had a wall at x or y coordinate, the loop must break
        //or else diagonals tiles (behind a wall and not visible by the player) are revealed
        return 1;
    }
    return 0;
}


/**
 * @brief This function processes the tile at the given coordinates and updates the revealed_map_arr array.
 * @param x current tile x-coordinates
 * @param y current tile y-coordinates
 * @param prev_wall_at index of the previous wall tile
 * @param dir the direction to check
 * @param j loop counter
 * @return
 */
int process_tile(int x, int y, int* prev_wall_at, const vector2d_t dir, int j) {
    int break_loop = 0;
    //calculated access index
    const int access_idx = x * map_height + y;

    revealed_map_arr[access_idx] = map_arr[access_idx];

    if (map_arr[access_idx] == WALL) {
        break_loop = need_loop_break(x, y, dir, j, prev_wall_at);
    }
    return break_loop;
}


/**
 * @brief This function checks if the tile at the given coordinates is valid and processes it.
 * @param x current tile x-coordinates
 * @param y current tile y-coordinates
 * @param prev_wall_at index of the previous wall tile
 * @param dir the direction to check
 * @param j loop counter
 * @param diagonal_check the diagonal check vector
 * @param reverse_check the reverse check vector
 * @return 1 if loops must break, 0 when not
 */
int check_and_process_tile(int x, int y, int* prev_wall_at, const vector2d_t dir, int j,
                           const vector2d_t diagonal_check, const vector2d_t reverse_check) {
    int quit = 0;
    //calculated access index
    const int access_idx = x * map_height + y;

    if (x < 0 || x >= map_width || y < 0 || y >= map_height) {
        //calculated x or y is out of bounds
        quit = 1;
    } else {
        if (revealed_map_arr[access_idx] == HIDDEN) {
            //initialize the relative diagonal and reverse tiles based on the y and x values
            const int rel_diagonal = map_arr[(x + diagonal_check.dx) * map_height + y + diagonal_check.dy];
            const int rel_reverse = map_arr[(x + reverse_check.dx) * map_height + y + reverse_check.dy];

            if (rel_diagonal == WALL && rel_reverse == WALL && j > 1) {
                //if the diagonal and reverse tiles are walls and the distance from the player is greater than 1
                // then the tile must be hidden because reverse tile is blocking the view
                quit = 1;
            } else {
                quit = process_tile(x, y, prev_wall_at, dir, j);
            }
        } else if (revealed_map_arr[access_idx] == WALL && need_loop_break(x, y, dir, j, prev_wall_at)) {
            quit = 1;
        }
    }
    return quit;
}


/**
 * @brief Processes light in a specific direction and updates the revealed map_arr.
 * @param player the player's position on the map_arr
 * @param dir the direction vector
 * @param diagonal_check the diagonal check vector
 * @param reverse_check the reverse check vector
 * @param light_radius the radius of the light around the player
 */
void process_light_in_direction(const vector2d_t player, const vector2d_t dir, const vector2d_t diagonal_check,
                                const vector2d_t reverse_check, const int light_radius) {
    int correction = 0;
    int prev_wall_at = -1;

    for (int j = 0; j <= light_radius; j++) {
        const int start_x = player.dx + j * dir.dy;
        int start_y = player.dy + j * dir.dx;

        if (dir.dx != 0) {
            //if the direction is vertical negate the subtrahend
            start_y = player.dy - j * dir.dx;
        }

        if (start_x < 0 || start_x >= map_width || start_y < 0 || start_y >= map_height) {
            //start position is out of bounds, skip to the next direction
            break;
        }
        for (int k = 1; k <= light_radius - correction; k++) {
            const int x = start_x + k * dir.dx;
            const int y = start_y + k * dir.dy;

            if (check_and_process_tile(x, y, &prev_wall_at, dir, j, diagonal_check, reverse_check)) {
                break;
            }
        }
        correction++;
    }
}

void draw_light_on_player(map_tile_t* arr1, map_tile_t* arr2, int height, int width, vector2d_t player,
                          const int light_radius) {
    map_arr = arr1;
    revealed_map_arr = arr2;
    map_height = height;
    map_width = width;
    player_position = player;
    radius = light_radius;

    if (light_radius <= 0) {
        //light radius is negative or 0, do nothing
        return;
    }

    for (int i = 0; i < 4; i++) {
        const vector2d_t dir = directions[i];
        const vector2d_t diagonal_check = checks_vector[i][0];
        const vector2d_t reverse_check = checks_vector[i][1];

        process_light_in_direction(player, dir, diagonal_check, reverse_check, light_radius);
    }
}
