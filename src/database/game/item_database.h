#ifndef ITEM_DATABASE_H
#define ITEM_DATABASE_H

#include "../database.h"
#include "../../item/gear.h"
#include "../../item/potion.h"
#include "../../character/stats.h"

/**
 * To get potion table from the database, we need to define a struct
 * This struct is for the initialization of the potion table
 * The values of the struct corresponds to the init_potion() method in @see{potion.h} & @see{potion.c}
 */
typedef struct potion_init_t {
    int potion_type;// The index of the potion in the table
    char* name;
    int value;
} potion_init_t;

/**
 * Get the potion table from the database
 * @param db_connection Pointer to the database connection
 *
 * @return potion_init_t* pointer to the potion table
 */
potion_init_t* init_potion_table_from_db(const db_connection_t* db_connection);

/**
 * Clean up the potion table
 * Call this function to free the memory allocated for the potion table
 *
 * @param potion_init_table Pointer to the potion table
 * @param db_connection Pointer to the database connection
 */
void free_potion_table_from_db(potion_init_t* potion_init_table, const db_connection_t* db_connection);

/**
 * Count the number of potions in the database
 * @param db_connection Pointer to the database connection
 *
 * @return int number of potions in the database
 */
int count_potions_in_db(const db_connection_t* db_connection);

#endif //ITEM_DATABASE_H