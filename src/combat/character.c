#include <stdlib.h>
#include <stdio.h>

#include "character.h"
#include "../logging/logger.h"

character_t* init_character(const character_type_t type, const char *name, const int health, const int armor, const int might, const int deflection, const int fortitude, const int will) {
    character_t* character = malloc(sizeof(character_t));
    if (character == NULL) {
        // memory for character could not be allocated
        log_msg(ERROR, "Character", "Failed to allocate memory for character: %s", name);
        return NULL;
    }

    character->type = type;
    snprintf(character->name, MAX_STRING_LENGTH, "%s", name);
    character->health = health;
    character->armor = armor;
    character->might = might;
    character->deflection = deflection;
    character->fortitude = fortitude;
    character->will = will;
    character->ability_count = 0;

    return character;
}

void free_character(character_t* character) {
    if (character != NULL) {
        //abilities must not be freed here, because they are managed by the ability table
        free(character);
    }
}

void add_ability_to_character(character_t* c, ability_t* ability) {
    if (c->ability_count < MAX_ABILITIES) {
        c->abilities[c->ability_count] = ability;
        c->ability_count++;
    } else {
        log_msg(INFO, "Character", "%s cannot learn more abilities!", c->name);
    }
}
