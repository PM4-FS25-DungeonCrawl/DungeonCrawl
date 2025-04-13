#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../src/character/character.h"
#include "../../src/combat/ability.h"

// initialize a character for testing
character_t* setup_character() {
    character_t* character = init_character(PLAYER, "Hero");
    set_character_stats(character, 5, 10, 15, 20);
    return character;
}

void test_init_character() {
    character_t* character = setup_character();
    assert(character != NULL);
    assert(strcmp(character->name, "Hero") == 0);
    assert(character->type == PLAYER);
    free_character(character);
    printf("test_init_character passed\n");
}

void test_set_character_stats() {
    character_t* character = setup_character();
    assert(character->base_stats.strength == 5);
    assert(character->base_stats.intelligence == 10);
    assert(character->base_stats.dexterity == 15);
    assert(character->base_stats.constitution == 20);
    free_character(character);
    printf("test_set_character_stats passed\n");
}

void test_add_ability() {
    character_t* character = setup_character();
    ability_t test_ability = {.name = "Test Ability"};
    add_ability(character, &test_ability);
    assert(character->abilities[0] == &test_ability);
    assert(character->ability_count == 1);
    free_character(character);
    printf("test_add_ability passed\n");
}

void test_add_gear() {
    character_t* character = setup_character();
    gear_t test_gear = {.name = "Test Gear", .slot = SLOT_HEAD};
    add_gear(character, &test_gear);
    assert(character->gear_inventory[0] == &test_gear);
    assert(character->gear_inventory_count == 1);
    free_character(character);
    printf("test_add_gear passed\n");
}

void test_add_potion() {
    character_t* character = setup_character();
    potion_t test_potion = {.name = "Healing Potion", .effectType = HEALING, .value = 20};
    add_potion(character, &test_potion);
    assert(character->potion_inventory[0] == &test_potion);
    assert(character->potion_inventory_count == 1);
    free_character(character);
    printf("test_add_potion passed\n");
}

void test_reset_current_stats() {
    character_t* character = setup_character();
    character->current_stats.strength = 10;
    reset_current_stats(character);
    assert(character->current_stats.strength == character->base_stats.strength);
    free_character(character);
    printf("test_reset_current_stats passed\n");
}

int main(void) {
    test_init_character();
    test_set_character_stats();
    test_add_ability();
    test_add_gear();
    test_add_potion();
    test_reset_current_stats();
    return 0;
}