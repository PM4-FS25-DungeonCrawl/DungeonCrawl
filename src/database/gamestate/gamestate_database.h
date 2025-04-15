#ifndef GAMESTATE_DATABASE_H
#define GAMESTATE_DATABASE_H

#include "../../common.h"
#include "../database.h"

typedef void (* player_pos_setter_t)(int player_x, int player_y);

typedef struct {
    int id;
    char* timestamp;
    char* name;
} save_file_info_t;

void save_game_state(const DBConnection* dbconnection, const int* map, const int* revealed_map, int width, int height, vector2d_t player, const char* save_name);
int get_game_state(const DBConnection* dbconnection, int* map, int* revealed_map, int width, int height, player_pos_setter_t setter);
int get_game_state_by_id(const DBConnection* dbconnection, int game_state_id, int* map, int* revealed_map, int width, int height, player_pos_setter_t setter);
int get_save_files(const DBConnection* dbconnection, save_file_info_t** save_files, int* count);
void free_save_files(save_file_info_t* save_files, int count);


/**
 * @brief Convert a 2D array to a Json-Array
 * @param arr The given array to convert to a Json-Array
 * @param width The width of the array
 * @param height The height of the array
 * @return The Json-Array as a string (must be freed by the caller), NULL if a malloc error occurred
 * @note The returned string is a flattened JSON Array (e.g. [[1,2],[3,4]] -> [1,2,3,4])
 */
char* arr2D_to_flat_json(const int* arr, int width, int height);
#endif//GAMESTATE_DATABASE_H
