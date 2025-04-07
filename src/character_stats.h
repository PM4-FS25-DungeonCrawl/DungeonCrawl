#ifndef CHARACTER_STATS_H
#define CHARACTER_STATS_H

#include "items.h"
#include <stdio.h>
#include <string.h>
// === CONSTANTS ===
#define MAX_ABILITIES 3
#define MAX_ITEMS 10

// === ENUMS ===

// Enum for Damage Types
typedef enum {
	PHYSICAL,
	MAGICAL,
	DAMAGE_TYPE_COUNT
} damage_type_t;

// Enum for Character Types
typedef enum {
	PLAYER,
	MONSTER,
	BOSS
} character_type_t;

// ENUM for dice sizes
typedef enum {
	D6 = 6,
	D8 = 8,
	D10 = 10,
	D12 = 12,
	D20 = 20
} dice_size_t;

// === STRUCTS ===

// Struct for Weakness
typedef struct {
	damage_type_t type;
	int value;
} weakness_t;

// Struct for Abilities
typedef struct {
	char name[50];
	int rollCount;
	int accuracy;
	dice_size_t diceSize;
	damage_type_t damageType;
} ability_t;

// Struct for Character
typedef struct {
	character_type_t type;
	char name[20];

	int health;
	int armor;
	int might;
	int deflection;
	int fortitude;
	int will;

	ability_t abilities[MAX_ABILITIES];
	int ability_count;
} character_t;

// Struct for Player
typedef struct {
	character_t base;
	Item* inventory[MAX_ITEMS];
	int item_count;
} player_t;

// Struct for Monster
typedef struct {
	character_t base;
	weakness_t weakness_map[DAMAGE_TYPE_COUNT];
} monster_t;

// === FUNCTION DECLARATIONS ===
void initAbility(ability_t* a, const char* name, int rollCount, int accuracy, dice_size_t diceSize, damage_type_t type);
void addAbilityToCharacter(character_t* c, ability_t ability);
void initCharacter(character_type_t type, character_t* c, const char* name, int health, int armor, int might, int deflection, int fortitude, int will);
void initWeaknesses(monster_t* m, int array[]);
int get_weakness_value(monster_t* m, damage_type_t key);
void add_item_to_player(player_t* player, Item* item);


#endif//CHARACTER_STATS_H
