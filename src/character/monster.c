/**
 * @file monster.c
 * @brief This file contains functions for creating and initializing monster characters
 */

#include "monster.h"

#include "../local/local_handler.h"
#include "../map/map_mode.h"
#include "../stats/stats.h"

stat_type_t goblin_scaling[10] = {CONSTITUTION, CONSTITUTION, STRENGTH, INTELLIGENCE, DEXTERITY, CONSTITUTION, STRENGTH, DEXTERITY, CONSTITUTION, STRENGTH};

character_t* create_new_goblin(memory_pool_t* memory_pool) {
    NULL_PTR_HANDLER_RETURN(memory_pool, NULL, "Goblin", "Memory pool is NULL");

    char* goblin_name = get_local_string("CHARACTER.GOBLIN");
    character_t* goblin = init_character(memory_pool, MONSTER, goblin_name);
    free(goblin_name);
    NULL_PTR_HANDLER_RETURN(goblin, NULL, "Goblin", "Failed to allocate memory for goblin");

    set_character_stats(goblin, 5, 5, 5, 5);
    set_character_dmg_modifier(goblin, PHYSICAL, 10);
    set_character_dmg_modifier(goblin, MAGICAL, 5);
    set_xp_reward(goblin, 120);
    set_level(goblin, current_floor);
    set_skill_points(goblin, current_floor - 1);
    distribute_monster_skillpoints(goblin);
    return goblin;
}

void distribute_monster_skillpoints(character_t* goblin) {
    for (int i = 0; i < (goblin->level - 1); i++) {
        raise_skill(&goblin->base_stats, goblin_scaling[i % 10], goblin->skill_points);
        goblin->skill_points--;
    }
    update_character_resources(&goblin->current_resources, &goblin->max_resources, &goblin->base_stats);
}