#ifndef GAME_H
#define GAME_H

#include "database/database.h"

#include <notcurses/notcurses.h>

// Define colors with notcurses
#define COLOR_FG_RGB 255, 255, 255// White
#define COLOR_BG_RGB 0, 0, 0      // Black

// Global notcurses instance and plane
extern struct notcurses* nc;
extern struct ncplane* stdplane;

typedef enum {
    MAIN_MENU,
    MAP_MODE,
    COMBAT_MODE,
    LOOT_MODE,
    INVENTORY_MODE,
    GENERATE_MAP,
    STATS_MODE,
    EXIT
} game_state_t;

extern db_connection_t db_connection;

void run_game();

void main_menu_state();

void map_mode_state();

void inventory_mode_state();

void loot_mode_state();

#endif// GAME_H
