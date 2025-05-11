#ifndef DATABASE_H
#define DATABASE_H

#include "../../include/sqlite3.h"

#define DB_OPEN_STATUS_SUCCESS 0
#define DB_OPEN_STATUS_FAILURE 1

#define DB_BUILD_DIR_PATH(database) #database
#define DB_RESOURCE_PATH(dir, database) "resources/database/" #dir "/" #database
#define DB_RESOURCE_PATH_UP(dir, database) "../" DB_RESOURCE_PATH(dir, database)

typedef struct {
    sqlite3* db;
    char* err_msg;
} db_connection_t;

typedef enum db_type_t {
    DB_GAME,
    DB_LOCAL
} db_type_t;


int db_open(db_connection_t* db_connection, const char* db_name);
void db_close(db_connection_t* db_connection);
int db_is_open(const db_connection_t* db_connection);
int db_open_multiple_access(db_connection_t* db_connection, db_type_t type);

#endif//DATABASE_H
