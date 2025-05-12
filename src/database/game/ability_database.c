#include "ability_database.h"

#include "../../logging/logger.h"

#include <string.h>

#define SQL_SELECT_ALL_ABILITIES "SELECT AB_NUMBER, AB_NAME, AB_ROLLAMOUNT, AB_ACCURACY, AB_RESSOURCECOST, AB_DICESIZE, DA_NUMBER FROM ability, damage JOIN ability_has_damage ON AD_AB_ID = AB_ID AND AD_DA_ID = DA_ID"

ability_init_t* get_ability_table_from_db(const db_connection_t* db_connection) {
    // Check if the database connection is open
    if (!db_is_open(db_connection)) {
        log_msg(ERROR, "Ability", "Database connection is not open");
        return NULL;
    }

    // Prepare the SQL statement
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_connection->db, SQL_SELECT_ALL_ABILITIES, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Ability", "Failed to prepare statement: %s", sqlite3_errmsg(db_connection->db));
        return NULL;
    }

    // Allocate memory for the ability table
    ability_init_t* ability_init_table = malloc(sizeof(ability_init_t) * MAX_ABILITIES);
    if (ability_init_table == NULL) {
        log_msg(ERROR, "Ability", "Failed to allocate memory for ability table");
        sqlite3_finalize(stmt);
        return NULL;
    }

    // Execute the statement and fetch the results
    int index = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW && index < MAX_ABILITIES) {
        ability_init_table[index].ability_number = sqlite3_column_int(stmt, 0);
        ability_init_table[index].name = strdup((const char*) sqlite3_column_text(stmt, 1));
        ability_init_table[index].roll_amount = sqlite3_column_int(stmt, 2);
        ability_init_table[index].accuracy = sqlite3_column_int(stmt, 3);
        ability_init_table[index].resource_cost = sqlite3_column_int(stmt, 4);
        ability_init_table[index].dice_size = (dice_size_t) sqlite3_column_int(stmt, 5);
        ability_init_table[index].damage_type = (damage_type_t) sqlite3_column_int(stmt, 6);
        index++;
    }

    if (rc != SQLITE_DONE) {
        log_msg(ERROR, "Ability", "Failed to execute statement: %s", sqlite3_errmsg(db_connection->db));
        free(ability_init_table);
        sqlite3_finalize(stmt);
        return NULL;
    }

    // Finalize the statement
    sqlite3_finalize(stmt);

    return ability_init_table;
}

void free_ability_table_from_db(ability_init_t* ability_init_table) {
    if (ability_init_table == NULL) {
        return;
    }

    for (int i = 0; i < MAX_ABILITIES; i++) {
        free(ability_init_table[i].name);
        ability_init_table[i].name = NULL;
    }
    free(ability_init_table);
}
