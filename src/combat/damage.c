/**
 * @file damage.c
 * @brief Implements damage rolling, hit checks, and damage application logic.
 */
#include "damage.h"

#include "../character/character.h"
#include "../local/local_handler.h"
#include "../logging/logger.h"
#include "ability.h"

#include <stdlib.h>

char** damage_type_names = NULL;

int roll_dice(dice_size_t dice_size);

void update_damage_local(void);

int init_damage_local(void) {
    damage_type_names = malloc(sizeof(char*) * MAX_DAMAGE_TYPES);
    RETURN_WHEN_NULL(damage_type_names, 1, "Damage Local", "Failed to allocate memory for damage type names");
    for (int i = 0; i < MAX_DAMAGE_TYPES; i++) {
        damage_type_names[i] = NULL;
    }

    update_damage_local();
    observe_local(update_damage_local);
    return 0;
}

void shutdown_damage_local(void) {
    if (damage_type_names != NULL) {
        for (int i = 0; i < MAX_DAMAGE_TYPES; i++) {
            if (damage_type_names[i] != NULL) {
                free(damage_type_names[i]);
                damage_type_names[i] = NULL;
            }
        }
        free(damage_type_names);
        damage_type_names = NULL;
    }
}

bool roll_hit(const int attacker_dex, const int defender_dex) {
    const int attacker_roll = roll_dice(D20);
    const int defender_roll = roll_dice(D20);

    return attacker_roll + (attacker_dex / 2) > defender_roll + (defender_dex / 2);
}

int roll_damage(const ability_t* ability) {
    int roll = 0;
    // Roll the dice several times
    for (int i = 0; i < ability->roll_amount; i++) {
        roll += roll_dice(ability->dice_size);
    }
    return roll;
}

int deal_damage(character_t* character, damage_type_t damage_type, const int damage) {
    // TODO critical hits are ignored
    // negative damage resistance leads to more damage
    // damage += character->resistance[damage_type].value;
    // damage -= character->current_stats.armor;

    // Suppress unused parameter warning - damage_type will be used when resistance is implemented
    (void) damage_type;

    if (damage < character->current_resources.health) {
        character->current_resources.health -= damage;
    } else {
        character->current_resources.health = 0;
    }
    return damage;
}

// Helper function to convert dice_size_t to string
const char* dice_size_to_string(const dice_size_t size) {
    switch (size) {
        case D6:
            return "D6";
        case D8:
            return "D8";
        case D10:
            return "D10";
        case D12:
            return "D12";
        case D20:
            return "D20";
        default:
            return "Unknown";
    }
}

const char* damage_type_to_string(const damage_type_t type) {
    const char* str;
    if (type < MAX_DAMAGE_TYPES) {
        if (damage_type_names[type] != NULL) {
            str = damage_type_names[type];
        } else {
            str = "ERROR_GETTING_STR";
        }
    } else {
        str = "UNKNOWN_POTION_TYPE";
    }
    return str;
}

/**
 * @brief Rolls given dice size and returns the result.
 * @param dice_size The dice that is rolled.
 * @return The result of the dice roll.
 */
int roll_dice(const dice_size_t dice_size) {
    /* Seed random number generator */
    return rand() % dice_size + 1;
}

void update_damage_local(void) {
    if (damage_type_names == NULL) return;// module was not initialized

    for (int i = 0; i < MAX_DAMAGE_TYPES; i++) {
        if (damage_type_names[i] != NULL) {
            free(damage_type_names[i]);
        }
    }

    damage_type_names[PHYSICAL] = get_local_string("DAMAGE.TYPE.PHYSICAL");
    damage_type_names[MAGICAL] = get_local_string("DAMAGE.TYPE.MAGICAL");
}
