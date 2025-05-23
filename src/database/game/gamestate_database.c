#include "gamestate_database.h"

#include "../../logging/logger.h"
#include "../database.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#define TIMESTAMP_FORMAT "%Y-%m-%d %H:%M:%S"

#define SQL_INSERT_GAME_STATE "INSERT INTO game_state (GS_SAVEDTIME, GS_NAME) VALUES (?, ?)"
#define SQL_INSERT_MAP_STATE "INSERT INTO map_state (MS_MAP, MS_REVEALED, MS_HEIGHT,MS_WIDTH, MS_GS_ID) VALUES (?, ?, ?, ?, ?)"
#define SQL_INSERT_PLAYER_STATE "INSERT INTO player_state (PS_X, PS_Y, PS_GS_ID) VALUES (?, ?, ?)"
#define SQL_SELECT_LAST_GAME_STATE "SELECT GS_ID FROM game_state ORDER BY GS_SAVEDTIME DESC LIMIT 1"
#define SQL_SELECT_MAP_STATE "SELECT MS_HEIGHT, MS_WIDTH FROM map_state WHERE MS_GS_ID = ?"
#define SQL_SELECT_MAP "SELECT value FROM map_state, json_each(map_state.MS_MAP) WHERE MS_GS_ID = ?"
#define SQL_SELECT_REVEALED_MAP "SELECT value FROM map_state, json_each(map_state.MS_REVEALED) WHERE MS_GS_ID = ?"
#define SQL_SELECT_PLAYER_STATE "SELECT PS_X, PS_Y FROM player_state WHERE PS_GS_ID = ?"
#define SQL_SELECT_ALL_GAME_STATES "SELECT GS_ID, GS_SAVEDTIME, GS_NAME FROM game_state ORDER BY GS_SAVEDTIME DESC"

// === Internal Functions ===
char* get_iso8601_time();

