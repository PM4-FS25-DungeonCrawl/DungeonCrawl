#include "database.h"

#include "../logging/logger.h"

#include <stdio.h>
#include <string.h>

int db_open(db_connection_t* db_connection, const char* db_name) {
    int rc = sqlite3_open(db_name, &db_connection->db);
    if (rc) {
        log_msg(ERROR, "Database", "Can't open database: %s", sqlite3_errmsg(db_connection->db));
        return DB_OPEN_STATUS_FAILURE;
    }
    return DB_OPEN_STATUS_SUCCESS;
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

int db_open_multiple_access(db_connection_t* db_connection, db_type_t type) {
    const char* potential_paths[3];
    switch (type) {
        case DB_GAME:
            potential_paths[0] = DB_BUILD_DIR_PATH(dungeoncrawl_game.db);
            potential_paths[1] = DB_RESOURCE_PATH(game, dungeoncrawl_game.db);
            potential_paths[2] = DB_RESOURCE_PATH_UP(game, dungeoncrawl_game.db);
            break;
        case DB_LOCAL:
            potential_paths[0] = DB_BUILD_DIR_PATH(dungeoncrawl_local.db);
            potential_paths[1] = DB_RESOURCE_PATH(local, dungeoncrawl_local.db);
            potential_paths[2] = DB_RESOURCE_PATH_UP(local, dungeoncrawl_local.db);
            break;
        default:
            log_msg(ERROR, "Database", "Invalid database type");
            return DB_OPEN_STATUS_FAILURE;
    }

    for (int i = 0; i < sizeof(potential_paths) / sizeof(char*); i++) {
        const int rc = sqlite3_open_v2(potential_paths[i], &db_connection->db, SQLITE_OPEN_READWRITE, NULL);
        if (rc == SQLITE_OK) {
            return DB_OPEN_STATUS_SUCCESS;
        }
        log_msg(WARNING, "Database", "Can't open database: %s", sqlite3_errmsg(db_connection->db));
    }
    return DB_OPEN_STATUS_FAILURE;
}