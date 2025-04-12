#include "../src/character/character.h"
#include "../src/combat/ability.h"
#include "../src/combat/combat_mode.h"
#include "../src/combat/damage.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

ability_t test_ability = {
        .name = "Test Ability",
        .roll_amount = 3,
        .accuracy = 100,
        .dice_size = D6,
        .damage_type = PHYSICAL};

// allocate in data segment
character_t* create_test_character() {
    character_t* player = init_character(PLAYER, "Hero");
    set_character_stats(player, 5, 5, 5, 20);
    return player;
}

void test_roll_hit() {
    character_t* attacker = create_test_character();
    character_t* defender = create_test_character();

    // attacker has a higher dexterity value, should hit
    attacker->current_stats.dexterity = 20;
    defender->current_stats.dexterity = 5;
    assert(roll_hit(attacker->current_stats.dexterity, defender->current_stats.dexterity) == true);

    // defender has a higher dexterity value, should miss
    attacker->current_stats.dexterity = 5;
    defender->current_stats.dexterity = 20;
    assert(roll_hit(attacker->current_stats.dexterity, defender->current_stats.dexterity) == false);

    printf("test_roll_hit passed!\n");
}

void test_roll_damage() {
    const int damage = roll_damage(&test_ability);

    // result should be inside the possible range (depending on ability)
    assert(damage >= 3 && damage <= 18);
    printf("test_roll_damage passed!\n");
}

void test_deal_damage_to_armor() {
    character_t test_character;
    character_t const* temp_character = create_test_character();
    test_character = *temp_character;
    test_character.current_resources.health = 100;
    test_character.defenses.armor = 10;

    // damage after armor reduction: 50 - 10 = 40
    deal_damage(&test_character, PHYSICAL, 50);
    assert(test_character.current_resources.health == 60); // 100 - (50 - 10) = 60

    printf("test_deal_damage_to_armor passed!\n");
}

void test_reset_current_stats() {
    character_t* test_character = create_test_character();

    // change base values
    test_character->current_stats.strength = 10;
    test_character->current_stats.intelligence = 15;
    test_character->current_stats.dexterity = 20;
    test_character->current_stats.constitution = 25;

    // reset stats
    reset_current_stats(test_character);

    // check if current stats are equal to base stats
    assert(test_character->current_stats.strength == test_character->base_stats.strength);
    assert(test_character->current_stats.intelligence == test_character->base_stats.intelligence);
    assert(test_character->current_stats.dexterity == test_character->base_stats.dexterity);
    assert(test_character->current_stats.constitution == test_character->base_stats.constitution);

    printf("test_reset_current_stats passed!\n");
}

int main(void) {
    test_roll_hit();
    test_roll_damage();
    //test_deal_damage_to_armor();
    test_reset_current_stats();
    return 0;
}
