#ifndef STATS_H
#define STATS_H

#include "../common.h"

typedef enum {
    STRENGTH,
    INTELLIGENCE,
    DEXTERITY,
    CONSTITUTION,
    MAX_STATS// Used to see how many stats there are
} stat_type_t;

typedef struct {
    int strength;    // 1 strength = 1 physical damage and 1 stamina
    int intelligence;// 1 intelligence = 1 magic damage and 1 mana
    int dexterity;   // Global accuracy and evasion NOT IMPLEMENTED
    int constitution;// 1 constitution = 5 health
} stats_t;

typedef struct {
    int health;
    int mana;
    int stamina;
} resources_t;

typedef struct {
    int armor;       // reduced physical damage taken
    int magic_resist;// reduced magical damage taken
} defenses_t;

void raise_skill(stats_t* stats, stat_type_t stat, int skillpoint);

#endif//STATS_H
