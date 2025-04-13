#ifndef GAME_H
#define GAME_H

#define COLOR_FOREGROUND TB_WHITE
#define COLOR_BACKGROUND TB_BLACK

int init_game();

enum game_state { MAIN_MENU, MAP_MODE, COMBAT_MODE, GENERATE_MAP, EXIT };

#endif // GAME_H
