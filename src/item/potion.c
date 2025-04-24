#include "potion.h"

#include <stdio.h>
#include <stdlib.h>

potion_t* init_potion(const char* name, potion_type_t type, int value) {
    potion_t* item = malloc(sizeof(potion_t));
    NULL_PTR_HANDLER_RETURN(item, NULL, "Usable Item", "Failed to allocate memory for item: %s", name);

    snprintf(item->name, sizeof(item->name), "%s", name);
    item->effectType = type;
    item->value = value;
    return item;
}

const char* potion_type_to_string(potion_type_t type) {
    switch (type) {
        case HEALING:
            return "Healing";
        case MANA:
            return "Mana";
        case STAMINA:
            return "Stamina";
        default:
            return "Unknown";
    }
}

void free_potion(potion_t* item) {
    if (item != NULL) {
        free(item);
    }
}
