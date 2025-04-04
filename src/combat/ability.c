#include <stdlib.h>
#include <stdio.h>

#include "../common.h"
#include "ability.h"

void init_ability(ability_t ability, char* name, int roll_count, int accuracy, dice_size_t dice_size, damage_type_t damage_type);

/**
 * Initialize the ability table, allocates memory and returns the pointer to the table.
 *
 * @return Pointer to the ability table.
 */
ability_table_t* init_ability_table(void) {
    ability_table_t* table = malloc(sizeof(ability_table_t));
    NULL_PTR_HANDLER(table, "Ability", "Failed to allocate memory for ability table");

    init_ability(table->table[0],"Fireball", 4, 10, D10, MAGICAL);
    init_ability(table->table[1],"Sword Slash", 4, 10, D6, PHYSICAL);
    init_ability(table->table[2],"Bite", 3, 20, D8, PHYSICAL);

    return table;
}

void init_ability(ability_t ability, char* name, const int roll_count, const int accuracy, const dice_size_t dice_size, const damage_type_t damage_type) {
    snprintf(ability.name, sizeof(ability.name), "%s", name);
    ability.roll_count = roll_count;
    ability.accuracy = accuracy;
    ability.dice_size = dice_size;
    ability.damage_type = damage_type;
}


void free_ability_table(ability_table_t* table) {
    if (table != NULL) {
        free(table);
    }
}




