/**
 * @file level.c
 * @brief Implements level progression and experience point (XP) management for characters in the DungeonCrawl game
 */

#include "level.h"

#include "../logging/logger.h"

/**
 * @brief Calculates the XP required for the next level
 * @param level The current level of the character
 * @return The XP required to reach the next level
 */
int calculate_xp_for_next_level(int level) {
    // TODO: Maybe change to a more complex formula
    return 100 + 10 * level;
}

/**
 * @brief Adds XP to a character and handles level-up if the XP threshold is reached
 * @param player Pointer to the character gaining XP
 * @param xp_earned The amount of XP earned
 */
void add_xp(character_t* player, int xp_earned) {
    player->xp += xp_earned;

    // Check if the player has enough XP to level up
    if (player->xp >= calculate_xp_for_next_level(player->level)) {
        level_up(player);
    }
}

/**
 * @brief Handles the level-up process for a character
 * @param player Pointer to the character leveling up
 */
void level_up(character_t* player) {
    player->level++;
    player->xp -= calculate_xp_for_next_level(player->level - 1);
    player->skill_points += 1;
}
