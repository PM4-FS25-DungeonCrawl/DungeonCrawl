/**
 * @file combat_mode.h
 * @brief Declares combat mode state machine, including menus and combat operations.
 */
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

/**
 * @brief Initialize the combat mode
 * @return int 0 on success, 1 on failure
 * @note This function must be called before using any other functions in this module.
 */
int init_combat_mode();
/**
 * @brief Starts the loop for combat between the player and the monster.
 * @param player Pointer to the player character.
 * @param monster Pointer to the monster character.
 * @return combat_result_t The result of the combat.
 */
combat_result_t start_combat(character_t* player, character_t* monster);
/**
 * @brief Collects the menu options for the ability menu.
 * @param player Pointer to the player character.
 * @param monster Pointer to the monster character.
 */
internal_combat_state_t combat_menu(const character_t* player, const character_t* monster);
/**
 * @brief Collects the menu options for the ability menu.
 * @param abilities Pointer to the player character.
 * @param monster Pointer to the monster character.
 */
internal_combat_state_t ability_menu(character_t* player, character_t* monster);
/**
 * @brief Collects the menu options for the potion menu.
 * @param abilities Pointer to the player character.
 * @param monster Pointer to the monster character.
 */
internal_combat_state_t potion_menu(character_t* player, character_t* monster);
/**
 * @brief Invoke the effect of a potion on a character.
 * @param character Pointer to the character which uses the potion.
 * @param potion Pointer to which potion to be used.
 */
bool invoke_potion_effect(character_t* character, potion_t* potion);
/** @brief Consumes the mana or stamina resource of the attacker character.
 * @param attacker Pointer to the attacker character.
 * @param ability Pointer to the ability to be used.
 * @return true if the resource was consumed, false otherwise.
 */
bool consume_ability_resource(character_t* attacker, const ability_t* ability);
/**
 * @brief Shuts down the combat mode and frees allocated memory resources.
 * @note This function deallocates memory associated with combat mode and
 *       must be called before exiting the module to prevent memory leaks.
 */
void shutdown_combat_mode(void);

#endif//COMBAT_MODE_H
