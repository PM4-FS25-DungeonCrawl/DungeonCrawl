#ifndef MAP_MODE_H
#define MAP_MODE_H

#include "map_generator_stupid.h"

#define COLOR_FOREGROUND TB_WHITE
#define COLOR_BACKGROUND TB_BLACK

void setNewMap(enum map_tile (*newMap)[WIDTH][HEIGHT], int newPlayerX, int newPlayerY);

int mapModeUpdate();

#endif //MAP_MODE_H
