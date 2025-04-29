#ifndef MAP_MODE_H
#define MAP_MODE_H

#include "../common.h"

#define COLOR_FOREGROUND TB_WHITE
#define COLOR_BACKGROUND TB_BLACK

#define LIGHT_RADIUS 3

typedef enum {
    CONTINUE,
    QUIT,
    NEXT_FLOOR,
    COMBAT,
    SHOW_INVENTORY,
    SHOW_MENU
} map_mode_result_t;

void set_player_start_pos(int player_x, int player_y);

/**
 * @brief Get the current player position
 * @return the player position as a vector2d_t structure
 */
vector2d_t get_player_pos();

map_mode_result_t map_mode_update(void);
void init_map_mode(void);

#endif//MAP_MODE_H
