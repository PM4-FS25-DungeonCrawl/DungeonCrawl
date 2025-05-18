#ifndef MONSTER_H
#define MONSTER_H

#include "character.h"

/**
 * Creates a new standard goblin with no abilities or items.
 * @param memory_pool Pointer to the memory pool for allocation.
 * @return the pointer to the new goblin character, or NULL if memory allocation failed.
 */
character_t* create_new_goblin(memory_pool_t* memory_pool);
void distribute_monster_skill_points(character_t* goblin);
int scale_xp_reward(int base);

#endif//MONSTER_H
