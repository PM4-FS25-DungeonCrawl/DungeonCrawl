#include "gamestate_database.h"
#include "../database.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "../../logging/logger.h"

#define SQL_INSERT_GAME_STATE "INSERT INTO game_state (GS_SAVEDTIME, GS_NAME) VALUES (?, ?)"
#define SQL_INSERT_MAP_STATE "INSERT INTO map_state (MS_MAP, MS_REVEALED, MS_HEIGHT,MS_WIDTH, MS_GS_ID) VALUES (?, ?, ?, ?, ?)"
#define SQL_INSERT_PLAYER_STATE "INSERT INTO player_state (PS_X, PS_Y, PS_GS_ID) VALUES (?, ?, ?)"
#define SQL_SELECT_LAST_GAME_STATE "SELECT GS_ID FROM game_state ORDER BY GS_SAVEDTIME DESC LIMIT 1"
#define SQL_SELECT_MAP_STATE "SELECT MS_HEIGHT, MS_WIDTH FROM map_state WHERE MS_GS_ID = ?"
#define SQL_SELECT_MAP "SELECT value FROM map_state, json_each(map_state.MS_MAP) WHERE MS_GS_ID = ?"
#define SQL_SELECT_REVEALED_MAP "SELECT value FROM map_state, json_each(map_state.MS_REVEALED) WHERE MS_GS_ID = ?"
#define SQL_SELECT_PLAYER_STATE "SELECT PS_X, PS_Y FROM player_state WHERE PS_GS_ID = ?"
#define SQL_SELECT_ALL_GAME_STATES "SELECT GS_ID, GS_SAVEDTIME, GS_NAME FROM game_state ORDER BY GS_SAVEDTIME DESC"

void save_game_state(DBConnection* db_connection, int width, int height, int map[width][height], int revealed_map[width][height], int player_x, int player_y, const char* save_name) {
    //TODO: Check if the database connection is open (can't do i rigt now beacause branch localisatzion is not merged yet)

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
    
    // Bind the save name to the statement
    rc = sqlite3_bind_text(stmt, 2, save_name, -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to bind save name: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt);
        free(current_time);
        return;
    }
    
    // We can free current_time after binding
    free(current_time);
    
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
    char* map_json = map_to_json_flattend(width, height, map);
    char* revealed_map_json = map_to_json_flattend(width, height, revealed_map);
    
    if (map_json == NULL || revealed_map_json == NULL) {
        log_msg(ERROR, "GameState", "Failed to convert map to JSON");
        free(map_json); // Safe to call on NULL
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
    rc = sqlite3_bind_int(stmt_player, 1, player_x);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to bind player x: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_player);
        return;
    }
    rc = sqlite3_bind_int(stmt_player, 2, player_y);
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
    struct timeval tv;
    gettimeofday(&tv, NULL);

    struct tm *tm_info = localtime(&tv.tv_sec);

    int buffer_size = 32;
    char* result = malloc(buffer_size);
    if (result == NULL) {
        log_msg(ERROR, "GameState", "Failed to allocate memory for timestamp");
        return NULL;
    }

    strftime(result, buffer_size, "%Y-%m-%d %H:%M:%S", tm_info);

    int millis = tv.tv_usec / 1000;
    snprintf(result + strlen(result), buffer_size - strlen(result), ".%03d", millis);

    return result;
}

char* map_to_json_flattend(int width, int height, int map[width][height]) {
    // Calculate buffer size (each int could be multiple digits plus comma and space)
    int buffer_size = width * height * 10 + 10; // Generous estimate
    char* buffer = malloc(buffer_size);
    if (buffer == NULL) {
        log_msg(ERROR, "GameState", "Failed to allocate memory for map JSON");
        return NULL;
    }
    
    int pos = 0;
    int total_elements = width * height;

    // Start the JSON array
    pos += sprintf(buffer + pos, "[");

    // Loop over the 2D map and append each element in a 1D fashion
    for (int i = 0; i < total_elements; i++) {
        // Write the value into the buffer
        pos += sprintf(buffer + pos, "%d", map[i / width][i % width]);

        // If it's not the last element, append a comma
        if (i < total_elements - 1) {
            pos += sprintf(buffer + pos, ", ");
        }
    }

    // Close the JSON array
    pos += sprintf(buffer + pos, "]");

    // Ensure the buffer is null-terminated
    buffer[pos] = '\0';
    return buffer;
}

