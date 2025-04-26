/**
 * @file level.c
 * @brief Implements level progression and experience point (XP) management for characters in the DungeonCrawl game
 */

#include "level.h"

#include "../logging/logger.h"

#include <math.h>

/**
 * @brief Calculates the XP required for the next level
 * @param level The current level of the character
 * @return The XP required to reach the next level
 */
int calculate_xp_for_next_level(int level) {
    // TODO: Maybe change to a more complex formula
    return (int) (100 + 10 * level);
}

/**
 * @brief Adds XP to a character and handles level-up if the XP threshold is reached
 * @param player Pointer to the character gaining XP
 * @param xp_earned The amount of XP earned
 */
void add_xp(character_t* player, int xp_earned) {
    player->xp += xp_earned;
    log_msg(INFO, "XP", "Player earned %d XP. Total XP: %d", xp_earned, player->xp);

    // Check if the player has enough XP to level up
    while (player->xp >= calculate_xp_for_next_level(player->level)) {
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
    log_msg(INFO, "Level", "Player leveled up to level %d!", player->level);
    log_msg(INFO, "Level", "Player has %d XP remaining.", player->xp);
    log_msg(INFO, "Level", "Player has %d skill points to spend.", player->skill_points);
}
