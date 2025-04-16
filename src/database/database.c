#include "database.h"

#include "../logging/logger.h"

#include <stdio.h>
#include <string.h>

static int check_column_exists(sqlite3* db, const char* table, const char* column) {
    char sql[256];
    sqlite3_stmt* stmt;
    int exists = 0;

    // Query to check if column exists in table
    snprintf(sql, sizeof(sql), "PRAGMA table_info(%s);", table);

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Database", "Failed to prepare statement: %s", sqlite3_errmsg(db));
        return 0;
    }

    // Check each column in the result
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* col_name = (const char*) sqlite3_column_text(stmt, 1);
        if (col_name && strcmp(col_name, column) == 0) {
            exists = 1;
            break;
        }
    }

    sqlite3_finalize(stmt);
    return exists;
}

int db_open(db_connection_t* db_connection, const char* db_name) {
    int rc = sqlite3_open(db_name, &db_connection->db);
    if (rc) {
        log_msg(ERROR, "Database", "Can't open database: %s", sqlite3_errmsg(db_connection->db));
        return 0;
    }
    log_msg(INFO, "Database", "Opened database successfully");

    // Create tables if they don't exist
    char* err_msg = NULL;
    const char* create_tables_sql =
            "CREATE TABLE IF NOT EXISTS game_state ("
            "GS_ID INTEGER PRIMARY KEY AUTOINCREMENT,"
            "GS_SAVEDTIME TEXT NOT NULL"
            ");"

            "CREATE TABLE IF NOT EXISTS map_state ("
            "MS_ID INTEGER PRIMARY KEY AUTOINCREMENT,"
            "MS_MAP TEXT NOT NULL,"
            "MS_REVEALED TEXT NOT NULL,"
            "MS_HEIGHT INTEGER NOT NULL,"
            "MS_WIDTH INTEGER NOT NULL,"
            "MS_GS_ID INTEGER NOT NULL,"
            "FOREIGN KEY (MS_GS_ID) REFERENCES game_state(GS_ID)"
            ");"

            "CREATE TABLE IF NOT EXISTS player_state ("
            "PS_ID INTEGER PRIMARY KEY AUTOINCREMENT,"
            "PS_X INTEGER NOT NULL,"
            "PS_Y INTEGER NOT NULL,"
            "PS_GS_ID INTEGER NOT NULL,"
            "FOREIGN KEY (PS_GS_ID) REFERENCES game_state(GS_ID)"
            ");";

    rc = sqlite3_exec(db_connection->db, create_tables_sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Database", "SQL error: %s", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }

    // Check if GS_NAME column exists in game_state table
    if (!check_column_exists(db_connection->db, "game_state", "GS_NAME")) {
        log_msg(INFO, "Database", "Adding GS_NAME column to game_state table");

        // Add the GS_NAME column if it doesn't exist
        const char* alter_table_sql = "ALTER TABLE game_state ADD COLUMN GS_NAME TEXT;";
        rc = sqlite3_exec(db_connection->db, alter_table_sql, 0, 0, &err_msg);
        if (rc != SQLITE_OK) {
            log_msg(ERROR, "Database", "SQL error when adding GS_NAME column: %s", err_msg);
            sqlite3_free(err_msg);
            return 0;
        }
    }

    return 1;
}


void db_close(db_connection_t* db_connection) {
    sqlite3_close(db_connection->db);
    db_connection->db = NULL;
}

int db_is_open(const db_connection_t* db_connection) {
    if (db_connection->db == NULL) {
        return 0;
    }
    return 1;
}