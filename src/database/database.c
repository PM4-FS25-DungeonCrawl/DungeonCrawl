#include "database.h"

#include "../logging/logger.h"

#include <stdio.h>

int db_open(DBConnection* db_connection, const char* db_name) {
    int rc = sqlite3_open(db_name, &db_connection->db);
    if (rc) {
        log_msg(ERROR, "Database", "Can't open database: %s\n", sqlite3_errmsg(db_connection->db));
        return 1;
    }
    return 0;
}

void db_close(DBConnection* db_connection) {
    if (db_connection->db != NULL) {
        sqlite3_close(db_connection->db);
        db_connection->db = NULL;
    }
}

int db_is_open(const DBConnection* db_connection) {
    if (db_connection->db == NULL) {
        return 0;
    }
    return 1;
}