#ifndef GAME_DATA_H
#define GAME_DATA_H

#include "combat/ability.h"
#include "item/potion.h"

extern ability_table_t* ability_table;
extern character_t* goblin;
extern character_t* player;
extern potion_t* healing_potion;

/**
 * Initialize game data
 * @return 0 if successful, 1 if failed
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

#endif//GAME_DATA_H