void save_game_state(const db_connection_t* db_connection, const int* map, const int* revealed_map, const int width, const int height, const vector2d_t player, const char* save_name) {
    // Check if the database connection is open
    if (!db_is_open(db_connection)) {
        log_msg(ERROR, "GameState", "Database connection is not open");
        return;
    }

    // Save the game state to the database into table game_state
    // Get the current time
    char* current_time = get_iso8601_time();
    if (current_time == NULL) {
        log_msg(ERROR, "GameState", "Failed to get current time");
        return;
    }

    // Prepare the SQL statement
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_connection->db, SQL_INSERT_GAME_STATE, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to prepare statement: %s", sqlite3_errmsg(db_connection->db));
        free(current_time);
        return;
    }

    // Bind the current time to the statement
    rc = sqlite3_bind_text(stmt, 1, current_time, -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to bind time: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt);
        free(current_time);
        return;
    }

    // We can free current_time after binding
    free(current_time);

    // Bind the save name to the statement
    rc = sqlite3_bind_text(stmt, 2, save_name, -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to bind save name: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt);
        return;
    }

    // Execute the statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        log_msg(ERROR, "GameState", "Failed to execute statement: %s", sqlite3_errmsg(db_connection->db));
    }
    // Get the last inserted row ID
    sqlite3_int64 game_state_id = sqlite3_last_insert_rowid(db_connection->db);

    // Finalize the statement
    sqlite3_finalize(stmt);

    // Save the map and revealed map to the database
    char* map_json = arr2D_to_flat_json(map, width, height);
    char* revealed_map_json = arr2D_to_flat_json(revealed_map, width, height);
    if (map_json == NULL || revealed_map_json == NULL) {
        free(map_json);// Safe to call on NULL
        free(revealed_map_json);
        return;
    }

    // Prepare the SQL statement
    sqlite3_stmt* stmt_map;
    rc = sqlite3_prepare_v2(db_connection->db, SQL_INSERT_MAP_STATE, -1, &stmt_map, NULL);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to prepare statement: %s", sqlite3_errmsg(db_connection->db));
        free(map_json);
        free(revealed_map_json);
        return;
    }

    // Bind the map and revealed map to the statement
    rc = sqlite3_bind_text(stmt_map, 1, map_json, -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to bind map: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_map);
        free(map_json);
        free(revealed_map_json);
        return;
    }

    rc = sqlite3_bind_text(stmt_map, 2, revealed_map_json, -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to bind revealed map: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_map);
        free(map_json);
        free(revealed_map_json);
        return;
    }

    // We can free JSON strings after binding
    free(map_json);
    free(revealed_map_json);

    rc = sqlite3_bind_int(stmt_map, 3, height);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to bind height: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_map);
        return;
    }
    rc = sqlite3_bind_int(stmt_map, 4, width);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to bind width: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_map);
        return;
    }
    rc = sqlite3_bind_int64(stmt_map, 5, game_state_id);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to bind game state ID: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_map);
        return;
    }
    // Execute the statement
    rc = sqlite3_step(stmt_map);
    if (rc != SQLITE_DONE) {
        log_msg(ERROR, "GameState", "Failed to execute statement: %s", sqlite3_errmsg(db_connection->db));
    }
    // Finalize the statement
    sqlite3_finalize(stmt_map);

    // Save the player position to the database
    sqlite3_stmt* stmt_player;
    rc = sqlite3_prepare_v2(db_connection->db, SQL_INSERT_PLAYER_STATE, -1, &stmt_player, NULL);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to prepare statement: %s", sqlite3_errmsg(db_connection->db));
        return;
    }
    // Bind the player position to the statement
    rc = sqlite3_bind_int(stmt_player, 1, player.dx);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to bind player x: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_player);
        return;
    }
    rc = sqlite3_bind_int(stmt_player, 2, player.dy);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to bind player y: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_player);
        return;
    }
    rc = sqlite3_bind_int64(stmt_player, 3, game_state_id);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to bind game state ID: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_player);
        return;
    }
    // Execute the statement
    rc = sqlite3_step(stmt_player);
    if (rc != SQLITE_DONE) {
        log_msg(ERROR, "GameState", "Failed to execute statement: %s", sqlite3_errmsg(db_connection->db));
    }
    // Finalize the statement
    sqlite3_finalize(stmt_player);
}

char* get_iso8601_time() {
    const time_t now = time(NULL);
    const struct tm* tm = localtime(&now);
    char timestamp[TIMESTAMP_LENGTH];
    strftime(timestamp, sizeof(timestamp), TIMESTAMP_FORMAT, tm);

    return strdup(timestamp);
}

char* arr2D_to_flat_json(const int* arr, const int width, const int height) {
    const size_t buffer_size = width * height * 12 + 2;// 16 is a safe estimate for int size + comma + space
    char* json = malloc(buffer_size);
    if (json == NULL) {
        log_msg(ERROR, "GameState", "Failed to allocate memory for JSON string");
        return NULL;
    }

    strcpy(json, "[");

    const int total_elements = width * height;
    // Loop over the 2D map and append each element in a 1D fashion
    for (int i = 0; i < total_elements; i++) {
        char number[10];// Buffer to hold the number as a string
        // Write the value into the buffer
        snprintf(number, sizeof(number), "%d", arr[i]);
        strcat(json, number);// Append the number to the JSON string

        // If it's not the last element, append a comma
        if (i < total_elements - 1) {
            strcat(json, ",");
        }
    }

    strcat(json, "]");//strcat always ensures that the string is null-terminated
    return json;
}

int get_game_state(const db_connection_t* db_connection, int* map, int* revealed_map, const int width, const int height, const player_pos_setter_t setter) {
    // Get the last game state ID
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_connection->db, SQL_SELECT_LAST_GAME_STATE, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to prepare statement: %s", sqlite3_errmsg(db_connection->db));
        return 0;
    }
    // Execute the statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        log_msg(ERROR, "GameState", "Failed to execute statement: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt);
        return 0;
    }

    const int game_state_id = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    // Call get_game_state_by_id to load the game state
    return get_game_state_by_id(db_connection, game_state_id, map, revealed_map, width, height, setter);
}

