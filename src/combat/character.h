#ifndef CHARACTER_H
#define CHARACTER_H

#include "ability.h"
#include "combat_common.h"

#define MAX_ABILITIES 3

typedef enum {
    PLAYER,
    MONSTER,
    BOSS
} character_type_t;

typedef struct {
    character_type_t type;
    char name[MAX_STRING_LENGTH];

    int health;
    int armor;
    int might;
    int deflection;
    int fortitude;
    int will;

    ability_t* abilities[MAX_ABILITIES];
    int ability_count;
} character_t;

character_t* init_character(character_type_t type, const char *name, int health, int armor, int might, int deflection, int fortitude, int will);
void free_character(character_t* character);

void add_ability_to_character(character_t* c, ability_t* ability);

#endif //CHARACTER_H
