#ifndef GAME_H
#define GAME_H

#define COLOR_FOREGROUND TB_WHITE
#define COLOR_BACKGROUND TB_BLACK

#define WIDTH 20
#define HEIGHT 10

int init_game();

int add(int a, int b);

extern int map[HEIGHT][WIDTH];

#endif // GAME_H
