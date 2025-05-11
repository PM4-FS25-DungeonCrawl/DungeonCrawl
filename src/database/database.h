#ifndef DATABASE_H
#define DATABASE_H

#include "../../include/sqlite3.h"

#define DB_OPEN_STATUS_SUCCESS 0
#define DB_OPEN_STATUS_FAILURE 1


#define DB_BUILD_DIR_PATH(database) #database
#define DB_RESOURCE_PATH(dir, database) "resources/database/" #dir "/" #database
#define DB_RESOURCE_PATH_UP(dir, database) "../" DB_RESOURCE_PATH(dir, database)


/**
 * This struct is used for the database connection in SQLite
 */
typedef struct {
    sqlite3* db;
    char* err_msg;
} db_connection_t;


typedef enum db_type_t {
    DB_GAME,
    DB_LOCAL
} db_type_t;


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
int db_open_multiple_access(db_connection_t* db_connection, db_type_t type);

#endif//DATABASE_H
