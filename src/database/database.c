//
// Created by jabar on 27.03.2025.
//
#include "database.h"
#include <stdio.h>

int db_open(DBConnection* db_connection, const char* db_name) {
    int rc = sqlite3_open(db_name, &db_connection->db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db_connection->db));
        return 0;
    }
    return 1;
}

void db_close(DBConnection* db_connection) {
    sqlite3_close(db_connection->db);
}
