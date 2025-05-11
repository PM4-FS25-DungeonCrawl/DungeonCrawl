#ifndef COMBAT_MODE_H
#define COMBAT_MODE_H

#include "../character/character_fw.h"
#include "../item/potion.h"
#include "ability_fw.h"

typedef enum {
    COMBAT_MENU,
    ABILITY_MENU,
    POTION_MENU,
    EVALUATE_COMBAT,// checks if the combat reached an end
    COMBAT_EXIT     // exit combat & game
} internal_combat_state_t;

typedef enum {
    CONTINUE_COMBAT,
    PLAYER_WON,
    PLAYER_LOST,
    EXIT_GAME
} combat_result_t;

typedef enum {
    RACE_GOBLIN
} enemy_race_t;

/**
 * @brief Initialize the combat mode
 * @return int 0 on success, 1 on failure
 * @note This function must be called before using any other functions in this module.
 */
int
init_combat_mode();
combat_result_t start_combat(character_t* player, character_t* monster);
internal_combat_state_t combat_menu(const character_t* player, const character_t* monster);
internal_combat_state_t ability_menu(character_t* player, character_t* monster);
internal_combat_state_t potion_menu(character_t* player, character_t* monster);

// TODO: These functions should not be here!
void use_ability(character_t* attacker, character_t* target, const ability_t* ability);
void use_potion(character_t* player, const character_t* monster, potion_t* potion);
void invoke_potion_effect(character_t* character, potion_t* potion);
bool consume_ability_resource(character_t* attacker, const ability_t* ability);
ability_t* get_random_ability(const character_t* character);

/**
 * @brief Shuts down the combat mode and frees allocated memory resources.
 * @note This function deallocates memory associated with combat mode and
 *       must be called before exiting the module to prevent memory leaks.
 */
void shutdown_combat_mode(void);

#endif//COMBAT_MODE_H
