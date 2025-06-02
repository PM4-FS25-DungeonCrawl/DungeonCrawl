/**
 * @file character.c
 * @brief Implements character functionallity.
 */
#include "character.h"

#include "../combat/ability.h"
#include "../common.h"
#include "../logging/logger.h"

#include <stdio.h>

// Internal functions
/**
 * @brief Sets the stats for a character
 * @param stats Pointer to the stats structure to set
 * @param strength Strength value
 * @param intelligence Intelligence value
 * @param dexterity Dexterity value
 * @param constitution Constitution value
 */
void set_stats(stats_t* stats, int strength, int intelligence, int dexterity, int constitution);

character_t* init_character(memory_pool_t* memory_pool, const character_type_t type, const char* name) {
    NULL_PTR_HANDLER_RETURN(memory_pool, NULL, "Character", "In init_character memory pool is NULL");
    NULL_PTR_HANDLER_RETURN(name, NULL, "Character", "In init_character name is NULL");

    character_t* character = memory_pool_alloc(memory_pool, sizeof(character_t));
    NULL_PTR_HANDLER_RETURN(character, NULL, "Character", "Failed to allocate memory for character: %s", name);

    character->type = type;
    snprintf(character->name, sizeof(character->name), "%s", name);
    character->base_stats = (stats_t) {0};
    character->current_stats = (stats_t) {0};
    character->max_resources = (resources_t) {0};
    character->current_resources = (resources_t) {0};

    for (int i = 0; i < MAX_DAMAGE_TYPE; i++) {
        character->resistance[i].type = i;
        character->resistance[i].value = 0;
    }

    character->defenses = (defenses_t) {0};

    for (int i = 0; i < MAX_ABILITY_LIMIT; i++) {
        character->abilities[i] = NULL;
    }
    character->base_attack = NULL;
    character->ability_count = 0;

    for (int i = 0; i < MAX_POTION_LIMIT; i++) {
        character->potion_inventory[i] = NULL;
    }
    character->potion_count = 0;

    for (int i = 0; i < MAX_GEAR_LIMIT; i++) {
        character->gear_inventory[i] = NULL;
    }
    character->gear_count = 0;

    for (int i = 0; i < MAX_SLOT; i++) {
        character->equipment[i] = NULL;
    }

    character->level = 0;
    character->xp = 0;
    character->xp_reward = 0;
    character->skill_points = 0;
    return character;
}

void free_character(memory_pool_t* memory_pool, character_t* character) {
    NULL_PTR_HANDLER_RETURN(memory_pool, , "Character", "In free_character memory pool is NULL");
    NULL_PTR_HANDLER_RETURN(character, , "Character", "In free_character character is NULL");
    memory_pool_free(memory_pool, character);
}

void set_character_stats(character_t* character, int strength, int intelligence, int dexterity, int constitution) {
    NULL_PTR_HANDLER_RETURN(character, , "Character", "In set_character_stats character is NULL");

    set_stats(&character->base_stats, strength, intelligence, dexterity, constitution);
    character->current_stats = character->base_stats;
    update_character_resources(&character->current_resources, &character->max_resources, &character->base_stats);
    character->current_resources = character->max_resources;
    character->defenses.armor = 0;
    character->defenses.magic_resist = 0;
}

void set_stats(stats_t* stats, int strength, int intelligence, int dexterity, int constitution) {
    NULL_PTR_HANDLER_RETURN(stats, , "Character", "In set_stats stats is NULL");

    stats->strength = strength;
    stats->intelligence = intelligence;
    stats->dexterity = dexterity;
    stats->constitution = constitution;
}

void update_character_resources(resources_t* current_resources, resources_t* max_resources, stats_t* base_stats) {
    NULL_PTR_HANDLER_RETURN(current_resources, , "Character", "In update_character_resources current_resources is NULL");
    NULL_PTR_HANDLER_RETURN(max_resources, , "Character", "In update_character_resources max_resources is NULL");
    NULL_PTR_HANDLER_RETURN(base_stats, , "Character", "In update_character_resources base_stats is NULL");

    int new_max_health = 5 * base_stats->constitution;
    int new_max_mana = 1 * base_stats->intelligence;
    int new_max_stamina = 2 * base_stats->strength;

    if (max_resources->health > 0 && current_resources->health > 0) {
        current_resources->health = (int) ((double) current_resources->health / max_resources->health * new_max_health + 0.5);
    }
    if (max_resources->mana > 0 && current_resources->mana > 0) {
        current_resources->mana = (int) ((double) current_resources->mana / max_resources->mana * new_max_mana + 0.5);
    }
    if (max_resources->stamina > 0 && current_resources->stamina > 0) {
        current_resources->stamina = (int) ((double) current_resources->stamina / max_resources->stamina * new_max_stamina + 0.5);
    }

    // Limit current values to new max values
    current_resources->health = current_resources->health > new_max_health ? new_max_health : current_resources->health;
    current_resources->mana = current_resources->mana > new_max_mana ? new_max_mana : current_resources->mana;
    current_resources->stamina = current_resources->stamina > new_max_stamina ? new_max_stamina : current_resources->stamina;

    max_resources->health = new_max_health;
    max_resources->mana = new_max_mana;
    max_resources->stamina = new_max_stamina;
}

