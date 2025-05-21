#include "inventory_output.h"

#include "../../../character/character.h"
#include "../../../common.h"
#include "../../../logging/logger.h"
#include "../../input/input_handler.h"
#include "../../io_handler.h"
#include "../common/output_handler.h"

/**
 * @brief Draws the inventory view UI.
 *
 * @param anchor The anchor point of the inventory view, representing the top left corner
 * @param player The player character
 * @return The new anchor point after drawing the inventory view
 */
vector2d_t draw_inventory_view(const vector2d_t anchor, const character_t* player) {
    // Clear the screen
    clear_screen();

    // Copy of the anchor
    vector2d_t vec = {anchor.dx, anchor.dy};

    // Draw the character's resource bar
    vec.dy = draw_inventory_resource_bar(vec, player);
    vec.dy += 2;

    // Render the frame
    render_frame();

    return vec;
}

/**
 * @brief Draws the inventory menu.
 *
 * @param anchor The anchor point of the inventory menu
 * @param menu_name The name of the menu
 * @param header_msg The message to display at the top (can be NULL)
 * @param menu_options The options of the menu
 * @param menu_option_count The number of options in the menu
 * @param selected_index The index of the selected option
 * @param key_msg First message to display at the bottom (can be NULL)
 * @param tail_msg Second message to display at the bottom (can be NULL)
 */
void draw_inventory_menu(const vector2d_t anchor, const char* menu_name, const char* header_msg,
                         char** menu_options, const int menu_option_count,
                         const int selected_index, const char* key_msg, const char* tail_msg) {
    // Validate parameters
    if (menu_name == NULL || menu_options == NULL) {
        log_msg(ERROR, "Inventory Output", "Menu options are NULL");
        return;
    }

    vector2d_t vec = {anchor.dx, anchor.dy};

    // Draw menu title
    print_text_default(vec.dy, 1, menu_name);
    vec.dy++;

    // Draw header message if provided
    if (header_msg != NULL) {
        print_text_default(vec.dy++, vec.dx, header_msg);
    }

    // Draw menu items highlighting the selected one
    for (int i = 0; i < menu_option_count; i++) {
        if (i == selected_index) {
            // Use > to indicate selected item and bold styling
            char buffer[MAX_STRING_LENGTH];
            snprintf(buffer, sizeof(buffer), "> %s", menu_options[i]);
            // For selected items, use inverted colors
            print_text(vec.dy, anchor.dx, buffer, INVERTED_COLORS);
        } else {
            // Use regular formatting for non-selected items
            char buffer[MAX_STRING_LENGTH];
            snprintf(buffer, sizeof(buffer), "  %s", menu_options[i]);
            print_text_default(vec.dy, anchor.dx, buffer);
        }
        vec.dy++;
    }

    // Draw key message if provided
    if (key_msg != NULL) {
        print_text_default(vec.dy + 1, vec.dx, key_msg);
    }

    // Draw tail message if provided
    if (tail_msg != NULL) {
        print_text_default(vec.dy + 2, vec.dx, tail_msg);
    }

    // Render the frame
    render_frame();
}

/**
 * @brief Draws the inventory log.
 *
 * @param anchor The anchor point of the inventory log
 * @param inventory_log_message The message to be displayed
 */
void draw_inventory_log(vector2d_t anchor, const char* inventory_log_message) {
    // Validate parameters
    if (inventory_log_message == NULL) {
        log_msg(ERROR, "Inventory Output", "Given inventory log message is NULL");
        return;
    }

    // Display the log message
    print_text_default(anchor.dy, anchor.dx, inventory_log_message);
    anchor.dy++;
    print_text_default(anchor.dy, anchor.dx, "Press any key to continue...");
    anchor.dy++;

    // Render the frame
    render_frame();

    // Use our input handler to get any key press
    input_event_t input_event;
    get_input_blocking(&input_event);
}

/**
 * @brief Draws the resource bar for a character in the inventory view.
 *
 * @param anchor The anchor point for the resource bar
 * @param c A pointer to the character whose resources are to be displayed
 * @return The updated y-coordinate after drawing the resource bar
 */
int draw_inventory_resource_bar(vector2d_t anchor, const character_t* c) {
    char c_info[MAX_STRING_LENGTH];
    snprintf(c_info, sizeof(c_info), "%-20s | HP: %4d/%-4d | Mana: %4d/%-4d | Stamina: %4d/%-4d",
             c->name,
             c->current_resources.health, c->max_resources.health,
             c->current_resources.mana, c->max_resources.mana,
             c->current_resources.stamina, c->max_resources.stamina);

    // Display the resource bar
    print_text_default(anchor.dy, anchor.dx, c_info);
    anchor.dy++;

    return anchor.dy;
}