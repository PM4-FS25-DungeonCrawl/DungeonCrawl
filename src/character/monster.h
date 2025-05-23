#ifndef MONSTER_H
#define MONSTER_H

#include "character.h"

/**
 * @brief Creates and initializes a new goblin character
 * @param memory_pool A pointer to the memory pool used for allocating the goblin character
 * @return A pointer to the newly created goblin character, or NULL if memory allocation fails
 */
character_t* create_new_goblin(memory_pool_t* memory_pool);

#endif//MONSTER_H
