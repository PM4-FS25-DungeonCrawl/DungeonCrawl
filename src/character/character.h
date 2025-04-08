#ifndef CHARACTER_H
#define CHARACTER_H

#include "../common.h"
#include "../combat/damage.h"
#include "../item/base_item.h"
#include "../combat/ability_fw.h"

#define ABILITY_LIMIT 3
#define ITEM_LIMIT 10

typedef enum {
    PLAYER,
    MONSTER,
    BOSS
} character_type_t;

typedef struct {
    int health;
    int armor;
    int might;
    int deflection;
    int fortitude;
    int will;
} stats_t;

typedef struct character_t {
    character_type_t type;
    char name[MAX_NAME_LENGTH];

    stats_t base_stats;
    stats_t current_stats;

    damage_resistance_t resistance[DAMAGE_TYPE_COUNT];

    ability_t* abilities[ABILITY_LIMIT];
    int ability_count;

    item_t* items[ITEM_LIMIT];
    int item_count;
} character_t;

character_t* init_character(character_type_t type, const char *name);
void free_character(character_t* character);

void set_character_stats(character_t* character, int health, int armor, int might, int deflection, int fortitude, int will);
void set_character_dmg_modifier(character_t* character, damage_type_t type, int value);

void add_ability(character_t* c, ability_t* ability);

void add_item(character_t* c, item_t* item);
void remove_item(character_t* c, item_t* item);
bool use_usable_item(character_t* character, item_t* item);

int deal_damage(character_t* character, damage_type_t damage_type, int damage);

void reset_current_stats(character_t * character);

#endif //CHARACTER_H
