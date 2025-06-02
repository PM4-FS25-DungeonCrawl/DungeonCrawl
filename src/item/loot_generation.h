/**
 * @file loot_generation.h
 * @brief Exposes functions for loot generation.
 */
#ifndef LOOT_GENERATION_H
#define LOOT_GENERATION_H

#include "../character/monster.h"
#include "../item/gear.h"
#include "../item/potion.h"

/**
 * @brief Generates loot for a character by randomly selecting a number of gear and potions.
 *
 * @param character The character to generate loot for.
 * @param gear_table The table containing available gear.
 * @param potion_table The table containing available potions.
 * @param rolls The number of loot rolls to perform.
 */
void generate_loot(character_t* character, gear_table_t* gear_table, potion_table_t* potion_table, int rolls);

#endif// LOOT_GENERATION_H