int get_game_state_by_id(const db_connection_t* db_connection, const int game_state_id, int* map, int* revealed_map, const int width, const int height, const player_pos_setter_t setter) {
    // Get the height and width from the database
    sqlite3_stmt* stmt_map;
    int rc = sqlite3_prepare_v2(db_connection->db, SQL_SELECT_MAP_STATE, -1, &stmt_map, NULL);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to prepare statement: %s", sqlite3_errmsg(db_connection->db));
        return 0;
    }
    // Bind the game state ID to the statement
    rc = sqlite3_bind_int64(stmt_map, 1, game_state_id);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to bind game state ID: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_map);
        return 0;
    }
    // Execute the statement
    rc = sqlite3_step(stmt_map);
    if (rc != SQLITE_ROW) {
        log_msg(ERROR, "GameState", "Failed to execute statement: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_map);
        return 0;
    }

    sqlite3_finalize(stmt_map);

    //Get the map from the database
    sqlite3_stmt* stmt_map_data;
    rc = sqlite3_prepare_v2(db_connection->db, SQL_SELECT_MAP, -1, &stmt_map_data, NULL);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to prepare statement: %s", sqlite3_errmsg(db_connection->db));
        return 0;
    }
    // Bind the game state ID to the statement
    rc = sqlite3_bind_int64(stmt_map_data, 1, game_state_id);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to bind game state ID: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_map_data);
        return 0;
    }
    // Execute the statement
    rc = sqlite3_step(stmt_map_data);
    if (rc != SQLITE_ROW) {
        log_msg(ERROR, "GameState", "Failed to execute statement: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_map_data);
        return 0;
    }

    int index = 0;
    const int total_cells = width * height;
    while (index < total_cells && rc == SQLITE_ROW) {
        map[index] = sqlite3_column_int(stmt_map_data, 0);
        index++;
        rc = sqlite3_step(stmt_map_data);
    }

    if (index != total_cells) {
        log_msg(ERROR, "GameState", "Didn't get all map data. Expected %d, got %d", total_cells, index);
        sqlite3_finalize(stmt_map_data);
        return 0;
    }

    sqlite3_finalize(stmt_map_data);

    //Get the revealed map from the database
    sqlite3_stmt* stmt_revealed_map_data;
    rc = sqlite3_prepare_v2(db_connection->db, SQL_SELECT_REVEALED_MAP, -1, &stmt_revealed_map_data, NULL);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to prepare statement: %s", sqlite3_errmsg(db_connection->db));
        return 0;
    }
    // Bind the game state ID to the statement
    rc = sqlite3_bind_int64(stmt_revealed_map_data, 1, game_state_id);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to bind game state ID: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_revealed_map_data);
        return 0;
    }
    // Execute the statement
    rc = sqlite3_step(stmt_revealed_map_data);
    if (rc != SQLITE_ROW) {
        log_msg(ERROR, "GameState", "Failed to execute statement: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_revealed_map_data);
        return 0;
    }

    // Build the revealed map from the result
    index = 0;
    while (index < total_cells && rc == SQLITE_ROW) {
        revealed_map[index] = sqlite3_column_int(stmt_revealed_map_data, 0);
        index++;
        rc = sqlite3_step(stmt_revealed_map_data);
    }

    if (index != total_cells) {
        log_msg(ERROR, "GameState", "Didn't get all revealed map data. Expected %d, got %d", total_cells, index);
        sqlite3_finalize(stmt_revealed_map_data);
        return 0;
    }

    sqlite3_finalize(stmt_revealed_map_data);

    //Get the player position from the database
    sqlite3_stmt* stmt_player_data;
    rc = sqlite3_prepare_v2(db_connection->db, SQL_SELECT_PLAYER_STATE, -1, &stmt_player_data, NULL);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to prepare statement: %s", sqlite3_errmsg(db_connection->db));
        return 0;
    }
    // Bind the game state ID to the statement
    rc = sqlite3_bind_int64(stmt_player_data, 1, game_state_id);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to bind game state ID: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_player_data);
        return 0;
    }
    // Execute the statement
    rc = sqlite3_step(stmt_player_data);
    if (rc != SQLITE_ROW) {
        log_msg(ERROR, "GameState", "Failed to execute statement: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_player_data);
        return 0;
    }
    // Get the player position from the result
    const int player_x = sqlite3_column_int(stmt_player_data, 0);
    const int player_y = sqlite3_column_int(stmt_player_data, 1);
    setter(player_x, player_y);
    sqlite3_finalize(stmt_player_data);
    return 1;
}

