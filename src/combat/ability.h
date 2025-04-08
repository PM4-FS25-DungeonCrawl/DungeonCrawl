#ifndef ABILITY_H
#define ABILITY_H

#include "../common.h"
#include "damage.h"

//index of the different abilities in the table
typedef enum {
    FIREBALL,
    SWORD_SLASH,
    BITE,
    ABILITIES_SIZE
} abilities_t;

typedef struct ability_t {
    char name[MAX_NAME_LENGTH];
    int roll_amount;
    int accuracy;
    dice_size_t dice_size;
    damage_type_t damage_type;
} ability_t;

typedef struct {
    ability_t table[ABILITIES_SIZE];
} ability_table_t;


ability_table_t* init_ability_table(void);
void free_ability_table(ability_table_t* table);

#endif //ABILITY_H