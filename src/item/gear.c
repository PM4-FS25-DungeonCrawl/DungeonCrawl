#include <stdlib.h>
#include <stdio.h>

#include "../common.h"
#include "../character/stats.h"
#include "gear.h"


gear_t* init_gear(const char* name, gear_slot_t slot, stats_t stats, defenses_t defenses) {
    gear_t* gear = malloc(sizeof(gear_t));
    NULL_PTR_HANDLER(gear, "Gear", "Failed to allocate memory for gear: %s", name);

    snprintf(gear->name, sizeof(gear->name), "%s", name);
    gear->slot = slot;
    gear->stats = stats;
    gear->defenses = defenses;
    return gear;
}

void free_equipable_gear(gear_t* gear) {
    if (gear != NULL) {
        free(gear);
    }
}
