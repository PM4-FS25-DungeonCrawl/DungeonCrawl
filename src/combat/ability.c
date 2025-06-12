/**
 * @file ability.c
 * @brief This file contains functions for initializing and managing abilities.
 */

#include "ability.h"

#include "../database/game/ability_database.h"

#include <stdio.h>

// Internal functions
void init_ability(ability_t* ability, char* name, int roll_amount, int accuracy, int resource_cost, dice_size_t dice_size, damage_type_t damage_type);


ability_table_t* init_ability_table(memory_pool_t* memory_pool, const db_connection_t* db_connection) {
    NULL_PTR_HANDLER_RETURN(memory_pool, NULL, "Ability", "Memory pool is NULL");

    ability_init_t* rows = get_ability_table_from_db(db_connection);
    NULL_PTR_HANDLER_RETURN(rows, NULL, "Ability", "Could not fetch ability data from DB");

    ability_table_t* table = memory_pool_alloc(memory_pool, sizeof(ability_table_t));
    NULL_PTR_HANDLER_RETURN(table, NULL, "Ability", "Failed to allocate memory for ability table");


    for (int i = 0; i < MAX_ABILITIES; ++i) {
        if (rows[i].name == NULL)
            break;

        const int slot = rows[i].ability_number;
        table->abilities[slot].id = slot;// Set the ID to the slot index

        init_ability(&table->abilities[slot],
                     rows[i].name,
                     rows[i].roll_amount,
                     rows[i].accuracy,
                     rows[i].resource_cost,
                     rows[i].dice_size,
                     rows[i].damage_type);
    }
    free_ability_table_from_db(rows);
    return table;
}


void free_ability_table(memory_pool_t* memory_pool, ability_table_t* table) {
    NULL_PTR_HANDLER_RETURN(memory_pool, , "Ability", "In free_ability_table memory pool is NULL");
    NULL_PTR_HANDLER_RETURN(table, , "Ability", "In free_ability_table table is NULL");

    memory_pool_free(memory_pool, table);
}

/**
 * @brief Initializes an ability with the given parameters.
 * @param ability Pointer to the ability to be initialized.
 * @param name Name of the ability.
 * @param roll_amount Number of dice to roll.
 * @param accuracy Accuracy of the ability.
 * @param resource_cost Resource cost of the ability.
 * @param dice_size Size of the dice used for rolling damage.
 * @param damage_type Type of damage dealt by the ability.
 */
void init_ability(ability_t* ability, char* name, const int roll_amount, const int accuracy, const int resource_cost, const dice_size_t dice_size, const damage_type_t damage_type) {
    NULL_PTR_HANDLER_RETURN(ability, , "Ability", "In init_ability ability is NULL");
    NULL_PTR_HANDLER_RETURN(name, , "Ability", "In init_ability name is NULL");

    snprintf(ability->name, sizeof(ability->name), "%s", name);
    ability->roll_amount = roll_amount;
    ability->accuracy = accuracy;
    ability->resource_cost = resource_cost;
    ability->dice_size = dice_size;
    ability->damage_type = damage_type;
}
