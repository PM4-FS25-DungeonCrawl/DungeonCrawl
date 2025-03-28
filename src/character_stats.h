#ifndef CHARACTER_STATS_H
#define CHARACTER_STATS_H

#include <stdio.h>
#include <string.h>
#include "items.h"
// === CONSTANTS ===
#define MAX_ABILITIES 3
#define MAX_WEAKNESSES 2
#define MAX_ITEMS 10

// === ENUMS ===

// Enum for Damage Types
typedef enum {
    PHYSICAL,
    MAGICAL
} DamageType;

// Enum for Character Types
typedef enum {
    PLAYER,
    MONSTER,
    BOSS
} CharacterType;

// ENUM for dice sizes
typedef enum {
    D6 = 6,
    D8 = 8,
    D10 = 10,
    D12 = 12,
    D20 = 20
} DiceSize;

// === STRUCTS ===

// Struct for Weakness
typedef struct {
    DamageType type;
    int value;
} Weakness;

// Struct for Abilities
typedef struct {
    char name[50];
    int rollCount;
    int accuracy;
    DiceSize diceSize;
    DamageType damageType;
} Ability;

// Struct for Character
typedef struct {
    CharacterType type;
    char name[20];

    int health;
    int armor;
    int might;
    int deflection;
    int fortitude;
    int will;

    Ability abilities[MAX_ABILITIES];
    int ability_count;
} Character;

// Struct for Player
typedef struct {
    Character base;
    Item *inventory[MAX_ITEMS];
    int item_count;
} Player;

// Struct for Monster
typedef struct {
    Character base;
    Weakness weakness_map[sizeof(DamageType)];
} Monster;

// === FUNCTION DECLARATIONS ===
void initAbility(Ability *a, const char *name, int rollCount, int accuracy, DiceSize diceSize, DamageType type);
void addAbilityToCharacter(Character *c, Ability ability);
void initCharacter(CharacterType type, Character *c, const char *name, int health, int armor, int might, int deflection, int fortitude, int will);
void initWeaknesses(Monster *m, int array[sizeof(Weakness)]);
int get_weakness_value(Monster *m, DamageType key);
void add_item_to_player(Player *player, Item *item);


#endif //CHARACTER_STATS_H
