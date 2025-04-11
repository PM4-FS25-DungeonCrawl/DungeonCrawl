#include <stdlib.h>
#include <stdio.h>

#include "character.h"
#include "../combat/ability.h"
#include "../item/equipable_item.h"

character_t* init_character(const character_type_t type, const char *name) {
    character_t* character = malloc(sizeof(character_t));
    NULL_PTR_HANDLER(character, "Character", "Failed to allocate memory for character: %s", name);

    character->type = type;
    snprintf(character->name, sizeof(character->name), "%s", name);
    character->ability_count = 0;
    character->equipable_item_count = 0;
    character->usable_item_count = 0;

    for (int i = 0; i < MAX_SLOT; i++) {
        character->equipped_items[i] = NULL;
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
    stats-> intelligence = intelligence;
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
    if (c->ability_count < ABILITY_LIMIT) {
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

void add_item(character_t* c, item_t* item) {
    if (item->type == EQUIPABLE) {
        add_equipable_item(c, item);
    } else if (item->type == USABLE) {
        add_usable_item(c, item);
    } else {
        log_msg(WARNING, "Character", "Unknown item type for %s!", item->name);
    }
}

void remove_item(character_t* c, item_t* item) {
    if (item->type == EQUIPABLE) {
        remove_equipable_item(c, item);
    } else if (item->type == USABLE) {
        remove_usable_item(c, item);
    } else {
        log_msg(WARNING, "Character", "Unknown item type for %s!", item->name);
    }
}

void add_equipable_item(character_t* c, item_t* item) {
    if (c->equipable_item_count < EQUIPABLE_ITEM_LIMIT) {
        c->equipable_items[c->equipable_item_count] = item;
        c->equipable_item_count++;
    } else {
        log_msg(INFO, "Character", "%s cannot carry more equipable items!", c->name);
    }
}

void remove_equipable_item(character_t* c, item_t* item) {
    for (int i = 0; i < c->equipable_item_count; i++) {
        if (c->equipable_items[i] == item) {
            for (int j = i; j < c->equipable_item_count - 1; j++) {
                c->equipable_items[j] = c->equipable_items[j + 1];
            }
            c->equipable_items[c->equipable_item_count - 1] = NULL;
            c->equipable_item_count--;
            return;
        }
    }
    log_msg(WARNING, "Character", "Equipable item %s not found in inventory!", item->name);
}

void add_usable_item(character_t* c, item_t* item) {
    if (c->usable_item_count < USABLE_ITEM_LIMIT) {
        c->usable_items[c->usable_item_count] = item;
        c->usable_item_count++;
    } else {
        log_msg(INFO, "Character", "%s cannot carry more useable items!", c->name);
    }
}

void remove_usable_item(character_t* c, item_t* item) {
    for (int i = 0; i < c->usable_item_count; i++) {
        if (c->usable_items[i] == item) {
            for (int j = i; j < c->usable_item_count - 1; j++) {
                c->usable_items[j] = c->usable_items[j + 1];
            }
            c->usable_items[c->usable_item_count - 1] = NULL;
            c->usable_item_count--;
            return;
        }
    }
    log_msg(WARNING, "Character", "Usable item %s not found in inventory!", item->name);
}

void equip_item(character_t* c, equipable_item_t* item) {
    if (item->slot < MAX_SLOT) {
        if (c->equipped_items[item->slot] != NULL) {
            log_msg(WARNING, "Character", "Slot %d is already occupied!", item->slot);
            return;
        }

        remove_equipable_item(c, item->base);
        c->equipped_items[item->slot] = item;
        c->defenses.armor += item->armor_bonus;
        c->defenses.magic_resist += item->magic_resist_bonus;

        log_msg(INFO, "Character", "%s equipped %s in slot %d.", c->name, item->base->name, item->slot);
    } else {
        log_msg(WARNING, "Character", "Invalid slot for item %s!", item->base->name);
    }
}

void unequip_item(character_t* c, gear_slot_t slot) {
    if (slot < MAX_SLOT && c->equipped_items[slot] != NULL) {

        equipable_item_t* item = c->equipped_items[slot];
        c->defenses.armor -= item->armor_bonus;
        c->defenses.magic_resist -= item->magic_resist_bonus;
        add_equipable_item(c, item->base);

        log_msg(INFO, "Character", "%s unequipped %s from slot %d.", c->name, item->base->name, slot);
        c->equipped_items[slot] = NULL;
    } else {
        log_msg(WARNING, "Character", "No item equipped in slot %d!", slot);
    }
}
