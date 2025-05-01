#include "draw_stats.h"

#include "../../character/level.h"
#include "../../include/termbox2.h"

void render_stats_window(character_t* player) {
    if (player == NULL) {
        log_msg(ERROR, "Render Stats Window", "Player is NULL");
        return;
    }

    tb_clear();
    vector2d_t anchor = {0, 0};

    // Display player stats
    tb_print(anchor.dx, anchor.dy++, TB_WHITE, TB_DEFAULT, "Player Stats:");
    char stats_info[MAX_STRING_LENGTH];
    snprintf(stats_info, sizeof(stats_info), "Health: %4d / %-4d| Mana: %4d / %-4d | Stamina: %4d / %-4d",
             player->current_resources.health, player->max_resources.health, player->current_resources.mana, player->max_resources.mana, player->current_resources.stamina, player->max_resources.stamina);
    tb_print(anchor.dx, anchor.dy++, TB_WHITE, TB_DEFAULT, stats_info);

    snprintf(stats_info, sizeof(stats_info), "Strength: %-4d | Intelligence: %-4d | Dexterity: %-4d | Constitution: %-4d",
             player->base_stats.strength, player->base_stats.intelligence, player->base_stats.dexterity, player->base_stats.constitution);
    tb_print(anchor.dx, anchor.dy++, TB_WHITE, TB_DEFAULT, stats_info);

    snprintf(stats_info, sizeof(stats_info), "Current level: %-4d | XP: %4d / %-4d",
             player->level, player->xp, calculate_xp_for_next_level(player->level));
    tb_print(anchor.dx, anchor.dy++, TB_WHITE, TB_DEFAULT, stats_info);


    // Display equipped armor
    tb_print(anchor.dx, anchor.dy++, TB_WHITE, TB_DEFAULT, "Equipped Armor:");
    for (int i = 0; i < MAX_SLOT; i++) {
        if (player->equipment[i] != NULL) {
            snprintf(stats_info, sizeof(stats_info), "Armor: %s | Defense: Armor %-4d, Magic Resist %-4d",
                     player->equipment[i]->name, player->equipment[i]->defenses.armor, player->equipment[i]->defenses.magic_resist);
            tb_print(anchor.dx, anchor.dy++, TB_WHITE, TB_DEFAULT, stats_info);
        } else {
            snprintf(stats_info, sizeof(stats_info), "No %s equipped",
                     player->equipment[i]->name);
            tb_print(anchor.dx, anchor.dy++, TB_WHITE, TB_DEFAULT, "No significant defenses equipped.");
        }
    }

    anchor.dy++;
    anchor.dy++;

    snprintf(stats_info, sizeof(stats_info), "Avaliable skillpoints: %d", player->skill_points);
    tb_print(anchor.dx, anchor.dy++, TB_WHITE, TB_DEFAULT, stats_info);
}

// Replace the tb_width_height function call with the correct termbox2 functions
void draw_stats_menu(const char* title, const string_max_t options[], int option_count, int selected_index, const char* footer) {
    // Get the height of the terminal
    int width = tb_width();
    int height = tb_height();

    // Calculate position to place the menu below the stats window
    // Starting at line 15 should be below most stat displays
    int y = 20;

    // Draw menu title
    tb_print(2, y++, TB_WHITE, TB_DEFAULT, title);
    y++;// Add a space after the title

    // Draw options
    for (int i = 0; i < option_count; i++) {
        if (i == selected_index) {
            tb_print(2, y++, TB_BLACK, TB_WHITE, options[i].characters);// Highlight selected option
        } else {
            tb_print(2, y++, TB_WHITE, TB_DEFAULT, options[i].characters);
        }
    }

    // Draw footer at the bottom
    y++;
    tb_print(2, y, TB_WHITE, TB_DEFAULT, footer);

    tb_present();
}

void draw_stats_log(const char* message) {
    tb_print(2, 1, TB_WHITE, TB_DEFAULT, message);
    tb_present();
}