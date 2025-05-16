#ifndef LOOT_GENERATION_H
#define LOOT_GENERATION_H

#include "../character/monster.h"
#include "../item/gear.h"
#include "../item/potion.h"

void generate_loot(character_t* c, gear_table_t* gear_table, potion_table_t* potion_table, int rolls);

#endif// LOOT_GENERATION_H
