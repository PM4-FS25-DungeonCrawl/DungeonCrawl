#ifndef ABILITY_H
#define ABILITY_H

#include "../common.h"
#include "../memory/memory_management.h"
#include "damage.h"

//index of the different abilities in the table
typedef enum {
    SWORD_SLASH,
    BITE,
    QUICK_SLASH,
    HEAD_CHOP,
    QUICK_SHOT,
    FIREBLAST,
    BACKSTAB,
    DEFLECT,
    MACE_STRIKE,
    ARCANE_BOLT,
    HEAVY_SWING,
    PIERCING_STRIKE,
    POWER_SHOT,
    FIREBALL,
    SINISTER_STRIKE,
    SHIELDWALL,
    CRUSHING_BLOW,
    ARCANE_MISSILE,
    RIPOSTE,
    SWEEPING_STRIKE,
    EXECUTE,
    STEADY_SHOT,
    PYROBLAST,
    GUARDING_STANCE,
    BERSERKER_RAGE,
    STEADY_AIM,
    MANA_SHIELD,
    CHOP,
    AXE_SWING,
    SHIELD_WALL,
    MAX_ABILITIES
} ability_names_t;

typedef struct ability_t {
    char name[MAX_NAME_LENGTH];
    int roll_amount;
    int accuracy;
    int resource_cost;
    dice_size_t dice_size;
    damage_type_t damage_type;
} ability_t;

typedef struct {
    ability_t abilities[MAX_ABILITIES];
} ability_table_t;

/**
 * Initialize the ability table, allocates memory and returns the pointer to the table.
 *
 * @param memory_pool Pointer to the memory pool for allocation.
 * @return Pointer to the ability table.
 */
ability_table_t* init_ability_table(memory_pool_t* memory_pool);
/**
 * Free the ability table, deallocates memory in the memory pool.
 *
 * @param memory_pool Pointer to the memory pool for deallocation.
 * @param table Pointer to the ability table to be freed.
 */
void free_ability_table(memory_pool_t* memory_pool, ability_table_t* table);

#endif//ABILITY_H
