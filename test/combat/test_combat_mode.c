#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "../src/combat/ability.h"
#include "../../src/character/character.h"
#include "../src/combat/pc.h"
//even CLion marks these as unused, they are needed for the tests
#include "../src/combat/damage.h"
#include "../src/combat/npc.h"
#include "../src/logging/logger.h"

#include "../src/combat/combat_mode.h"



void test_roll_hit() {
    // allocate memory for the test objects
    character_t* base = malloc(sizeof(character_t));
    ability_t* test_ability = malloc(sizeof(ability_t));

    base->health = 100; // set health
    base->armor = 0; // set armor

    test_ability->roll_count = 3;
    test_ability->accuracy = 100;
    test_ability->dice_size = D6;
    test_ability->damage_type = PHYSICAL;



    // Test with high accuracy ability (should always hit)
    if (roll_hit(base, test_ability) != true) {
        free(test_ability);
        free(base);
        assert(false);
    }

    // Test with low accuracy ability (should miss often)
    test_ability->accuracy = 0;
    base->deflection = 20;
    if (roll_hit(base, test_ability) != false) {
        free(test_ability);
        free(base);
        assert(false);
    }

    free(test_ability);
    free(base);
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
    character_t* base = malloc(sizeof(character_t));
    base->health = 100;
    base->armor = 10; // set armor

    player_t* test_player = malloc(sizeof(player_t));
    test_player->base = base;

    deal_damage_to_player(50, PHYSICAL, test_player);
    const int new_health = test_player->base->health;
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
