#ifndef GAMESTATE_DATABASE_H
#define GAMESTATE_DATABASE_H
#include "../database.h"

void save_game_state(DBConnection* dbconnection,int width,int heigth, int map[width][heigth], int revealed_map[width][heigth], int player_x, int player_y);
int get_game_state(DBConnection* dbconnection, int* width, int* height, int** map, int** revealed_map, int* player_x, int* player_y);
char* get_iso8601_time();
char* map_to_json_flattend(int width, int height, int map[width][height]);
#endif //GAMESTATE_DATABASE_H
