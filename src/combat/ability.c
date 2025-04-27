#include "ability.h"

#include <stdio.h>

void init_ability(ability_t* ability, char* name, int roll_amount, int accuracy, int resource_cost, dice_size_t dice_size, damage_type_t damage_type);

ability_table_t* init_ability_table(memory_pool_t* memory_pool) {
    NULL_PTR_HANDLER_RETURN(memory_pool, NULL, "Ability", "Memory pool is NULL");

    ability_table_t* table = memory_pool_alloc(memory_pool, sizeof(ability_table_t));
    NULL_PTR_HANDLER_RETURN(table, NULL, "Ability", "Failed to allocate memory for ability table");


    init_ability(&table->abilities[BITE], "Bite", 3, 20, 2, D8, PHYSICAL);
    init_ability(&table->abilities[QUICK_SLASH], "Quick Slash", 1, 10, 0, D6, PHYSICAL);
    init_ability(&table->abilities[HEAVY_SWING], "Heavy Swing", 3, 20, 2, D10, PHYSICAL);
    init_ability(&table->abilities[SWEEPING_STRIKE], "Sweeping Strike", 2, 15, 2, D8, PHYSICAL);
    init_ability(&table->abilities[HEAD_CHOP], "Head Chop", 1, 12, 0, D8, PHYSICAL);
    init_ability(&table->abilities[PIERCING_STRIKE], "Piercing Strike", 2, 18, 2, D8, PHYSICAL);
    init_ability(&table->abilities[EXECUTE], "Execute", 3, 25, 3, D10, PHYSICAL);
    init_ability(&table->abilities[QUICK_SHOT], "Quick Shot", 1, 10, 0, D6, PHYSICAL);
    init_ability(&table->abilities[POWER_SHOT], "Power Shot", 3, 22, 2, D8, PHYSICAL);
    init_ability(&table->abilities[STEADY_SHOT], "Steady Shot", 2, 15, 1, D8, PHYSICAL);
    init_ability(&table->abilities[FIREBLAST], "Fireblast", 1, 12, 1, D6, MAGICAL);
    init_ability(&table->abilities[FIREBALL], "Fireball", 3, 25, 2, D8, MAGICAL);
    init_ability(&table->abilities[PYROBLAST], "Pyroblast", 3, 30, 3, D10, MAGICAL);
    init_ability(&table->abilities[CHOP], "Chop", 1, 10, 0, D6, PHYSICAL);
    init_ability(&table->abilities[AXE_SWING], "Axe Swing", 2, 16, 2, D8, PHYSICAL);
    init_ability(&table->abilities[BACKSTAB], "Backstab", 1, 12, 0, D6, PHYSICAL);
    init_ability(&table->abilities[SINISTER_STRIKE], "Sinister Strike", 2, 18, 2, D6, PHYSICAL);
    init_ability(&table->abilities[MACE_STRIKE], "Mace Strike", 1, 12, 0, D8, PHYSICAL);
    init_ability(&table->abilities[CRUSHING_BLOW], "Crushing Blow", 3, 22, 2, D10, PHYSICAL);
    init_ability(&table->abilities[ARCANE_BOLT], "Arcane Bolt", 1, 10, 0, D6, MAGICAL);
    init_ability(&table->abilities[ARCANE_MISSILE], "Arcane Missile", 2, 18, 2, D8, MAGICAL);
    init_ability(&table->abilities[SWORD_SLASH], "Sword Slash", 1, 10, 0, D6, PHYSICAL);
    init_ability(&table->abilities[RIPOSTE], "Riposte", 2, 16, 2, D8, PHYSICAL);


    // ALL BUFFING/DEFENCE ABilities, not correctly implemented
    init_ability(&table->abilities[DEFLECT], "Deflect", 1, 0, 0, D6, PHYSICAL);
    init_ability(&table->abilities[SHIELD_WALL], "Shield Wall", 3, 0, 0, D6, PHYSICAL);
    init_ability(&table->abilities[GUARDING_STANCE], "Guarding Stance", 2, 0, 0, D6, PHYSICAL);
    init_ability(&table->abilities[BERSERKER_RAGE], "Berserker Rage", 2, 0, 0, D6, PHYSICAL);
    init_ability(&table->abilities[STEADY_AIM], "Steady Aim", 2, 0, 0, D6, PHYSICAL);
    init_ability(&table->abilities[MANA_SHIELD], "Mana Shield", 2, 0, 0, D6, MAGICAL);


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
