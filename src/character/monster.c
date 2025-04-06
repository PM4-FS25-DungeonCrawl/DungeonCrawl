#include <stdlib.h>

#include "monster.h"

/**
 * Creates a new standard goblin with no abilities or items.
 * @return the pointer to the new goblin character, or NULL if memory allocation failed.
 */
character_t* create_new_goblin(void) {
    character_t* goblin = init_character(MONSTER, "Goblin");
    NULL_PTR_HANDLER(goblin, "Goblin", "Failed to allocate memory for goblin");

    set_character_stats(goblin, 50, 5, 3, 3, 3, 3);
    set_character_dmg_modifier(goblin, PHYSICAL, 10);
    set_character_dmg_modifier(goblin, MAGICAL, 5);
    return goblin;
}
