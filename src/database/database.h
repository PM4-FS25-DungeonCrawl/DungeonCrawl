#ifndef DATABASE_H
#define DATABASE_H

#include "../../include/sqlite3.h"

typedef struct {
    sqlite3* db;
    char* err_msg;
} db_connection_t;

int db_open(db_connection_t* db_connection, const char* db_name);
void db_close(db_connection_t* db_connection);

#endif//DATABASE_H
