/**
 * @file player.c
 * @brief This file contains functions for creating and initializing player characters
 */

#include "player.h"

#include <stdlib.h>

/**
 * @brief Creates and initializes a new player character
 * @param memory_pool A pointer to the memory pool used for allocating the player character
 * @return A pointer to the newly created player character, or NULL if memory allocation fails
 */
character_t* create_new_player(memory_pool_t* memory_pool) {
    NULL_PTR_HANDLER_RETURN(memory_pool, NULL, "Player", "Memory pool is NULL");

    character_t* player = init_character(memory_pool, PLAYER, "Hero");
    NULL_PTR_HANDLER_RETURN(player, NULL, "Player", "Failed to allocate memory for player");

    set_character_stats(player, 5, 5, 5, 10);
    set_initial_xp(player, 0);
    set_level(player, 1);
    return player;
}
