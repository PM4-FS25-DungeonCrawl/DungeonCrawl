/**
 * @file character.c
 * @brief Implements character-related functionality for the DungeonCrawl game
 */

#include "character.h"

#include "../combat/ability.h"
#include "../common.h"
#include "../logging/logger.h"

#include <stdio.h>
#include <stdlib.h>

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
    character->ability_count = 0;
    character->gear_count = 0;
    character->potion_count = 0;

    for (int i = 0; i < MAX_SLOT; i++) {
        character->equipment[i] = NULL;
    }

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
    update_character_resources(&c->max_resources, &c->base_stats);
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
 * @param max_resources Pointer to the max resources structure to update
 * @param base_stats Pointer to the base stats structure
 */
void update_character_resources(resources_t* max_resources, stats_t* base_stats) {
    NULL_PTR_HANDLER_RETURN(max_resources, , "Character", "In update_character_resources max_resources is NULL");
    NULL_PTR_HANDLER_RETURN(base_stats, , "Character", "In update_character_resources base_stats is NULL");

    max_resources->health = (5 * base_stats->constitution);
    max_resources->mana = (1 * base_stats->intelligence);
    max_resources->stamina = (2 * base_stats->strength);
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
        log_msg(INFO, "Character", "%s cannot learn more abilities!", c->name);
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
        log_msg(INFO, "Character", "%s cannot carry more gear!", c->name);
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
    log_msg(WARNING, "Character", "Gear %s not found in inventory!", gear->name);
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
        log_msg(INFO, "Character", "%s cannot carry more potions!", c->name);
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


        remove_gear(c, gear);//removing from inventory

        for (int i = 0; i < 4; ++i) {
            if (gear->abilities[i].name[0] != '\0') {
                add_ability(c, &gear->abilities[i]);
            }
        }

        c->equipment[gear->slot] = gear;
        c->base_stats.strength += gear->stats.strength;
        c->base_stats.intelligence += gear->stats.intelligence;
        c->base_stats.dexterity += gear->stats.dexterity;
        c->base_stats.constitution += gear->stats.constitution;
        c->defenses.armor += gear->defenses.armor;
        c->defenses.magic_resist += gear->defenses.magic_resist;

        update_character_resources(&c->max_resources, &c->base_stats);

        log_msg(INFO, "Character", "%s equipped %s — resources updated.", c->name, gear->name);
        log_msg(INFO, "Character", "%s equipped %s in slot %d.", c->name, gear->name, gear->slot);

    } else {
        log_msg(WARNING, "Character", "Invalid slot for gear %s!", gear->name);
    }
}

/**
 * @brief Unequips a gear item from a character
 * @param c Pointer to the character
 * @param slot The slot to unequip from
 */
void unequip_gear(character_t* c, const gear_slot_t slot) {
    NULL_PTR_HANDLER_RETURN(c, , "Character", "In unequip_gear character is NULL");
    CHECK_ARG_RETURN(slot < 0 && slot >= MAX_SLOT, , "Character", "In unequip_gear slot is invalid: %d", slot);

    if (c->equipment[slot] != NULL) {
        gear_t* item = c->equipment[slot];

        for (int i = 0; i < 4; ++i) {
            if (item->abilities[i].name[0] != '\0') {
                remove_ability(c, &item->abilities[i]);
            }
        }

        c->base_stats.strength -= item->stats.strength;
        c->base_stats.intelligence -= item->stats.intelligence;
        c->base_stats.dexterity -= item->stats.dexterity;
        c->base_stats.constitution -= item->stats.constitution;
        c->defenses.armor -= item->defenses.armor;
        c->defenses.magic_resist -= item->defenses.magic_resist;

        update_character_resources(&c->max_resources, &c->base_stats);

        add_gear(c, item);

        log_msg(INFO, "Character", "%s unequipped %s from slot %d.", c->name, item->name, slot);
        c->equipment[slot] = NULL;
    } else {
        log_msg(WARNING, "Character", "No gear equipped in slot %d!", slot);
    }
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

    log_msg(INFO, "Character", "Player stats reset to base values.");
}
