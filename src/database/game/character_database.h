#ifndef CHARACTER_DATABASE_H
#define CHARACTER_DATABASE_H

#include "../../character/character.h"
#include "../database.h"

/**
 * This function saves the character to the database.
 *
 * @param db_connection the database connection
 * @param character the character to save
 * @param game_state_id the game state id
 */
void save_character(db_connection_t* db_connection, const character_t character, const sqlite3_int64 game_state_id);
int get_character_from_db();

#endif//CHARACTER_DATABASE_H
