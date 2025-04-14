#include "gamestate_database.h"
#include "../database.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define SQL_INSERT_GAME_STATE "INSERT INTO game_state (GS_SAVEDTIME) VALUES (?)"
#define SQL_INSERT_MAP_STATE "INSERT INTO map_state (MS_MAP, MS_REVEALED, MS_HEIGHT,MS_WIDTH, MS_GS_ID) VALUES (?, ?, ?, ?, ?)"
#define SQL_INSERT_PLAYER_STATE "INSERT INTO player_state (PS_X, PS_Y, PS_GS_ID) VALUES (?, ?, ?)"

void save_game_state(DBConnection* db_connection, int width, int height, int map[width][height], int revealed_map[width][height], int player_x, int player_y) {
    //TODO: Check if the database connection is open (can't do i rigt now beacause branch localisatzion is not merged yet)

   // Save the game state to the database into table game_state
    // Get the current time
    char* current_time = get_iso8601_time();
    if (current_time == NULL) {
        //TODO: Logging with logger.c
        fprintf(stderr, "Failed to get current time\n");
        return;
    }

    // Prepare the SQL statement
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_connection->db, SQL_INSERT_GAME_STATE, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db_connection->db));
        return;
    }
    // Bind the current time to the statement
    rc = sqlite3_bind_text(stmt, 1, current_time, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to bind time: %s\n", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt);
        return;
    }
    // Execute the statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db_connection->db));
    }
    // Get the last inserted row ID
    sqlite3_int64 game_state_id = sqlite3_last_insert_rowid(db_connection->db);

    // Finalize the statement
    sqlite3_finalize(stmt);


    // Save the map and revealed map to the database
	char* map_json = map_to_json_flattend(width, height, map);
    char* revealed_map_json = map_to_json_flattend(width, height, revealed_map);

    // Prepare the SQL statement
    sqlite3_stmt* stmt_map;
    rc = sqlite3_prepare_v2(db_connection->db, SQL_INSERT_MAP_STATE, -1, &stmt_map, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db_connection->db));
//        free(map_json);
//        free(revealed_map_json);
        return;
    }

    // Bind the map and revealed map to the statement
    rc = sqlite3_bind_text(stmt_map, 1, map_json, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to bind map: %s\n", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_map);
    }

    rc = sqlite3_bind_text(stmt_map, 2, revealed_map_json, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to bind revealed map: %s\n", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_map);
    }
    rc = sqlite3_bind_int(stmt_map, 3, height);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to bind height: %s\n", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_map);
    }
    rc = sqlite3_bind_int(stmt_map, 4, width);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to bind width: %s\n", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_map);
    }
    rc = sqlite3_bind_int64(stmt_map, 5, game_state_id);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to bind game state ID: %s\n", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_map);
    }
    // Execute the statement
    rc = sqlite3_step(stmt_map);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db_connection->db));
    }
    // Finalize the statement
    sqlite3_finalize(stmt_map);

    // Save the player position to the database
    sqlite3_stmt* stmt_player;
    rc = sqlite3_prepare_v2(db_connection->db, SQL_INSERT_PLAYER_STATE, -1, &stmt_player, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db_connection->db));
        return;
    }
    // Bind the player position to the statement
    rc = sqlite3_bind_int(stmt_player, 1, player_x);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to bind player x: %s\n", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_player);
        return;
    }
    rc = sqlite3_bind_int(stmt_player, 2, player_y);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to bind player y: %s\n", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_player);
        return;
    }
    rc = sqlite3_bind_int64(stmt_player, 3, game_state_id);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to bind game state ID: %s\n", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_player);
        return;
    }
    // Execute the statement
    rc = sqlite3_step(stmt_player);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db_connection->db));
    }
    // Finalize the statement
    sqlite3_finalize(stmt_player);
}

char* get_iso8601_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    struct tm *tm_info = localtime(&tv.tv_sec);

    int buffer_size = 32;
    char buffer[buffer_size];


    strftime(buffer, buffer_size, "%Y-%m-%d %H:%M:%S", tm_info);

    int millis = tv.tv_usec / 1000;
    snprintf(buffer + strlen(buffer), buffer_size - strlen(buffer), ".%03d", millis);

    return buffer;
}

char* map_to_json_flattend(int width, int height, int map[width][height]) {
    char buffer[width * height * 4]; // Rough estimate for the size of the JSON string
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
