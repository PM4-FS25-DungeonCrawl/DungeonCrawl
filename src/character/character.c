#include "character.h"

#include "../combat/ability.h"
#include "../common.h"
#include "../logging/logger.h"

#include <stdio.h>

/**
 * @brief Initializes a new character
 * @param memory_pool Pointer to the memory pool for dynamic allocation
 * @param type The type of the character (e.g., player, enemy)
 * @param name The name of the character
 * @return Pointer to the initialized character, or NULL on failure
 */
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

    for (int i = 0; i < DAMAGE_TYPE_COUNT; i++) {
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

/**
 * @brief Frees the memory allocated for a character
 * @param memory_pool Pointer to the memory pool used for allocation
 * @param c Pointer to the character to be freed
 */
void free_character(memory_pool_t* memory_pool, character_t* c) {
    NULL_PTR_HANDLER_RETURN(memory_pool, , "Character", "In free_character memory pool is NULL");
    NULL_PTR_HANDLER_RETURN(c, , "Character", "In free_character character is NULL");
    memory_pool_free(memory_pool, c);
}

/**
 * @brief Sets the stats for a character
 * @param c Pointer to the character to set stats for
 * @param strength Strength value
 * @param intelligence Intelligence value
 * @param dexterity Dexterity value
 * @param constitution Constitution value
 */
void set_character_stats(character_t* c, int strength, int intelligence, int dexterity, int constitution) {
    NULL_PTR_HANDLER_RETURN(c, , "Character", "In set_character_stats character is NULL");

    set_stats(&c->base_stats, strength, intelligence, dexterity, constitution);
    c->current_stats = c->base_stats;
    update_character_resources(&c->current_resources, &c->max_resources, &c->base_stats);
    c->current_resources = c->max_resources;
    c->defenses.armor = 0;
    c->defenses.magic_resist = 0;
}

/**
 * @brief Sets the stats for a character
 * @param stats Pointer to the stats structure to set
 * @param strength Strength value
 * @param intelligence Intelligence value
 * @param dexterity Dexterity value
 * @param constitution Constitution value
 */
void set_stats(stats_t* stats, int strength, int intelligence, int dexterity, int constitution) {
    NULL_PTR_HANDLER_RETURN(stats, , "Character", "In set_stats stats is NULL");

    stats->strength = strength;
    stats->intelligence = intelligence;
    stats->dexterity = dexterity;
    stats->constitution = constitution;
}

/**
 * @brief Updates the character's resources based on their stats
 * @param current_resources Pointer to the current resources structure to update
 * @param max_resources Pointer to the max resources structure to update
 * @param base_stats Pointer to the base stats structure
 */
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

/**
 * @brief Sets the damage modifier for a character
 * @param c Pointer to the character to set the damage modifier for
 * @param type The type of damage to modify
 * @param value The value of the damage modifier
 */
void set_character_dmg_modifier(character_t* c, damage_type_t type, int value) {
    NULL_PTR_HANDLER_RETURN(c, , "Character", "In set_character_dmg_modifier character is NULL");

    for (int i = 0; i < DAMAGE_TYPE_COUNT; i++) {
        if (c->resistance[i].type == type) {
            c->resistance[i].value = value;
            return;
        }
    }
    log_msg(WARNING, "Character", "Unknown damage type: %d", type);
}

/**
 * @brief Adds an ability to a character
 * @param c Pointer to the character
 * @param ability Pointer to the ability to add
 */
void add_ability(character_t* c, ability_t* ability) {
    NULL_PTR_HANDLER_RETURN(c, , "Character", "In add_ability character is NULL");
    NULL_PTR_HANDLER_RETURN(ability, , "Character", "In add_ability ability is NULL");

    if (c->ability_count < MAX_ABILITY_LIMIT) {
        c->abilities[c->ability_count] = ability;
        c->ability_count++;
    } else {
        log_msg(WARNING, "Character", "%s cannot learn more abilities!", c->name);
    }
}

/**
 * @brief Removes an ability from a character
 * @param c Pointer to the character
 * @param ability Pointer to the ability to remove
 */
void remove_ability(character_t* c, const ability_t* ability) {
    NULL_PTR_HANDLER_RETURN(c, , "Character", "In remove_ability character is NULL");
    NULL_PTR_HANDLER_RETURN(ability, , "Character", "In remove_ability ability is NULL");

    for (int i = 0; i < c->ability_count; i++) {
        if (c->abilities[i] == ability) {
            for (int j = i; j < c->ability_count - 1; j++) {
                c->abilities[j] = c->abilities[j + 1];
            }
            c->abilities[c->ability_count - 1] = NULL;
            c->ability_count--;

            break;
        }
    }
}

/**
 * @brief Adds gear to a character's inventory
 * @param c Pointer to the character
 * @param gear Pointer to the gear to add
 */
void add_gear(character_t* c, gear_t* gear) {
    NULL_PTR_HANDLER_RETURN(c, , "Character", "In add_gear character is NULL");
    NULL_PTR_HANDLER_RETURN(gear, , "Character", "In add_gear gear is NULL");

    if (c->gear_count < MAX_GEAR_LIMIT) {
        c->gear_inventory[c->gear_count] = gear;
        c->gear_count++;

    } else {
        log_msg(WARNING, "Character", "%s cannot carry more gear!", c->name);
    }
}

/**
 * @brief Removes a gear item from a character's inventory
 * @param c Pointer to the character
 * @param gear Pointer to the gear item to remove
 */
void remove_gear(character_t* c, gear_t* gear) {
    NULL_PTR_HANDLER_RETURN(c, , "Character", "In remove_gear character is NULL");
    NULL_PTR_HANDLER_RETURN(gear, , "Character", "In remove_gear gear is NULL");

    for (int i = 0; i < c->gear_count; i++) {
        if (c->gear_inventory[i] == gear) {
            for (int j = i; j < c->gear_count - 1; j++) {
                c->gear_inventory[j] = c->gear_inventory[j + 1];
            }
            c->gear_inventory[c->gear_count - 1] = NULL;
            c->gear_count--;

            return;
        }
    }
    log_msg(WARNING, "Character", "Gear %s not found in inventory!", gear->local_key);
}

/**
 * @brief Removes the gear equipped in a specific slot of a character.
 *
 * @param c Pointer to the character whose gear is to be removed.
 * @param slot The slot from which the gear should be removed.
 */
void remove_equipped_gear(character_t* c, gear_slot_t slot) {
    NULL_PTR_HANDLER_RETURN(c, , "Character", "In remove_equipped_gear character is NULL");
    CHECK_ARG_RETURN(slot < 0 && slot >= MAX_SLOT, , "Character", "In remove_equipped_gear slot is invalid: %d", slot);

    if (c->equipment[slot] != NULL) {
        gear_t* gear = c->equipment[slot];

        c->base_stats.strength -= gear->stats.strength;
        c->base_stats.intelligence -= gear->stats.intelligence;
        c->base_stats.dexterity -= gear->stats.dexterity;
        c->base_stats.constitution -= gear->stats.constitution;
        c->defenses.armor -= gear->defenses.armor;
        c->defenses.magic_resist -= gear->defenses.magic_resist;
        update_character_resources(&c->current_resources, &c->max_resources, &c->base_stats);

        for (int i = 0; i < gear->num_abilities; ++i) {
            remove_ability(c, gear->abilities[i]);
        }

        if (c->ability_count == 0) {
            add_ability(c, c->base_attack);
        }

        c->equipment[slot] = NULL;
    } else {
        log_msg(WARNING, "Character", "No gear equipped in slot %d!", slot);
    }
}

/**
 * @brief Adds a potion to a character's inventory
 * @param c Pointer to the character
 * @param potion Pointer to the potion to add
 */
void add_potion(character_t* c, potion_t* potion) {
    NULL_PTR_HANDLER_RETURN(c, , "Character", "In add_potion character is NULL");
    NULL_PTR_HANDLER_RETURN(potion, , "Character", "In add_potion potion is NULL");

    if (c->potion_count < MAX_POTION_LIMIT) {
        c->potion_inventory[c->potion_count] = potion;
        c->potion_count++;
    } else {
        log_msg(WARNING, "Character", "%s cannot carry more potions!", c->name);
    }
}

/**
 * @brief Removes a potion from a character's inventory
 * @param c Pointer to the character
 * @param potion Pointer to the potion to remove
 */
void remove_potion(character_t* c, potion_t* potion) {
    NULL_PTR_HANDLER_RETURN(c, , "Character", "In remove_potion character is NULL");
    NULL_PTR_HANDLER_RETURN(potion, , "Character", "In remove_potion potion is NULL");

    for (int i = 0; i < c->potion_count; i++) {
        if (c->potion_inventory[i] == potion) {
            for (int j = i; j < c->potion_count - 1; j++) {
                c->potion_inventory[j] = c->potion_inventory[j + 1];
            }
            c->potion_inventory[c->potion_count - 1] = NULL;
            c->potion_count--;
            return;
        }
    }
    log_msg(WARNING, "Character", "Potion %s not found in inventory!", potion->name);
}

/**
 * @brief Equips a gear item to a character
 * @param c Pointer to the character
 * @param gear Pointer to the gear item to equip
 */
void equip_gear(character_t* c, gear_t* gear) {
    NULL_PTR_HANDLER_RETURN(c, , "Character", "In equip_gear character is NULL");
    NULL_PTR_HANDLER_RETURN(gear, , "Character", "In equip_gear gear is NULL");

    if (gear->slot < MAX_SLOT) {
        if (c->equipment[gear->slot] != NULL) {
            log_msg(WARNING, "Character", "Slot %d is already occupied!", gear->slot);
            return;
        }

        remove_gear(c, gear);

        c->equipment[gear->slot] = gear;

        c->base_stats.strength += gear->stats.strength;
        c->base_stats.intelligence += gear->stats.intelligence;
        c->base_stats.dexterity += gear->stats.dexterity;
        c->base_stats.constitution += gear->stats.constitution;
        c->defenses.armor += gear->defenses.armor;
        c->defenses.magic_resist += gear->defenses.magic_resist;
        update_character_resources(&c->current_resources, &c->max_resources, &c->base_stats);

        if (c->abilities[0] == c->base_attack) {
            remove_ability(c, c->abilities[0]);
        }

        for (int i = 0; i < gear->num_abilities; ++i) {
            add_ability(c, gear->abilities[i]);
        }
    } else {
        log_msg(WARNING, "Character", "Invalid slot for gear %s!", gear->local_key);
    }
}

/**
 * @brief Unequips a gear item from a character and adds it to the character's inventory.
 * @param c Pointer to the character
 * @param slot The slot to unequip from
 */
void unequip_gear(character_t* c, const gear_slot_t slot) {
    NULL_PTR_HANDLER_RETURN(c, , "Character", "In unequip_gear character is NULL");
    CHECK_ARG_RETURN(slot < 0 && slot >= MAX_SLOT, , "Character", "In unequip_gear slot is invalid: %d", slot);

    add_gear(c, c->equipment[slot]);
    remove_equipped_gear(c, slot);
}

/**
 * @brief sets initial xp for a character
 * @param c Pointer to the character
 * @param xp The initial xp value
 */
void set_initial_xp(character_t* c, int xp) {
    NULL_PTR_HANDLER_RETURN(c, , "Character", "In set_initial_xp character is NULL");
    c->xp = xp;
}

/**
 * @brief Sets the level for a character
 * @param c Pointer to the character
 * @param level The level value
 */
void set_level(character_t* c, int level) {
    NULL_PTR_HANDLER_RETURN(c, , "Character", "In set_level character is NULL");
    c->level = level;
}

/**
 * @brief Sets the XP reward for a character
 * @param c Pointer to the character
 * @param xp_reward The XP reward value
 */
void set_xp_reward(character_t* c, int xp_reward) {
    NULL_PTR_HANDLER_RETURN(c, , "Character", "In set_xp_reward character is NULL");
    c->xp_reward = xp_reward;
}

void set_skill_points(character_t* character, int skill_points) {
    NULL_PTR_HANDLER_RETURN(character, , "Character", "In set_skill_points character is NULL");
    character->skill_points = skill_points;
}

void reset_player_stats(character_t* player) {
    if (player == NULL) return;

    // reset current stats to their starting values
    player->current_resources.health = player->max_resources.health;
    player->current_resources.mana = player->max_resources.mana;
    player->current_resources.stamina = player->max_resources.stamina;
}
