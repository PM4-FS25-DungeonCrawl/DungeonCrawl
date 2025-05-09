#include "item_database.h"

#include "../../logging/logger.h"

#include <string.h>

#define SQL_SELECT_COUNT_POTIONS "SELECT COUNT(*) FROM potion"
#define SQL_SELECT_ALL_POTIONS "SELECT PO_TYPE, PO_NAME, PO_VALUE FROM potion"
#define SQL_SELECT_COUNT_GEARS "SELECT COUNT(*) FROM gear"
#define SQL_SELECT_ALL_GEARS "WITH SplitAbilities AS (SELECT GR_ID, "                                                                                   \
                             "AB_NUMBER, "                                                                                                              \
                             "ROW_NUMBER() OVER (PARTITION BY GR_ID ORDER BY AB_ID) AS RowNum, "                                                        \
                             "COUNT(*) OVER (PARTITION BY GR_ID) AS TotalAbilities "                                                                    \
                             "FROM ability, "                                                                                                           \
                             "gear "                                                                                                                    \
                             "JOIN gear_involves_ability ON main.ability.AB_ID = gear_involves_ability.GA_AB_ID AND "                                   \
                             "gear_involves_ability.GA_GR_ID = gear.GR_ID), "                                                                           \
                             "GroupedAbilities AS (SELECT GR_ID, "                                                                                      \
                             "TotalAbilities, "                                                                                                         \
                             "MAX(CASE WHEN RowNum = 1 THEN AB_NUMBER END) AS Ability_1, "                                                              \
                             "MAX(CASE WHEN RowNum = 2 THEN AB_NUMBER END) AS Ability_2, "                                                              \
                             "MAX(CASE WHEN RowNum = 3 THEN AB_NUMBER END) AS Ability_3, "                                                              \
                             "MAX(CASE WHEN RowNum = 4 THEN AB_NUMBER END) AS Ability_4 "                                                               \
                             "FROM SplitAbilities "                                                                                                     \
                             "GROUP BY GR_ID) "                                                                                                         \
                             "SELECT GR_NAME, "                                                                                                         \
                             "GR_IDENT, "                                                                                                               \
                             "SL_NUMBER, "                                                                                                              \
                             "GR_ARMOR, "                                                                                                               \
                             "GR_MAGICRESIST, "                                                                                                         \
                             "ST_STRENGTH, "                                                                                                            \
                             "ST_DEXTERNITY, "                                                                                                          \
                             "ST_INTELLIGENCE, "                                                                                                        \
                             "ST_CONSTITUTION, "                                                                                                        \
                             "coalesce(TotalAbilities, 0) AS TotalAbilities, "                                                                          \
                             "coalesce(Ability_1, 0) AS Ability_1, "                                                                                    \
                             "coalesce(Ability_2, 0) AS Ability_2, "                                                                                    \
                             "coalesce(Ability_3, 0) AS Ability_3, "                                                                                    \
                             "coalesce(Ability_4, 0) AS Ability_4 "                                                                                     \
                             "FROM gear, "                                                                                                              \
                             "slot, "                                                                                                                   \
                             "stats "                                                                                                                   \
                             "JOIN gear_located_slot ON main.slot.SL_ID = gear_located_slot.GL_SL_ID AND main.gear.GR_ID = gear_located_slot.GL_GR_ID " \
                             "JOIN gear_has_stats ON stats.ST_ID = gear_has_stats.GT_ST_ID AND gear.GR_ID = main.gear_has_stats.GT_GR_ID "              \
                             "LEFT JOIN GroupedAbilities ON GroupedAbilities.GR_ID = main.gear.GR_ID"

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

