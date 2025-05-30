/**
 * @file combat_output.c
 * @brief Implements outputting to the screen for the combat mode.
 */
#include "combat_output.h"

#include "../../../character/character.h"
#include "../../../combat/local/combat_mode_local.h"
#include "../../../common.h"
#include "../../../logging/logger.h"
#include "../../input/input_handler.h"
#include "../../io_handler.h"
#include "../common/output_handler.h"
#include "../media/media_output.h"

vector2d_t draw_combat_view(const vector2d_t anchor, const character_t* player, const character_t* enemy,
                            const char* enemy_sprite, const int sprite_height, const bool red_enemy_sprite) {
    clear_screen();

    // Copy of the anchor
    vector2d_t vec = {anchor.dx, anchor.dy};

    // Draw title
    print_text_default(vec.dy, anchor.dx + 20, combat_mode_strings[COMBAT_MODE_TITLE]);
    vec.dy += 2;

    // Draw resource bars
    vec.dy = draw_resource_bar(vec, player);
    vec.dy = draw_resource_bar(vec, enemy);
    vec.dy += 2;

    // display goblin
    // display image stretched to specific size
    display_image_at(enemy_sprite, vec.dx + 10, vec.dy, sprite_height, 2 * sprite_height, SCALE_STRETCH);
    vec.dy += sprite_height + 2;

    // Render the frame
    render_frame();

    return vec;
}

void draw_combat_menu(const vector2d_t anchor, const char* menu_name, char** menu_options,
                      const int menu_option_count, const int selected_index, const char* tail_msg) {
    if (menu_name == NULL || menu_options == NULL) {
        log_msg(ERROR, "Combat Output", "Menu options are NULL");
        return;
    }
    const vector2d_t vec = {anchor.dx, anchor.dy};

    // Use centralized menu drawing function
    print_menu_default(menu_name, menu_options, menu_option_count, selected_index, vec.dy, anchor.dx);

    // Draw tail message if provided
    if (tail_msg != NULL) {
        print_text_default(anchor.dy + menu_option_count + 2, anchor.dx, tail_msg);
    }

    // Render the frame
    render_frame();
}

void draw_combat_log(vector2d_t anchor, const char* combat_log_message) {
    if (combat_log_message == NULL) {
        log_msg(ERROR, "Combat Output", "Given combat log message is NULL");
        return;
    }

    print_text_default(anchor.dy, anchor.dx, combat_log_message);
    anchor.dy++;
    print_text_default(anchor.dy, anchor.dx, combat_mode_strings[PRESS_ANY_CONTINUE]);
    anchor.dy++;
    render_frame();

    // Use our input handler to get any key press
    input_event_t input_event;
    get_input_blocking(&input_event);
}

void draw_game_over(void) {
    clear_screen();

    // Display game over message
    print_text(1, 1, combat_mode_strings[LOST_COMBAT_MSG], RED_TEXT_COLORS);
    print_text_default(2, 1, combat_mode_strings[PRESS_ANY_EXIT]);
    render_frame();

    // Use our input handler to get any key press
    input_event_t input_event;
    get_input_blocking(&input_event);
}

int draw_resource_bar(vector2d_t anchor, const character_t* c) {
    if (c == NULL) {
        log_msg(ERROR, "Combat Output", "Character is NULL");
        return anchor.dy;
    }

    char c_info[64];
    snprintf(c_info, sizeof(c_info), "%-20s | %s: %4d/%-4d | %s: %4d/%-4d | %s: %4d/%-4d",
             c->name,
             combat_mode_strings[HEALTH_STR], c->current_resources.health, c->max_resources.health,
             combat_mode_strings[MANA_STR], c->current_resources.mana, c->max_resources.mana,
             combat_mode_strings[STAMINA_STR], c->current_resources.stamina, c->max_resources.stamina);

    print_text_default(anchor.dy, anchor.dx, c_info);
    anchor.dy++;
    return anchor.dy;
}
