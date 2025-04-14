#include "map_mode.h"

#include "../../include/termbox2.h"
#include "../logging/logger.h"
#include "draw/draw_light.h"
#include "draw/draw_map_mode.h"
#include "map.h"

vector2d_t map_anchor = {0, 0};
vector2d_t player_pos;
int player_has_key = 0;


void set_player_start_pos(const int player_x, const int player_y) {
    player_pos.dx = player_x;
    player_pos.dy = player_y;
    // at the start, tile under the player must be revealed
    revealed_map[player_pos.dx][player_pos.dy] = FLOOR;
}

map_mode_result_t handle_input(const struct tb_event* event) {
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
                // ignore wall
                // break;
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
            case GOBLIN:
                player_pos.dx = new_x;
                player_pos.dy = new_y;
                map[new_x][new_y] = FLOOR;
                revealed_map[new_x][new_y] = FLOOR;
                return COMBAT;
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
    map_mode_result_t next_state = CONTINUE;
    struct tb_event ev;
    const int ret = tb_peek_event(&ev, 10);

    if (ret == TB_OK) {
        next_state = handle_input(&ev);
    }
    tb_clear();

    draw_light_on_player(map, revealed_map, HEIGHT, WIDTH, player_pos, LIGHT_RADIUS);
    draw_map_mode(revealed_map, HEIGHT, WIDTH, map_anchor, player_pos);

    tb_present();

    return next_state;
}

int init_map_mode(void) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            revealed_map[x][y] = HIDDEN;
        }
    }
    return 0;
}
