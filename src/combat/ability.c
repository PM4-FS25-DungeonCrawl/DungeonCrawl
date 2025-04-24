#include "ability.h"

#include <stdio.h>
#include <stdlib.h>

void init_ability(ability_t* ability, char* name, int roll_amount, int accuracy, int resource_cost, dice_size_t dice_size, damage_type_t damage_type);

/**
 * Initialize the ability table, allocates memory and returns the pointer to the table.
 *
 * @return Pointer to the ability table.
 */
ability_table_t* init_ability_table(void) {
    ability_table_t* table = malloc(sizeof(ability_table_t));
    NULL_PTR_HANDLER_RETURN(table, NULL, "Ability", "Failed to allocate memory for ability table");

    init_ability(&table->abilities[FIREBALL], "Fireball", 4, 15, 1, D10, MAGICAL);
    init_ability(&table->abilities[SWORD_SLASH], "Sword Slash", 4, 15, 1, D6, PHYSICAL);
    init_ability(&table->abilities[BITE], "Bite", 3, 20, 2, D8, PHYSICAL);
    return table;
}

void init_ability(ability_t* ability, char* name, const int roll_amount, const int accuracy, const int resource_cost, const dice_size_t dice_size, const damage_type_t damage_type) {
    snprintf(ability->name, sizeof(ability->name), "%s", name);
    ability->roll_amount = roll_amount;
    ability->accuracy = accuracy;
    ability->resource_cost = resource_cost;
    ability->dice_size = dice_size;
    ability->damage_type = damage_type;
}


void free_ability_table(ability_table_t* table) {
    if (table != NULL) {
        free(table);
    }
}
