#ifndef COMBAT_OUTPUT_H
#define COMBAT_OUTPUT_H

#include <stdbool.h>

// Forward declarations
typedef struct character_s character_t;
typedef struct ability_s ability_t;

// Combat action types
typedef enum {
    COMBAT_ACTION_ATTACK,
    COMBAT_ACTION_ABILITY,
    COMBAT_ACTION_POTION,
    COMBAT_ACTION_FLEE
} combat_action_t;

// Combat menu result
typedef enum {
    COMBAT_MENU_CONTINUE,
    COMBAT_MENU_EXIT,
    COMBAT_MENU_ABILITY,
    COMBAT_MENU_POTION
} combat_menu_result_t;

/**
 * @brief Initialize the combat output handler
 * 
 * @return true on success, false on failure
 */
bool init_combat_output(void);

/**
 * @brief Render the combat screen
 * 
 * @param player The player character
 * @param monster The monster character
 * @return true on success, false on failure
 */
bool render_combat_screen(const character_t* player, const character_t* monster);

/**
 * @brief Display damage animation
 * 
 * @param target The character receiving damage
 * @param damage The amount of damage
 * @param is_player Whether the target is the player
 * @return true on success, false on failure
 */
bool display_damage(const character_t* target, int damage, bool is_player);

/**
 * @brief Display ability animation
 * 
 * @param ability The ability being used
 * @param attacker The character using the ability
 * @param target The target of the ability
 * @param damage The amount of damage (if applicable)
 * @return true on success, false on failure
 */
bool display_ability(const ability_t* ability, const character_t* attacker, 
                    const character_t* target, int damage);

/**
 * @brief Show the combat menu
 * 
 * @param player The player character
 * @param monster The monster character
 * @return The menu result
 */
combat_menu_result_t show_combat_menu(const character_t* player, const character_t* monster);

/**
 * @brief Shutdown the combat output handler
 */
void shutdown_combat_output(void);

#endif // COMBAT_OUTPUT_H