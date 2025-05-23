#ifndef MAP_MODE_H
#define MAP_MODE_H

#include "../character/character.h"
#include "../common.h"
#include "../io/input/input_handler.h"

// Notcurses uses RGB channels directly, but we can define our colors for consistency
#define COLOR_FOREGROUND 0xffffff// White
#define COLOR_BACKGROUND 0x000000// Black

#define LIGHT_RADIUS 3

typedef enum {
    CONTINUE,
    QUIT,
    NEXT_FLOOR,
    COMBAT,
    SHOW_MENU,
    SHOW_STATS,
    SHOW_INVENTORY
} map_mode_result_t;

void set_player_start_pos(int player_x, int player_y);

/**
 * @brief Get the current player position
 * @return the player position as a vector2d_t structure
 */
vector2d_t get_player_pos();

map_mode_result_t map_mode_update(character_t* player);

void init_map_mode(void);
void shutdown_map_mode(void);

// has to be exposed for testing
map_mode_result_t handle_input(const input_event_t* input_event, character_t* player);
#endif//MAP_MODE_H
