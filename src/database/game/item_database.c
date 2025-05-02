#include "item_database.h"
#include "../../logging/logger.h"

#include <string.h>

#define SQL_SELECT_COUNT_POTIONS "SELECT COUNT(*) FROM potion"
#define SQL_SELECT_ALL_POTIONS "SELECT PO_TYPE, PO_NAME, PO_VALUE FROM potion"

potion_init_t* init_potion_table_from_db(const db_connection_t* db_connection) {
    // Check if the database connection is open
    if (!db_is_open(db_connection)) {
        log_msg(ERROR, "Potion", "Database connection is not open");
        return NULL;
    }

    // Prepare the SQL statement to select all potions
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_connection->db, SQL_SELECT_ALL_POTIONS, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Potion", "Failed to prepare statement: %s", sqlite3_errmsg(db_connection->db));
        return NULL;
    }
    // Count the number of potions in the database
    const int potion_counted = count_potions_in_db(db_connection);
    // Allocate memory for the potion table
    if (potion_counted <= 0) {
        log_msg(ERROR, "Potion", "No potions found in the database");
        sqlite3_finalize(stmt);
        return NULL;
    }
    potion_init_t* potion_init_table = malloc(sizeof(potion_init_t) * potion_counted);
    if (potion_init_table == NULL) {
        log_msg(ERROR, "Potion", "Failed to allocate memory for potion table");
        sqlite3_finalize(stmt);
        return NULL;
    }
    // Execute the statement and fetch the results
    int index = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW && index < potion_counted) {
        potion_init_table[index].potion_type = sqlite3_column_int(stmt, 0);
        potion_init_table[index].name = strdup((const char*) sqlite3_column_text(stmt, 1));
        potion_init_table[index].value = sqlite3_column_int(stmt, 2);
        index++;
    }
    if (rc != SQLITE_DONE) {
        log_msg(ERROR, "Potion", "Failed to execute statement: %s", sqlite3_errmsg(db_connection->db));
        free(potion_init_table);
        sqlite3_finalize(stmt);
        return NULL;
    }
    // Finalize the statement
    sqlite3_finalize(stmt);
    return potion_init_table;
}

void free_potion_table_from_db(potion_init_t* potion_init_table, const db_connection_t* db_connection) {
    if (potion_init_table == NULL) { return; }

    for (int i = 0; i < count_potions_in_db(db_connection); i++) {
        free(potion_init_table[i].name);
        potion_init_table[i].name = NULL;
    }
    free(potion_init_table);
}

int count_potions_in_db(const db_connection_t* db_connection) {
    // Check if the database connection is open
    if (!db_is_open(db_connection)) {
        log_msg(ERROR, "Potion", "Database connection is not open");
        return 0;
    }
    // Prepare the SQL statement
    sqlite3_stmt* stmt_count;
    int rc = sqlite3_prepare_v2(db_connection->db, SQL_SELECT_COUNT_POTIONS, -1, &stmt_count, NULL);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Potion", "Failed to prepare statement: %s", sqlite3_errmsg(db_connection->db));
        return 0;
    }
    // Execute the statement
    rc = sqlite3_step(stmt_count);
    if (rc != SQLITE_ROW) {
        log_msg(ERROR, "Potion", "Failed to execute statement: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_count);
        return 0;
    }
    // Get the count of potions
    const int potion_count = sqlite3_column_int(stmt_count, 0);
    sqlite3_finalize(stmt_count);
    // Check if there are any potions
    if (potion_count == 0) {
        log_msg(ERROR, "Potion", "No potions found in the database");
        return 0;
    }
    return potion_count;
}