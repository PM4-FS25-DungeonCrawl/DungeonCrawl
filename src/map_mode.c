#include <stdio.h>
#include "map_mode.h"
#include "../debug/debug.h"
#include "../include/termbox2.h"


int maze[HEIGHT][WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1},
    {1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1},
    {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

int revealed_maze[HEIGHT][WIDTH];

Vector2D player_position = {3, 3};

/**
 * @brief draws the maze to the screen, based on the revealed_maze array
 */
void draw_maze(const int init_y, const int init_x) {
    for (int y = init_y; y < HEIGHT + init_y; y++) {
        for (int x = init_x; x < WIDTH + init_x; x++) {
            if (revealed_maze[y][x] == Wall) {
                tb_printf(x, y,TB_BLUE,TB_BLUE, "#");
            } else if (revealed_maze[y][x] == Hidden) {
                tb_printf(x, y,TB_GREEN,TB_GREEN, "X");
            } else if (x == player_position.x && y == player_position.y) {
                tb_printf(x, y, TB_RED, TB_BLACK, "@");
            } else {
                tb_printf(x, y, TB_BLACK, TB_BLACK, " ");
            }
        }
    }
}

void draw_ui(void) {
    tb_printf(0, HEIGHT, TB_WHITE, TB_BLACK, "HP: 100");
    tb_printf(0, HEIGHT + 2, TB_WHITE, TB_BLACK, "Player Position: %d, %d", player_position.x, player_position.y);
}

void handle_input(const struct tb_event *event) {
    int new_x = player_position.x;
    int new_y = player_position.y;

    if (event->key == TB_KEY_ARROW_UP) new_y--;
    if (event->key == TB_KEY_ARROW_DOWN) new_y++;
    if (event->key == TB_KEY_ARROW_LEFT) new_x--;
    if (event->key == TB_KEY_ARROW_RIGHT) new_x++;

    if (maze[new_y][new_x] == 0) {
        player_position.x = new_x;
        player_position.y = new_y;
    }
}

int absolute(const int val) {
    if (val < 0) {
        return -val;
    }
    return val;
}


const Vector2D directions[4] = {
        { 1,  0},  // +x
        {-1,  0},  // -x
        { 0,  1},  // +y
        { 0, -1}   // -y
};

/*
 * Each array row corresponds to the vector in {@link directions} array.
 * A row contains:
 * - the diagonal check vector
 * - the reverse check vector
 */
const Vector2D checks_vector[4][2] = {
    {{-1, 1}, {0, 1}},
    {{1, -1}, {0, -1}},
    {{-1, -1}, {-1, 0}},
    {{1, 1}, {1, 0}}
};

/**
 * @note function description in the header file
 */
void draw_light_on_player(const int* base_map, int* revealed_map, const int height, const int width,
                          const Vector2D player, const int light_radius) {

    if (light_radius <= 0) {
        //light radius is negative or 0, do nothing
        return;
    }

    //line for debug print
    int line = 5;


    for (int i = 0; i < 4; i++) {
        const Vector2D dir = directions[i];
        const Vector2D diagonal_check = checks_vector[i][0];
        const Vector2D reverse_check = checks_vector[i][1];

        int correction = 0;
        int prev_wall_at = -1;

        for (int j = 0; j <= light_radius; j++) {

            const int start_x = player.x + j * dir.y;
            int start_y = player.y + j * dir.x;

            if (dir.x != 0) {
                //if the direction is vertical negate the subtrahend
                start_y = player.y - j * dir.x;
            }

            if (start_x < 0 || start_x >= width || start_y < 0 || start_y >= height) {
                //start position is out of bounds, skip to the next direction
                break;
            }
            for (int k = 1; k <= light_radius - correction; k++) {
                const int x = start_x + k * dir.x;
                const int y = start_y + k * dir.y;
                if (x < 0 || x >= width || y < 0 || y >= height) {
                    //calculated x or y is out of bound
                    break;
                }

                //calculated access index
                const int access_idx = y * width + x;

                //debug print
                DEBUG_PRINT(WIDTH + 5, line, "dir=(%d,%d), j=%d, k=%d, sx=%d, sy=%d, x=%d, y=%d, idx=%d\n", dir.x, dir.y, j, k, start_x, start_y, x, y, access_idx);
                line += 1;

                if (revealed_map[access_idx] == Hidden) {
                    //initialize the relative diagonal and reverse tiles based on the y and x values
                    const int rel_diagonal = base_map[(y + diagonal_check.y) * width + (x + diagonal_check.x)];
                    const int rel_reverse = base_map[(y + reverse_check.y) * width + (x + reverse_check.x)];

                    if (rel_diagonal == Wall && rel_reverse == Wall && j > 1) {
                        //if the diagonal and reverse tiles are walls, and the distance from the player is greater than 1
                        //then the tile must be hidden because reverse tile is blocking the view
                        DEBUG_PRINT(WIDTH + 5, line, "Edge case (diag + reverse) detected");
                        line += 1;
                        break;
                    }

                    DEBUG_PRINT(WIDTH + 5, line, "Tile with access index %d is %d", access_idx, base_map[access_idx]);
                    line += 1;

                    if (base_map[access_idx] == Wall) {
                        revealed_map[access_idx] = Wall;
                        DEBUG_PRINT(WIDTH + 5, line, "Wall detected at (%d,%d) with dir(%d,%d) -> break", x, y, dir.x, dir.y);
                        line += 1;

                        if (j == 0) {
                            //gets the x or y value of the calculated coordinates
                            prev_wall_at = absolute(y * dir.y + x * dir.x);
                            break;
                        }
                        if (prev_wall_at == absolute(y * dir.y + x * dir.x)) {
                            //if the previous j-loop had a wall at x or y coordinate, the loop must break
                            //or else diagonals tiles (behind a wall and not visible by the player) are revealed
                            break;
                        }
                    }
                    if (base_map[access_idx] == Floor) {
                        revealed_map[access_idx] = Floor;
                    }
                } else if (revealed_map[access_idx] == Wall) {
                    if (j == 0) {
                        //gets the x or y value of the calculated coordinates
                        prev_wall_at = absolute(y * dir.y + x * dir.x);
                        break;
                    }
                    if (prev_wall_at == absolute(y * dir.y + x * dir.x)) {
                        //if the previous j-loop had a wall at x or y coordinate, the loop must break
                        //or else diagonals tiles (behind a wall and not visible by the player) are revealed
                        break;
                    }
                }
            }
            correction++;
        }
    }
}

//debug counters to count how many times the function is called
int count_call_mmu = 0;
//debug counter to count
int ret_i_o = 0;

int map_mode_update(void) {
    count_call_mmu++;
    DEBUG_PRINT(WIDTH+5, 2, "map_mode_update called nr: %d", count_call_mmu);

    struct tb_event ev;
    const int ret = tb_peek_event(&ev, 10);
    db_printEventStruct(3, 20, &ev);
    
    if (ret == TB_OK) {
        ret_i_o++;
        DEBUG_PRINT(WIDTH+5, 3, "ret in map_mode_update i.O. nr: %d", ret_i_o);

        tb_printf(50, 50, TB_WHITE, TB_BLACK, "%d", ev.type);
        if (ev.key == TB_KEY_ESC || ev.key == TB_KEY_CTRL_C) return QUIT;
        handle_input(&ev);

        draw_light_on_player((int *)maze, (int *)revealed_maze, HEIGHT, WIDTH, player_position, LIGHT_RADIUS);
    }

    draw_maze(0, 0);
    draw_ui();
    tb_present();

    return CONTINUE;
}

int init_map_mode(void) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            revealed_maze[y][x] = Hidden;
        }
    }
    DEBUG_PRINT(WIDTH+5, 0, "init_map_mode");
    // at the start, tile under the player must be revealed
    revealed_maze[player_position.y][player_position.x] = Floor;
    DEBUG_PRINT(WIDTH+5, 1, "player position revealed");
    return 0;
}
