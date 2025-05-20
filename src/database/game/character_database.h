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
void save_character(const db_connection_t* db_connection, const character_t character, const sqlite3_int64 game_state_id);

/**
 * This function saves the character's inventory to the database.
 *
 * @param db_connection the database connection
 * @param character the character whose inventory to save
 * @param character_id the character id
 */
void save_character_inventory(const db_connection_t* db_connection, const character_t character, const sqlite3_int64 character_id);

/**
 * This function retrieves a character from the database.
 *
 * @param db_connection the database connection
 * @param character the character to retrieve
 * @param game_state_id the game state id
 */
void get_character_from_db(db_connection_t* db_connection, character_t* character , int game_state_id);


#endif//CHARACTER_DATABASE_H