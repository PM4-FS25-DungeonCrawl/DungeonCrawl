#ifndef CHARACTER_STATS_H
#define CHARACTER_STATS_H

#include <stdio.h>
#include <string.h>
#include "items.h"
// === CONSTANTS ===
#define MAX_ABILITIES 3
#define MAX_ITEMS 10

// === ENUMS ===

// Enum for Damage Types
typedef enum {
    PHYSICAL,
    MAGICAL,
    DAMAGE_TYPE_COUNT
} damage_type;

// Enum for Character Types
typedef enum {
    PLAYER,
    MONSTER,
    BOSS
} character_type;

// ENUM for dice sizes
typedef enum {
    D6 = 6,
    D8 = 8,
    D10 = 10,
    D12 = 12,
    D20 = 20
} dice_size;

// === STRUCTS ===

// Struct for Weakness
typedef struct {
    damage_type type;
    int value;
} weakness;

// Struct for Abilities
typedef struct {
    char name[50];
    int rollCount;
    int accuracy;
    dice_size diceSize;
    damage_type damageType;
} ability;

// Struct for Character
typedef struct {
    character_type type;
    char name[20];

    int health;
    int armor;
    int might;
    int deflection;
    int fortitude;
    int will;

    ability abilities[MAX_ABILITIES];
    int ability_count;
} character;

// Struct for Player
typedef struct {
    character base;
    Item *inventory[MAX_ITEMS];
    int item_count;
} player;

// Struct for Monster
typedef struct {
    character base;
    weakness weakness_map[DAMAGE_TYPE_COUNT];
} monster;

// === FUNCTION DECLARATIONS ===
void initAbility(ability *a, const char *name, int rollCount, int accuracy, dice_size diceSize, damage_type type);
void addAbilityToCharacter(character *c, ability ability);
void initCharacter(character_type type, character *c, const char *name, int health, int armor, int might, int deflection, int fortitude, int will);
void initWeaknesses(monster *m, int array[]);
int get_weakness_value(monster *m, damage_type key);
void add_item_to_player(player *player, Item *item);


#endif //CHARACTER_STATS_H
