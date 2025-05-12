#include "combat_output.h"

#include "../../../character/character.h"
#include "../../../common.h"
#include "../../../game.h"
#include "../../../logging/logger.h"
#include "../../input/input_handler.h"
#include "../../io_handler.h"
#include "../common/common_output.h"

#include <string.h>

/**
 * @brief Draws the combat view UI
 * 
 * @param anchor The anchor point of the combat view, representing the top left corner
 * @param player The player character
 * @param enemy The enemy character
 * @param enemy_sprite The sprite of the enemy
 * @param sprite_height The height of the enemy sprite
 * @param red_enemy_sprite If true, the enemy sprite will be drawn in red
 * @return The new anchor point after drawing the combat view
 */
vector2d_t draw_combat_view(const vector2d_t anchor, const character_t* player, const character_t* enemy,
                            const char* enemy_sprite, const int sprite_height, const bool red_enemy_sprite) {
    clear_screen();

    // Copy of the anchor
    vector2d_t vec = {anchor.dx, anchor.dy};

    // Draw title
    print_text(vec.dy, anchor.dx + 20, "Combat Mode", DEFAULT_COLORS);
    vec.dy += 2;

    // Draw resource bars
    vec.dy = draw_resource_bar(vec, player);
    vec.dy = draw_resource_bar(vec, enemy);
    vec.dy += 2;

    // Print the enemy sprite line for line
    if (red_enemy_sprite) {
        print_text(vec.dy, anchor.dx, enemy_sprite, RED_TEXT_COLORS);
    } else {
        print_text_default(vec.dy, anchor.dx, enemy_sprite);
    }

    vec.dy += sprite_height;
    vec.dy += 1;

    // Render the frame
    render_io_frame();

    return vec;
}

/**
 * @brief Draws the combat menu
 * 
 * @param anchor The anchor point of the combat menu, representing the top left corner
 * @param menu_name The name of the menu
 * @param menu_options The options of the menu
 * @param menu_option_count The number of options in the menu
 * @param selected_index The index of the selected option
 * @param tail_msg The message to be displayed at the bottom of the menu
 */
void draw_combat_menu(const vector2d_t anchor, const char* menu_name, string_max_t* menu_options,
                      const int menu_option_count, const int selected_index, const char* tail_msg) {
    if (menu_name == NULL || menu_options == NULL) {
        log_msg(ERROR, "Combat Output", "Menu options are NULL");
        return;
    }
    vector2d_t vec = {anchor.dx, anchor.dy};

    // Convert string_max_t options to char* array for print_menu
    const char* options[menu_option_count];
    for (int i = 0; i < menu_option_count; i++) {
        options[i] = (const char*) &menu_options[i];
    }

    // Use centralized menu drawing function
    print_menu_default(menu_name, options, menu_option_count, selected_index, vec.dy, anchor.dx);

    // Draw tail message if provided
    if (tail_msg != NULL) {
        print_text_default(vec.dy + menu_option_count + 2, 1, tail_msg);
    }

    // Render the frame
    render_io_frame();
}

/**
 * @brief Draws the combat log
 * 
 * @param anchor The anchor point of the combat log, representing the top-left corner
 * @param combat_log_message The message to be displayed in the combat log
 */
void draw_combat_log(vector2d_t anchor, const char* combat_log_message) {
    if (combat_log_message == NULL) {
        log_msg(ERROR, "Combat Output", "Given combat log message is NULL");
        return;
    }

    print_text_default(anchor.dy, anchor.dx, combat_log_message);
    anchor.dy++;
    print_text_default(anchor.dy, anchor.dx, "Press any key to continue...");
    anchor.dy++;
    render_io_frame();

    // Use our input handler to get any key press
    input_event_t input_event;
    get_input_blocking(&input_event);

    log_msg(DEBUG, "Combat Output", "Key pressed to continue: id=%d", (int) input_event.raw_input.id);
}

/**
 * @brief Draws the game over screen
 */
void draw_game_over(void) {
    clear_screen();

    print_text(1, 1, "Game over", RED_TEXT_COLORS);
    print_text_default(2, 1, "Press any key to exit...");
    render_io_frame();

    // Use our input handler to get any key press
    input_event_t input_event;
    get_input_blocking(&input_event);

    log_msg(DEBUG, "Combat Output", "Key pressed to exit game over: id=%d", (int) input_event.raw_input.id);
}

/**
 * @brief Draws the resource bar for a character
 * 
 * @param anchor The anchor point for the resource bar, representing the top-left corner
 * @param c A pointer to the character whose resources are to be displayed
 * @return The updated y-coordinate after drawing the resource bar
 */
int draw_resource_bar(vector2d_t anchor, const character_t* c) {
    char c_info[MAX_STRING_LENGTH];
    snprintf(c_info, sizeof(c_info), "%-20s | HP: %4d/%-4d | Mana: %4d/%-4d | Stamina: %4d/%-4d",
             c->name,
             c->current_resources.health, c->max_resources.health,
             c->current_resources.mana, c->max_resources.mana,
             c->current_resources.stamina, c->max_resources.stamina);

    print_text_default(anchor.dy, anchor.dx, c_info);
    anchor.dy++;
    return anchor.dy;
}