#include "../../src/character/character.h"
#include "../../src/combat/ability.h"
#include "../../src/combat/combat_mode.h"
#include "../../src/common.h"
#include "../../src/local/local.h"

#include <assert.h>
#include <stdio.h>


void setup() {
    init_local();
    printf("Test");
    main_memory_pool = init_memory_pool(MIN_MEMORY_POOL_SIZE);
    if (main_memory_pool == NULL) {
        printf("Failed to initialize memory pool\n");
        exit(EXIT_FAILURE);
    }
    init_combat_mode();
}


character_t* create_test_character() {
    character_t* character = init_character(main_memory_pool, PLAYER, "Hero");
    set_character_stats(character, 5, 5, 5, 20);
    return character;
}

void test_use_ability() {
    character_t* player = create_test_character();
    character_t* enemy = create_test_character();

    ability_t test_ability = {
            .name = "Test Ability",
            .roll_amount = 3,
            .accuracy = 100,
            .dice_size = D6,
            .damage_type = PHYSICAL,
            .resource_cost = 15};

    // Test: Ability with sufficient stamina
    player->current_resources.stamina = 15;// to be removed after implementation of stamina calculation based on strength
    use_ability(player, enemy, &test_ability);
    assert(player->current_resources.stamina == 0);

    // Test: Ability with insufficient stamina
    player->current_resources.stamina = 14;
    use_ability(player, enemy, &test_ability);
    assert(player->current_resources.stamina == 14);

    // Test: Ability with sufficient mana
    test_ability.damage_type = MAGICAL;
    player->current_resources.mana = 15;// to be removed after implementation of mana calculation based on intelligence
    use_ability(player, enemy, &test_ability);
    assert(player->current_resources.mana == 0);

    // Test: Ability with insufficient mana
    player->current_resources.mana = 14;
    use_ability(player, enemy, &test_ability);
    assert(player->current_resources.mana == 14);

    printf("test_use_ability passed\n");
}

void test_use_item() {
    character_t* player = create_test_character();
    character_t* enemy = create_test_character();

    potion_t test_potion = {
            .effectType = HEALING,
            .value = 20};

    player->potion_inventory[0] = &test_potion;
    player->potion_inventory[1] = &test_potion;
    player->potion_count = 2;
    player->current_resources.health = 65;

    // Test: Use potion to heal
    use_potion(player, enemy, player->potion_inventory[0]);
    assert(player->current_resources.health == 85);

    // Test: Use potion to heal to max health
    use_potion(player, enemy, player->potion_inventory[0]);
    assert(player->current_resources.health == 100);

    // Test: Ensure potion inventory count is updated
    assert(player->potion_count == 0);

    printf("test_use_item passed\n");
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

    printf("test_get_random_ability passed\n");
}

int main(void) {
    setup();
    test_use_ability();
    test_use_item();
    test_get_random_ability();
    shutdown_memory_pool(main_memory_pool);
    return 0;
}
