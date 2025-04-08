#include "../src/combat_mode.h"

#include <assert.h>
#include <stdio.h>

// Helper function to initialize a test Character
character_t create_test_character(character_type_t type, int health, int armor, int might, int deflection, int fortitude, int will) {
    character_t character = {
            .type = type,
            .health = health,
            .armor = armor,
            .might = might,
            .deflection = deflection,
            .fortitude = fortitude,
            .will = will,
            .ability_count = 0};
    return character;
}

// Helper function to initialize a test Player
player_t create_test_player(int health, int armor, int might, int deflection, int fortitude, int will) {
    player_t player;
    player.base = create_test_character(PLAYER, health, armor, might, deflection, fortitude, will);
    return player;
}

// Helper function to initialize a test Monster
monster_t create_test_monster(int health, int armor, int might, int deflection, int fortitude, int will, int weaknesses[]) {
    monster_t monster;
    monster.base = create_test_character(MONSTER, health, armor, might, deflection, fortitude, will);
    initWeaknesses(&monster, weaknesses);
    return monster;
}

// Helper function to create a test Ability
ability_t create_test_ability(const char* name, int rollCount, int accuracy, dice_size_t diceSize, damage_type_t type) {
    ability_t ability;
    initAbility(&ability, name, rollCount, accuracy, diceSize, type);
    return ability;
}

void test_roll_hit() {
    monster_t defender = create_test_monster(100, 0, 0, 0, 0, 0, (int[]) {0, 0});
    ability_t test_ability = create_test_ability("Test Ability", 3, 100, D6, PHYSICAL);

    // Test with high accuracy ability (should always hit)
    assert(roll_hit(&test_ability, &defender.base) == true);

    // Test with low accuracy ability (should miss often)
    test_ability.accuracy = 0;
    defender.base.deflection = 20;
    assert(roll_hit(&test_ability, &defender.base) == false);

    printf("test_roll_hit passed!\n");
}

void test_roll_damage() {
    ability_t test_ability = create_test_ability("Test Ability", 3, 100, D6, PHYSICAL);
    int damage = roll_damage(&test_ability);

    // The result should be between 3 (minimum) and 18 (maximum)
    assert(damage >= 3 && damage <= 18);
    printf("test_roll_damage passed!\n");
}

void test_deal_damage_to_armor() {
    player_t test_player = create_test_player(100, 10, 0, 0, 0, 0);

    deal_damage(50, PHYSICAL, &test_player.base);

    // Damage after armor reduction: 50 - 10 = 40
    assert(test_player.base.health == 60);
    printf("test_deal_damage_to_armor passed!\n");
}

int main(void) {
    test_roll_hit();
    test_roll_damage();
    test_deal_damage_to_armor();
    return 0;
}