void set_character_dmg_modifier(character_t* character, damage_type_t type, int value) {
    NULL_PTR_HANDLER_RETURN(character, , "Character", "In set_character_dmg_modifier character is NULL");

    if (type >= 0 && type < MAX_DAMAGE_TYPE) {
        character->resistance[type].value = value;
        return;
    }

    log_msg(WARNING, "Character", "Unknown damage type: %d", type);
}

void add_ability(character_t* character, ability_t* ability) {
    NULL_PTR_HANDLER_RETURN(character, , "Character", "In add_ability character is NULL");
    NULL_PTR_HANDLER_RETURN(ability, , "Character", "In add_ability ability is NULL");

    if (character->ability_count < MAX_ABILITY_LIMIT) {
        character->abilities[character->ability_count] = ability;
        character->ability_count++;
    } else {
        log_msg(WARNING, "Character", "%s cannot learn more abilities!", character->name);
    }
}

void remove_ability(character_t* character, const ability_t* ability) {
    NULL_PTR_HANDLER_RETURN(character, , "Character", "In remove_ability character is NULL");
    NULL_PTR_HANDLER_RETURN(ability, , "Character", "In remove_ability ability is NULL");

    for (int i = 0; i < character->ability_count; i++) {
        if (character->abilities[i] == ability) {
            for (int j = i; j < character->ability_count - 1; j++) {
                character->abilities[j] = character->abilities[j + 1];
            }
            character->abilities[character->ability_count - 1] = NULL;
            character->ability_count--;

            break;
        }
    }
}

void add_gear(character_t* character, gear_t* gear) {
    NULL_PTR_HANDLER_RETURN(character, , "Character", "In add_gear character is NULL");
    NULL_PTR_HANDLER_RETURN(gear, , "Character", "In add_gear gear is NULL");

    if (character->gear_count < MAX_GEAR_LIMIT) {
        character->gear_inventory[character->gear_count] = gear;
        character->gear_count++;

    } else {
        log_msg(WARNING, "Character", "%s cannot carry more gear!", character->name);
    }
}

void remove_gear(character_t* character, gear_t* gear) {
    NULL_PTR_HANDLER_RETURN(character, , "Character", "In remove_gear character is NULL");
    NULL_PTR_HANDLER_RETURN(gear, , "Character", "In remove_gear gear is NULL");

    for (int i = 0; i < character->gear_count; i++) {
        if (character->gear_inventory[i] == gear) {
            for (int j = i; j < character->gear_count - 1; j++) {
                character->gear_inventory[j] = character->gear_inventory[j + 1];
            }
            character->gear_inventory[character->gear_count - 1] = NULL;
            character->gear_count--;

            return;
        }
    }
    log_msg(WARNING, "Character", "Gear %s not found in inventory!", gear->local_key);
}

void remove_equipped_gear(character_t* character, gear_slot_t slot) {
    NULL_PTR_HANDLER_RETURN(character, , "Character", "In remove_equipped_gear character is NULL");
    CHECK_ARG_RETURN(slot < 0 && slot >= MAX_SLOT, , "Character", "In remove_equipped_gear slot is invalid: %d", slot);

    if (character->equipment[slot] != NULL) {
        gear_t* gear = character->equipment[slot];

        character->base_stats.strength -= gear->stats.strength;
        character->base_stats.intelligence -= gear->stats.intelligence;
        character->base_stats.dexterity -= gear->stats.dexterity;
        character->base_stats.constitution -= gear->stats.constitution;
        character->defenses.armor -= gear->defenses.armor;
        character->defenses.magic_resist -= gear->defenses.magic_resist;
        update_character_resources(&character->current_resources, &character->max_resources, &character->base_stats);

        for (int i = 0; i < gear->num_abilities; ++i) {
            remove_ability(character, gear->abilities[i]);
        }

        if (character->ability_count == 0) {
            add_ability(character, character->base_attack);
        }

        character->equipment[slot] = NULL;
    } else {
        log_msg(WARNING, "Character", "No gear equipped in slot %d!", slot);
    }
}

