#ifndef CHARACTER_H
#define CHARACTER_H

#include "../combat/ability_fw.h"
#include "../combat/damage.h"
#include "../common.h"
#include "../item/gear.h"
#include "../item/potion.h"
#include "../memory/memory_management.h"
#include "../stats/stats.h"

#define MAX_ABILITY_LIMIT 20
#define MAX_GEAR_LIMIT 20
#define MAX_POTION_LIMIT 20

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

    ability_t* abilities[MAX_ABILITY_LIMIT];
    int ability_count;

    potion_t* potion_inventory[MAX_POTION_LIMIT];
    int potion_count;

    gear_t* gear_inventory[MAX_GEAR_LIMIT];
    int gear_count;

    gear_t* equipment[MAX_SLOT];

    int level;
    int xp;
    int xp_reward;
    int skill_points;
} character_t;

character_t* init_character(memory_pool_t* memory_pool, character_type_t type, const char* name);
void free_character(memory_pool_t* memory_pool, character_t* character);

void set_character_stats(character_t* character, int strength, int intelligence, int dexterity, int constitution);
void set_stats(stats_t* stats, int strength, int intelligence, int dexterity, int constitution);
void update_character_resources(resources_t* current_resources, resources_t* max_resources, stats_t* base_stats);
void set_character_dmg_modifier(character_t* character, damage_type_t type, int value);

void add_ability(character_t* c, ability_t* ability);
void remove_ability(character_t* c, const ability_t* ability);

void add_potion(character_t* c, potion_t* potion);
void remove_potion(character_t* c, potion_t* potion);

void add_gear(character_t* c, gear_t* gear);
void remove_gear(character_t* c, gear_t* gear);
void remove_equipped_gear(character_t* c, gear_slot_t slot);
void equip_gear(character_t* c, gear_t* gear);
void unequip_gear(character_t* c, gear_slot_t slot);

void reset_current_stats(character_t* character);

void set_level(character_t* character, int level);
void set_xp_reward(character_t* character, int xp_reward);
void set_initial_xp(character_t* character, int xp);
void set_skill_points(character_t* character, int skill_points);

void reset_player_stats(character_t* player);

#endif//CHARACTER_H
