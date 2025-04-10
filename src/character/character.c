#include <stdlib.h>
#include <stdio.h>

#include "character.h"
#include "../combat/ability.h"
#include "../item/usable_item.h"

character_t* init_character(const character_type_t type, const char *name) {
    character_t* character = malloc(sizeof(character_t));
    NULL_PTR_HANDLER(character, "Character", "Failed to allocate memory for character: %s", name);

    character->type = type;
    snprintf(character->name, sizeof(character->name), "%s", name);
    character->ability_count = 0;
    character->item_count = 0;

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

void add_item(character_t* c, item_t* item) {
    if (c->item_count < ITEM_LIMIT) {
        c->items[c->item_count] = item;
        c->item_count++;
    } else {
        log_msg(INFO, "Character", "%s cannot carry more items!", c->name);
    }
}

void remove_item(character_t* c, item_t* item) {
    for (int i = 0; i < c->item_count; i++) {
        if (c->items[i] == item) {
            for (int j = i; j < c->item_count - 1; j++) {
                c->items[j] = c->items[j + 1];
            }
            c->items[c->item_count - 1] = NULL;
            c->item_count--;
            break;
        }
    }
}

bool use_usable_item(character_t* character, item_t* item) {
    if (item->type != USABLE) {
        log_msg(ERROR, "Character", "%s cannot use usable_item %s", character->name, item->name);
        return false;
    }
    const usable_item_t* usable_item = (usable_item_t*) (item->extension);

    switch (usable_item->effectType) {
        case HEALING:
            if (usable_item->value > (character->max_resources.health - character->current_resources.health)) {
                character->current_resources.health = character->max_resources.health;
            } else {
                character->current_resources.health += usable_item->value;
            }
            break;
        default:
            log_msg(ERROR, "Character", "Unknown usable_item effect type: %d", usable_item->effectType);
            break;
    }

    remove_item(character, item);
    return true;
}

int deal_damage(character_t* character, damage_type_t damage_type, int damage) {
    // TODO critical hits are ignored
    // negative damage resistance leads to more damage
    damage += character->resistance[damage_type].value;
    // damage -= character->current_stats.armor;
    if (damage > 0) character->current_resources.health -= damage;
    return damage;
}

void reset_current_stats(character_t * character) {
    character->current_stats = character->base_stats;
}