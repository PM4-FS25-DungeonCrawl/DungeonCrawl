#include "database.h"

#include "../logging/logger.h"

#include <stdio.h>
#include <string.h>

int db_open(db_connection_t* db_connection, const char* db_name) {
    int rc = sqlite3_open(db_name, &db_connection->db);
    if (rc) {
        log_msg(ERROR, "Database", "Can't open database: %s", sqlite3_errmsg(db_connection->db));
        return 0;
    }
    log_msg(INFO, "Database", "Opened database successfully");
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