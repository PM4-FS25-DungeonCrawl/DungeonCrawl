#include <stdlib.h>
#include <stdio.h>

#include "base_item.h"
#include "../logging/logger.h"

item_t* init_item(const char* name, item_type_t type, void* extension) {
    item_t* item = malloc(sizeof(item_t));
    if (item == NULL) {
        // memory for item could not be allocated
        log_msg(ERROR, "Item", "Failed to allocate memory for item: %s", name);
        return NULL;
    }
    snprintf(item->name, sizeof(item->name), "%s", name);
    item->type = type;
    item->extension = extension;
    return item;
}

void free_item(item_t* item) {
    if (item != NULL) {
        if (item->type == USABLE) {
            free_usable_item((usable_item_t*)item->extension);
        } else if (item->type == EQUIPABLE) {
            free_equipable_item((equipable_item_t*)item->extension);
        }
        free(item);
    }
}
