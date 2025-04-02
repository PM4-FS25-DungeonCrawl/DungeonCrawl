#ifndef MONSTER_H
#define MONSTER_H

#include "damage.h"
#include "character.h"

typedef struct {
    character_t* base;
    weakness_t* weakness_table[DAMAGE_TYPE_COUNT];
} monster_t;

monster_t* init_goblin(void);
void free_goblin(monster_t* goblin);

#endif //MONSTER_H