int get_save_files(DBConnection* dbconnection, SaveFileInfo** save_files, int* count) {
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(dbconnection->db, SQL_SELECT_ALL_GAME_STATES, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to prepare statement: %s", sqlite3_errmsg(dbconnection->db));
        return 0;
    }
    
    // First, count the number of saves
    int save_count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        save_count++;
    }
    
    // Reset the statement to start from the beginning
    sqlite3_reset(stmt);
    
    if (save_count == 0) {
        sqlite3_finalize(stmt);
        *count = 0;
        *save_files = NULL;
        return 1; // Success, but no saves found
    }
    
    // Allocate memory for the save files
    SaveFileInfo* files = malloc(save_count * sizeof(SaveFileInfo));
    if (files == NULL) {
        log_msg(ERROR, "GameState", "Failed to allocate memory for save files");
        sqlite3_finalize(stmt);
        return 0;
    }
    
    // Retrieve save information
    int index = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        files[index].id = sqlite3_column_int(stmt, 0);
        
        const char* timestamp = (const char*)sqlite3_column_text(stmt, 1);
        if (timestamp) {
            size_t len = strlen(timestamp);
            files[index].timestamp = malloc(len + 1);
            if (files[index].timestamp) {
                strcpy(files[index].timestamp, timestamp);
            } else {
                log_msg(ERROR, "GameState", "Failed to allocate memory for timestamp");
                // Clean up allocated memory so far
                for (int i = 0; i < index; i++) {
                    free(files[i].timestamp);
                    free(files[i].name);
                }
                free(files);
                sqlite3_finalize(stmt);
                return 0;
            }
        } else {
            files[index].timestamp = NULL;
        }
        
        const char* name = (const char*)sqlite3_column_text(stmt, 2);
        if (name) {
            size_t len = strlen(name);
            files[index].name = malloc(len + 1);
            if (files[index].name) {
                strcpy(files[index].name, name);
            } else {
                log_msg(ERROR, "GameState", "Failed to allocate memory for save name");
                // Clean up allocated memory so far
                free(files[index].timestamp); // Free the timestamp we just allocated
                for (int i = 0; i < index; i++) {
                    free(files[i].timestamp);
                    free(files[i].name);
                }
                free(files);
                sqlite3_finalize(stmt);
                return 0;
            }
        } else {
            // If no name is specified, use "Unnamed Save"
            const char* unnamed = "Unnamed Save";
            size_t len = strlen(unnamed);
            files[index].name = malloc(len + 1);
            if (files[index].name) {
                strcpy(files[index].name, unnamed);
            } else {
                log_msg(ERROR, "GameState", "Failed to allocate memory for default save name");
                free(files[index].timestamp);
                for (int i = 0; i < index; i++) {
                    free(files[i].timestamp);
                    free(files[i].name);
                }
                free(files);
                sqlite3_finalize(stmt);
                return 0;
            }
        }
        
        index++;
    }
    
    sqlite3_finalize(stmt);
    
    *save_files = files;
    *count = save_count;
    
    return 1;
}

void free_save_files(SaveFileInfo* save_files, int count) {
    if (save_files == NULL) {
        return;
    }
    
    for (int i = 0; i < count; i++) {
        free(save_files[i].timestamp);
        free(save_files[i].name);
    }
    
    free(save_files);
}

int get_game_state(DBConnection* dbconnection, int* width, int* height, int** map, int** revealed_map, int* player_x, int* player_y) {
    // Get the last game state ID
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(dbconnection->db, SQL_SELECT_LAST_GAME_STATE, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to prepare statement: %s", sqlite3_errmsg(dbconnection->db));
        return 0;
    }
    // Execute the statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        log_msg(ERROR, "GameState", "Failed to execute statement: %s", sqlite3_errmsg(dbconnection->db));
        sqlite3_finalize(stmt);
        return 0;
    }
    
    int game_state_id = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    
    // Call get_game_state_by_id to load the game state
    return get_game_state_by_id(dbconnection, game_state_id, width, height, map, revealed_map, player_x, player_y);
}

