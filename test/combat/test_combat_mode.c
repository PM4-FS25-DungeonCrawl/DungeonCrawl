#include "../../src/character/character.h"
#include "../../src/combat/ability.h"
#include "../../src/combat/combat_mode.h"
#include "../../src/common.h"

#include <assert.h>
#include <stdio.h>

memory_pool_t* test_combat_memory;

void setup() {
    test_combat_memory = init_memory_pool(MIN_MEMORY_POOL_SIZE);
    if (test_combat_memory == NULL) {
        printf("Failed to initialize memory pool\n");
        exit(EXIT_FAILURE);
    }
    init_combat_mode();
    printf("Setup complete\n");
}

character_t* create_test_character() {
    character_t* character = init_character(test_combat_memory, PLAYER, "Hero");
    set_character_stats(character, 5, 5, 5, 20);
    return character;
}

void test_health_potion() {
    character_t* player = create_test_character();
    potion_t test_potion = {
            .effectType = HEALING,
            .value = 20};

    add_potion(player, &test_potion);
    add_potion(player, &test_potion);

    int* max_resource = &player->max_resources.health;
    int* curr_resource = &player->current_resources.health;

    *curr_resource -= test_potion.value - 1;
    assert(*curr_resource < *max_resource);
    invoke_potion_effect(player, &test_potion);
    assert(*curr_resource == *max_resource);
    assert(player->potion_count == 1);

    *curr_resource -= test_potion.value + 1;
    assert(*curr_resource < *max_resource);
    invoke_potion_effect(player, &test_potion);
    assert(*curr_resource == (*max_resource - 1));
    assert(player->potion_count == 0);

    printf("Test: test_health_potion() passed\n");
}

void test_stamina_potion() {
    character_t* player = create_test_character();
    potion_t test_potion = {
            .effectType = STAMINA,
            .value = 5};

    add_potion(player, &test_potion);
    add_potion(player, &test_potion);

    int* max_resource = &player->max_resources.stamina;
    int* curr_resource = &player->current_resources.stamina;

    *curr_resource -= test_potion.value - 1;
    assert(*curr_resource < *max_resource);
    invoke_potion_effect(player, &test_potion);
    assert(*curr_resource == *max_resource);
    assert(player->potion_count == 1);

    *curr_resource -= test_potion.value + 1;
    assert(*curr_resource < *max_resource);
    invoke_potion_effect(player, &test_potion);
    assert(*curr_resource == (*max_resource - 1));
    assert(player->potion_count == 0);

    printf("Test: test_stamina_potion() passed\n");
}

void test_mana_potion() {
    character_t* player = create_test_character();
    potion_t test_potion = {
            .effectType = MANA,
            .value = 3};

    add_potion(player, &test_potion);
    add_potion(player, &test_potion);

    int* max_resource = &player->max_resources.mana;
    int* curr_resource = &player->current_resources.mana;

    *curr_resource -= test_potion.value - 1;
    assert(*curr_resource < *max_resource);
    invoke_potion_effect(player, &test_potion);
    assert(*curr_resource == *max_resource);
    assert(player->potion_count == 1);

    *curr_resource -= test_potion.value + 1;
    assert(*curr_resource < *max_resource);
    invoke_potion_effect(player, &test_potion);
    assert(*curr_resource == (*max_resource - 1));
    assert(player->potion_count == 0);

    printf("Test: test_mana_potion() passed\n");
}

void test_use_stamina_ability() {
    character_t* player = create_test_character();
    ability_t test_stamina_ability = {
            .damage_type = PHYSICAL,
            .resource_cost = 5};

    int currStamina = player->current_resources.stamina;
    assert(consume_ability_resource(player, &test_stamina_ability));
    assert(player->current_resources.stamina == currStamina - test_stamina_ability.resource_cost);
    printf("Test: test_use_stamina_ability() passed\n");
}

void test_use_mana_ability() {
    character_t* player = create_test_character();
    ability_t test_mana_ability = {
            .damage_type = MAGICAL,
            .resource_cost = 5};

    int currMana = player->current_resources.mana;
    assert(consume_ability_resource(player, &test_mana_ability));
    assert(player->current_resources.mana == currMana - test_mana_ability.resource_cost);
    printf("Test: test_use_mana_ability() passed\n");
}

void test_use_stamina_ability_insufficient_resource() {
    character_t* player = create_test_character();
    ability_t test_stamina_ability = {
            .damage_type = PHYSICAL,
            .resource_cost = player->current_resources.stamina + 1};

    int currStamina = player->current_resources.stamina;
    assert(!consume_ability_resource(player, &test_stamina_ability));
    assert(player->current_resources.stamina == currStamina);
    printf("Test: test_use_stamina_ability_insufficient_resource() passed\n");
}

void test_use_mana_ability_insufficient_resource() {
    character_t* player = create_test_character();
    ability_t test_mana_ability = {
            .damage_type = MAGICAL,
            .resource_cost = player->current_resources.mana + 1};

    int currMana = player->current_resources.mana;
    assert(!consume_ability_resource(player, &test_mana_ability));
    assert(player->current_resources.mana == currMana);
    printf("Test: test_use_mana_ability_insufficient_resource() passed\n");
}

void test_get_random_ability() {
    character_t* player = create_test_character();

    ability_t test_abilities[2] = {
            {.name = "Physical Attack", .roll_amount = 2, .accuracy = 80, .resource_cost = 5, .dice_size = D6, .damage_type = PHYSICAL},
            {.name = "Magical Attack", .roll_amount = 3, .accuracy = 70, .resource_cost = 3, .dice_size = D8, .damage_type = MAGICAL}};

    for (int i = 0; i < 2; i++) {
        player->abilities[i] = &test_abilities[i];
    }
    player->ability_count = 2;

    bool ability_found[2] = {false, false};
    int found_count = 0;

    while (found_count < 2) {
        ability_t* random_ability = get_random_ability(player);
        for (int i = 0; i < 2; i++) {
            if (random_ability == player->abilities[i] && !ability_found[i]) {
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
    // test_get_random_ability(); not working due to recent changes on develop

    shutdown_memory_pool(test_combat_memory);
    return 0;
}
