/**
 * @file inventory_output.c
 * @brief Implements outputting to the screen for the inventory mode.
 */
#include "inventory_output.h"

#include "../../../character/character.h"
#include "../../../common.h"
#include "../../../logging/logger.h"
#include "../../input/input_handler.h"
#include "../../io_handler.h"
#include "../common/output_handler.h"
#include "src/inventory/local/inventory_mode_local.h"


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

void draw_inventory_log(vector2d_t anchor, const char* inventory_log_message) {
    // Validate parameters
    if (inventory_log_message == NULL) {
        log_msg(ERROR, "Inventory Output", "Given inventory log message is NULL");
        return;
    }

    // Display the log message
    print_text_default(anchor.dy, anchor.dx, inventory_log_message);
    anchor.dy++;
    print_text_default(anchor.dy, anchor.dx, inventory_mode_strings[PRESS_ANY_CONTINUE]);
    anchor.dy++;

    // Render the frame
    render_frame();

    // Use our input handler to get any key press
    input_event_t input_event;
    get_input_blocking(&input_event);
}

int draw_inventory_resource_bar(vector2d_t anchor, const character_t* c) {
    char c_info[MAX_STRING_LENGTH];
    snprintf(c_info, sizeof(c_info), "%-20s | %s: %4d/%-4d | %s: %4d/%-4d | %s: %4d/%-4d",
             c->name,
             inventory_mode_strings[HEALTH_STR], c->current_resources.health, c->max_resources.health,
             inventory_mode_strings[MANA_STR], c->current_resources.mana, c->max_resources.mana,
             inventory_mode_strings[STAMINA_STR], c->current_resources.stamina, c->max_resources.stamina);

    // Display the resource bar
    print_text_default(anchor.dy, anchor.dx, c_info);
    anchor.dy++;

    return anchor.dy;
}
