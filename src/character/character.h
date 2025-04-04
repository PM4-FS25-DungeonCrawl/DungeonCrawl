#ifndef CHARACTER_H
#define CHARACTER_H

#include "src/combat/ability.h"
#include "src/common.h"
#include "src/item/base_item.h"

#define ABILITY_LIMIT 3
#define ITEM_LIMIT 10

typedef enum {
    PLAYER,
    MONSTER,
    BOSS
} character_type_t;

typedef struct {
    character_type_t type;
    char name[MAX_NAME_LENGTH];

    int health;
    int armor;
    int might;
    int deflection;
    int fortitude;
    int will;

    damage_modifier_t dmg_modifier[DAMAGE_TYPE_COUNT];

    ability_t* abilities[ABILITY_LIMIT];
    int ability_count;

    item_t* items[ITEM_LIMIT];
    int item_count;
} character_t;

character_t* init_character(character_type_t type, const char *name);
void free_character(character_t* character);

void set_character_stats(character_t* character, int health, int armor, int might, int deflection, int fortitude, int will);
void set_character_dmg_modifier(character_t* character, damage_type_t type, int value);

void add_ability_to_character(character_t* c, ability_t* ability);

void add_item_to_character(character_t* c, item_t* item);
void remove_item_from_character(character_t* c, item_t* item);

#endif //CHARACTER_H
