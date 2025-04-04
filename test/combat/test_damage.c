#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "../src/combat/ability.h"
#include "../src/character/character.h"
#include "../src/combat/damage.h"

#include "../src/combat/combat_mode.h"


void test_roll_hit() {
    // allocate memory for the test objects
    character_t* test_character = malloc(sizeof(character_t));
    ability_t* test_ability = malloc(sizeof(ability_t));

    test_character->health = 100; // set health
    test_character->armor = 0; // set armor

    test_ability->roll_amount = 3;
    test_ability->accuracy = 100;
    test_ability->dice_size = D6;
    test_ability->damage_type = PHYSICAL;



    // Test with high accuracy ability (should always hit)
    if (roll_hit(test_character, test_ability) != true) {
        free(test_ability);
        free(test_character);
        assert(false);
    }

    // Test with low accuracy ability (should miss often)
    test_ability->accuracy = 0;
    test_character->deflection = 20;
    if (roll_hit(test_character, test_ability) != false) {
        free(test_ability);
        free(test_character);
        assert(false);
    }

    free(test_ability);
    free(test_character);
    printf("test_roll_hit passed!\n");
}

void test_roll_damage() {
    ability_t* test_ability = malloc(sizeof(ability_t));

    const int damage = roll_damage(test_ability);
    free(test_ability); // free memory before assert

    // The result should be between 3 (minimum) and 18 (maximum)
    assert(damage >= 3 && damage <= 18);
    printf("test_roll_damage passed!\n");
}

void test_deal_damage_to_armor() {
    character_t* test_player = malloc(sizeof(character_t));
    test_player->health = 100;
    test_player->armor = 10; // set armor


    deal_damage(test_player, PHYSICAL, 50);
    const int new_health = test_player->health;
    free(test_player); // free memory before assert

    // Damage after armor reduction: 50 - 10 = 40
    assert(new_health == 60);
    printf("test_deal_damage_to_armor passed!\n");
}

int main(void){
    test_roll_hit();
    test_roll_damage();
    test_deal_damage_to_armor();
    return 0;
}