gear_init_t* init_gear_table_from_db(const db_connection_t* db_connection) {
    // Check if the database connection is open
    if (!db_is_open(db_connection)) {
        log_msg(ERROR, "Gear", "Database connection is not open");
        return NULL;
    }
    // Prepare the SQL statement to select all gears
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_connection->db, SQL_SELECT_ALL_GEARS, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Gear", "Failed to prepare statement: %s", sqlite3_errmsg(db_connection->db));
        return NULL;
    }
    // Count the number of gears in the database
    const int gear_counted = count_gear_in_db(db_connection);
    // Allocate memory for the gear table
    if (gear_counted <= 0) {
        log_msg(ERROR, "Gear", "No gears found in the database");
        sqlite3_finalize(stmt);
        return NULL;
    }
    gear_init_t* gear_init_table = malloc(sizeof(gear_init_t) * gear_counted);
    if (gear_init_table == NULL) {
        log_msg(ERROR, "Gear", "Failed to allocate memory for gear table");
        sqlite3_finalize(stmt);
        return NULL;
    }
    // Execute the statement and fetch the results
    int index = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW && index < gear_counted) {
        gear_init_table[index].name = strdup((const char*) sqlite3_column_text(stmt, 0));
        gear_init_table[index].gear_identifier = sqlite3_column_int(stmt, 1);
        gear_init_table[index].slot = (gear_slot_t) sqlite3_column_int(stmt, 2);
        gear_init_table[index].defenses.armor = sqlite3_column_int(stmt, 3);
        gear_init_table[index].defenses.magic_resist = sqlite3_column_int(stmt, 4);
        gear_init_table[index].stats.strength = sqlite3_column_int(stmt, 5);
        gear_init_table[index].stats.dexterity = sqlite3_column_int(stmt, 6);
        gear_init_table[index].stats.intelligence = sqlite3_column_int(stmt, 7);
        gear_init_table[index].stats.constitution = sqlite3_column_int(stmt, 8);
        gear_init_table[index].num_abilities = sqlite3_column_int(stmt, 9);
        gear_init_table[index].ability_names[0] = sqlite3_column_int(stmt, 10);
        gear_init_table[index].ability_names[1] = sqlite3_column_int(stmt, 11);
        gear_init_table[index].ability_names[2] = sqlite3_column_int(stmt, 12);
        gear_init_table[index].ability_names[3] = sqlite3_column_int(stmt, 13);
        index++;
    }
    if (rc != SQLITE_DONE) {
        log_msg(ERROR, "Gear", "Failed to execute statement: %s", sqlite3_errmsg(db_connection->db));
        free(gear_init_table);
        sqlite3_finalize(stmt);
        return NULL;
    }
    // Finalize the statement
    sqlite3_finalize(stmt);
    return gear_init_table;
}

void free_gear_table_from_db(gear_init_t* gear_init_table, const db_connection_t* db_connection) {
    if (gear_init_table == NULL) { return; }

    for (int i = 0; i < count_gear_in_db(db_connection); i++) {
        free(gear_init_table[i].name);
        gear_init_table[i].name = NULL;
    }
    free(gear_init_table);
}

int count_gear_in_db(const db_connection_t* db_connection) {
    // Check if the database connection is open
    if (!db_is_open(db_connection)) {
        log_msg(ERROR, "Gear", "Database connection is not open");
        return 0;
    }
    // Prepare the SQL statement
    sqlite3_stmt* stmt_count;
    int rc = sqlite3_prepare_v2(db_connection->db, SQL_SELECT_COUNT_GEARS, -1, &stmt_count, NULL);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Gear", "Failed to prepare statement: %s", sqlite3_errmsg(db_connection->db));
        return 0;
    }
    // Execute the statement
    rc = sqlite3_step(stmt_count);
    if (rc != SQLITE_ROW) {
        log_msg(ERROR, "Gear", "Failed to execute statement: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_count);
        return 0;
    }
    // Get the count of gears
    const int gear_count = sqlite3_column_int(stmt_count, 0);
    sqlite3_finalize(stmt_count);
    // Check if there are any gears
    if (gear_count == 0) {
        log_msg(ERROR, "Gear", "No gears found in the database");
        return 0;
    }
    return gear_count;
}