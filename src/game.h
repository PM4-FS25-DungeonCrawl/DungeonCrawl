#ifndef GAME_H
#define GAME_H

#define COLOR_FOREGROUND TB_WHITE
#define COLOR_BACKGROUND TB_BLACK

typedef enum {
    MAIN_MENU,
    MAP_MODE,
    COMBAT_MODE,
    GENERATE_MAP,
    EXIT,
    EXIT_WITH_ERROR
} game_state_t;

typedef enum {
    SUCCESS = 0,
    FAIL_TB_INIT = 1,
    FAIL_LOCAL_INIT = 2,
    FAIL_GAME_ENTITY_INIT = 3,
} exit_code_t;

int init_game();

void main_menu_state();

void map_mode_state();

#endif// GAME_H
