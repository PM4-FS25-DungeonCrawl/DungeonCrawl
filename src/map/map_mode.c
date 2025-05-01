#include "map_mode.h"

#include "../game.h"
#include "draw/draw_light.h"
#include "draw/draw_map_mode.h"
#include "map.h"

#include <notcurses/notcurses.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __APPLE__
    #define KEY_EVENT NCTYPE_PRESS
#else
    #define KEY_EVENT NCTYPE_UNKNOWN
#endif /* ifdef __APPLE__ */

vector2d_t map_anchor = {5, 2};
vector2d_t player_pos;
int player_has_key = 0;
bool first_function_call = true;


void set_player_start_pos(const int player_x, const int player_y) {
    player_pos.dx = player_x;
    player_pos.dy = player_y;
    // at the start, tile under the player must be revealed
    revealed_map[player_pos.dx][player_pos.dy] = FLOOR;
}


vector2d_t get_player_pos() {
    return player_pos;
}


map_mode_result_t handle_input(const ncinput* event, character_t* player) {
    int new_x = player_pos.dx;
    int new_y = player_pos.dy;

    if (event->id == 'c' && (event->modifiers & NCKEY_MOD_CTRL)) return QUIT;

    // Check for 'M' key press for menu
    if (event->id == 'm' || event->id == 'M' || event->id == NCKEY_ESC) return SHOW_MENU;

    // Only process arrow key events that are PRESS type (ignore RELEASE events)
    if (event->evtype == NCTYPE_UNKNOWN || event->evtype == NCTYPE_PRESS) {
        if (event->id == NCKEY_UP) new_y--;
        if (event->id == NCKEY_DOWN) new_y++;
        if (event->id == NCKEY_LEFT) new_x--;
        if (event->id == NCKEY_RIGHT) new_x++;
    }


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
            case LIFE_FOUNTAIN:
                player->current_resources.health = player->max_resources.health;
                player_pos.dx = new_x;
                player_pos.dy = new_y;
                revealed_map[new_x][new_y] = FLOOR;
                break;
            case MANA_FOUNTAIN:
                player->current_resources.mana = player->max_resources.mana;
                player_pos.dx = new_x;
                player_pos.dy = new_y;
                revealed_map[new_x][new_y] = FLOOR;
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
map_mode_result_t map_mode_update(character_t* player) {
    map_mode_result_t next_state = CONTINUE;
    ncinput ev;
    memset(&ev, 0, sizeof(ev));
    if (!first_function_call) {
        const uint32_t ret = notcurses_get_blocking(nc, &ev);
        if (ret > 0) {
            // Only process the event if it's a key press (not release or repeat)
            if (ev.evtype == NCTYPE_UNKNOWN || ev.evtype == NCTYPE_PRESS) {
                next_state = handle_input(&ev, player);
            }
        }
    }
    first_function_call = false;

    ncplane_erase(stdplane);
    draw_light_on_player((map_tile_t*) map, (map_tile_t*) revealed_map, HEIGHT, WIDTH, player_pos, LIGHT_RADIUS);
    draw_map_mode((const map_tile_t*) revealed_map, HEIGHT, WIDTH, map_anchor, player_pos);
    notcurses_render(nc);

    return next_state;
}

void init_map_mode(void) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            revealed_map[x][y] = HIDDEN;
        }
    }
}
