/**
 * @file gear.c
 * @brief Implementation of the gear system.
 */
#include "gear.h"

#include "../database/game/item_database.h"
#include "../local/local_handler.h"
#include "../logging/logger.h"

#include <stdio.h>

char** gear_slot_names = NULL;

void update_gear_slot_local(void);

gear_t* init_gear(memory_pool_t* memory_pool, const char* name, gear_identifier_t gear_identifier, gear_slot_t slot, stats_t stats, defenses_t defenses, ability_table_t* ability_table, const ability_names_t* abilities, int num_abilities) {
    NULL_PTR_HANDLER_RETURN(memory_pool, NULL, "Gear", "In init_gear memory pool is NULL");
    NULL_PTR_HANDLER_RETURN(name, NULL, "Gear", "In init_gear name is NULL");
    gear_t* gear = memory_pool_alloc(memory_pool, sizeof(gear_t));


    NULL_PTR_HANDLER_RETURN(gear, NULL, "Gear", "Failed to allocate memory for gear: %s", name);

    snprintf(gear->name, sizeof(gear->name), "%s", name);
    gear->gear_identifier = gear_identifier;
    gear->slot = slot;
    gear->stats = stats;
    gear->defenses = defenses;
    gear->num_abilities = num_abilities;

    int i = 0;
    for (; i < MAX_ABILITY_PER_GEAR && i < num_abilities; ++i) {
        ability_names_t idx = abilities[i];
        if (idx < MAX_ABILITIES) {
            gear->abilities[i] = &ability_table->abilities[abilities[i]];
        } else {
            gear->abilities[i] = &(ability_t) {0};
        }
    }
    for (; i < MAX_ABILITY_PER_GEAR; ++i) {
        gear->abilities[i] = &(ability_t) {0};
    }
    return gear;
}


gear_table_t* init_gear_table(memory_pool_t* memory_pool, const db_connection_t* db_connection, ability_table_t* ability_table) {
    NULL_PTR_HANDLER_RETURN(memory_pool, NULL, "Gear", "Memory pool is NULL");

    gear_slot_names = malloc(sizeof(char*) * MAX_SLOT);
    RETURN_WHEN_NULL(gear_slot_names, NULL, "Gear", "Failed to allocate memory for gear slot names");

    gear_init_t* rows = init_gear_table_from_db(db_connection);
    NULL_PTR_HANDLER_RETURN(rows, NULL, "Gear", "Could not fetch gear data from DB");

    int count = count_gear_in_db(db_connection);

    gear_table_t* table = memory_pool_alloc(memory_pool, sizeof(gear_table_t));
    NULL_PTR_HANDLER_RETURN(table, NULL, "Gear", "Failed to allocate gear table");

    table->num_gears = count;
    NULL_PTR_HANDLER_RETURN(table->gears, NULL, "Gear", "Failed to allocate gear array for table");

    for (int i = 0; i < MAX_SLOT; i++) {
        gear_slot_names[i] = NULL;
    }

    for (int i = 0; i < count; ++i) {
        table->gears[i] = init_gear(memory_pool,
                                    rows[i].name,
                                    rows[i].gear_identifier,
                                    rows[i].slot,
                                    rows[i].stats,
                                    rows[i].defenses,
                                    ability_table,
                                    rows[i].ability_names,
                                    rows[i].num_abilities);
    }
    free_gear_table_from_db(rows, db_connection);

    update_gear_slot_local();
    observe_local(update_gear_slot_local);
    return table;
}

void free_gear_table(memory_pool_t* memory_pool, gear_table_t* table) {
    NULL_PTR_HANDLER_RETURN(memory_pool, , "Gear", "In free_gear_table memory pool is NULL");
    NULL_PTR_HANDLER_RETURN(table, , "Gear", "In free_gear_table table is NULL");
    for (int i = 0; i < table->num_gears; ++i) {
        gear_t* gear = table->gears[i];
        if (gear != NULL) {
            memory_pool_free(memory_pool, gear);
        }
    }
    memory_pool_free(memory_pool, table);

    if (gear_slot_names != NULL) {
        for (int i = 0; i < MAX_SLOT; i++) {
            if (gear_slot_names[i] != NULL) {
                free(gear_slot_names[i]);
            }
        }
        free(gear_slot_names);
        gear_slot_names = NULL;
    }
}

const char* gear_slot_to_string(const gear_slot_t slot) {
    const char* str;
    if (slot < MAX_SLOT) {
        if (gear_slot_names[slot] != NULL) {
            str = gear_slot_names[slot];
        } else {
            str = "ERROR_GETTING_STR";
        }
    } else {
        str = "ERROR_UNKNOWN_SLOT";
    }
    return str;
}

void update_gear_slot_local(void) {
    if (gear_slot_names == NULL) return;// module not initialized

    for (int i = 0; i < MAX_SLOT; i++) {
        if (gear_slot_names[i] != NULL) {
            free(gear_slot_names[i]);
        }
    }

    gear_slot_names[SLOT_HEAD] = get_local_string("GEAR.SLOT.HEAD");
    gear_slot_names[SLOT_CHEST] = get_local_string("GEAR.SLOT.CHEST");
    gear_slot_names[SLOT_LEGS] = get_local_string("GEAR.SLOT.LEGS");
    gear_slot_names[SLOT_FEET] = get_local_string("GEAR.SLOT.FEET");
    gear_slot_names[SLOT_HANDS] = get_local_string("GEAR.SLOT.HANDS");
    gear_slot_names[SLOT_NECK] = get_local_string("GEAR.SLOT.NECK");
    gear_slot_names[SLOT_FINGER_RIGHT] = get_local_string("GEAR.SLOT.FINGER.RIGHT");
    gear_slot_names[SLOT_FINGER_LEFT] = get_local_string("GEAR.SLOT.FINGER.LEFT");
    gear_slot_names[SLOT_LEFT_HAND] = get_local_string("GEAR.SLOT.HAND.LEFT");
    gear_slot_names[SLOT_RIGHT_HAND] = get_local_string("GEAR.SLOT.HAND.RIGHT");
    gear_slot_names[SLOT_BOTH_HANDS] = get_local_string("GEAR.SLOT.HAND.BOTH");
}
