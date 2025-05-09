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
    char name[MAX_NAME_LENGTH];
    gear_identifier_t gear_identifier;
    gear_slot_t slot;
    stats_t stats;
    defenses_t defenses;// Armor Pieces can have other stats, e.g. +might etc. for now only armor
    ability_t* abilities[MAX_ABILITY_PER_GEAR];
} gear_t;


typedef struct
{
    gear_t* gears[MAX_GEARS];
    int num_gears;
} gear_table_t;

gear_t* init_gear(memory_pool_t* memory_pool, const char* name, gear_identifier_t gear_identifier, gear_slot_t slot, stats_t stats, defenses_t defenses, ability_table_t* ability_table, ability_names_t* abilities, int num_abilities);
gear_table_t* init_gear_table(memory_pool_t* memory_pool, const db_connection_t* db_connection, ability_table_t* ability_table);
void free_gear_table(memory_pool_t* memory_pool, gear_table_t* table);


#endif//GEAR_H
