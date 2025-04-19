#ifndef MAP_MODE_H
#define MAP_MODE_H

#include <notcurses/notcurses.h>
#include "../common.h"

// These are now defined in game.h
// #define COLOR_FG_RGB 255, 255, 255  // White
// #define COLOR_BG_RGB 0, 0, 0        // Black

#define LIGHT_RADIUS 3

typedef enum {
    CONTINUE,
    QUIT,
    NEXT_FLOOR,
    COMBAT,
    SHOW_MENU
} map_mode_result_t;

void set_player_start_pos(int player_x, int player_y);

/**
 * @brief Get the current player position
 * @return the player position as a vector2d_t structure
 */
vector2d_t get_player_pos();

map_mode_result_t map_mode_update(void);
int init_map_mode(void);

#endif//MAP_MODE_H
