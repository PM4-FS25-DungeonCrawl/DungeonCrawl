/**
 * @file gamestate_database.h
 * @brief Declares functions to create, save, load, and manage game state data in the SQLite database.
 */
#ifndef GAMESTATE_DATABASE_H
#define GAMESTATE_DATABASE_H

#include "../../common.h"
#include "../database.h"

#define MAX_NUMBER_SAVES 20
#define TIMESTAMP_LENGTH 20

#define SQL_CREATE_TABLES_GAMESTATE_GS            \
    "CREATE TABLE IF NOT EXISTS \"game_state\" (" \
    "\"GS_ID\"	INTEGER NOT NULL UNIQUE,"          \
    "\"GS_SAVEDTIME\"	TEXT,"                      \
    "\"GS_NAME\"	TEXT,"                           \
    "PRIMARY KEY(\"GS_ID\" AUTOINCREMENT)"        \
    ");"
#define SQL_CREATE_TABLES_GAMESTATE_MS                               \
    "CREATE TABLE IF NOT EXISTS \"map_state\" ("                     \
    "\"MS_ID\"	INTEGER NOT NULL UNIQUE,"                             \
    "\"MS_MAP\"	TEXT,"                                               \
    "\"MS_REVEALED\"	TEXT,"                                          \
    "\"MS_HEIGHT\"	INTEGER,"                                         \
    "\"MS_WIDTH\"	INTEGER,"                                          \
    "\"MS_GS_ID\"	INTEGER NOT NULL UNIQUE,"                          \
    "PRIMARY KEY(\"MS_ID\" AUTOINCREMENT),"                          \
    "FOREIGN KEY(\"MS_GS_ID\") REFERENCES \"game_state\"(\"GS_ID\")" \
    ");"
#define SQL_CREATE_TABLES_GAMESTATE_PS                               \
    "CREATE TABLE IF NOT EXISTS \"player_state\" ("                  \
    "\"PS_ID\"	INTEGER NOT NULL UNIQUE,"                             \
    "\"PS_X\"	INTEGER,"                                              \
    "\"PS_Y\"	INTEGER,"                                              \
    "\"PS_GS_ID\"	INTEGER NOT NULL UNIQUE,"                          \
    "PRIMARY KEY(\"PS_ID\" AUTOINCREMENT),"                          \
    "FOREIGN KEY(\"PS_GS_ID\") REFERENCES \"game_state\"(\"GS_ID\")" \
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
/**
 * @brief  Save the game state.
 *
 * @param db_connection A database connection.
 * @param map The current map where nothing is hidden.
 * @param revealed_map The current state of the revealed map.
 * @param width The width of the map.
 * @param height The hight of the map.
 * @param player The player position.
 * @param save_name A name for the save file.
 * @return 0 on success none 0 on failure.
 */
sqlite_int64 save_game_state(const db_connection_t* db_connection, const int* map, const int* revealed_map, int width, int height, vector2d_t player, const char* save_name);
/**
 * @brief  Load the game state from the database. 
 *
 * @param db_connection A database connection.
 * @param map Pointer to the map to load into.
 * @param revealed_map Pointer to the revealed map to load into.
 * @param width Width of the map.
 * @param height Hieght of the map.
 * @param setter A setter for the player position.
 * @return 0 on success none 0 on failure.
 */
int get_game_state(const db_connection_t* db_connection, int* map, int* revealed_map, int width, int height, player_pos_setter_t setter);
/**
 * @brief  Load the game state for a specific id from the database. 
 *
 * @param db_connection A database connection.
 * @param game_state_id The id of the game state to be loaded.
 * @param map Pointer to the map to load into.
 * @param revealed_map Pointer to the revealed map to load into.
 * @param width Width of the map.
 * @param height Hieght of the map.
 * @param setter A setter for the player position.
 * @return 0 on success none 0 on failure.
 */
int get_game_state_by_id(const db_connection_t* db_connection, int game_state_id, int* map, int* revealed_map, int width, int height, player_pos_setter_t setter);
/**
 * @brief Get the info of the saves.
 *
 * @param db_connection A database connedtion.
 * @return A save_info_container containing the save info.
 */
save_info_container_t* get_save_infos(const db_connection_t* db_connection);
/**
 * @brief Free the resources associated with a save_info_container.
 *
 * @param save_infos Save info to free.
 */
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

/**
 *
 * @param db_connection Connection to the database
 * @return The ID of the latest save
 */
int get_latest_save_id(const db_connection_t* db_connection);
#endif//GAMESTATE_DATABASE_H
