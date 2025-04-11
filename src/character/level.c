#include "level.h"

#include "../logging/logger.h"

#include <math.h>


int calculate_xp_for_next_level(int level) {
    //TODO Maybe change to a more complex formula
    return (int) (100 + 10 * level);
}

void add_xp(character_t* player, int xp_earned) {
    player->xp += xp_earned;
    log_msg(INFO, "XP", "Player earned %d XP. Total XP: %d", xp_earned, player->xp);

    // Check if the player has enough XP to level up
    while (player->xp >= calculate_xp_for_next_level(player->level)) {
        level_up(player);
    }
}

void level_up(character_t* player) {
    player->level++;
    player->xp -= calculate_xp_for_next_level(player->level - 1);
    player->skill_points += 1;
    log_msg(INFO, "Level", "Player leveled up to level %d!", player->level);
    log_msg(INFO, "Level", "Player has %d XP remaining.", player->xp);
    log_msg(INFO, "Level", "Player has %d skill points to spend.", player->skill_points);
}