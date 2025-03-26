//
// Created by Nicola on 23.03.2025.
//

#ifndef CHARACTER_STATS_H
#define CHARACTER_STATS_H

#include <stdio.h>

// === CONSTANTS ===
#define MAX_ABILITIES 3

// === ENUMS ===

// Enum for Damage Types
typedef enum {
    PHYSICAL,
    MAGICAL
} DamageType;

// === STRUCTS ===

// Struct for Abilities
typedef struct {
    char name[50];
    int damageValue;
    DamageType damageType;
} Ability;

// Struct for Player
typedef struct {
    char name[50];

    int health;
    int armor;
    int accuracy;
    int might;
    int deflection;
    int fortitude;
    int will;

    Ability basicAttack;
    Ability abilities[MAX_ABILITIES];
    int abilityCount;
} Player;

// Struct for Monster
typedef struct {
    char name[50];

    int health;
    int armor;
    int accuracy;
    int might;
    int deflection;
    int fortitude;
    int will;

    Ability basicAttack;
    Ability abilities[MAX_ABILITIES];

    int abilityCount;
} Monster;

// === FUNCTION DECLARATIONS ===
void initAbility(Ability *a, const char *name, int damageValue, DamageType type);

void initPlayer(Player *p, const char *name, int health, int armor, int accuracy, int might, int deflection, int fortitude, int will, Ability basicAttack);

void addAbilityToPlayer(Player *p, Ability ability);

void initMonster(Monster *monster, const char *name, int health, int armor, int accuracy, int might, int deflection, int fortitude, int will, Ability basicAttack);


#endif //CHARACTER_STATS_H
