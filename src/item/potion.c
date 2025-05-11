#include "potion.h"

#include "../database/game/item_database.h"

#include <stdio.h>
#include <stdlib.h>

potion_t* init_potion(potion_t* potion, const char* name, const potion_type_t type, const int value) {
    NULL_PTR_HANDLER_RETURN(name, NULL, "Potion", "In init_potion name is NULL");

    snprintf(potion->name, sizeof(potion->name), "%s", name);
    potion->effectType = type;
    potion->value = value;
    return potion;
}

potion_table_t* init_potion_table(memory_pool_t* memory_pool, const db_connection_t* db_connection) {
    NULL_PTR_HANDLER_RETURN(memory_pool, NULL, "Potion", "Memory pool is NULL");

    potion_init_t* rows = init_potion_table_from_db(db_connection);
    NULL_PTR_HANDLER_RETURN(rows, NULL, "Potion", "Could not fetch potion data from DB");

    potion_table_t* table = memory_pool_alloc(memory_pool, sizeof(potion_table_t));
    NULL_PTR_HANDLER_RETURN(table, NULL, "Potion", "Failed to allocate potion for potion table");

    for (int i = 0; i < MAX_POTION_TYPES; ++i) {
        if (rows[i].name == NULL)
            break;

        const int slot = rows[i].potion_type;
        init_potion(&table->potions[slot], rows[i].name, rows[i].potion_type, rows[i].value);
    }
    free_potion_table_from_db(rows, db_connection);
    return table;
}

const char* potion_type_to_string(potion_type_t type) {
    switch (type) {
        case HEALING:
            return "Health";
        case MANA:
            return "Mana";
        case STAMINA:
            return "Stamina";
        default:
            return "Unknown";
    }
}

void free_potion_table(memory_pool_t* memory_pool, potion_table_t* table) {
    NULL_PTR_HANDLER_RETURN(memory_pool, , "Potion", "In free_potion_table memory pool is NULL");
    NULL_PTR_HANDLER_RETURN(table, , "Potion", "In free_potion_table table is NULL");

    memory_pool_free(memory_pool, table);
}
