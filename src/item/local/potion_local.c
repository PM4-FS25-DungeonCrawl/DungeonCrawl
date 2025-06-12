#include "potion_local.h"

#include "../../local/local_handler.h"
#include "../../logging/logger.h"
#include "../potion.h"

char** potion_names = NULL;

void update_potion_local(void);

int init_potion_local(void) {
    potion_names = malloc(sizeof(char*) * MAX_POTION_TYPES);
    RETURN_WHEN_NULL(potion_names, 1, "Potion Local", "Failed to allocate memory for potion names");
    for (int i = 0; i < MAX_POTION_TYPES; i++) {
        potion_names[i] = NULL;
    }

    update_potion_local();
    observe_local(update_potion_local);
    return 0;
}

void shutdown_potion_local(void) {
    if (potion_names != NULL) {
        for (int i = 0; i < MAX_POTION_TYPES; i++) {
            if (potion_names[i] != NULL) {
                free(potion_names[i]);
                potion_names[i] = NULL;
            }
        }
        free(potion_names);
        potion_names = NULL;
    }
}

void update_potion_local(void) {
    if (potion_names == NULL) return;// module not initialized
    for (int i = 0; i < MAX_POTION_TYPES; i++) {
        if (potion_names[i] != NULL) {
            free(potion_names[i]);
        }
    }

    potion_names[HEALING] = get_local_string("POTION.HEALTH");
    potion_names[MANA] = get_local_string("POTION.MANA");
    potion_names[STAMINA] = get_local_string("POTION.STAMINA");
}
