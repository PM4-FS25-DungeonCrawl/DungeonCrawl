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

int init_game();

void main_menu_state();

void map_mode_state();

#endif// GAME_H
