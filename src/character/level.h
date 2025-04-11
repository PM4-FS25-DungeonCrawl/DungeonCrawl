#ifndef XP_LEVEL_H
#define XP_LEVEL_H
#include "character.h"


// Define constants for XP progression
#define BASE_XP 100
#define XP_MULTIPLIER 1.5

// Function prototypes
int calculate_xp_for_next_level(int level);
void add_xp(character_t *player, int xp_earned);
void level_up(character_t *player);

#endif // XP_LEVEL_H
