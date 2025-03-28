#include <assert.h>
#include <stdio.h>
#include "../src/combat_mode.h"

const Character test_base = {
    .health = 100,
    .armor = 10,
    .might = 10,
    .deflection = 10,
    .fortitude = 10,
    .will = 10
};

Ability test_ability = {
    .damageType = PHYSICAL,
    .accuracy = 100,
    .rollCount = 3,
    .diceSize = 6
};

void test_roll_damage() {
    int damage = roll_damage(&test_ability);

    // The result should be between 3 (minimum) and 18 (maximum)
    assert(damage >= 3 && damage <= 18);
    printf("test_roll_damage passed!\n");
}

void test_deal_damage_to_armor() {
    Player test_player;
    test_player.base = test_base;

    deal_damage(50, PHYSICAL, &test_player.base);

    // Damage after armor reduction: 50 - 10 = 40
    assert(test_player.base.health == 60);
    printf("test_deal_damage_to_armor passed!\n");
}

int main(void){
    test_roll_damage();
    test_deal_damage_to_armor();
    return 0;
}