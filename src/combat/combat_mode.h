#ifndef COMBAT_MODE_H
#define COMBAT_MODE_H

#include "../character/character_fw.h"
#include "../item/potion.h"
#include "ability_fw.h"

typedef enum {
    COMBAT_MENU,
    ABILITY_MENU,
    ITEM_MENU,
    GEAR_MENU,
    EVALUATE_COMBAT,// checks if the combat reached an end
    COMBAT_EXIT     // exit combat & game
} internal_combat_state_t;

typedef enum {
    PLAYER_WON,
    PLAYER_LOST,
    EXIT_GAME
} combat_result_t;

void init_combat_mode(void);
combat_result_t start_combat(character_t* player, character_t* monster);
internal_combat_state_t combat_menu(const character_t* player, const character_t* monster);
internal_combat_state_t ability_menu(character_t* player, character_t* monster);
internal_combat_state_t potion_menu(character_t* player, character_t* monster);
internal_combat_state_t gear_menu(character_t* player);
internal_combat_state_t gear_inventory_menu(character_t* player);
internal_combat_state_t equipment_menu(character_t* player);

void use_ability(character_t* attacker, character_t* target, const ability_t* ability);
void use_potion(character_t* player, const character_t* monster, potion_t* item);
bool use_usable_item(character_t* character, potion_t* item);
bool consume_ability_resource(character_t* attacker, const ability_t* ability);
ability_t* get_random_ability(const character_t* character);
void shutdown_combat_mode(void);

#endif//COMBAT_MODE_H
