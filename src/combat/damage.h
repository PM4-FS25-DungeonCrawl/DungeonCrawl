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

bool roll_hit(int attacker_dex, int defender_dex);
int roll_damage(const ability_t* ability);
int deal_damage(character_t* character, damage_type_t damage_type, int damage);

void reset_current_stats(character_t* character);
const char* dice_size_to_string(dice_size_t size);
const char* damage_type_to_string(damage_type_t type);

#endif//DAMAGE_H
