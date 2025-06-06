/**
 * @file game_data.h
 * @brief Declares functions and globals for initializing, resetting, and freeing game data such as player, goblin, abilities, and potions.
 */
#ifndef GAME_DATA_H
#define GAME_DATA_H

#include "combat/ability.h"
#include "item/gear.h"
#include "item/potion.h"

extern ability_table_t* ability_table;
extern character_t* goblin;
extern character_t* player;
extern potion_t* healing_potion;
extern gear_table_t* gear_table;
extern potion_table_t* potion_table;

/**
 * @brief Initializes game data for the application.
 *
 * This function sets up the game data structures, including the ability table,
 * potion table, gear table, and player character. It also initializes the goblin
 * character and adds potions to the player.
 *
 * @return 0 if successful, 1 if initialization failed.
 */
int init_game_data(void);
/**
 * Frees game-related data structures and resources, such as ability tables,
 * characters, and potions, by using the provided memory pool.
 *
 * @return 0 if all resources are successfully freed.
 */
int free_game_data(void);
/**
 * Resets the goblin character data by deallocating the current goblin instance
 * and creating a new one. The new goblin is initialized and assigned with the
 * "BITE" ability.
 *
 * @return 0 if the operation is successful, 1 if it fails due to invalid input
 *         or memory allocation issues.
 */
int reset_goblin(void);

/**
 * Initializes the player character with default abilities and items.
 * This function sets up the player with a base attack ability,
 * adds default potions, and equips a starting piece of gear.
 * It is used to prepare the player only for a new game.
 * @param name The name of the player character.
 *
 * @return 0 if successful, 1 if failed
 */
int init_player(char* name);

/**
 * Resets the player character to its initial state, clearing all abilities,
 * items, and stats. This function is used when loading a saved game.
 *
 * @return 0 if successful, 1 if failed
 */
int reset_player(void);

/**
 * Gets the current player's name.
 *
 * @return Pointer to the player's name string, or "Unknown" if player is null
 */
const char* get_player_name(void);

#endif//GAME_DATA_H