/**
 * @file potion.h
 * @brief Exposes functions for working with potions.
 */
#ifndef POTION_H
#define POTION_H

#include "../common.h"
#include "../database/database.h"
#include "../memory/memory_management.h"

typedef enum {
    HEALING,
    MANA,
    STAMINA,
    MAX_POTION_TYPES
} potion_type_t;

typedef struct {
    char name[MAX_NAME_LENGTH];
    potion_type_t effectType;
    int value;// e.g. value = 30, healing potion heals for 30 health
    // for other item effects more values might be needed
} potion_t;

typedef struct {
    potion_t potions[MAX_POTION_TYPES];
} potion_table_t;

/**
 * @brief creates a potion object with the given name, type and value
 *
 * @param potion Potion object to be initialized
 * @param name Name of the potion
 * @param type Type defines the effect of the potion
 * @param value Value defining the strength of the effect
 */
potion_t* init_potion(potion_t* potion, const char* name, potion_type_t type, int value);

/**
 * @brief initializes a potion table with potions from the database
 *
 * @param memory_pool Memory pool to allocate the potion table
 * @param db_connection Database connection to fetch the potions from
 * @return Pointer to the initialized potion table
 */
potion_table_t* init_potion_table(memory_pool_t* memory_pool, const db_connection_t* db_connection);

/**
 * @brief Converts a potion type to a string representation
 *
 * @param type The potion type to convert
 * @return A string representation of the potion type
 */
const char* potion_type_to_string(potion_type_t type);

/**
 * @brief Frees the memory allocated for a potion
 *
 * @param memory_pool Memory pool to free the potion
 * @param potion Potion object to be freed
 */
void free_potion(memory_pool_t* memory_pool, potion_t* potion);

/**
 * @brief Frees the memory allocated for a potion table
 *
 * @param memory_pool Memory pool to free the potion table
 * @param table Potion table to be freed
 */
void free_potion_table(memory_pool_t* memory_pool, potion_table_t* table);

#endif//POTION_H
