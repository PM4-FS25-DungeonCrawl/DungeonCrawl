#include "draw_inventory_mode.h"

#include "../../../include/termbox2.h"
#include "../../character/character.h"

// Internal functions
int draw_inventory_resource_bar(vector2d_t anchor, const character_t* c);

/**
 * @brief This function draws the inventory view.
 * @param anchor The anchor point of the inventory view, representing the top left corner.
 * @param c The character of the inventory.
 * @return The new anchor point after drawing the inventory view.
 * @note This function clears and presents the inventory view.
 */
vector2d_t draw_inventory_view(const vector2d_t anchor, const character_t* c) {
    tb_clear();
    // Copy of the anchor
    vector2d_t vec = {anchor.dx, anchor.dy};

    // Draw the character's resource bar
    vec.dy = draw_inventory_resource_bar(vec, c);
    vec.dy += 2;

    tb_present();
    return vec;
}

/**
 * @brief This function draws the inventory menu.
 * @param anchor The anchor point of the inventory menu, representing the top left corner.
 * @param menu_name The name of the menu.
 * @param header_msg The message to be displayed at the top of the menu. If the message is NULL, it will not be displayed.
 * @param menu_options The options of the menu.
 * @param menu_option_count The number of options in the menu.
 * @param selected_index The index of the selected option.
 * @param tail_msg The message to be displayed at the bottom of the menu. If the message is NULL, it will not be displayed.
 * @note This function only presents the drawn menu (without clearing the screen).
 */
void draw_inventory_menu(const vector2d_t anchor, const char* menu_name, const char* header_msg, string_max_t* menu_options, const int menu_option_count, const int selected_index, const char* tail_msg) {
    if (menu_name == NULL || menu_options == NULL) {
        log_msg(ERROR, "Draw Inventory Menu", "Menu options are NULL");
        return;
    }
    vector2d_t vec = {anchor.dx, anchor.dy};
    tb_print(vec.dx, vec.dy++, TB_WHITE, TB_DEFAULT, menu_name);

    if (header_msg != NULL) {
        tb_print(vec.dx, vec.dy++, TB_WHITE, TB_DEFAULT, header_msg);
    }

    for (int i = 0; i < menu_option_count; i++) {
        if (i == selected_index) {
            char buffer[MAX_STRING_LENGTH];
            snprintf(buffer, sizeof(buffer), "> %-253s", menu_options[i].characters);
            tb_print(vec.dx, vec.dy++, TB_BOLD, TB_DEFAULT, buffer);
        } else {
            char buffer[MAX_STRING_LENGTH];
            snprintf(buffer, sizeof(buffer), "  %-253s", menu_options[i].characters);
            tb_print(vec.dx, vec.dy++, TB_WHITE, TB_DEFAULT, buffer);
        }
    }
    if (tail_msg != NULL) {
        tb_print(vec.dx, vec.dy + 2, TB_WHITE, TB_DEFAULT, tail_msg);
    }
    tb_present();
}

/**
 * @brief Draws an inventory log, showing the current action.
 * @param anchor The anchor point for the log.
 * @param inventory_log_message The message to be displayed in the log.
 */
void draw_inventory_log(vector2d_t anchor, const char* inventory_log_message) {
    if (inventory_log_message == NULL) {
        log_msg(ERROR, "Draw Inventory Log", "Given inventory log message is NULL");
        return;
    }

    char message[MAX_STRING_LENGTH];
    snprintf(message, sizeof(message), "%s", inventory_log_message);
    tb_print(anchor.dx, anchor.dy++, TB_WHITE, TB_DEFAULT, message);
    tb_print(anchor.dx, anchor.dy++, TB_WHITE, TB_DEFAULT, "Press any key to continue...");
    tb_present();

    // Waiting for input
    struct tb_event event;
    tb_poll_event(&event);
}

/**
 * @brief Helper function to draw the resource bar.
 * @param anchor The anchor point for the resource bar.
 * @param c The character whose resources are displayed.
 * @return The updated y-coordinate after drawing the resource bar.
 */
int draw_inventory_resource_bar(vector2d_t anchor, const character_t* c) {
    const char* str_format = "%-10s | Health %-4d | Mana %-4d | Stamina %-4d";

    char c_info[MAX_STRING_LENGTH];
    snprintf(c_info, sizeof(c_info), str_format, c->name, c->current_resources.health, c->current_resources.mana, c->current_resources.stamina);

    tb_print(anchor.dx, anchor.dy++, TB_WHITE, TB_DEFAULT, c_info);
    return anchor.dy;
}
