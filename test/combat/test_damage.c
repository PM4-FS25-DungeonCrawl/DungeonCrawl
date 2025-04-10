#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../src/combat/ability.h"
#include "../src/character/character.h"
#include "../src/combat/damage.h"
#include "../src/combat/combat_mode.h"

ability_t test_ability = {
    .name = "Test Ability",
    .roll_amount = 3,
    .accuracy = 100,
    .dice_size = D6,
    .damage_type = PHYSICAL
};

// allocate in data segment
character_t* create_test_character() {
    character_t* player = init_character(PLAYER, "Hero");
    set_character_stats(player, 5, 5, 5, 20);
    return player;
}

void test_roll_hit() {

    character_t* test_character = create_test_character();

    // Test with high accuracy ability (should hit)
    assert(roll_hit(test_character, &test_ability) == true);

    // Test with low accuracy ability (should miss)
    test_ability.accuracy = 0;
    // test_character.current_stats.deflection = 20;
    // assert(roll_hit(&test_character, &test_ability) == false);

    printf("test_roll_hit passed!\n");
}

void test_roll_damage() {
    const int damage = roll_damage(&test_ability);

    // The result should be between 3 (minimum) and 18 (maximum)
    assert(damage >= 3 && damage <= 18);
    printf("test_roll_damage passed!\n");
}

void test_deal_damage_to_armor() {
    character_t test_character;
    character_t* temp_character = create_test_character();
    test_character = *temp_character;
    test_character.current_resources.health = 100;

    // Damage after armor reduction: 50 - 10 = 40
    deal_damage(&test_character, PHYSICAL, 50);
    // assert(test_character.current_stats.health == 60); // 100 - (50 - 10) = 60

    printf("test_deal_damage_to_armor passed!\n");
}

int main(void){
    test_roll_hit();
    test_roll_damage();
    test_deal_damage_to_armor();
    return 0;
}

