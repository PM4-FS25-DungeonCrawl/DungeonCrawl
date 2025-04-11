#ifndef CHARACTER_H
#define CHARACTER_H

#include "stats.h"
#include "../common.h"
#include "../combat/damage.h"
#include "../item/base_item.h"
#include "../item/equipable_item.h"
#include "../combat/ability_fw.h"

#define ABILITY_LIMIT 3
#define EQUIPABLE_ITEM_LIMIT 20
#define USABLE_ITEM_LIMIT 20

typedef enum {
    PLAYER,
    MONSTER,
    BOSS
} character_type_t;

typedef struct character_t {
    character_type_t type;
    char name[MAX_NAME_LENGTH];

    stats_t base_stats;
    stats_t current_stats;

    resources_t max_resources;
    resources_t current_resources;

    damage_resistance_t resistance[DAMAGE_TYPE_COUNT];
    defenses_t defenses;

    ability_t* abilities[ABILITY_LIMIT];
    int ability_count;

    item_t* equipable_items[EQUIPABLE_ITEM_LIMIT];
    int equipable_item_count;

    item_t* usable_items[USABLE_ITEM_LIMIT];
    int usable_item_count;

    equipable_item_t* equipped_items[MAX_SLOT];

    int level;
    int xp;
    int xp_reward;
    int skill_points;
} character_t;

character_t* init_character(character_type_t type, const char *name);
void free_character(character_t* character);

void set_character_stats(character_t* character, int strength, int intelligence, int dexterity, int constitution);
void set_stats(stats_t* stats, int strength, int intelligence, int dexterity, int constitution);
void update_character_resources(resources_t* max_resources, stats_t* base_stats);
void set_character_dmg_modifier(character_t* character, damage_type_t type, int value);

void add_ability(character_t* c, ability_t* ability);

void add_item(character_t* c, item_t* item);
void remove_item(character_t* c, item_t* item);
void add_equipable_item(character_t* c, item_t* item);
void remove_equipable_item(character_t* c, item_t* item);
void add_usable_item(character_t* c, item_t* item);
void remove_usable_item(character_t* c, item_t* item);
void equip_item(character_t* c, equipable_item_t* item);
void unequip_item(character_t* c, gear_slot_t slot);

void reset_current_stats(character_t * character);

void set_level(character_t * character, int level);
void set_xp_reward(character_t * character, int xp_reward);
void set_initial_xp(character_t * character, int xp);

#endif //CHARACTER_H
