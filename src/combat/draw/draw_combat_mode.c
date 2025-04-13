#include "draw_combat_mode.h"

#include "../../../include/termbox2.h"
#include "../../character/character.h"
#include "../../logging/logger.h"

// Internal functions
int draw_resource_bar(vector2d_t anchor, const character_t* c);

/**
 * @brief this function draws the combat view
 * @param anchor the anchor point of the combat view, representing the top left corner
 * @param player the player character
 * @param enemy the enemy character
 * @param enemy_sprite the sprite of the enemy
 * @param sprite_height the height of the enemy sprite
 * @param red_enemy_sprite if true, the enemy sprite will be drawn in red
 * @return the new anchor point after drawing the combat view
 * @note this function clears and presents the combat view
 */
vector2d_t draw_combat_view(const vector2d_t anchor, const character_t* player, const character_t* enemy, const char* enemy_sprite, const int sprite_height, const bool red_enemy_sprite) {
    tb_clear();
    //copy of the anchor
    vector2d_t vec = {anchor.dx, anchor.dy};

    vec.dy = draw_resource_bar(vec, player);
    vec.dy = draw_resource_bar(vec, enemy);
    vec.dy += 2;

    //print the enemy sprite line for line
    if (red_enemy_sprite) {
        tb_print(anchor.dx, vec.dy, TB_RED, TB_DEFAULT, enemy_sprite);
    } else {
        tb_print(anchor.dx, vec.dy, TB_WHITE, TB_DEFAULT, enemy_sprite);
    }

    vec.dy += sprite_height;
    vec.dy += 1;
    tb_present();
    return vec;
}

/**
 * @brief this function draws the combat menu
 * @param anchor the anchor point of the combat menu, representing the top left corner
 * @param menu_name the name of the menu
 * @param menu_options the options of the menu
 * @param menu_option_count the number of options in the menu
 * @param selected_index the index of the selected option
 * @note this function only presents the drawn menu (without clearing the screen)
 */
void draw_combat_menu(const vector2d_t anchor, const char* menu_name, const char* menu_options[], const int menu_option_count, const int selected_index) {
    if (menu_name == NULL || menu_options == NULL) {
        log_msg(ERROR, "Draw Combat Menu", "Menu options are NULL");
        return;
    }
    vector2d_t vec = {anchor.dx, anchor.dy};

    tb_print(1, vec.dy++, TB_WHITE, TB_DEFAULT, menu_name);
    for (int i = 0; i < menu_option_count; i++) {
        if (i == selected_index) {
            char buffer[MAX_STRING_LENGTH];
            snprintf(buffer, sizeof(buffer), "> %-254s", menu_options[i]);
            tb_print(anchor.dx, vec.dy++, TB_BOLD, TB_DEFAULT, buffer);
        } else {
            char buffer[MAX_STRING_LENGTH];
            snprintf(buffer, sizeof(buffer), "  %-254s", menu_options[i]);
            tb_print(anchor.dx, vec.dy++, TB_WHITE, TB_DEFAULT, buffer);
        }
    }
    tb_print(1, vec.dy + 2, TB_WHITE, TB_DEFAULT, "[ESC] Return to menu");
    tb_present();
}

// Draw a combat log, showing the current action
void draw_combat_log(vector2d_t anchor, const char* combat_log_message) {
    char message[MAX_STRING_LENGTH];
    snprintf(message, sizeof(message), combat_log_message);
    tb_print(anchor.dx, anchor.dy++, TB_WHITE, TB_DEFAULT, message);
    tb_print(anchor.dx, anchor.dy++, TB_WHITE, TB_DEFAULT, "Press any key to continue...");
    tb_present();

    // Waiting for input
    struct tb_event event;
    tb_poll_event(&event);
}

// Helper function to draw resource bar
int draw_resource_bar(vector2d_t anchor, const character_t* c) {
    const char* str_format = "%-10s | Health %-4d | Mana %-4d | Stamina %-4d";

    char c_info[MAX_STRING_LENGTH];
    snprintf(c_info, sizeof(c_info), str_format, c->name, c->current_resources.health, c->current_resources.mana, c->current_resources.stamina);

    tb_print(anchor.dx, anchor.dy++, TB_WHITE, TB_DEFAULT, c_info);
    return anchor.dy;
}