#include <stdlib.h>
#include <stdio.h>

#include "character.h"
#include "src/logging/logger.h"

character_t* init_character(const character_type_t type, const char *name) {
    character_t* character = malloc(sizeof(character_t));
    if (character == NULL) {
        // memory for character could not be allocated
        log_msg(ERROR, "Character", "Failed to allocate memory for character: %s", name);
        return NULL;
    }

    character->type = type;
    snprintf(character->name, MAX_STRING_LENGTH, "%s", name);
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

void set_character_stats(character_t* character, int health, int armor, int might, int deflection, int fortitude, int will) {
    character->health = health;
    character->armor = armor;
    character->might = might;
    character->deflection = deflection;
    character->fortitude = fortitude;
    character->will = will;
}

void set_character_dmg_modifier(character_t* character, damage_type_t type, int value) {
    for (int i = 0; i < DAMAGE_TYPE_COUNT; i++) {
        if (character->dmg_modifier[i].type == type) {
            character->dmg_modifier[i].value = value;
            return;
        }
    }
    log_msg(WARNING, "Character", "Unknown damage type: %d", type);
}

void add_ability_to_character(character_t* c, ability_t* ability) {
    if (c->ability_count < ABILITY_LIMIT) {
        c->abilities[c->ability_count] = ability;
        c->ability_count++;
    } else {
        log_msg(INFO, "Character", "%s cannot learn more abilities!", c->name);
    }
}

void add_item_to_character(character_t* c, item_t* item) {
    if (c->item_count < ITEM_LIMIT) {
        c->items[c->item_count] = item;
        c->item_count++;
    } else {
        log_msg(INFO, "Character", "%s cannot carry more items!", c->name);
    }
}

void remove_item_from_character(character_t* c, item_t* item) {
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
