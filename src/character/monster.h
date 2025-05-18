#ifndef MONSTER_H
#define MONSTER_H

#include "character.h"

/**
 * Creates a new standard goblin with no abilities or items.
 * @param memory_pool Pointer to the memory pool for allocation.
 * @return the pointer to the new goblin character, or NULL if memory allocation failed.
 */
character_t* create_new_goblin(memory_pool_t* memory_pool);
int scale_to_floor(int base);

#endif//MONSTER_H
