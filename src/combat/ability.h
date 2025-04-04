#ifndef ABILITY_H
#define ABILITY_H

#include "damage.h"

#define ABILITY_TABLE_SIZE 3
//index of the different abilities in the table
#define FIREBALL 0
#define SWORD_SLASH 1
#define BITE 2

typedef struct {
    char name[MAX_NAME_LENGTH];
    int roll_count;
    int accuracy;
    dice_size_t dice_size;
    damage_type_t damage_type;
} ability_t;

typedef struct {
    ability_t table[ABILITY_TABLE_SIZE];
} ability_table_t;


ability_table_t* init_ability_table(void);
void free_ability_table(ability_table_t* table);

#endif //ABILITY_H



