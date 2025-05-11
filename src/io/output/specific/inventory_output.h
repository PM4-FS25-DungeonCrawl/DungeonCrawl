#ifndef INVENTORY_OUTPUT_H
#define INVENTORY_OUTPUT_H

#include "../../../character/character_fw.h"
#include "../../../common.h"
#include <stdbool.h>

/**
 * @brief Draws the inventory view UI.
 *
 * This function renders the inventory view including player resources and inventory space.
 *
 * @param anchor The anchor point of the inventory view, representing the top left corner
 * @param player The player character
 * @return The new anchor point after drawing the inventory view
 */
vector2d_t draw_inventory_view(vector2d_t anchor, const character_t* player);

/**
 * @brief Draws the inventory menu.
 *
 * This function renders a menu for the inventory mode with selectable options.
 *
 * @param anchor The anchor point of the inventory menu, representing the top left corner
 * @param menu_name The name of the menu
 * @param header_msg The message to display at the top of the menu (can be NULL)
 * @param menu_options The options of the menu
 * @param menu_option_count The number of options in the menu
 * @param selected_index The index of the selected option
 * @param key_msg First message to display at the bottom of the menu (can be NULL)
 * @param tail_msg Second message to display at the bottom of the menu (can be NULL)
 */
void draw_inventory_menu(vector2d_t anchor, const char* menu_name, const char* header_msg, 
                         string_max_t* menu_options, int menu_option_count, 
                         int selected_index, const char* key_msg, const char* tail_msg);

/**
 * @brief Draws the inventory log.
 *
 * This function displays an inventory log message and waits for user input.
 *
 * @param anchor The anchor point of the inventory log, representing the top-left corner
 * @param inventory_log_message The message to be displayed in the inventory log
 */
void draw_inventory_log(vector2d_t anchor, const char* inventory_log_message);

/**
 * @brief Draws the resource bar for a character in the inventory view.
 *
 * This function displays the character's name, health, mana, and stamina.
 *
 * @param anchor The anchor point for the resource bar, representing the top-left corner
 * @param c A pointer to the character whose resources are to be displayed
 * @return The updated y-coordinate after drawing the resource bar
 */
int draw_inventory_resource_bar(vector2d_t anchor, const character_t* c);

#endif // INVENTORY_OUTPUT_H