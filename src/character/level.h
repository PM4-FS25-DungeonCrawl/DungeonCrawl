#ifndef XP_LEVEL_H
#define XP_LEVEL_H
#include "character.h"


// Define constants for XP progression
#define BASE_XP 100
#define XP_MULTIPLIER 1.5

// Function prototypes
/**
 * @brief Calculates the XP required for the next level
 * @param level The current level of the character
 * @return The XP required to reach the next level
 */
int calculate_xp_for_next_level(int level);
/**
 * @brief Adds XP to a character and handles level-up if the XP threshold is reached
 * @param player Pointer to the character gaining XP
 * @param xp_earned The amount of XP earned
 */
void add_xp(character_t* player, int xp_earned);
/**
 * @brief Handles the level-up process for a character
 * @param player Pointer to the character leveling up
 */
void level_up(character_t* player);

#endif// XP_LEVEL_H
