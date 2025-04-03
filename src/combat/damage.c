#include <stdlib.h>

#include "damage.h"
#include "../logging/logger.h"

weakness_t* init_weakness(damage_type_t type, int value) {
    weakness_t* weaknesses = malloc(sizeof(weakness_t));
    if (weaknesses == NULL) {
        // memory for weaknesses could not be allocated
        log_msg(ERROR, "Weakness", "Failed to allocate memory for weaknesses");
        return NULL;
    }

    weaknesses->type = type;
    weaknesses->value = value;

    return weaknesses;
}


void free_weakness(weakness_t* weaknesses) {
    if (weaknesses != NULL) {
        free(weaknesses);
    }
}
