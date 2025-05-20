#include "character_database.h"

#include "../../logging/logger.h"
#include "src/game_data.h"

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
#define SQL_INSERT_INVENTORY "INSERT INTO inventory (IV_TYPE) VALUES (?);"
#define SQL_INSERT_INVENTORY_GEAR "INSERT INTO inventory_stores_gear (IG_IV_ID, IG_GR_ID, IG_EQUIPPED) VALUES (?, (SELECT GR_ID from gear WHERE GR_IDENT = ? LIMIT 1), ?);"
#define SQL_INSERT_INVENTORY_POTION "INSERT INTO inventory_stores_potion (IP_IV_ID, IP_PO_ID) VALUES (?, (SELECT PO_ID from potion WHERE PO_TYPE = ? LIMIT 1));"
#define SQL_INSERT_CHARACTER_INVENTORY "INSERT INTO character_has_inventory (CI_CH_ID, CI_IV_ID) VALUES (?, ?);"

void save_character(const db_connection_t* db_connection, const character_t character, const sqlite3_int64 game_state_id) {
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

    save_character_inventory(db_connection, character, character_id);

    // Finalize the statement
    sqlite3_finalize(stmt_player);
}

void save_character_inventory(const db_connection_t* db_connection, const character_t character, const sqlite3_int64 character_id) {
    // Prepare the SQL statement gear
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_connection->db, SQL_INSERT_INVENTORY, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to prepare statement: %s", sqlite3_errmsg(db_connection->db));
        return;
    }
    // Bind the inventory type to the statement
    rc = sqlite3_bind_int(stmt, 1, 0);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to bind inventory type: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt);
        return;
    }
    // Execute the statement
    rc = sqlite3_step(stmt);

    // Get the inventory for gear ID
    const sqlite3_int64 inventory_gear_id = sqlite3_last_insert_rowid(db_connection->db);
    if (rc != SQLITE_DONE) {
        log_msg(ERROR, "Character", "Failed to execute statement: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt);
        return;
    }
    // Finalize the statement
    sqlite3_finalize(stmt);

    // Prepare the SQL statement for potions
    sqlite3_stmt* stmt_potion;
    rc = sqlite3_prepare_v2(db_connection->db, SQL_INSERT_INVENTORY, -1, &stmt_potion, NULL);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to prepare statement: %s", sqlite3_errmsg(db_connection->db));
        return;
    }
    // Bind the inventory type to the statement
    rc = sqlite3_bind_int(stmt_potion, 1, 1);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to bind inventory type: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_potion);
        return;
    }
    // Execute the statement
    rc = sqlite3_step(stmt_potion);
    // Get the inventory for potions ID
    const sqlite3_int64 inventory_potion_id = sqlite3_last_insert_rowid(db_connection->db);
    if (rc != SQLITE_DONE) {
        log_msg(ERROR, "Character", "Failed to execute statement: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_potion);
        return;
    }
    // Finalize the statement
    sqlite3_finalize(stmt_potion);

    // Loop through gears in character
    for (int i = 0; i < character.gear_count; i++) {
        // Prepare the SQL statement for gear
        sqlite3_stmt* stmt_gear_save;
        rc = sqlite3_prepare_v2(db_connection->db, SQL_INSERT_INVENTORY_GEAR, -1, &stmt_gear_save, NULL);
        if (rc != SQLITE_OK) {
            log_msg(ERROR, "Character", "Failed to prepare statement: %s", sqlite3_errmsg(db_connection->db));
            return;
        }
        // Bind the inventory ID to the statement
        rc = sqlite3_bind_int64(stmt_gear_save, 1, inventory_gear_id);
        if (rc != SQLITE_OK) {
            log_msg(ERROR, "Character", "Failed to bind inventory ID: %s", sqlite3_errmsg(db_connection->db));
            sqlite3_finalize(stmt_gear_save);
            return;
        }
        // Bind the gear data to the statement
        rc = sqlite3_bind_int(stmt_gear_save, 2, character.gear_inventory[i]->gear_identifier);
        if (rc != SQLITE_OK) {
            log_msg(ERROR, "Character", "Failed to bind gear type: %s", sqlite3_errmsg(db_connection->db));
            sqlite3_finalize(stmt_gear_save);
            return;
        }
        // Bind the equipped status to the statement
        rc = sqlite3_bind_int(stmt_gear_save, 3, 0);

        if (rc != SQLITE_OK) {
            log_msg(ERROR, "Character", "Failed to bind equipped status: %s", sqlite3_errmsg(db_connection->db));
            sqlite3_finalize(stmt_gear_save);
            return;
        }
        // Execute the statement
        rc = sqlite3_step(stmt_gear_save);
        if (rc != SQLITE_DONE) {
            log_msg(ERROR, "Character", "Failed to execute statement: %s", sqlite3_errmsg(db_connection->db));
            sqlite3_finalize(stmt_gear_save);
            return;
        }
        // Finalize the statement
        sqlite3_finalize(stmt_gear_save);
    }

    // Loop through all slots in character and check if in character.equipment has gear
    for (int i = 0; i < MAX_SLOT; i++) {
        if (character.equipment[i] != NULL) {
            // Prepare the SQL statement for gear
            sqlite3_stmt* stmt_gear_save;
            rc = sqlite3_prepare_v2(db_connection->db, SQL_INSERT_INVENTORY_GEAR, -1, &stmt_gear_save, NULL);
            if (rc != SQLITE_OK) {
                log_msg(ERROR, "Character", "Failed to prepare statement: %s", sqlite3_errmsg(db_connection->db));
                return;
            }
            // Bind the inventory ID to the statement
            rc = sqlite3_bind_int64(stmt_gear_save, 1, inventory_gear_id);
            if (rc != SQLITE_OK) {
                log_msg(ERROR, "Character", "Failed to bind inventory ID: %s", sqlite3_errmsg(db_connection->db));
                sqlite3_finalize(stmt_gear_save);
                return;
            }
            // Bind the gear data to the statement
            rc = sqlite3_bind_int(stmt_gear_save, 2, character.equipment[i]->gear_identifier);
            if (rc != SQLITE_OK) {
                log_msg(ERROR, "Character", "Failed to bind gear type: %s", sqlite3_errmsg(db_connection->db));
                sqlite3_finalize(stmt_gear_save);
                return;
            }
            // Bind the equipped status to the statement
            rc = sqlite3_bind_int(stmt_gear_save, 3, 1);

            if (rc != SQLITE_OK) {
                log_msg(ERROR, "Character", "Failed to bind equipped status: %s", sqlite3_errmsg(db_connection->db));
                sqlite3_finalize(stmt_gear_save);
                return;
            }
            // Execute the statement
            rc = sqlite3_step(stmt_gear_save);
            if (rc != SQLITE_DONE) {
                log_msg(ERROR, "Character", "Failed to execute statement: %s", sqlite3_errmsg(db_connection->db));
                sqlite3_finalize(stmt_gear_save);
                return;
            }
            // Finalize the statement
            sqlite3_finalize(stmt_gear_save);
        }
    }

    // Loop through potions in character
    for (int i = 0; i < character.potion_count; i++) {
        // Prepare the SQL statement for potions
        sqlite3_stmt* stmt_potion_save;
        rc = sqlite3_prepare_v2(db_connection->db, SQL_INSERT_INVENTORY_POTION, -1, &stmt_potion_save, NULL);
        if (rc != SQLITE_OK) {
            log_msg(ERROR, "Character", "Failed to prepare statement: %s", sqlite3_errmsg(db_connection->db));
            return;
        }
        // Bind the inventory ID to the statement
        rc = sqlite3_bind_int64(stmt_potion_save, 1, inventory_potion_id);
        if (rc != SQLITE_OK) {
            log_msg(ERROR, "Character", "Failed to bind inventory ID: %s", sqlite3_errmsg(db_connection->db));
            sqlite3_finalize(stmt_potion_save);
            return;
        }
        // Bind the potion data to the statement
        rc = sqlite3_bind_int(stmt_potion_save, 2, character.potion_inventory[i]->effectType);
        if (rc != SQLITE_OK) {
            log_msg(ERROR, "Character", "Failed to bind potion type: %s", sqlite3_errmsg(db_connection->db));
            sqlite3_finalize(stmt_potion_save);
            return;
        }
        // Execute the statement
        rc = sqlite3_step(stmt_potion_save);
        if (rc != SQLITE_DONE) {
            log_msg(ERROR, "Character", "Failed to execute statement: %s", sqlite3_errmsg(db_connection->db));
            sqlite3_finalize(stmt_potion_save);
            return;
        }
        // Finalize the statement
        sqlite3_finalize(stmt_potion_save);
    }

    // Prepare the SQL statement for character inventory
    sqlite3_stmt* stmt_character_inventory;
    rc = sqlite3_prepare_v2(db_connection->db, SQL_INSERT_CHARACTER_INVENTORY, -1, &stmt_character_inventory, NULL);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to prepare statement: %s", sqlite3_errmsg(db_connection->db));
        return;
    }
    // Bind the character ID to the statement
    rc = sqlite3_bind_int64(stmt_character_inventory, 1, character_id);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to bind character ID: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_character_inventory);
        return;
    }
    // Bind the inventory ID to the statement
    rc = sqlite3_bind_int64(stmt_character_inventory, 2, inventory_gear_id);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to bind inventory ID: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_character_inventory);
        return;
    }
    // Execute the statement
    rc = sqlite3_step(stmt_character_inventory);
    if (rc != SQLITE_DONE) {
        log_msg(ERROR, "Character", "Failed to execute statement: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_character_inventory);
        return;
    }
    // Finalize the statement
    sqlite3_finalize(stmt_character_inventory);

    // Prepare the SQL statement for character inventory
    sqlite3_stmt* stmt_character_inventory_potion;
    rc = sqlite3_prepare_v2(db_connection->db, SQL_INSERT_CHARACTER_INVENTORY, -1, &stmt_character_inventory_potion, NULL);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to prepare statement: %s", sqlite3_errmsg(db_connection->db));
        return;
    }
    // Bind the character ID to the statement
    rc = sqlite3_bind_int64(stmt_character_inventory_potion, 1, character_id);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to bind character ID: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_character_inventory_potion);
        return;
    }
    // Bind the inventory ID to the statement
    rc = sqlite3_bind_int64(stmt_character_inventory_potion, 2, inventory_potion_id);
    if (rc != SQLITE_OK) {
        log_msg(ERROR, "Character", "Failed to bind inventory ID: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_character_inventory_potion);
        return;
    }
    // Execute the statement
    rc = sqlite3_step(stmt_character_inventory_potion);
    if (rc != SQLITE_DONE) {
        log_msg(ERROR, "Character", "Failed to execute statement: %s", sqlite3_errmsg(db_connection->db));
        sqlite3_finalize(stmt_character_inventory_potion);
        return;
    }
    // Finalize the statement
    sqlite3_finalize(stmt_character_inventory_potion);


}

void get_character_from_db(db_connection_t* db_connection, character_t* character, int game_state_id) {
    // add_gear(character, gear_table->gears[ARMING_SWORD]);
}