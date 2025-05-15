#include "../../src/character/character.h"
#include "../../src/combat/ability.h"
#include "../../src/combat/combat_mode.h"
#include "../../src/common.h"

#include <assert.h>
#include <stdio.h>

void setup() {
    test_memory_pool = init_memory_pool(MIN_MEMORY_POOL_SIZE);
    if (test_memory_pool == NULL) {
        printf("Failed to initialize memory pool\n");
        exit(EXIT_FAILURE);
    }
    init_combat_mode();
    printf("Setup complete\n");
}

character_t* create_test_character() {
    character_t* c = init_character(test_memory_pool, PLAYER, "Hero");
    set_character_stats(c, 5, 5, 5, 20);
    return c;
}

void test_health_potion() {
    character_t* c = create_test_character();
    potion_t test_potion = {
        .effectType = HEALING,
        .value = 20};
        
    add_potion(c, &test_potion);
    add_potion(c, &test_potion);
    
    int* max_resource = &c->max_resources.health;
    int* curr_resource = &c->current_resources.health;

    *curr_resource -= test_potion.value - 1;
    assert(*curr_resource < *max_resource);
    invoke_potion_effect(c, &test_potion);
    assert(*curr_resource == *max_resource);
    assert(c->potion_count == 1);

    *curr_resource -= test_potion.value + 1;
    assert(*curr_resource < *max_resource);
    invoke_potion_effect(c, &test_potion);
    assert(*curr_resource == (*max_resource - 1));
    assert(c->potion_count == 0);

    printf("Test: test_health_potion() passed\n");
}

void test_stamina_potion() {
    character_t* c = create_test_character();
    potion_t test_potion = {
        .effectType = STAMINA,
        .value = 5};
        
    add_potion(c, &test_potion);
    add_potion(c, &test_potion);
    
    int* max_resource = &c->max_resources.stamina;
    int* curr_resource = &c->current_resources.stamina;

    *curr_resource -= test_potion.value - 1;
    assert(*curr_resource < *max_resource);
    invoke_potion_effect(c, &test_potion);
    assert(*curr_resource == *max_resource);
    assert(c->potion_count == 1);

    *curr_resource -= test_potion.value + 1;
    assert(*curr_resource < *max_resource);
    invoke_potion_effect(c, &test_potion);
    assert(*curr_resource == (*max_resource - 1));
    assert(c->potion_count == 0);

    printf("Test: test_stamina_potion() passed\n");
}

void test_mana_potion() {
    character_t* c = create_test_character();
    potion_t test_potion = {
        .effectType = MANA,
        .value = 3};
        
    add_potion(c, &test_potion);
    add_potion(c, &test_potion);
    
    int* max_resource = &c->max_resources.mana;
    int* curr_resource = &c->current_resources.mana;

    *curr_resource -= test_potion.value - 1;
    assert(*curr_resource < *max_resource);
    invoke_potion_effect(c, &test_potion);
    assert(*curr_resource == *max_resource);
    assert(c->potion_count == 1);

    *curr_resource -= test_potion.value + 1;
    assert(*curr_resource < *max_resource);
    invoke_potion_effect(c, &test_potion);
    assert(*curr_resource == (*max_resource - 1));
    assert(c->potion_count == 0);

    printf("Test: test_mana_potion() passed\n");
}

void test_use_stamina_ability() {
    character_t* c = create_test_character();
    ability_t test_stamina_ability = {
        .damage_type = PHYSICAL,
        .resource_cost = 5};

    int currStamina = c->current_resources.stamina;
    assert(consume_ability_resource(c, &test_stamina_ability));
    assert(c->current_resources.stamina == currStamina - test_stamina_ability.resource_cost);
    printf("Test: test_use_stamina_ability() passed\n");
}

void test_use_mana_ability() {
    character_t* c = create_test_character();
    ability_t test_mana_ability = {
        .damage_type = MAGICAL,
        .resource_cost = 5};

    int currMana = c->current_resources.mana;
    assert(consume_ability_resource(c, &test_mana_ability));
    assert(c->current_resources.mana == currMana - test_mana_ability.resource_cost);
    printf("Test: test_use_mana_ability() passed\n");
}

void test_use_stamina_ability_insufficient_resource() {
    character_t* c = create_test_character();
    ability_t test_stamina_ability = {
        .damage_type = PHYSICAL,
        .resource_cost = c->current_resources.stamina + 1};

    int currStamina = c->current_resources.stamina;
    assert(!consume_ability_resource(c, &test_stamina_ability));
    assert(c->current_resources.stamina == currStamina);
    printf("Test: test_use_stamina_ability_insufficient_resource() passed\n");
}

void test_use_mana_ability_insufficient_resource() {
    character_t* c = create_test_character();
    ability_t test_mana_ability = {
        .damage_type = MAGICAL,
        .resource_cost = c->current_resources.mana + 1};

    int currMana = c->current_resources.mana;
    assert(!consume_ability_resource(c, &test_mana_ability));
    assert(c->current_resources.mana == currMana);
    printf("Test: test_use_mana_ability_insufficient_resource() passed\n");
}

void test_get_random_ability() {
    character_t* c = create_test_character();

    ability_t test_abilities[2] = {
            {.name = "Physical Attack", .roll_amount = 2, .accuracy = 80, .resource_cost = 5, .dice_size = D6, .damage_type = PHYSICAL},
            {.name = "Magical Attack", .roll_amount = 3, .accuracy = 70, .resource_cost = 3, .dice_size = D8, .damage_type = MAGICAL}};

    for (int i = 0; i < 2; i++) {
        c->abilities[i] = &test_abilities[i];
    }
    c->ability_count = 2;

    bool ability_found[2] = {false, false};
    int found_count = 0;

    while (found_count < 2) {
        ability_t* random_ability = get_random_ability(c);
        for (int i = 0; i < 2; i++) {
            if (random_ability == c->abilities[i] && !ability_found[i]) {
                ability_found[i] = true;
                found_count++;
            }
        }
    }

    // Test: Ensure all abilities can be found
    for (int i = 0; i < 2; i++) {
        assert(ability_found[i]);
    }

    printf("Test: test_get_random_ability() passed\n");
}

int main(void) {
    setup();

    // invoke_potion_effect() tests
    test_health_potion();
    test_stamina_potion();
    test_mana_potion();

    // consume_ability_resource() tests
    test_use_stamina_ability();
    test_use_mana_ability();
    test_use_stamina_ability_insufficient_resource();
    test_use_mana_ability_insufficient_resource();

    // others
    test_get_random_ability();

    shutdown_memory_pool(test_memory_pool);
    return 0;
}