int get_game_state_by_id(DBConnection* dbconnection, int game_state_id, int* width, int* height, int** map, int** revealed_map, int* player_x, int* player_y) {
    // Get the height and width from the database
    sqlite3_stmt* stmt_map;
    int rc = sqlite3_prepare_v2(dbconnection->db, SQL_SELECT_MAP_STATE, -1, &stmt_map, NULL);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to prepare statement: %s", sqlite3_errmsg(dbconnection->db));
        return 0;
    }
    // Bind the game state ID to the statement
    rc = sqlite3_bind_int64(stmt_map, 1, game_state_id);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to bind game state ID: %s", sqlite3_errmsg(dbconnection->db));
        sqlite3_finalize(stmt_map);
        return 0;
    }
    // Execute the statement
    rc = sqlite3_step(stmt_map);
    if (rc != SQLITE_ROW) {
        log_msg(ERROR, "GameState", "Failed to execute statement: %s", sqlite3_errmsg(dbconnection->db));
        sqlite3_finalize(stmt_map);
        return 0;
    }

    // Get the height and width from the result
    *height = sqlite3_column_int(stmt_map, 0);
    *width = sqlite3_column_int(stmt_map, 1);
    sqlite3_finalize(stmt_map);

    // Allocate memory for the map and revealed map
    int* map_data = malloc((*width) * (*height) * sizeof(int));
    int* revealed_map_data = malloc((*width) * (*height) * sizeof(int));
    
    if (map_data == NULL || revealed_map_data == NULL) {
        log_msg(ERROR, "GameState", "Failed to allocate memory for map data");
        free(map_data); // Safe to call on NULL
        free(revealed_map_data);
        return 0;
    }
    
    // Set the output pointers to the allocated memory
    *map = map_data;
    *revealed_map = revealed_map_data;

    //Get the map from the database
    sqlite3_stmt* stmt_map_data;
    rc = sqlite3_prepare_v2(dbconnection->db, SQL_SELECT_MAP, -1, &stmt_map_data, NULL);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to prepare statement: %s", sqlite3_errmsg(dbconnection->db));
        free(map_data);
        free(revealed_map_data);
        return 0;
    }
    // Bind the game state ID to the statement
    rc = sqlite3_bind_int64(stmt_map_data, 1, game_state_id);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to bind game state ID: %s", sqlite3_errmsg(dbconnection->db));
        sqlite3_finalize(stmt_map_data);
        free(map_data);
        free(revealed_map_data);
        return 0;
    }
    // Execute the statement
    rc = sqlite3_step(stmt_map_data);
    if (rc != SQLITE_ROW) {
        log_msg(ERROR, "GameState", "Failed to execute statement: %s", sqlite3_errmsg(dbconnection->db));
        sqlite3_finalize(stmt_map_data);
        free(map_data);
        free(revealed_map_data);
        return 0;
    }
    
    // Build the map from the result - using a 1D array indexed as y*width + x
    int index = 0;
    int total_cells = (*width) * (*height);
    
    while (index < total_cells && rc == SQLITE_ROW) {
        map_data[index] = sqlite3_column_int(stmt_map_data, 0);
        index++;
        rc = sqlite3_step(stmt_map_data);
    }
    
    if (index != total_cells) {
        log_msg(ERROR, "GameState", "Didn't get all map data. Expected %d, got %d", total_cells, index);
        sqlite3_finalize(stmt_map_data);
        free(map_data);
        free(revealed_map_data);
        return 0;
    }
    
    sqlite3_finalize(stmt_map_data);

    //Get the revealed map from the database
    sqlite3_stmt* stmt_revealed_map_data;
    rc = sqlite3_prepare_v2(dbconnection->db, SQL_SELECT_REVEALED_MAP, -1, &stmt_revealed_map_data, NULL);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to prepare statement: %s", sqlite3_errmsg(dbconnection->db));
        free(map_data);
        free(revealed_map_data);
        return 0;
    }
    // Bind the game state ID to the statement
    rc = sqlite3_bind_int64(stmt_revealed_map_data, 1, game_state_id);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to bind game state ID: %s", sqlite3_errmsg(dbconnection->db));
        sqlite3_finalize(stmt_revealed_map_data);
        free(map_data);
        free(revealed_map_data);
        return 0;
    }
    // Execute the statement
    rc = sqlite3_step(stmt_revealed_map_data);
    if (rc != SQLITE_ROW) {
        log_msg(ERROR, "GameState", "Failed to execute statement: %s", sqlite3_errmsg(dbconnection->db));
        sqlite3_finalize(stmt_revealed_map_data);
        free(map_data);
        free(revealed_map_data);
        return 0;
    }
    
    // Build the revealed map from the result
    index = 0;
    while (index < total_cells && rc == SQLITE_ROW) {
        revealed_map_data[index] = sqlite3_column_int(stmt_revealed_map_data, 0);
        index++;
        rc = sqlite3_step(stmt_revealed_map_data);
    }
    
    if (index != total_cells) {
        log_msg(ERROR, "GameState", "Didn't get all revealed map data. Expected %d, got %d", total_cells, index);
        sqlite3_finalize(stmt_revealed_map_data);
        free(map_data);
        free(revealed_map_data);
        return 0;
    }
    
    sqlite3_finalize(stmt_revealed_map_data);

    //Get the player position from the database
    sqlite3_stmt* stmt_player_data;
    rc = sqlite3_prepare_v2(dbconnection->db, SQL_SELECT_PLAYER_STATE, -1, &stmt_player_data, NULL);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to prepare statement: %s", sqlite3_errmsg(dbconnection->db));
        free(map_data);
        free(revealed_map_data);
        return 0;
    }
    // Bind the game state ID to the statement
    rc = sqlite3_bind_int64(stmt_player_data, 1, game_state_id);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "GameState", "Failed to bind game state ID: %s", sqlite3_errmsg(dbconnection->db));
        sqlite3_finalize(stmt_player_data);
        free(map_data);
        free(revealed_map_data);
        return 0;
    }
    // Execute the statement
    rc = sqlite3_step(stmt_player_data);
    if (rc != SQLITE_ROW) {
        log_msg(ERROR, "GameState", "Failed to execute statement: %s", sqlite3_errmsg(dbconnection->db));
        sqlite3_finalize(stmt_player_data);
        free(map_data);
        free(revealed_map_data);
        return 0;
    }
    // Get the player position from the result
    *player_x = sqlite3_column_int(stmt_player_data, 0);
    *player_y = sqlite3_column_int(stmt_player_data, 1);
    sqlite3_finalize(stmt_player_data);
    return 1;
}
