#ifndef PLAYER_H
#define PLAYER_H

#include "character.h"

/**
 * @brief Creates and initializes a new player character
 * @param memory_pool A pointer to the memory pool used for allocating the player character
 * @return A pointer to the newly created player character, or NULL if memory allocation fails
 */
character_t* create_new_player(memory_pool_t* memory_pool);

#endif//PLAYER_H
