#ifndef GAME_H
#define GAME_H

#include <notcurses/notcurses.h>
#include "database/database.h"

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
    GENERATE_MAP,
    EXIT
} game_state_t;

extern db_connection_t db_connection;

void run_game();

void main_menu_state();

void map_mode_state();

#endif// GAME_H
