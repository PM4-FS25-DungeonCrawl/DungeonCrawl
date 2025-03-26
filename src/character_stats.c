//
// Created by Nicola on 23.03.2025.
//

#include <stdio.h>
#include <string.h>
#include "character_stats.h"


// Function to initialize a Player
void initPlayer(Player *p, const char *name, int health, int armor, int accuracy, int might, int deflection, int fortitude, int will, Ability basicAttack) {
    strncpy(p->name, name, sizeof(p->name) - 1);
    p->name[sizeof(p->name) - 1] = '\0';

    p->health = health;
    p->armor = armor;
    p->accuracy = accuracy;
    p->might = might;
    p->deflection = deflection;
    p->fortitude = fortitude;
    p->will = will;

    p->basicAttack = basicAttack;
    p->abilityCount = 0;
}


// Function to initialize a Monster
void initMonster(Monster *monster, const char *name, int health, int armor, int accuracy, int might, int deflection, int fortitude, int will, Ability basicAttack) {
    strncpy(monster->name, name, sizeof(monster->name) - 1);
    monster->name[sizeof(monster->name) - 1] = '\0';

    monster->health = health;
    monster->armor = armor;
    monster->accuracy = accuracy;
    monster->might = might;
    monster->deflection = deflection;
    monster->fortitude = fortitude;
    monster->will = will;

    monster->basicAttack = basicAttack;
    monster->abilityCount = 0;
}


// Function to initialize an Ability
void initAbility(Ability *a, const char *name, int damageValue, DamageType type) {
    strncpy(a->name, name, sizeof(a->name) - 1);
    a->name[sizeof(a->name) - 1] = '\0';
    a->damageValue = damageValue;
    a->damageType = type;
}

// Function to add an Ability to a Player
void addAbilityToPlayer(Player *p, Ability ability) {
    if (p->abilityCount < MAX_ABILITIES) {
        p->abilities[p->abilityCount++] = ability;
    } else {
        printf("Player %s cannot learn more abilities!\n", p->name);
    }
}
