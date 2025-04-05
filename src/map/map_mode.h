#ifndef MAP_MODE_H
#define MAP_MODE_H

#define COLOR_FOREGROUND TB_WHITE
#define COLOR_BACKGROUND TB_BLACK

#define CONTINUE 0
#define QUIT 1
#define NEXT_FLOOR 2

#define LIGHT_RADIUS 3

int init_map_mode(void);

void set_start(int newPlayerX, int newPlayerY);

int map_mode_update(void);

#endif //MAP_MODE_H
