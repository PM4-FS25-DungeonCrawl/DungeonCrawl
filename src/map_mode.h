#ifndef MAP_MODE_H
#define MAP_MODE_H

#define COLOR_FOREGROUND TB_WHITE
#define COLOR_BACKGROUND TB_BLACK

#define CONTINUE 0
#define QUIT 1

#define WIDTH 20
#define HEIGHT 10

/**
 * @brief Player position struct
 */
typedef struct {
    int x;
    int y;
} position;

/**
 * Updates the player position based on the player's input and redraws the maze.
 * @return CONTINUE (0) if the game continue, QUIT (1) if the player pressed the exit key.
 */
int map_mode_update();

extern int map[HEIGHT][WIDTH];

#endif //MAP_MODE_H
