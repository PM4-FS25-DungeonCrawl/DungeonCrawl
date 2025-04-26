#include "potion.h"

#include <stdio.h>
#include <stdlib.h>

potion_t* init_potion(memory_pool_t* memory_pool, const char* name, const potion_type_t type, const int value) {
    NULL_PTR_HANDLER_RETURN(memory_pool, NULL, "Potion", "In init_potion memory pool is NULL");
    NULL_PTR_HANDLER_RETURN(name, NULL, "Potion", "In init_potion name is NULL");

    potion_t* potion = memory_pool_alloc(memory_pool, sizeof(potion_t));
    NULL_PTR_HANDLER_RETURN(potion, NULL, "Potion", "Failed to allocate memory for item: %s", name);

    snprintf(potion->name, sizeof(potion->name), "%s", name);
    potion->effectType = type;
    potion->value = value;
    return potion;
}

const char* potion_type_to_string(potion_type_t type) {
    switch (type) {
        case HEALING:
            return "Healing";
        default:
            return "Unknown";
    }
}

void free_potion(memory_pool_t* memory_pool, potion_t* potion) {
    NULL_PTR_HANDLER_RETURN(memory_pool, , "Potion", "In free_potion memory pool is NULL");
    NULL_PTR_HANDLER_RETURN(potion, , "Potion", "In free_potion potion is NULL");

    memory_pool_free(memory_pool, potion);
}
