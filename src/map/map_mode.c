#include <stdio.h>
#include <math.h>

#include "../../include/termbox2.h"
#include "../../debug/debug.h"

#include "map.h"
#include "map_mode.h"
#include "drawop/draw_light.h"


map_tile revealed_map[WIDTH][HEIGHT];

Vector2D player_pos;

void set_start(const int newPlayerX, const int newPlayerY) {
    player_pos.dx = newPlayerX;
    player_pos.dy = newPlayerY;

    // at the start, tile under the player must be revealed
    revealed_map[player_pos.dx][player_pos.dy] = FLOOR;
}

void draw_map(void) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == player_pos.dx && y == player_pos.dy) {
                tb_printf(x, y, TB_RED, TB_BLACK, "@");
            } else {
                switch (revealed_map[x][y]) {
                    case WALL:
                        tb_printf(x, y, TB_BLUE, TB_BLUE, "#");
                        break;
                    case FLOOR:
                        tb_printf(x, y, TB_WHITE, TB_BLACK, " ");
                        break;
                    case START_DOOR:
                        tb_printf(x, y, TB_GREEN, TB_BLACK, "#");
                        break;
                    case EXIT_DOOR:
                        tb_printf(x, y, TB_YELLOW, TB_BLACK, "#");
                        break;
                    case KEY:
                        tb_printf(x, y, TB_YELLOW, TB_BLACK, "$");
                        break;
                    case SKELETON:
                        tb_printf(x, y, TB_WHITE, TB_RED, "!");
                        break;
                    case HIDDEN:
                        tb_printf(x, y, TB_WHITE, TB_WHITE, " ");
                        break;
                    default:
                        //TODO log error
                        return;
                }
            }
        }
    }
}

void draw_ui(void) {
    tb_printf(0, HEIGHT, TB_WHITE, TB_BLACK, "HP: 100");
    tb_printf(0, HEIGHT + 2, TB_WHITE, TB_BLACK, "Player Position: %d, %d", player_pos.dx, player_pos.dy);
}

void handle_input(const struct tb_event *event) {
    int new_x = player_pos.dx;
    int new_y = player_pos.dy;

    if (event->key == TB_KEY_ARROW_UP) new_y--;
    if (event->key == TB_KEY_ARROW_DOWN) new_y++;
    if (event->key == TB_KEY_ARROW_LEFT) new_x--;
    if (event->key == TB_KEY_ARROW_RIGHT) new_x++;


    if (new_x >= 0 && new_x < WIDTH && new_y >= 0 && new_y < HEIGHT) {
        switch (map[new_x][new_y]) {
            case WALL:
                //ignore wall
                break;
            default:
                //TODO: extend functionality with different tiles
                player_pos.dx = new_x;
                player_pos.dy = new_y;

                draw_light_on_player((int *) map, (int *) revealed_map, HEIGHT, WIDTH, player_pos, LIGHT_RADIUS);
                break;
        }
    }
}

/**
 * Updates the player position based on the player's input and redraws the maze.
 * @return CONTINUE (0) if the game continue, QUIT (1) if the player pressed the exit key.
 */
int map_mode_update(void) {
    struct tb_event ev;
    const int ret = tb_peek_event(&ev, 10);
    db_printEventStruct(3, 20, &ev);
    
    if (ret == TB_OK) {

        tb_printf(50, 50, TB_WHITE, TB_BLACK, "%d", ev.type);
        if (ev.key == TB_KEY_ESC || ev.key == TB_KEY_CTRL_C) return QUIT;
        handle_input(&ev);
    }

    draw_map();
    draw_ui();
    tb_present();

    return CONTINUE;
}

int init_map_mode(void) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            revealed_map[x][y] = HIDDEN;
        }
    }
    return 0;
}
