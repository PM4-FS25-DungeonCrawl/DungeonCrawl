#ifndef ABILITY_DATABASE_H
#define ABILITY_DATABASE_H

#include "../database.h"
#include "../../combat/ability.h"

/**
 * To get the ability table from the database, we need to define a struct
 * This struct is for the initialization of the ability table
 * The values of the struct corresponds to the init_ability() method in @see{ability.h} & @see{ability.c}
 */
typedef struct ability_init_t {
    int ability_number; // The index of the ability in the table
    char* name;
    int roll_amount;
    int accuracy;
    int resource_cost;
    dice_size_t dice_size;
    damage_type_t damage_type;
} ability_init_t;

/**
 * Get the ability table from the database
 * @param db_connection Pointer to the database connection
 *
 * @return ability_init_t* pointer to the ability table
 */
ability_init_t* get_ability_table_from_db(const db_connection_t* db_connection);

/**
 * Clean up the ability table
 * Call this function to free the memory allocated for the ability table
 */
void free_ability_table(ability_init_t* ability_table);

#endif //ABILITY_DATABASE_H