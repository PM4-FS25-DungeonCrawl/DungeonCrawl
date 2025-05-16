#include "../../src/character/character.h"
#include "../../src/character/level.h"
#include "../../src/combat/ability.h"

#include <assert.h>
#include <stdio.h>

memory_pool_t* test_character_memory_pool;

void setup() {
    test_character_memory_pool = init_memory_pool(MIN_MEMORY_POOL_SIZE);
    if (test_character_memory_pool == NULL) {
        printf("Failed to initialize memory pool\n");
        exit(EXIT_FAILURE);
    }
    printf("Setup complete\n");
}

// initialize a character for testing
character_t* setup_character() {
    character_t* character = init_character(test_character_memory_pool, PLAYER, "Hero");
    set_character_stats(character, 5, 10, 15, 20);
    return character;
}

void test_init_character() {
    character_t* c = setup_character();
    assert(c != NULL);
    assert(strcmp(c->name, "Hero") == 0);
    assert(c->type == PLAYER);
    printf("Test: test_init_character() passed\n");
}

void test_set_character_stats() {
    character_t* c = setup_character();

    assert(c->base_stats.strength == 5);
    assert(c->base_stats.intelligence == 10);
    assert(c->base_stats.dexterity == 15);
    assert(c->base_stats.constitution == 20);

    printf("Test: test_set_character_stats() passed\n");
}

void test_update_character_resources() {
    character_t* c = setup_character();

    assert(c->current_resources.health == c->max_resources.health);
    assert(c->current_resources.stamina == c->max_resources.stamina);
    assert(c->current_resources.mana == c->max_resources.mana);

    printf("Test: test_update_character_resources() passed\n");
}

void test_character_dmg_modifier() {
    character_t* c = setup_character();
    
    set_character_dmg_modifier(c, PHYSICAL, 10);
    assert(c->resistance[PHYSICAL].type == PHYSICAL);
    assert(c->resistance[PHYSICAL].value == 10);

    printf("Test: test_character_dmg_modifier() passed\n");
}

void test_add_and_remove_ability() {
    character_t* c = setup_character();
    ability_t test_ability = {.name = "Test Ability"};
    
    add_ability(c, &test_ability);
    assert(c->abilities[0] == &test_ability);
    assert(c->ability_count == 1);

    remove_ability(c, &test_ability);
    assert(c->abilities[0] == NULL);
    assert(c->ability_count == 0);
    printf("Test: test_add_and_remove_ability() passed\n");
}

void test_add_and_remove_potion() {
    character_t* c = setup_character();
    potion_t test_potion = {.name = "Healing Potion", .effectType = HEALING, .value = 20};
    
    add_potion(c, &test_potion);
    assert(c->potion_inventory[0] == &test_potion);
    assert(c->potion_count == 1);

    remove_potion(c, &test_potion);
    assert(c->potion_inventory[0] == NULL);
    assert(c->potion_count == 0);
    printf("Test: test_add_and_remove_potion() passed\n");
}

void test_add_and_remove_gear() {
    character_t* c = setup_character();
    gear_t test_gear = {.local_key = "Test Gear", .slot = SLOT_HEAD};
    
    add_gear(c, &test_gear);
    assert(c->gear_inventory[0] == &test_gear);
    assert(c->gear_count == 1);

    remove_gear(c, &test_gear);
    assert(c->gear_inventory[0] == NULL);
    assert(c->gear_count == 0);

    printf("Test: test_add_and_remove_gear() passed\n");
}

void test_equip_and_unequip_gear() {
    character_t* c = setup_character();
    gear_t test_gear = {
        .local_key = "Test Gear",
        .local_name = "Test Gear",
        .slot = SLOT_HEAD,
        .stats = {
            .constitution = 1,
            .dexterity = 1,
            .intelligence = 1,
            .strength = 1
        },
        .defenses = {
            .armor = 10,
            .magic_resist = 10
        }};
    
    add_gear(c, &test_gear);
    equip_gear(c, &test_gear);
    assert(c->equipment[SLOT_HEAD] == &test_gear);
    assert(c->gear_count == 0);
    
    unequip_gear(c, SLOT_HEAD);
    assert(c->equipment[SLOT_HEAD] == NULL);
    assert(c->gear_count == 1);

    printf("Test: test_equip_and_unequip_gear() passed\n");
}

void test_reset_current_stats() {
    character_t* c = setup_character();

    c->current_stats.strength = c->base_stats.strength * 2;
    c->current_stats.intelligence = c->base_stats.intelligence * 2;
    c->current_stats.dexterity = c->base_stats.dexterity * 2;
    c->current_stats.constitution = c->base_stats.constitution * 2;
    
    assert(c->current_stats.strength != c->base_stats.strength);
    assert(c->current_stats.intelligence != c->base_stats.intelligence);
    assert(c->current_stats.dexterity != c->base_stats.dexterity);
    assert(c->current_stats.constitution != c->base_stats.constitution);

    reset_current_stats(c);
    assert(c->current_stats.strength == c->base_stats.strength);
    assert(c->current_stats.intelligence == c->base_stats.intelligence);
    assert(c->current_stats.dexterity == c->base_stats.dexterity);
    assert(c->current_stats.constitution == c->base_stats.constitution);

    printf("Test: test_reset_current_stats() passed\n");
}

void test_reset_current_resources() {
    character_t* c = setup_character();

    c->current_resources.health = c->max_resources.health - 1;
    c->current_resources.stamina = c->max_resources.stamina - 1;
    c->current_resources.mana = c->max_resources.mana -1;
    
    assert(c->current_resources.health != c->max_resources.health);
    assert(c->current_resources.stamina != c->max_resources.stamina);
    assert(c->current_resources.mana != c->max_resources.mana);

    reset_current_resources(c);
    assert(c->current_resources.health == c->max_resources.health);
    assert(c->current_resources.stamina == c->max_resources.stamina);
    assert(c->current_resources.mana == c->max_resources.mana);

    printf("Test: test_reset_current_resources() passed\n");
}

void test_character_level_up() {
    character_t* c = setup_character();
    int initial_level = c->level;
    level_up(c);
    assert(c->level == initial_level + 1);
    printf("Test: test_character_level_up() passed\n");
}

int main(void) {
    setup();

    test_init_character();

    test_set_character_stats();
    test_update_character_resources();
    test_character_dmg_modifier();

    test_add_and_remove_ability();
    test_add_and_remove_potion();
    test_add_and_remove_gear();
    test_equip_and_unequip_gear();
    
    test_reset_current_stats();
    test_reset_current_resources();
    test_character_level_up();

    shutdown_memory_pool(test_character_memory_pool);
    return 0;
}
