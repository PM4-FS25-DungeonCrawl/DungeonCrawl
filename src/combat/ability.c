#include "ability.h"

#include <stdio.h>

void init_ability(ability_t* ability, char* name, int roll_amount, int accuracy, int resource_cost, dice_size_t dice_size, damage_type_t damage_type);

ability_table_t* init_ability_table(memory_pool_t* memory_pool) {
    NULL_PTR_HANDLER_RETURN(memory_pool, NULL, "Ability", "Memory pool is NULL");

    ability_table_t* table = memory_pool_alloc(memory_pool, sizeof(ability_table_t));
    NULL_PTR_HANDLER_RETURN(table, NULL, "Ability", "Failed to allocate memory for ability table");

    init_ability(&table->abilities[FIREBALL], "Fireball", 4, 10, 1, D10, MAGICAL);
    init_ability(&table->abilities[SWORD_SLASH], "Sword Slash", 4, 10, 1, D6, PHYSICAL);
    init_ability(&table->abilities[BITE], "Bite", 3, 20, 2, D8, PHYSICAL);
    return table;
}

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


void free_ability_table(memory_pool_t* memory_pool, ability_table_t* table) {
    NULL_PTR_HANDLER_RETURN(memory_pool, , "Ability", "In free_ability_table memory pool is NULL");
    NULL_PTR_HANDLER_RETURN(table, , "Ability", "In free_ability_table table is NULL");

    memory_pool_free(memory_pool, table);
}
