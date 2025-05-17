#include "character_database.h"

#include "../../logging/logger.h"

#define SQL_INSERT_CHARACTER "INSERT INTO character (CH_MAXHEALTH, " \
                             "CH_MAXMANA, "                          \
                             "CH_MAXSTAMINA, "                       \
                             "CH_CURRENTHEALTH, "                    \
                             "CH_CURRENTMANA, "                      \
                             "CH_CURRENTSTAMINA, "                   \
                             "CH_ARMOR, "                            \
                             "CH_MAGICRESIST, "                      \
                             "CH_LEVEL, "                            \
                             "CH_XP, "                               \
                             "CH_XPREWARD, "                         \
                             "CH_SKILLPOINTS, "                      \
                             "CH_BASESTRENGTH, " \
                             "CH_BASEINTELLIGENCE, " \
                             "CH_BASEDEXTERITY, " \
                             "CH_BASECONSTITUTION, " \
                             "CH_CURRENTSTRENGTH, " \
                             "CH_CURRENTINTELLIGENCE, " \
                             "CH_CURRENTDEXTERITY, " \
                             "CH_CURRENTCONSTITUTION) " \
                             "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"
#define SQL_INSERT_PLAYER "INSERT INTO player (PY_CH_ID, " \
                          "PY_PS_ID, "                     \
                          "PY_NAME) "                      \
                          "VALUES (?, ?, ?);"

void save_character(db_connection_t* db_connection, const character_t character, const sqlite3_int64 game_state_id) {
    // Check if the database connection is open
    if (!db_is_open(db_connection)) {
        log_msg(ERROR, "Character", "Database connection is not open");
        return;
    }
    // Prepare the SQL statement
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_connection->db, SQL_INSERT_CHARACTER, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to prepare statement: %s", sqlite3_errmsg(db_connection->db));
        return;
    }
    // Bind the character data to the statement
    rc = sqlite3_bind_int(stmt, 1, character.max_resources.health);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to bind max health: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt);
        return;
    }
    rc = sqlite3_bind_int(stmt, 2, character.max_resources.mana);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to bind max mana: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt);
        return;
    }
    rc = sqlite3_bind_int(stmt, 3, character.max_resources.stamina);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to bind max stamina: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt);
        return;
    }
    rc = sqlite3_bind_int(stmt, 4, character.current_resources.health);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to bind current health: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt);
        return;
    }
    rc = sqlite3_bind_int(stmt, 5, character.current_resources.mana);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to bind current mana: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt);
        return;
    }
    rc = sqlite3_bind_int(stmt, 6, character.current_resources.stamina);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to bind current stamina: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt);
        return;
    }
    rc = sqlite3_bind_int(stmt, 7, character.defenses.armor);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to bind armor: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt);
        return;
    }
    rc = sqlite3_bind_int(stmt, 8, character.defenses.magic_resist);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to bind magic resist: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt);
        return;
    }
    rc = sqlite3_bind_int(stmt, 9, character.level);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to bind level: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt);
        return;
    }
    rc = sqlite3_bind_int(stmt, 10, character.xp);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to bind xp: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt);
        return;
    }
    rc = sqlite3_bind_int(stmt, 11, character.xp_reward);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to bind xp reward: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt);
        return;
    }
    rc = sqlite3_bind_int(stmt, 12, character.skill_points);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to bind skill points: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt);
        return;
    }
    rc = sqlite3_bind_int(stmt, 13, character.base_stats.strength);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to bind base strength: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt);
        return;
    }
    rc = sqlite3_bind_int(stmt, 14, character.base_stats.intelligence);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to bind base intelligence: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt);
        return;
    }
    rc = sqlite3_bind_int(stmt, 15, character.base_stats.dexterity);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to bind base dexterity: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt);
        return;
    }
    rc = sqlite3_bind_int(stmt, 16, character.base_stats.constitution);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to bind base constitution: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt);
        return;
    }
    rc = sqlite3_bind_int(stmt, 17, character.current_stats.strength);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to bind current strength: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt);
        return;
    }
    rc = sqlite3_bind_int(stmt, 18, character.current_stats.intelligence);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to bind current intelligence: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt);
        return;
    }
    rc = sqlite3_bind_int(stmt, 19, character.current_stats.dexterity);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to bind current dexterity: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt);
        return;
    }
    rc = sqlite3_bind_int(stmt, 20, character.current_stats.constitution);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to bind current constitution: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt);
        return;
    }
    // Execute the statement
    rc = sqlite3_step(stmt);
    // Get the character ID
    const sqlite3_int64 character_id = sqlite3_last_insert_rowid(db_connection->db);
    if (rc != SQLITE_DONE) {
        log_msg(ERROR, "Character", "Failed to execute statement: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt);
        return;
    }
    // Finalize the statement
    sqlite3_finalize(stmt);

    // Prepare the SQL statement for player
    sqlite3_stmt* stmt_player;
    rc = sqlite3_prepare_v2(db_connection->db, SQL_INSERT_PLAYER, -1, &stmt_player, NULL);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to prepare statement: %s", sqlite3_errmsg(db_connection->db));
        return;
    }
    // Bind the player data to the statement
    rc = sqlite3_bind_int64(stmt_player, 1, character_id);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to bind character ID: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_player);
        return;
    }
    rc = sqlite3_bind_int64(stmt_player, 2, game_state_id);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to bind game state ID: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_player);
        return;
    }
    rc = sqlite3_bind_text(stmt_player, 3, character.name, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to bind character name: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_player);
        return;
    }
    // Execute the statement
    rc = sqlite3_step(stmt_player);
    if (rc != SQLITE_DONE) {
        log_msg(ERROR, "Character", "Failed to execute statement: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_player);
        return;
    }
    // Finalize the statement
    sqlite3_finalize(stmt_player);
}

character_t get_character_from_db(db_connection_t* db_connection, int game_state_id) {

}
