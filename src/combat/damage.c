#include <stdlib.h>

#include "damage.h"

int roll_dice(const dice_size_t dice_size) {
    // TODO better randomness? (warning message)
    return rand() % dice_size + 1;
}

bool roll_hit(const character_t* defender, const ability_t* ability) {
    const int roll = roll_dice(D20);
    bool hit = false;

    switch (ability->damage_type) {
        case PHYSICAL:
            hit = roll + ability->accuracy > defender->deflection;
            break;
        case MAGICAL:
            hit = roll + ability->accuracy > defender->fortitude;
            break;
    }
    return hit;
}

int roll_damage(const ability_t* ability) {
    int roll = 0;
    // Roll the dice several times
    for (int i = 0; i < ability->roll_amount; i++){
        roll += roll_dice(ability->dice_size);
    }
    return roll;
}
