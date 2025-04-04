#include <stdlib.h>
#include <stdio.h>

#include "equipable_item.h"
#include "../logging/logger.h"

equipable_item_t* init_equipable_item(const char* name, const gear_slot_t slot, const int armor_bonus) {
    equipable_item_t* item = malloc(sizeof(equipable_item_t));
    if (item == NULL) {
        // memory for item could not be allocated
        log_msg(ERROR, "Equipable Item", "Failed to allocate memory for item: %s", name);
        return NULL;
    }
    item_t* base = init_item(name, EQUIPABLE, item);
    if (base == NULL) {
        free(item);
        return NULL;
    }
    item->base = base;
    item->slot = slot;
    item->armor_bonus = armor_bonus;
    return item;
}

void free_equipable_item(equipable_item_t* item) {
    if (item != NULL) {
        free(item);
    }
}
