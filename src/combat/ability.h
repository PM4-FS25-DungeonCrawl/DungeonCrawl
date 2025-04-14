#ifndef ABILITY_H
#define ABILITY_H

#include "../common.h"
#include "damage.h"

//index of the different abilities in the table
typedef enum {
    FIREBALL,
    SWORD_SLASH,
    BITE,
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


ability_table_t* init_ability_table(void);
void free_ability_table(ability_table_t* table);

#endif//ABILITY_H
