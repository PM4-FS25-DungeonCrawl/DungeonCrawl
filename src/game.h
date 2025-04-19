#ifndef GAME_H
#define GAME_H

#include <notcurses/notcurses.h>

// Define colors with notcurses
#define COLOR_FG_RGB 255, 255, 255  // White
#define COLOR_BG_RGB 0, 0, 0        // Black

// Global notcurses instance and plane
extern struct notcurses* nc;
extern struct ncplane* stdplane;

int init_game();

#endif// GAME_H
