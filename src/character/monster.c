#include "monster.h"

#include <stdlib.h>

/**
 * Creates a new standard goblin with no abilities or items.
 * @return the pointer to the new goblin character, or NULL if memory allocation failed.
 */
character_t* create_new_goblin(void) {
    character_t* goblin = init_character(MONSTER, "Goblin");
    NULL_PTR_HANDLER(goblin, "Goblin", "Failed to allocate memory for goblin");


    set_character_stats(goblin, 5, 5, 5, 5);
    set_character_dmg_modifier(goblin, PHYSICAL, 10);
    set_character_dmg_modifier(goblin, MAGICAL, 5);
    set_xp_reward(goblin, 120);
    set_level(goblin, 1);
    return goblin;
}
