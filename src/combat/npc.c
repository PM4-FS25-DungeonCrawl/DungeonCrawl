#include <stdlib.h>

#include "npc.h"
#include "ability.h"
#include "../logging/logger.h"

monster_t* init_goblin(void) {
    monster_t* goblin = malloc(sizeof(monster_t));
    if (goblin == NULL) {
        // memory for goblin could not be allocated
        log_msg(ERROR, "NPC", "Failed to allocate memory for goblin");
        return NULL;
    }

    weakness_t* weak_physical = init_weakness(PHYSICAL, 10);
    weakness_t* weak_magical = init_weakness(MAGICAL, 5);
    if (weak_physical == NULL || weak_magical == NULL) {
        // memory for weaknesses failed to allocate
        free(goblin);
        return NULL;
    }

    character_t* base = init_character(MONSTER, "NPC", 50, 5, 3, 3, 3, 3);
    if (base == NULL) {
        // memory for character failed to allocate
        free(goblin);
        return NULL;
    }

    goblin->base = base;
    goblin->weakness_table[0] = weak_physical;
    goblin->weakness_table[1] = weak_magical;

    return goblin;
}

void free_goblin(monster_t* goblin) {
    if (goblin != NULL) {
        free_character(goblin->base);
        for (int i = 0; i < DAMAGE_TYPE_COUNT; i++) {
            free_weakness(goblin->weakness_table[i]);
        }
        free(goblin);
    }
}
