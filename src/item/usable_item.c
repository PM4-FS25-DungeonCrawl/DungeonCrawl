#include <stdlib.h>
#include <stdio.h>

#include "src/common.h"
#include "usable_item.h"

usable_item_t* init_usable_item(const char* name, usable_item_effect_t effectType, int value) {
    usable_item_t* item = malloc(sizeof(usable_item_t));
    NULL_PTR_HANDLER(item, "Usable Item", "Failed to allocate memory for item: %s", name);

    item_t* base = init_item(name, USABLE, item);
    if (base == NULL) {
        free(item);
        return NULL;
    }

    item->base = base;
    item->effectType = effectType;
    item->value = value;
    return item;
}

void free_usable_item(usable_item_t* item) {
    if (item != NULL) {
        free(item);
    }
}
