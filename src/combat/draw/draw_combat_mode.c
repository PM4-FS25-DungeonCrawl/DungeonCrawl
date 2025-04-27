#include "draw_combat_mode.h"

#include "../../character/character.h"
#include "../../game.h"

#include <notcurses/notcurses.h>

// External reference to notcurses context


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
    //copy of the anchor
    vector2d_t vec = {anchor.dx, anchor.dy};


    // Clear the screen before drawing
    ncplane_erase(stdplane);

    // Draw title
    ncplane_printf_yx(stdplane, vec.dy, anchor.dx + 20, "Combat Mode");
    vec.dy += 2;

    vec.dy = draw_resource_bar(vec, player);
    vec.dy = draw_resource_bar(vec, enemy);
    vec.dy += 2;

    //print the enemy sprite line for line
    if (red_enemy_sprite) {
        ncplane_set_channels(stdplane, NCCHANNELS_INITIALIZER(255, 0, 0, 0, 0, 0));// RED on BLACK
        ncplane_putstr_yx(stdplane, vec.dy, anchor.dx, enemy_sprite);
    } else {
        ncplane_set_channels(stdplane, NCCHANNELS_INITIALIZER(255, 255, 255, 0, 0, 0));// WHITE on BLACK
        ncplane_putstr_yx(stdplane, vec.dy, anchor.dx, enemy_sprite);
    }

    vec.dy += sprite_height;
    vec.dy += 1;
    notcurses_render(nc);
    return vec;
}

/**
 * @brief this function draws the combat menu
 * @param anchor the anchor point of the combat menu, representing the top left corner
 * @param menu_name the name of the menu
 * @param menu_options the options of the menu
 * @param menu_option_count the number of options in the menu
 * @param selected_index the index of the selected option
 * @param tail_msg the message to be displayed at the bottom of the menu. If the message is NULL, it will not be displayed.
 * @note this function only presents the drawn menu (without clearing the screen)
 */
void draw_combat_menu(const vector2d_t anchor, const char* menu_name, string_max_t* menu_options, const int menu_option_count, const int selected_index, const char* tail_msg) {
    if (menu_name == NULL || menu_options == NULL) {
        log_msg(ERROR, "Draw Combat Menu", "Menu options are NULL");
        return;
    }
    vector2d_t vec = {anchor.dx, anchor.dy};

    // White on black
    uint64_t white_on_black = NCCHANNELS_INITIALIZER(255, 255, 255, 0, 0, 0);

    ncplane_set_channels(stdplane, white_on_black);
    ncplane_putstr_yx(stdplane, vec.dy, 1, menu_name);
    vec.dy++;

    for (int i = 0; i < menu_option_count; i++) {
        char buffer[MAX_STRING_LENGTH];

        if (i == selected_index) {
            snprintf(buffer, sizeof(buffer), "> %-253s", menu_options[i]);
            // Bold is handled through styling channels in notcurses
            ncplane_set_styles(stdplane, NCSTYLE_BOLD);
        } else {
            snprintf(buffer, sizeof(buffer), "  %-253s", menu_options[i]);
            ncplane_set_styles(stdplane, NCSTYLE_NONE);
        }

        ncplane_putstr_yx(stdplane, vec.dy, anchor.dx, buffer);
        vec.dy++;
    }

        if (tail_msg != NULL) {
    ncplane_set_styles(stdplane, NCSTYLE_NONE);
    ncplane_printf_yx(stdplane, vec.dy + 2, 1, "%s", tail_msg);
    }
    notcurses_render(nc);
}

// Draw a combat log, showing the current action
void draw_combat_log(vector2d_t anchor, const char* combat_log_message) {
    if (combat_log_message == NULL) {
        log_msg(ERROR, "Draw Combat Mode", "given combat log messsage is NULL");
        return;
    }

    char message[MAX_STRING_LENGTH];
    snprintf(message, sizeof(message), "%s", combat_log_message);

    // White on black
    uint64_t white_on_black = NCCHANNELS_INITIALIZER(255, 255, 255, 0, 0, 0);
    ncplane_set_channels(stdplane, white_on_black);

    ncplane_putstr_yx(stdplane, anchor.dy, anchor.dx, message);
    anchor.dy++;
    ncplane_putstr_yx(stdplane, anchor.dy, anchor.dx, "Press any key to continue...");
    anchor.dy++;
    notcurses_render(nc);

    // Waiting for input
    ncinput input;
    notcurses_get_blocking(nc, &input);
}

// Helper function to draw resource bar
int draw_resource_bar(vector2d_t anchor, const character_t* c) {
    const char* str_format = "%-10s | Health %-4d | Mana %-4d | Stamina %-4d";

    char c_info[MAX_STRING_LENGTH];
    snprintf(c_info, sizeof(c_info), str_format, c->name, c->current_resources.health, c->current_resources.mana, c->current_resources.stamina);

    // White on black
    uint64_t white_on_black = NCCHANNELS_INITIALIZER(255, 255, 255, 0, 0, 0);
    ncplane_set_channels(stdplane, white_on_black);

    ncplane_putstr_yx(stdplane, anchor.dy, anchor.dx, c_info);
    anchor.dy++;
    return anchor.dy;
}

void draw_game_over() {
    tb_clear();
    tb_print(1, 1, TB_RED, TB_DEFAULT, "Game Over");
    tb_print(1, 2, TB_WHITE, TB_DEFAULT, "Press any key to exit...");
    tb_present();

    // Waiting for input
    struct tb_event event;
    tb_poll_event(&event);
}
