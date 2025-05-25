#ifndef GEAR_H
#define GEAR_H

#include "../combat/ability.h"
#include "../common.h"
#include "../stats/stats.h"
#include "gear_identifier.h"


#define MAX_ABILITY_PER_GEAR 4

typedef enum {
    SLOT_HEAD,
    SLOT_CHEST,
    SLOT_LEGS,
    SLOT_FEET,
    SLOT_HANDS,
    SLOT_NECK,
    SLOT_FINGER_RIGHT,
    SLOT_FINGER_LEFT,
    SLOT_LEFT_HAND, // Weapon slot
    SLOT_RIGHT_HAND,// Weapon slot
    SLOT_BOTH_HANDS,// Weapon slot
    MAX_SLOT
} gear_slot_t;

typedef struct
{
    char local_key[MAX_NAME_LENGTH];
    char* local_name;
    gear_identifier_t gear_identifier;
    gear_slot_t slot;
    stats_t stats;
    defenses_t defenses;// Armor Pieces can have other stats, e.g. +might etc. for now only armor
    int num_abilities;
    ability_t* abilities[MAX_ABILITY_PER_GEAR];
} gear_t;


typedef struct
{
    gear_t* gears[MAX_GEARS];
    int num_gears;
} gear_table_t;

/**
 * @brief Initializes a gear object.
 *
 * This function creates and initializes a new `gear_t` object with the specified parameters.
 * It allocates memory, sets the properties of the object, and links it with the provided abilities.
 *
 * @param memory_pool A pointer to the memory pool used for memory allocation.
 * @param name The name of the gear as a string.
 * @param gear_identifier The unique identifier of the gear of type `gear_identifier_t`.
 * @param slot The slot in which the gear will be equipped, of type `gear_slot_t`.
 * @param stats The base stats of the gear, of type `stats_t`.
 * @param defenses The defense values of the gear, of type `defenses_t`.
 * @param ability_table A pointer to the ability table containing the available abilities.
 * @param abilities An array of ability names to be assigned to the gear.
 * @param num_abilities The number of abilities to be assigned to the gear.
 * @return A pointer to the initialized `gear_t` object or `NULL` if initialization fails.
 */
gear_t* init_gear(memory_pool_t* memory_pool, const char* name, gear_identifier_t gear_identifier, gear_slot_t slot, stats_t stats, defenses_t defenses, ability_table_t* ability_table, ability_names_t* abilities, int num_abilities);
/**
 * @brief Initializes a gear table.
 *
 * This function creates and initializes a `gear_table_t` object by fetching gear data from the database,
 * allocating memory for the table, and populating it with gear objects.
 *
 * @param memory_pool A pointer to the memory pool used for memory allocation.
 * @param db_connection A pointer to the database connection used to fetch gear data.
 * @param ability_table A pointer to the ability table containing the available abilities.
 * @return A pointer to the initialized `gear_table_t` object or `NULL` if initialization fails.
 */
gear_table_t* init_gear_table(memory_pool_t* memory_pool, const db_connection_t* db_connection, ability_table_t* ability_table);
/**
 * @brief Frees the memory allocated for a gear table.
 *
 * This function releases all memory associated with a `gear_table_t` object, including
 * the memory for each individual gear object and the table itself. It ensures that
 * the memory pool and table pointers are not NULL before attempting to free memory.
 *
 * @param memory_pool A pointer to the memory pool used for memory allocation.
 * @param table A pointer to the `gear_table_t` object to be freed.
 */
void free_gear_table(memory_pool_t* memory_pool, gear_table_t* table);
/**
 * @brief Converts a gear slot enumeration value to its string representation.
 *
 * This function takes a `gear_slot_t` enumeration value and returns a human-readable
 * string that represents the corresponding gear slot. If the provided slot does not
 * match any known value, the function returns "Unknown Slot".
 *
 * @param slot The gear slot enumeration value of type `gear_slot_t`.
 * @return A string representing the gear slot. If the slot is invalid, returns "Unknown Slot".
 */
const char* gear_slot_to_string(gear_slot_t slot);


#endif//GEAR_H
