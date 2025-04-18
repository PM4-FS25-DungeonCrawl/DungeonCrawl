#include "stats.h"
#include "../character/character.h"
#include "../logging/logger.h"




void allocate_skill_points(character_t* player, stat_type_t stat, int points) {
    if (points > player->skill_points) {
        log_msg(INFO, "Stats", "Not enough skill points to allocate.");
        return;
    }

    switch (stat) {
        case STRENGTH:
            player->base_stats.strength += points;
        break;
        case INTELLIGENCE:
            player->base_stats.intelligence += points;
        break;
        case DEXTERITY:
            player->base_stats.dexterity += points;
        break;
        case CONSTITUTION:
            player->base_stats.constitution += points;
        break;
        default:
            log_msg(ERROR, "Stats", "Invalid stat type.");
        return;
    }
    player->skill_points -= points;
    log_msg(INFO, "Stats", "Allocated %d points to %d. Remaining skill points: %d", points, stat, player->skill_points);
    update_character_resources(&player->max_resources, &player->base_stats);
    log_msg(INFO, "Stats", "Updated max resources: Health: %d, Mana: %d, Stamina: %d", player->max_resources.health, player->max_resources.mana, player->max_resources.stamina);
}

