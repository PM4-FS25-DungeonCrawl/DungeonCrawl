#ifndef DAMAGE_H
#define DAMAGE_H

#include "../character/character_fw.h"
#include "ability_fw.h"

#define DAMAGE_TYPE_COUNT 2

typedef enum damage_type_t {
    PHYSICAL,
    MAGICAL
} damage_type_t;

typedef enum dice_size_t {
    D6 = 6,
    D8 = 8,
    D10 = 10,
    D12 = 12,
    D20 = 20
} dice_size_t;

typedef struct damage_resistance_t {
    damage_type_t type;
    int value;
} damage_resistance_t;

/**
 * @brief Rolls a D20 to determine if an attack hits.
 * @param attacker_dex The dexterity of the attacker.
 * @param defender_dex The dexterity of the defender.
 * @return The result of the dice roll.
 */
bool roll_hit(int attacker_dex, int defender_dex);
/**
 * @brief Rolls damage based on the ability's roll amount and dice size.
 * @param ability Pointer to the ability used for rolling damage.
 * @return The total damage rolled.
 */
int roll_damage(const ability_t* ability);
/**
 * @brief Deals damage to a character based on the damage type and amount.
 * @param character Pointer to the character receiving damage.
 * @param damage_type The type of damage being dealt.
 * @param damage The amount of damage to be dealt.
 * @return The actual damage dealt.
 */
int deal_damage(character_t* character, damage_type_t damage_type, int damage);

/**
 * @brief Resets the current stats of a character to their base stats.
 * @param character Pointer to the character whose stats are to be reset.
 */
void reset_current_stats(character_t* character);
/**
 * @brief Converts a dice size enum to a string representation.
 * @param size The dice size to convert.
 * @return The string representation of the dice size.
 */
const char* dice_size_to_string(dice_size_t size);

/**
 * @brief Converts a damage type enum to a string representation.
 * @param type The damage type to convert.
 * @return The string representation of the damage type.
 */
const char* damage_type_to_string(damage_type_t type);

#endif//DAMAGE_H
