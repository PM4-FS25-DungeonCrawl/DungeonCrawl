#ifndef GAME_DATA_H
#define GAME_DATA_H
#include "combat/ability.h"
#include "item/potion.h"
#include "memory/memory_management.h"

extern ability_table_t* ability_table;
extern character_t* goblin;
extern character_t* player;
extern potion_t* healing_potion;

/**
 * Initialize game data
 * @return 0 if successful, 1 if failed
 */
int init_game_data(memory_pool_t* memory_pool);
/**
 * Free game data
 */
int free_game_data();
/**
 * Reset the goblin's state
 * @return 0 if successful, 1 if failed
 */
int reset_goblin();

#endif//GAME_DATA_H
