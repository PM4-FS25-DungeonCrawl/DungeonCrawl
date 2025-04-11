#include "damage.h"

#include "../character/character.h"
#include "../logging/logger.h"
#include "ability.h"

#include <stdlib.h>

int roll_dice(const dice_size_t dice_size) {
    // TODO better randomness? (warning message)
    /* Seed random number generator */
    return rand() % dice_size + 1;
}

bool roll_hit(int attacker_dex, int defender_dex) {
    const int attacker_roll = roll_dice(D20);
    log_msg(INFO, "Attacker", "rolled %d", attacker_roll);
    const int defender_roll = roll_dice(D20);
    log_msg(INFO, "Defender", "rolled %d", defender_roll);
    bool hit = false;

    return attacker_roll + (attacker_dex / 2) > defender_roll + (defender_dex / 2);
}

int roll_damage(const ability_t* ability) {
    int roll = 0;
    // Roll the dice several times
    for (int i = 0; i < ability->roll_amount; i++) {
        roll += roll_dice(ability->dice_size);
    }
    log_msg(INFO, "ability:", "%s rolled %d damage", ability->name, roll);
    return roll;
}

int deal_damage(character_t* character, damage_type_t damage_type, int damage) {
    // TODO critical hits are ignored
    // negative damage resistance leads to more damage
    // damage += character->resistance[damage_type].value;
    // damage -= character->current_stats.armor;
    if (damage > 0) character->current_resources.health -= damage;
    return damage;
}

void reset_current_stats(character_t* character) {
    character->current_stats = character->base_stats;
}