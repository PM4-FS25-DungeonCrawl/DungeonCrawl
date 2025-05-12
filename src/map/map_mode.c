#include "map_mode.h"

#include "../game.h"
#include "../inventory/inventory_mode.h"
#include "../io/input/input_handler.h"
#include "../io/io_handler.h"
#include "../io/output/common/common_output.h"
#include "../io/output/specific/map_output.h"
#include "draw/draw_light.h"
#include "map.h"

#include <notcurses/notcurses.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

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

map_mode_result_t handle_input(const input_event_t* input_event, character_t* player) {
    int new_x = player_pos.dx;
    int new_y = player_pos.dy;

    if (input_event->type == INPUT_QUIT) return QUIT;
    if (input_event->type == INPUT_MENU || input_event->type == INPUT_CANCEL) return SHOW_MENU;

    // For special keys like 'I', we need to check the raw input value
    uint32_t key_id = input_event->raw_input.id;
    if (key_id == 'i' || key_id == 'I') return SHOW_INVENTORY;
    if (input_event->type == INPUT_STATS) return SHOW_STATS;

    // Process movement using logical input types
    switch (input_event->type) {
        case INPUT_UP:
            new_y--;
            break;
        case INPUT_DOWN:
            new_y++;
            break;
        case INPUT_LEFT:
            new_x--;
            break;
        case INPUT_RIGHT:
            new_x++;
            break;
        default:
            break;
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

    if (!first_function_call) {
        input_event_t input_event;

        // Use our input handler to get input
        if (get_input_blocking(&input_event)) {
            next_state = handle_input(&input_event, player);
        }
    }
    first_function_call = false;

    // clear screen using the IO handler
    clear_screen();
    draw_light_on_player((map_tile_t*) map, (map_tile_t*) revealed_map, HEIGHT, WIDTH, player_pos, LIGHT_RADIUS);
    draw_map_mode((const map_tile_t*) revealed_map, HEIGHT, WIDTH, map_anchor, player_pos);
    // Use the centralized render function instead of direct notcurses call
    render_io_frame();

    return next_state;
}

void init_map_mode(void) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            revealed_map[x][y] = HIDDEN;
        }
    }
}

void shutdown_map_mode(void) {
    // Free any resources if needed
    log_msg(INFO, "map_mode", "Map mode shut down");
}
