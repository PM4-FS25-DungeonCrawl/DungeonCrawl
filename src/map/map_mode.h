#ifndef MAP_MODE_H
#define MAP_MODE_H

#define COLOR_FOREGROUND TB_WHITE
#define COLOR_BACKGROUND TB_BLACK

#define LIGHT_RADIUS 3

typedef enum {
    CONTINUE,
    QUIT,
    NEXT_FLOOR,
    COMBAT
} map_mode_result_t;


int init_map_mode(void);

void set_player_start_pos(int player_x, int player_y);

map_mode_result_t map_mode_update(void);

#endif//MAP_MODE_H
