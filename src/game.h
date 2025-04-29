#ifndef GAME_H
#define GAME_H

#define COLOR_FOREGROUND TB_WHITE
#define COLOR_BACKGROUND TB_BLACK
#include "database/database.h"

typedef enum {
    MAIN_MENU,
    MAP_MODE,
    COMBAT_MODE,
    INVENTORY_MODE,
    GENERATE_MAP,
    EXIT
} game_state_t;

extern db_connection_t db_connection;

void run_game();

void main_menu_state();

void map_mode_state();

void inventory_mode_state();

#endif// GAME_H
