#ifndef DATABASE_H
#define DATABASE_H

#include "../../include/sqlite3.h"

#define DB_OPEN_STATUS_SUCCESS 0
#define DB_OPEN_STATUS_FAILURE 1

/**
 * This struct is used for the database connection in SQLite
 */
typedef struct {
    sqlite3* db;
    char* err_msg;
} db_connection_t;

/**
 * This function is for the opening of the database.
 *
 * @param db_connection the database connection
 * @param db_name the path name of the database
 * @return 0 for success
 */
int db_open(db_connection_t* db_connection, const char* db_name);

/**
 * This function is for the closing of the database.
 *
 * @param db_connection the database conncetion
 */
void db_close(db_connection_t* db_connection);

/**
 * This function is to check if the database is open.
 *
 * @param db_connection the database connection
 * @return 1 if open, otherwise 0
 */
int db_is_open(const db_connection_t* db_connection);

#endif//DATABASE_H
