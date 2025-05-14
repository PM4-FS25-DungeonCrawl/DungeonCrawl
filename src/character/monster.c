/**
 * @file monster.c
 * @brief This file contains functions for creating and initializing monster characters
 */

#include "monster.h"
#include "../game.h"
#include <math.h>

/**
 * @brief Creates and initializes a new goblin character
 * @param memory_pool A pointer to the memory pool used for allocating the goblin character
 * @return A pointer to the newly created goblin character, or NULL if memory allocation fails
 */
character_t* create_new_goblin(memory_pool_t* memory_pool) {
    NULL_PTR_HANDLER_RETURN(memory_pool, NULL, "Goblin", "Memory pool is NULL");

    character_t* goblin = init_character(memory_pool, MONSTER, "Goblin");
    NULL_PTR_HANDLER_RETURN(goblin, NULL, "Goblin", "Failed to allocate memory for goblin");

    set_character_stats(goblin, 5, 5, 5, 5);
    set_character_dmg_modifier(goblin, PHYSICAL, 10);
    set_character_dmg_modifier(goblin, MAGICAL, 5);
    set_xp_reward(goblin, 120);
    set_level(goblin, 1);

    // scale monster stats based on the current floor level
    scale_monster_stats(goblin);

    return goblin;
}


/**
 * @brief scales the monster's stats based on the current floor level
 * @param monster monster character to scale
 * @param floor current floor level
 */
void scale_monster_stats(character_t* monster) {
    if (monster == NULL || current_floor <= 0) return;

    const float multiplier = 1.0f + (current_floor * 0.1f); // 10% increase per floor
    monster->base_stats.strength = (int)round(monster->base_stats.strength * multiplier);
    monster->base_stats.dexterity = (int)round(monster->base_stats.dexterity * multiplier);
    monster->base_stats.intelligence = (int)round(monster->base_stats.intelligence * multiplier);
    monster->base_stats.constitution = (int)round(monster->base_stats.constitution * multiplier);

    // recalculate the current stats based on the new base stats
    reset_current_stats(monster);
}
