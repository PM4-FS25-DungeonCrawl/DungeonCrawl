/**
 * @file draw_combat_mode.c
 * @brief This file contains functions for rendering the combat mode in the Dungeon Crawl game
 */
#include "draw_combat_mode.h"

#include "../../character/character.h"
#include "../../game.h"
#include "../../src/common.h"

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
    clear_screen(stdplane);

    //copy of the anchor
    vector2d_t vec = {anchor.dx, anchor.dy};

    // Draw title
    ncplane_printf_yx(stdplane, vec.dy, anchor.dx + 20, "Combat Mode");
    vec.dy += 2;

    vec.dy = draw_resource_bar(vec, player);
    vec.dy = draw_resource_bar(vec, enemy);
    vec.dy += 2;

    //print the enemy sprite line for line
    if (red_enemy_sprite) {
        ncplane_set_channels(stdplane, RED_TEXT_COLORS);
        ncplane_putstr_yx(stdplane, vec.dy, anchor.dx, enemy_sprite);
    } else {
        ncplane_set_channels(stdplane, DEFAULT_COLORS);
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

    ncplane_set_channels(stdplane, DEFAULT_COLORS);
    ncplane_set_styles(stdplane, NCSTYLE_NONE);
    ncplane_putstr_yx(stdplane, vec.dy, 1, menu_name);
    vec.dy++;

    for (int i = 0; i < menu_option_count; i++) {
        char buffer[MAX_STRING_LENGTH];

        if (i == selected_index) {
            snprintf(buffer, sizeof(buffer), "> %-253s", (char*) &menu_options[i]);
            // Bold is handled through styling channels in notcurses
            ncplane_set_styles(stdplane, NCSTYLE_BOLD);
        } else {
            snprintf(buffer, sizeof(buffer), "  %-253s", (char*) &menu_options[i]);
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

/**
 * @brief Draws the combat log
 * @param anchor The anchor point of the combat log, representing the top-left corner
 * @param combat_log_message The message to be displayed in the combat log
 * @note This function displays a combat log message and waits for user input before continuing
 */
void draw_combat_log(vector2d_t anchor, const char* combat_log_message) {
    if (combat_log_message == NULL) {
        log_msg(ERROR, "Draw Combat Mode", "given combat log messsage is NULL");
        return;
    }

    char message[MAX_STRING_LENGTH];
    snprintf(message, sizeof(message), "%s", combat_log_message);

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
 * @brief Draws the resource bar for a character
 * @param anchor The anchor point for the resource bar, representing the top-left corner
 * @param c A pointer to the character whose resources are to be displayed
 * @return The updated y-coordinate after drawing the resource bar
 * @note This function displays the character's name, health, mana, and stamina in a formatted bar
 */
int draw_resource_bar(vector2d_t anchor, const character_t* c) {
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

void draw_game_over() {
    clear_screen(stdplane);

    ncplane_set_channels(stdplane, RED_TEXT_COLORS);
    ncplane_printf_yx(stdplane, 1, 1, "Game over");
    ncplane_set_channels(stdplane, DEFAULT_COLORS);
    ncplane_printf_yx(stdplane, 2, 1, "Press any key to exit...");
    notcurses_render(nc);

    // Waiting for input
    ncinput input;
    notcurses_get_blocking(nc, &input);
}
