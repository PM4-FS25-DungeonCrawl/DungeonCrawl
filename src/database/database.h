#ifndef DATABASE_H
#define DATABASE_H

#include "../../include/sqlite3.h"

typedef struct {
    sqlite3* db;
    char* err_msg;
} DBConnection;

int db_open(DBConnection* db_connection, const char* db_name);
void db_close(DBConnection* db_connection);
int db_is_open(const DBConnection* db_connection);

#endif//DATABASE_H
