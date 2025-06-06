/**
 * @file level.c
 * @brief Implements level progression and experience point (XP) management for characters in the DungeonCrawl game
 */

#include "level.h"

#include "../logging/logger.h"


int calculate_xp_for_next_level(int level) {
    // TODO: Maybe change to a more complex formula
    return 100 + 10 * level;
}


void add_xp(character_t* player, int xp_earned) {
    player->xp += xp_earned;

    // Check if the player has enough XP to level up
    if (player->xp >= calculate_xp_for_next_level(player->level)) {
        level_up(player);
    }
}


void level_up(character_t* player) {
    player->level++;
    player->xp -= calculate_xp_for_next_level(player->level - 1);
    player->skill_points += 1;
}
