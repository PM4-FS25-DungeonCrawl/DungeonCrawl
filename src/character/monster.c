/**
 * @file monster.c
 * @brief This file contains functions for creating and initializing monster characters
 */

#include "monster.h"

#include "../character/level.h"
#include "../game.h"

#include <math.h>

stat_type_t scaling_stats[8] = {STRENGTH, CONSTITUTION, STRENGTH, DEXTERITY, INTELLIGENCE, CONSTITUTION, DEXTERITY, STRENGTH};

character_t* create_new_goblin(memory_pool_t* memory_pool) {
    NULL_PTR_HANDLER_RETURN(memory_pool, NULL, "Goblin", "Memory pool is NULL");

    character_t* goblin = init_character(memory_pool, MONSTER, "Goblin");
    NULL_PTR_HANDLER_RETURN(goblin, NULL, "Goblin", "Failed to allocate memory for goblin");

    // Parameters are the default values for floor 1, which then get scaled
    set_character_stats(goblin, 10, 5, 5, 1);

    set_character_dmg_modifier(goblin, PHYSICAL, 10);
    set_character_dmg_modifier(goblin, MAGICAL, 5);
    set_xp_reward(goblin, scale_xp_reward(120));
    set_level(goblin, 0);

    return goblin;
}

/**
 * @brief scales the xp reward based on the current floor
 * @param base base value for floor 1
 * @return new scaled value
 */
int scale_xp_reward(int base) {
    float mult_increase = 0.1f;// increase per floor
    float multiplier = 1 + ((current_floor - 1) * mult_increase);
    return (int) round(base * multiplier);
}

/**
 * @brief Distributes skill points to the monster
 * @param monster Pointer to the monster
 */
void distribute_monster_skill_points(character_t* monster) {
    monster->skill_points = current_floor;
    monster->level = current_floor;
    while (monster->skill_points > 0) {
        stat_type_t stat = scaling_stats[monster->level % 8];
        raise_skill(&monster->base_stats, stat, 1);
        monster->skill_points--;
    }
    update_character_resources(&monster->current_resources, &monster->max_resources, &monster->base_stats);
}
