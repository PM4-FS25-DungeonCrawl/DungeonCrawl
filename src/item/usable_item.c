#include <stdlib.h>
#include <stdio.h>

#include "usable_item.h"
#include "../logging/logger.h"

usable_item_t* init_usable_item(const char* name, usable_item_effect_t effectType, int value) {
    usable_item_t* item = malloc(sizeof(usable_item_t));
    if (item == NULL) {
        // memory for item could not be allocated
        log_msg(ERROR, "Usable Item", "Failed to allocate memory for item: %s", name);
        return NULL;
    }
    snprintf(item->name, sizeof(item->name), "%s", name);
    item->effectType = effectType;
    item->value = value;
    return item;
}

void free_usable_item(usable_item_t* item) {
    if (item != NULL) {
        free(item);
    }
}
