#include <stdio.h>
#include <math.h>

#include "../../include/termbox2.h"
#include "../../debug/debug.h"

#include "map.h"
#include "map_mode.h"
#include "drawop/draw_light.h"
#include "../logging/logger.h"


vector2d_t player_pos;
int player_has_key = 0;


void set_player_start_pos(const int player_x, const int player_y) {
    player_pos.dx = player_x;
    player_pos.dy = player_y;
    // at the start, tile under the player must be revealed
    revealed_map[player_pos.dx][player_pos.dy] = FLOOR;
}

void draw_map(void) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == player_pos.dx && y == player_pos.dy) {
                tb_printf(x, y, TB_RED, TB_BLACK, "@");
                continue;
            }

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
                    log_msg(ERROR, "map_mode", "Unknown tile type: %d", revealed_map[x][y]);
                    return;
            }
        }
    }
}

void draw_ui(void) {
    tb_printf(0, HEIGHT, TB_WHITE, TB_BLACK, "HP: 100");
    tb_printf(0, HEIGHT + 2, TB_WHITE, TB_BLACK, "Player Position: %d, %d", player_pos.dx, player_pos.dy);
}

int handle_input(const struct tb_event *event) {
    int new_x = player_pos.dx;
    int new_y = player_pos.dy;

    if (event->key == TB_KEY_ESC || event->key == TB_KEY_CTRL_C) return QUIT;

    if (event->key == TB_KEY_ARROW_UP) new_y--;
    if (event->key == TB_KEY_ARROW_DOWN) new_y++;
    if (event->key == TB_KEY_ARROW_LEFT) new_x--;
    if (event->key == TB_KEY_ARROW_RIGHT) new_x++;


    if (new_x >= 0 && new_x < WIDTH && new_y >= 0 && new_y < HEIGHT) {
        switch (map[new_x][new_y]) {
            case WALL:
                //ignore wall
            case START_DOOR:
                // ignore start door
                break;
            case KEY:
                player_has_key = 1;
                player_pos.dx = new_x;
                player_pos.dy = new_y;
                revealed_map[new_x][new_y] = FLOOR;
                break;
            case EXIT_DOOR:
                if (player_has_key) {
                    player_has_key = 0;
                    player_pos.dx = new_x;
                    player_pos.dy = new_y;
                    return NEXT_FLOOR;
                }
            break;
            default:
                player_pos.dx = new_x;
                player_pos.dy = new_y;
                break;
        }
    }
    return CONTINUE;
}

/**
 * Updates the player position based on the player's input and redraws the maze.
 * @return CONTINUE (0) if the game continue, QUIT (1) if the player pressed the exit key.
 */
map_mode_result_t map_mode_update(void) {
    map_mode_result_t do_quit = CONTINUE;
    struct tb_event ev;
    const int ret = tb_peek_event(&ev, 10);
    db_printEventStruct(3, 20, &ev);

    if (ret == TB_OK) {
        tb_printf(50, 50, TB_WHITE, TB_BLACK, "%d", ev.type);
        do_quit = handle_input(&ev);
    }

    draw_light_on_player((int *) map, (int *) revealed_map, HEIGHT, WIDTH, player_pos, LIGHT_RADIUS);
    draw_map();
    draw_ui();
    tb_present();

    return do_quit;
}

int init_map_mode(void) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            revealed_map[x][y] = HIDDEN;
        }
    }
    return 0;
}
