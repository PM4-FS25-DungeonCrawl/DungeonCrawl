#ifndef MAP_MODE_H
#define MAP_MODE_H

#include "notcurses/notcurses.h"
#define COLOR_FOREGROUND TB_WHITE
#define COLOR_BACKGROUND TB_BLACK

#define CONTINUE 0
#define QUIT 1

#define LIGHT_RADIUS 3

enum directions {DIR_UP,DIR_DOWN,DIR_LEFT,DIR_RIGHT};

int init_map_mode(struct notcurses* nc,struct ncplane* ncplane);

void set_start(int newPlayerX, int newPlayerY);

int map_mode_update(void);

void move_player(enum directions direction);

#endif //MAP_MODE_H
