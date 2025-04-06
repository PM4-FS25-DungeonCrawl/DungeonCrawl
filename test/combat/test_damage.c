#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../src/combat/ability.h"
#include "../src/character/character.h"
#include "../src/combat/damage.h"

#include "../src/combat/combat_mode.h"


// allocate in data segment
character_t test_character = {
    .type = PLAYER,
    .name = "Test Character",
    .health = 100,
    .armor = 0,
    .might = 0,
    .deflection = 0,
    .fortitude = 0,
    .will = 0,
    .resistance = {0},
    .abilities = NULL,
    .ability_count = 0,
    .items = NULL,
    .item_count = 0
};
ability_t test_ability = {
    .name = "Test Ability",
    .roll_amount = 3,
    .accuracy = 100,
    .dice_size = D6,
    .damage_type = PHYSICAL
};

void test_roll_hit() {

    // Test with high accuracy ability (should hit)
    assert(roll_hit(&test_character, &test_ability) == true);

    // Test with low accuracy ability (should miss)
    test_ability.accuracy = 0;
    test_character.deflection = 20;
    assert(roll_hit(&test_character, &test_ability) == false);

    printf("test_roll_hit passed!\n");
}

void test_roll_damage() {
    const int damage = roll_damage(&test_ability);

    // The result should be between 3 (minimum) and 18 (maximum)
    assert(damage >= 3 && damage <= 18);
    printf("test_roll_damage passed!\n");
}

void test_deal_damage_to_armor() {
    test_character.health = 100;
    test_character.armor = 10; // set armor

    // Damage after armor reduction: 50 - 10 = 40
    deal_damage(&test_character, PHYSICAL, 50);
    assert(test_character.health == 60); // 100 - (50 - 10) = 60

    printf("test_deal_damage_to_armor passed!\n");
}

int main(void){
    test_roll_hit();
    test_roll_damage();
    test_deal_damage_to_armor();
    return 0;
}

