#include "../database.h"
#include "local_database.h"
#include "attribute_database.h"
#include "../../logging/logger.h"
#include <stdio.h>

/**
 * Get the attribute key (AT_ID) from the table attribute
 * @param attribute_name (AT_NAME)
 */
int attribute_key(const char *attribute_name) {
    // Get the database connection
  const DBConnection db_connection = *db_get_connection();
    //Check if the database is open
	if (!db_is_open()) {
		log_msg(ERROR, "Local Database", "Database could not be open, error: %s", sqlite3_errmsg(db_connection.db));
//		fprintf(stderr, "Database is not open\n");
	    return -1;
	}

	// Prepare the SQL statement
	char *stmt = "SELECT AT_ID FROM attribute WHERE AT_NAME = ?";
    sqlite3_stmt *statement;
	int rc = sqlite3_prepare_v2(db_connection.db, stmt, -1, &statement, 0);
	if (rc != SQLITE_OK) {
	    log_msg(ERROR, "Localization Database", "Failed to prepare statement: %s", sqlite3_errmsg(db_connection.db));
	    //fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db_connection.db));
	    return -1;
	}
	// Bind the attribute name to the statement
	sqlite3_bind_text(statement, 1, attribute_name, -1, SQLITE_STATIC);
	// Execute the statement
	rc = sqlite3_step(statement);
	if (rc != SQLITE_ROW) {
        log_msg(ERROR, "Localization Database", "Failed to execute statement: %s", sqlite3_errmsg(db_connection.db));
        //fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db_connection.db));
        sqlite3_finalize(statement);
        return -1;
	}
	// Get the attribute key
    const int attribute_key = sqlite3_column_int(statement, 0);

    return attribute_key;
}
