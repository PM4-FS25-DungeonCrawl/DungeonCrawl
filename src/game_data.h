#ifndef GAME_DATA_H
#define GAME_DATA_H
#include "combat/ability.h"
#include "item/potion.h"

extern ability_table_t *ability_table;
extern character_t *goblin;
extern character_t *player;
extern potion_t *healing_potion;

int init_game_data();

#endif //GAME_DATA_H
