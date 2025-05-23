/**
 * @file monster.c
 * @brief This file contains functions for creating and initializing monster characters
 */

#include "monster.h"


character_t* create_new_goblin(memory_pool_t* memory_pool) {
    NULL_PTR_HANDLER_RETURN(memory_pool, NULL, "Goblin", "Memory pool is NULL");

    character_t* goblin = init_character(memory_pool, MONSTER, "Goblin");
    NULL_PTR_HANDLER_RETURN(goblin, NULL, "Goblin", "Failed to allocate memory for goblin");

    set_character_stats(goblin, 5, 5, 5, 5);
    set_character_dmg_modifier(goblin, PHYSICAL, 10);
    set_character_dmg_modifier(goblin, MAGICAL, 5);
    set_xp_reward(goblin, 120);
    set_level(goblin, 1);
    return goblin;
}
