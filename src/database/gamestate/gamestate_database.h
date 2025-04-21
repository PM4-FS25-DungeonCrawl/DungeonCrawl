#ifndef GAMESTATE_DATABASE_H
#define GAMESTATE_DATABASE_H

#include "../../common.h"
#include "../database.h"

#define MAX_NUMBER_SAVES 20
#define TIMESTAMP_LENGTH 20

#define SQL_CREATE_TABLES_GAMESTATE \
            "CREATE TABLE IF NOT EXISTS 'game_state' ("\
            "'GS_ID'	INTEGER NOT NULL UNIQUE,"\
            "'GS_SAVEDTIME'	TEXT,"\
            "'GS_NAME'	TEXT,"\
            "PRIMARY KEY('GS_ID' AUTOINCREMENT)"\
            ");"\
            "CREATE TABLE IF NOT EXISTS 'map_state' ("\
            "'MS_ID'	INTEGER NOT NULL UNIQUE,"\
            "'MS_MAP'	TEXT,"\
            "'MS_REVEALED'	TEXT,"\
            "'MS_HEIGHT'	INTEGER,"\
            "'MS_WIDTH'	INTEGER,"\
            "'MS_GS_ID'	INTEGER NOT NULL UNIQUE,"\
            "PRIMARY KEY('MS_ID' AUTOINCREMENT),"\
            "FOREIGN KEY('MS_GS_ID') REFERENCES 'game_state'('GS_ID')"\
            ");"\
            "CREATE TABLE IF NOT EXISTS 'player_state' ("\
            "'PS_ID'	INTEGER NOT NULL UNIQUE,"\
            "'PS_X'	INTEGER,"\
            "'PS_Y'	INTEGER,"\
            "'PS_GS_ID'	INTEGER NOT NULL UNIQUE,"\
            "PRIMARY KEY('PS_ID' AUTOINCREMENT),"\
            "FOREIGN KEY('PS_GS_ID') REFERENCES 'game_state'('GS_ID')"\
            ");"

typedef void (*player_pos_setter_t)(int player_x, int player_y);

typedef struct {
    int id;
    char timestamp[TIMESTAMP_LENGTH];
    char name[MAX_STRING_LENGTH];
} save_info_t;

typedef struct {
    int count;
    save_info_t* infos;
} save_info_container_t;

/**
 * @brief Create the tables for the game state
 * @param db_connection The database connection
 * @note The tables will be created if they do not exist
 */
void create_tables_game_state(const db_connection_t* db_connection);
void save_game_state(const db_connection_t* db_connection, const int* map, const int* revealed_map, int width, int height, vector2d_t player, const char* save_name);
int get_game_state(const db_connection_t* db_connection, int* map, int* revealed_map, int width, int height, player_pos_setter_t setter);
int get_game_state_by_id(const db_connection_t* db_connection, int game_state_id, int* map, int* revealed_map, int width, int height, player_pos_setter_t setter);
save_info_container_t* get_save_infos(const db_connection_t* db_connection);
void free_save_infos(save_info_container_t* save_infos);


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
