/**
 * @file stats.h
 * @brief Exposes functions for working with stats.
 */
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

/**
 * @brief Raises a specified skill by one point.
 *
 * This function increases the value of the specified skill in the stats structure
 * by one, provided that the skill point is valid.
 *
 * @param stats Pointer to the stats structure.
 * @param stat The type of stat to raise (e.g., STRENGTH, INTELLIGENCE).
 * @param skillpoint The number of skill points available for allocation.
 */
void raise_skill(stats_t* stats, stat_type_t stat, int skillpoint);

#endif//STATS_H
