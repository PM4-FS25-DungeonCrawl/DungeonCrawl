#include "draw_inventory_mode.h"

#include "../../character/character.h"
#include "../../game.h"

#include <notcurses/notcurses.h>

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
    clear_screen(stdplane);

    // Copy of the anchor
    vector2d_t vec = {anchor.dx, anchor.dy};

    // Draw the character's resource bar
    vec.dy = draw_inventory_resource_bar(vec, c);
    vec.dy += 2;

    notcurses_render(nc);
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
 * @param key_msg The first message to be displayed at the bottom of the menu. If the message is NULL, it will not be displayed.
 * @param tail_msg The second message to be displayed at the bottom of the menu. If the message is NULL, it will not be displayed.
 * @note This function only presents the drawn menu (without clearing the screen).
 */
void draw_inventory_menu(const vector2d_t anchor, const char* menu_name, const char* header_msg, string_max_t* menu_options, const int menu_option_count, const int selected_index, const char* key_msg, const char* tail_msg) {
    if (menu_name == NULL || menu_options == NULL) {
        log_msg(ERROR, "Draw Inventory Menu", "Menu options are NULL");
        return;
    }
    vector2d_t vec = {anchor.dx, anchor.dy};

    ncplane_set_channels(stdplane, DEFAULT_COLORS);
    ncplane_set_styles(stdplane, NCSTYLE_NONE);
    ncplane_putstr_yx(stdplane, vec.dy, 1, menu_name);
    vec.dy++;

    if (header_msg != NULL) {
        ncplane_set_styles(stdplane, NCSTYLE_NONE);
        ncplane_printf_yx(stdplane, vec.dy++, vec.dx, header_msg);
    }

    for (int i = 0; i < menu_option_count; i++) {
        char buffer[MAX_STRING_LENGTH];

        if (i == selected_index) {
            snprintf(buffer, sizeof(buffer), "> %-253s", (char*) &menu_options[i]);
            ncplane_set_styles(stdplane, NCSTYLE_BOLD);
        } else {
            snprintf(buffer, sizeof(buffer), "  %-253s", (char*) &menu_options[i]);
            ncplane_set_styles(stdplane, NCSTYLE_NONE);
        }

        ncplane_putstr_yx(stdplane, vec.dy, anchor.dx, buffer);
        vec.dy++;
    }

    if (key_msg != NULL) {
        ncplane_set_styles(stdplane, NCSTYLE_NONE);
        ncplane_printf_yx(stdplane, vec.dy + 1, vec.dx, key_msg);
    }

    if (tail_msg != NULL) {
        ncplane_set_styles(stdplane, NCSTYLE_NONE);
        ncplane_printf_yx(stdplane, vec.dy + 2, vec.dx, tail_msg);
    }
    notcurses_render(nc);
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

    ncplane_set_channels(stdplane, DEFAULT_COLORS);

    ncplane_putstr_yx(stdplane, anchor.dy, anchor.dx, message);
    anchor.dy++;
    ncplane_putstr_yx(stdplane, anchor.dy, anchor.dx, "Press any key to continue...");
    anchor.dy++;
    notcurses_render(nc);

    // Waiting for input
    ncinput input;
    notcurses_get_blocking(nc, &input);
}

/**
 * @brief Helper function to draw the resource bar.
 * @param anchor The anchor point for the resource bar.
 * @param c The character whose resources are displayed.
 * @return The updated y-coordinate after drawing the resource bar.
 */
int draw_inventory_resource_bar(vector2d_t anchor, const character_t* c) {
    char c_info[MAX_STRING_LENGTH];
    snprintf(c_info, sizeof(c_info), "%-20s | HP: %4d/%-4d | Mana: %4d/%-4d | Stamina: %4d/%-4d",
         c->name,
         c->current_resources.health, c->max_resources.health,
         c->current_resources.mana, c->max_resources.mana,
         c->current_resources.stamina, c->max_resources.stamina);

    // White on black
    ncplane_set_channels(stdplane, DEFAULT_COLORS);

    ncplane_putstr_yx(stdplane, anchor.dy, anchor.dx, c_info);
    anchor.dy++;
    return anchor.dy;
}
