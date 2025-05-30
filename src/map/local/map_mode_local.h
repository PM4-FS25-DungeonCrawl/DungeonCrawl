#ifndef MAP_MODE_LOCAL_H
#define MAP_MODE_LOCAL_H

enum map_mode_index {
    PRESS_KEY_MENU,
    PRESS_KEY_STATS,
    PRESS_KEY_INVENTORY,
    PLAYER_POSITION_STR,
    MAX_MAP_MODE_STRINGS
};

extern char** map_mode_strings;

int init_map_mode_local(void);

void update_map_mode_local(void);

void shutdown_map_mode_local(void);

#endif //MAP_MODE_LOCAL_H
