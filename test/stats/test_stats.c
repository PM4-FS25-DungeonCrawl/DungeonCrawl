#include "../../src/character/character.h"
#include "../../src/memory/memory_management.h"
#include "../../src/stats/stats.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void setup() {
    test_memory_pool = init_memory_pool(MIN_MEMORY_POOL_SIZE);
    if (test_memory_pool == NULL) {
        printf("Failed to initialize memory pool\n");
        exit(EXIT_FAILURE);
    }
    printf("Setup complete\n");
}


character_t* create_test_character() {
    character_t* character = init_character(test_memory_pool, PLAYER, "Hero");
    set_character_stats(character, 5, 5, 5, 20);
    set_skill_points(character, 10);
    return character;
}


void test_skill_point_allocation() {
    character_t* test_character = create_test_character();
    assert(test_character != NULL);
    assert(test_character->skill_points == 10);
    printf("Testing skill point allocation...\n");


    // Record initial max resources
    int initial_max_health = test_character->max_resources.health;
    int initial_max_mana = test_character->max_resources.mana;
    int initial_max_stamina = test_character->max_resources.stamina;
    // Due to dexterity accuracy not implemented yet, we will use dexterity as a placeholder
    int initial_max_dexterity = test_character->base_stats.dexterity;

    // Allocate a point to constitution
    assert(test_character->skill_points > 0);
    // Level up constitution to raise HP
    raise_skill(&test_character->base_stats, CONSTITUTION, test_character->skill_points);
    test_character->skill_points--;
    update_character_resources(&test_character->max_resources, &test_character->base_stats);

    // Check if health increased (strength affects health)
    assert(test_character->max_resources.health > initial_max_health);
    printf("✓ Allocating to strength increases max health\n");


    // Allocate a point to intelligence
    assert(test_character->skill_points > 0);
    raise_skill(&test_character->base_stats, INTELLIGENCE, test_character->skill_points);
    test_character->skill_points--;
    update_character_resources(&test_character->max_resources, &test_character->base_stats);

    // Check if mana increased (intelligence affects mana)
    assert(test_character->max_resources.mana > initial_max_mana);
    printf("✓ Allocating to intelligence increases max mana\n");


    // Allocate a point to strength
    assert(test_character->skill_points > 0);
    raise_skill(&test_character->base_stats, STRENGTH, test_character->skill_points);
    test_character->skill_points--;
    update_character_resources(&test_character->max_resources, &test_character->base_stats);

    // Check if dexterity increased (strength affects dexterity)
    assert(test_character->max_resources.stamina > initial_max_stamina);
    printf("✓ Allocating to strength increases dexterity\n");

    // Allocate a point to dexterity
    assert(test_character->skill_points > 0);
    raise_skill(&test_character->base_stats, DEXTERITY, test_character->skill_points);
    test_character->skill_points--;
    update_character_resources(&test_character->max_resources, &test_character->base_stats);

    // Check if stamina increased (dexterity affects stamina)
    assert(test_character->base_stats.dexterity > initial_max_dexterity);
    printf("✓ Allocating to dexterity\n");

    // Test when skill points are 0
    test_character->skill_points = 0;
    int old_strength = test_character->base_stats.strength;
    raise_skill(&test_character->base_stats, STRENGTH, test_character->skill_points);

    // Verify stat doesn't increase when no skill points are available
    assert(test_character->base_stats.strength == old_strength);
    printf("✓ Cannot allocate stats when skill points are 0\n");

    printf("All skill point allocation tests passed!\n");
}

int main() {
    setup();
    printf("Running stats tests...\n");

    test_skill_point_allocation();


    printf("All stats tests passed successfully!\n");
    shutdown_memory_pool(test_memory_pool);
    return 0;
}