#include <assert.h>
#include <stdio.h>
#include "../src/combat_mode.h"

// Helper function to initialize a test Character
Character create_test_character(CharacterType type, int health, int armor, int might, int deflection, int fortitude, int will) {
    Character character = {
        .type = type,
        .health = health,
        .armor = armor,
        .might = might,
        .deflection = deflection,
        .fortitude = fortitude,
        .will = will,
        .ability_count = 0
    };
    return character;
}

// Helper function to initialize a test Player
Player create_test_player(int health, int armor, int might, int deflection, int fortitude, int will) {
    Player player;
    player.base = create_test_character(PLAYER, health, armor, might, deflection, fortitude, will);
    return player;
}

// Helper function to initialize a test Monster
Monster create_test_monster(int health, int armor, int might, int deflection, int fortitude, int will, int weaknesses[]) {
    Monster monster;
    monster.base = create_test_character(MONSTER, health, armor, might, deflection, fortitude, will);
    initWeaknesses(&monster, weaknesses);
    return monster;
}

// Helper function to create a test Ability
Ability create_test_ability(const char *name, int rollCount, int accuracy, DiceSize diceSize, DamageType type) {
    Ability ability;
    initAbility(&ability, name, rollCount, accuracy, diceSize, type);
    return ability;
}

void test_roll_hit() {
    Monster defender = create_test_monster(100, 0, 0, 0, 0, 0, (int[]){0,0});
    Ability test_ability = create_test_ability("Test Ability", 3, 100, D6, PHYSICAL);

    // Test with high accuracy ability (should always hit)
    assert(roll_hit(&test_ability, &defender.base) == true);

    // Test with low accuracy ability (should miss often)
    test_ability.accuracy = 0;
    defender.base.deflection = 20;
    assert(roll_hit(&test_ability, &defender.base) == false);

    printf("test_roll_hit passed!\n");    
}

void test_roll_damage() {
    Ability test_ability = create_test_ability("Test Ability", 3, 100, D6, PHYSICAL);
    int damage = roll_damage(&test_ability);

    // The result should be between 3 (minimum) and 18 (maximum)
    assert(damage >= 3 && damage <= 18);
    printf("test_roll_damage passed!\n");
}

void test_deal_damage_to_armor() {
    Player test_player = create_test_player(100, 10, 0, 0, 0, 0);

    deal_damage(50, PHYSICAL, &test_player.base);

    // Damage after armor reduction: 50 - 10 = 40
    assert(test_player.base.health == 60);
    printf("test_deal_damage_to_armor passed!\n");
}

int main(void){
    test_roll_hit();
    test_roll_damage();
    test_deal_damage_to_armor();
    return 0;
}