bool add_equipped_gear(character_t* character, gear_t* gear) {
    NULL_PTR_HANDLER_RETURN(character, false, "Character", "In add_equipped_gear character is NULL");
    NULL_PTR_HANDLER_RETURN(gear, false, "Character", "In add_equipped_gear gear is NULL");

    if (gear->slot < MAX_SLOT) {
        if (character->equipment[gear->slot] != NULL) {
            log_msg(WARNING, "Character", "Slot %d is already occupied!", gear->slot);
            return false;
        }

        character->equipment[gear->slot] = gear;
        if (character->abilities[0] == character->base_attack) {
            remove_ability(character, character->abilities[0]);
        }

        for (int i = 0; i < gear->num_abilities; ++i) {
            add_ability(character, gear->abilities[i]);
        }

        return true;
    }

    log_msg(WARNING, "Character", "Invalid slot for gear %s!", gear->local_key);
    return false;
}

void add_potion(character_t* character, potion_t* potion) {
    NULL_PTR_HANDLER_RETURN(character, , "Character", "In add_potion character is NULL");
    NULL_PTR_HANDLER_RETURN(potion, , "Character", "In add_potion potion is NULL");

    if (character->potion_count < MAX_POTION_LIMIT) {
        character->potion_inventory[character->potion_count] = potion;
        character->potion_count++;
    } else {
        log_msg(WARNING, "Character", "%s cannot carry more potions!", character->name);
    }
}

void remove_potion(character_t* character, potion_t* potion) {
    NULL_PTR_HANDLER_RETURN(character, , "Character", "In remove_potion character is NULL");
    NULL_PTR_HANDLER_RETURN(potion, , "Character", "In remove_potion potion is NULL");

    for (int i = 0; i < character->potion_count; i++) {
        if (character->potion_inventory[i] == potion) {
            for (int j = i; j < character->potion_count - 1; j++) {
                character->potion_inventory[j] = character->potion_inventory[j + 1];
            }
            character->potion_inventory[character->potion_count - 1] = NULL;
            character->potion_count--;
            return;
        }
    }
    log_msg(WARNING, "Character", "Potion %s not found in inventory!", potion->name);
}


void equip_gear(character_t* character, gear_t* gear) {
    NULL_PTR_HANDLER_RETURN(character, , "Character", "In equip_gear character is NULL");
    NULL_PTR_HANDLER_RETURN(gear, , "Character", "In equip_gear gear is NULL");

    if (add_equipped_gear(character, gear)) {
        remove_gear(character, gear);

        character->base_stats.strength += gear->stats.strength;
        character->base_stats.intelligence += gear->stats.intelligence;
        character->base_stats.dexterity += gear->stats.dexterity;
        character->base_stats.constitution += gear->stats.constitution;
        character->defenses.armor += gear->defenses.armor;
        character->defenses.magic_resist += gear->defenses.magic_resist;
        update_character_resources(&character->current_resources, &character->max_resources, &character->base_stats);

    } else {
        log_msg(WARNING, "Character", "Invalid slot for gear %s!", gear->local_key);
    }
}

void unequip_gear(character_t* character, const gear_slot_t slot) {
    NULL_PTR_HANDLER_RETURN(character, , "Character", "In unequip_gear character is NULL");
    CHECK_ARG_RETURN(slot < 0 && slot >= MAX_SLOT, , "Character", "In unequip_gear slot is invalid: %d", slot);

    add_gear(character, character->equipment[slot]);
    remove_equipped_gear(character, slot);
}

void set_initial_xp(character_t* character, int xp) {
    NULL_PTR_HANDLER_RETURN(character, , "Character", "In set_initial_xp character is NULL");
    character->xp = xp;
}

void set_level(character_t* character, int level) {
    NULL_PTR_HANDLER_RETURN(character, , "Character", "In set_level character is NULL");
    character->level = level;
}

void set_xp_reward(character_t* character, int xp_reward) {
    NULL_PTR_HANDLER_RETURN(character, , "Character", "In set_xp_reward character is NULL");
    character->xp_reward = xp_reward;
}

void set_skill_points(character_t* character, int skill_points) {
    NULL_PTR_HANDLER_RETURN(character, , "Character", "In set_skill_points character is NULL");
    character->skill_points = skill_points;
}

void reset_current_stats(character_t* character) {
    NULL_PTR_HANDLER_RETURN(character, , "Character", "In reset_current_stats character is NULL");
    character->current_stats = character->base_stats;
}

void reset_current_resources(character_t* character) {
    NULL_PTR_HANDLER_RETURN(character, , "Character", "In reset_current_resources character is NULL");
    character->current_resources = character->max_resources;
}

void reset_player_stats(character_t* character) {
    NULL_PTR_HANDLER_RETURN(character, , "Character", "In reset_current_resources character is NULL");

    // reset current stats to their starting values
    reset_current_stats(character);
    reset_current_resources(character);
}
