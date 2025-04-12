#include "draw_combat_mode.h"

// Internal functions
int draw_resource_bar(int y, character_t* c);
int draw_enemy(int y, bool red_target_sprite);
void clear_lines(int y);

// Draws the upper part of the screen, containing resource bar and enemy sprite
vector2d_t draw_combat_view(const character_t* player, const character_t* enemy, char* enemy_sprite, bool red_target_sprite) {
    tb_clear();
    vector2d_t anchor = {
        .dx = 1,
        .dy = 1
    };

    anchor.dy = draw_resource_bar(anchor.dy, player);
    anchor.dy = draw_resource_bar(anchor.dy, enemy);
    anchor.dy += 2;

    for (int i = 0; i < 20; i++) {
        tb_printf(1, anchor.dy, TB_WHITE, TB_DEFAULT, "");
    }

    anchor.dy = draw_enemy(anchor.dy, red_target_sprite);
    tb_present();
    return anchor;
}

// Draws the menu and the options available
void draw_combat_menu(vector2d_t anchor, char* menu_name, const char* menu_options[], int menu_option_count, int selected_index) {
    clear_lines(anchor.dy);

    tb_print(1, anchor.dy++, TB_WHITE, TB_DEFAULT, ("%s:", menu_name));
    for (int i = 0; i < menu_option_count; i++) {
        if (menu_options[i] != NULL) {
            if (i == selected_index) {
                tb_print(1, anchor.dy++, TB_WHITE, TB_WHITE, menu_options[i]);
            } else {
                tb_print(1, anchor.dy++, TB_WHITE, TB_DEFAULT, menu_options[i]);
            }
        }
    }
    tb_print(1, anchor.dy + 2, TB_WHITE, TB_DEFAULT, "[ESC] Return to menu");
    tb_present();
}

// Draw a combat log, showing the current action
void draw_combat_log(vector2d_t anchor, const char* combat_log_message) {
    clear_lines(anchor.dy);

    char message[256];
    snprintf(message, sizeof(message), combat_log_message);
    tb_print(anchor.dx, anchor.dy++, TB_WHITE, TB_DEFAULT, message);
    tb_print(anchor.dx, anchor.dy++, TB_WHITE, TB_DEFAULT, "Press any key to continue...");
    tb_present();

    // Waiting for input
    struct tb_event event;
    tb_poll_event(&event);
}

// Helper function to draw resource bar
int draw_resource_bar(int y, character_t* c) {
    char c_info[256];
    if (c->type == PLAYER) {
        snprintf(c_info, sizeof(c_info), "Player: %s | Health %d | Mana %d | Stamina %d", c->name, c->current_resources.health, c->current_resources.mana, c->current_resources.stamina);
    } else {
        snprintf(c_info, sizeof(c_info), "Enemy: %s | Health %d | Mana %d | Stamina %d", c->name, c->current_resources.health, c->current_resources.mana, c->current_resources.stamina);
    }
    tb_print(1, y++, TB_WHITE, TB_DEFAULT, c_info);
    tb_present();
    return y;
}

// Helper function to draw enemy sprite
int draw_enemy(int y, bool red_target_sprite) {
    char monster_sprite[256];
    snprintf(monster_sprite, sizeof(monster_sprite), "  (\\_/)\n  (o.o) \n  <( )>  \n");// TODO: make a list of sprites and connect to monster

    if (red_target_sprite) {
        tb_print(1, y++, TB_RED, TB_DEFAULT, monster_sprite);
    } else {
        tb_print(1, y++, TB_WHITE, TB_DEFAULT, monster_sprite);
    }

    // Add two empty lines after the monster sprite
    y += 2;
    tb_print(1, y++, TB_WHITE, TB_DEFAULT, "");
    tb_print(1, y++, TB_WHITE, TB_DEFAULT, "");
    tb_present();
    return y;
}

// Helper function to clear part of the screen
void clear_lines(int y) {
    int width = tb_width();
    int height = tb_height();
    for (y; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Set each cell to blank (clear it)
            tb_set_cell(x, y, ' ', TB_DEFAULT, TB_DEFAULT);
        }
    }
    tb_present();
}