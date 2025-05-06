#include "gear.h"

#include <stdio.h>
#include <stdlib.h>


gear_t* init_gear(const char* name, gear_slot_t slot, stats_t stats, defenses_t defenses, ability_names_t* abilities, int num_abilities) {
    gear_t* gear = malloc(sizeof(gear_t));
    NULL_PTR_HANDLER_RETURN(gear, NULL, "Gear", "Failed to allocate memory for gear: %s", name);

    snprintf(gear->name, sizeof(gear->name), "%s", name);
    gear->slot = slot;
    gear->stats = stats;
    gear->defenses = defenses;

    // TODO: Implement abilities
    // for (int i = 0; i < num_abilities && i < MAX_ABILITY_PER_GEAR; ++i) {
    // gear->abilities[i] = ability_table_t->abilities[abilities];
    // }

    return gear;
}

void free_equipable_gear(gear_t* gear) {
    if (gear != NULL) {
        free(gear);
    }
}
