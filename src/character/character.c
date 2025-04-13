#include "character.h"

#include "../combat/ability.h"

#include <stdio.h>
#include <stdlib.h>

character_t* init_character(const character_type_t type, const char* name) {
    character_t* character = malloc(sizeof(character_t));
    NULL_PTR_HANDLER(character, "Character", "Failed to allocate memory for character: %s", name);

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

void free_character(character_t* character) {
    if (character != NULL) {
        // abilities and items must not be freed here,
        // because they are managed by the ability table
        free(character);
    }
}

void set_character_stats(character_t* character, int strength, int intelligence, int dexterity, int constitution) {
    set_stats(&character->base_stats, strength, intelligence, dexterity, constitution);
    character->current_stats = character->base_stats;
    update_character_resources(&character->max_resources, &character->base_stats);
    character->current_resources = character->max_resources;
    character->defenses.armor = 0;
    character->defenses.magic_resist = 0;
}

void set_stats(stats_t* stats, int strength, int intelligence, int dexterity, int constitution) {
    stats->strength = strength;
    stats->intelligence = intelligence;
    stats->dexterity = dexterity;
    stats->constitution = constitution;
}

void update_character_resources(resources_t* max_resources, stats_t* base_stats) {
    max_resources->health = (5 * base_stats->constitution);
    max_resources->mana = (1 * base_stats->intelligence);
    max_resources->stamina = (1 * base_stats->strength);
}

void set_character_dmg_modifier(character_t* character, damage_type_t type, int value) {
    for (int i = 0; i < DAMAGE_TYPE_COUNT; i++) {
        if (character->resistance[i].type == type) {
            character->resistance[i].value = value;
            return;
        }
    }
    log_msg(WARNING, "Character", "Unknown damage type: %d", type);
}

void add_ability(character_t* c, ability_t* ability) {
    if (c->ability_count < MAX_ABILITY_LIMIT) {
        c->abilities[c->ability_count] = ability;
        c->ability_count++;
    } else {
        log_msg(INFO, "Character", "%s cannot learn more abilities!", c->name);
    }
}

void remove_ability(character_t* c, ability_t* ability) {
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

void add_gear(character_t* c, gear_t* item) {
    if (c->gear_count < MAX_GEAR_LIMIT) {
        c->gear_inventory[c->gear_count] = item;
        c->gear_count++;
    } else {
        log_msg(INFO, "Character", "%s cannot carry more equipable items!", c->name);
    }
}

void remove_gear(character_t* c, gear_t* item) {
    for (int i = 0; i < c->gear_count; i++) {
        if (c->gear_inventory[i] == item) {
            for (int j = i; j < c->gear_count - 1; j++) {
                c->gear_inventory[j] = c->gear_inventory[j + 1];
            }
            c->gear_inventory[c->gear_count - 1] = NULL;
            c->gear_count--;
            return;
        }
    }
    log_msg(WARNING, "Character", "Equipable item %s not found in inventory!", item->name);
}

void add_potion(character_t* c, potion_t* item) {
    if (c->potion_count < MAX_POTION_LIMIT) {
        c->potion_inventory[c->potion_count] = item;
        c->potion_count++;
    } else {
        log_msg(INFO, "Character", "%s cannot carry more useable items!", c->name);
    }
}

void remove_potion(character_t* c, potion_t* item) {
    for (int i = 0; i < c->potion_count; i++) {
        if (c->potion_inventory[i] == item) {
            for (int j = i; j < c->potion_count - 1; j++) {
                c->potion_inventory[j] = c->potion_inventory[j + 1];
            }
            c->potion_inventory[c->potion_count - 1] = NULL;
            c->potion_count--;
            return;
        }
    }
    log_msg(WARNING, "Character", "Usable item %s not found in inventory!", item->name);
}

void equip_gear(character_t* c, gear_t* item) {
    if (item->slot < MAX_SLOT) {
        if (c->equipment[item->slot] != NULL) {
            log_msg(WARNING, "Character", "Slot %d is already occupied!", item->slot);
            return;
        }

        remove_gear(c, item);
        c->equipment[item->slot] = item;
        c->defenses.armor += item->defenses.armor;
        c->defenses.magic_resist += item->defenses.magic_resist;

        log_msg(INFO, "Character", "%s equipped %s in slot %d.", c->name, item->name, item->slot);
    } else {
        log_msg(WARNING, "Character", "Invalid slot for item %s!", item->name);
    }
}

void unequip_gear(character_t* c, gear_slot_t slot) {
    if (slot < MAX_SLOT && c->equipment[slot] != NULL) {
        gear_t* item = c->equipment[slot];
        c->defenses.armor -= item->defenses.armor;
        c->defenses.magic_resist -= item->defenses.magic_resist;
        add_gear(c, item);

        log_msg(INFO, "Character", "%s unequipped %s from slot %d.", c->name, item->name, slot);
        c->equipment[slot] = NULL;
    } else {
        log_msg(WARNING, "Character", "No item equipped in slot %d!", slot);
    }
}

void set_initial_xp(character_t* character, int xp) {
    character->xp = xp;
}

void set_level(character_t* character, int level) {
    character->level = level;
}

void set_xp_reward(character_t* character, int xp_reward) {
    character->xp_reward = xp_reward;
}
