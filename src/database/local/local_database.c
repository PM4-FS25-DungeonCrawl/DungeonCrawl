#include "local_database.h"

#include "../../logging/logger.h"
#include "../database.h"
#include "attribute_database.h"

#include <stdio.h>
#include <string.h>

#define ATTRIBUTE_KEY_STATEMENT "SELECT AT_ID FROM attribute WHERE AT_NAME = ?"
#define LOCAL_STRING_STATEMENT " FROM localization WHERE LC_AT_1 || '.' || LC_AT_2 || '.' || LC_AT_3 || '.' || LC_AT_4 = (SELECT a.AT_ID || '.' || b.AT_ID || '.' || c.AT_ID || '.' || d.AT_ID FROM attribute a, attribute b, attribute c, attribute d WHERE a.AT_NAME || '.' || b.AT_NAME || '.' || c.AT_NAME || '.' || d.AT_NAME = ?)"

/**
 * Get the attribute key (AT_ID) from the table attribute
 * @param db_connection (DBConnection)
 * @param attribute_name (AT_NAME)
 */
int attribute_key(const DBConnection* db_connection, const char* attribute_name) {
    //Check if the database is open
    if (!db_is_open(db_connection)) {
        log_msg(ERROR, "Localization Database", "Database is not open");
        return -1;
    }

    // Prepare the SQL statement
    sqlite3_stmt* statement;
    int rc = sqlite3_prepare_v2(db_connection->db, ATTRIBUTE_KEY_STATEMENT, -1, &statement, 0);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Localization Database", "Failed to prepare statement: %s", sqlite3_errmsg(db_connection->db));
        return -1;
    }
    // Bind the attribute name to the statement
    sqlite3_bind_text(statement, 1, attribute_name, -1, SQLITE_STATIC);
    // Execute the statement
    rc = sqlite3_step(statement);
    if (rc != SQLITE_ROW) {
        log_msg(ERROR, "Localization Database", "Failed to execute statement: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(statement);
        return -1;
    }
    // Get the attribute key
    const int attribute_key = sqlite3_column_int(statement, 0);

    return attribute_key;
}

/**
 * Get the localization string from the table localization
 * @param db_connection (DBConnection)
 * @param attribute_path (the chain of LC_AT_X where X is the attribute key (AT_ID), separated by dots, e.g. LC_AT_1.LC_AT_2.LC_AT_3.LC_AT_4 (4 is max))
 * @param language enum language_t where the value define the database column to use
 */
char* get_localization_string(const DBConnection* db_connection, const char* attribute_path,
                              const local_language_t* language) {
    //Check if the database is open
    if (!db_is_open(db_connection)) {
        log_msg(ERROR, "Localization Database", "Database is not open");
        return "NULL";
    }

    // a buffer for the full SQL statement
    const size_t buffer_size = strlen("SELECT ") + strlen(language->column_name) + strlen(LOCAL_STRING_STATEMENT) + 1;
    char local_string_stmt[buffer_size];

    // Construct the SQL statement
    snprintf(local_string_stmt, buffer_size, "SELECT %s%s", language->column_name, LOCAL_STRING_STATEMENT);

    // Prepare the SQL statement
    sqlite3_stmt* statement;
    int rc = sqlite3_prepare_v2(db_connection->db, local_string_stmt, -1, &statement, 0);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Localization Database", "Failed to prepare statement: %s", sqlite3_errmsg(db_connection->db));
        return "NULL";
    }
    // Bind the attribute path to the statement
    sqlite3_bind_text(statement, 1, attribute_path, -1, SQLITE_STATIC);
    // Execute the statement
    rc = sqlite3_step(statement);
    if (rc != SQLITE_ROW) {
        log_msg(ERROR, "Localization Database", "Failed to execute statement: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(statement);
        return "NULL";
    }
    // Get the localization string
    const char* localization_string = (const char*) sqlite3_column_text(statement, 0);

    return localization_string;
}