save_info_container_t* get_save_infos(const db_connection_t* db_connection) {
    sqlite3_stmt* stmt;
    const int rc = sqlite3_prepare_v2(db_connection->db, SQL_SELECT_ALL_GAME_STATES, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to prepare statement: %s", sqlite3_errmsg(db_connection->db));
        return NULL;
    }

    // First, count the number of saves
    int save_count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        save_count++;
    }

    // Reset the statement to start from the beginning
    sqlite3_reset(stmt);

    save_info_container_t* save_infos = malloc(sizeof(save_info_container_t));
    if (save_infos == NULL) {
        log_msg(ERROR, "GameState", "Failed to allocate memory for save info container");
        sqlite3_finalize(stmt);
        return NULL;
    }

    if (save_count == 0) {
        sqlite3_finalize(stmt);
        save_infos->count = 0;
        save_infos->infos = NULL;
        return save_infos;
    }

    save_infos->count = save_count;
    save_infos->infos = malloc(sizeof(save_info_t) * save_count);
    if (save_infos->infos == NULL) {
        log_msg(ERROR, "GameState", "Failed to allocate memory for save infos");
        free(save_infos);
        sqlite3_finalize(stmt);
        return NULL;
    }


    // Retrieve save information
    int index = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        save_infos->infos[index].id = sqlite3_column_int(stmt, 0);

        snprintf(save_infos->infos[index].timestamp, TIMESTAMP_LENGTH, "%s", (const char*) sqlite3_column_text(stmt, 1));
        snprintf(save_infos->infos[index].name, MAX_STRING_LENGTH, "%s", (const char*) sqlite3_column_text(stmt, 2));
        index++;
    }

    sqlite3_finalize(stmt);

    return save_infos;
}

void free_save_infos(save_info_container_t* save_infos) {
    if (save_infos == NULL) {
        return;
    }

    free(save_infos->infos);
    free(save_infos);
}

void create_tables_game_state(const db_connection_t* db_connection) {
    if (!db_is_open(db_connection)) {
        log_msg(ERROR, "GameState", "Database is not open");
        return;
    }

    //Create GS table
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_connection->db, SQL_CREATE_TABLES_GAMESTATE_GS, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to prepare statement: %s", sqlite3_errmsg(db_connection->db));
        return;
    }
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        log_msg(ERROR, "GameState", "Failed to execute statement: %s", sqlite3_errmsg(db_connection->db));
    }
    sqlite3_finalize(stmt);

    // Create MS table
    rc = sqlite3_prepare_v2(db_connection->db, SQL_CREATE_TABLES_GAMESTATE_MS, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to prepare statement: %s", sqlite3_errmsg(db_connection->db));
        return;
    }
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        log_msg(ERROR, "GameState", "Failed to execute statement: %s", sqlite3_errmsg(db_connection->db));
    }
    sqlite3_finalize(stmt);

    // Create PS table
    rc = sqlite3_prepare_v2(db_connection->db, SQL_CREATE_TABLES_GAMESTATE_PS, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to prepare statement: %s", sqlite3_errmsg(db_connection->db));
        return;
    }
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        log_msg(ERROR, "GameState", "Failed to execute statement: %s", sqlite3_errmsg(db_connection->db));
    }
    sqlite3_finalize(stmt);
}
