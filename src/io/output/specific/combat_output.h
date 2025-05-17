#ifndef COMBAT_OUTPUT_H
#define COMBAT_OUTPUT_H

#include "../../../character/character_fw.h"
#include "../../../common.h"

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Draws the combat view UI.
 *
 * This function renders the combat view including player and enemy information,
 * as well as the enemy sprite.
 *
 * @param anchor The anchor point of the combat view, representing the top left corner
 * @param player The player character
 * @param enemy The enemy character
 * @param enemy_sprite The sprite of the enemy
 * @param sprite_height The height of the enemy sprite
 * @param red_enemy_sprite If true, the enemy sprite will be drawn in red
 * @return The new anchor point after drawing the combat view
 */
vector2d_t draw_combat_view(vector2d_t anchor, const character_t* player, const character_t* enemy, const char* enemy_sprite, int sprite_height, bool red_enemy_sprite);

/**
 * @brief Draws the combat menu.
 *
 * This function renders a menu for the combat mode with selectable options.
 *
 * @param anchor The anchor point of the combat menu, representing the top left corner
 * @param menu_name The name of the menu
 * @param menu_options The options of the menu
 * @param menu_option_count The number of options in the menu
 * @param selected_index The index of the selected option
 * @param tail_msg The message to be displayed at the bottom of the menu. If the message is NULL, it will not be displayed.
 */
void draw_combat_menu(vector2d_t anchor, const char* menu_name, char** menu_options, int menu_option_count, int selected_index, const char* tail_msg);

/**
 * @brief Draws the combat log.
 *
 * This function displays a combat log message and waits for user input before continuing.
 *
 * @param anchor The anchor point of the combat log, representing the top-left corner
 * @param combat_log_message The message to be displayed in the combat log
 */
void draw_combat_log(vector2d_t anchor, const char* combat_log_message);

/**
 * @brief Draws the game over screen.
 *
 * This function displays a game over message and waits for user input.
 */
void draw_game_over(void);


/**
 * @brief Draws the resource bar for a character.
 *
 * This function displays the character's name, health, mana, and stamina in a formatted bar.
 *
 * @param anchor The anchor point for the resource bar, representing the top-left corner
 * @param c A pointer to the character whose resources are to be displayed
 * @return The updated y-coordinate after drawing the resource bar
 */
int draw_resource_bar(vector2d_t anchor, const character_t* c);

#endif// COMBAT_OUTPUT_H
