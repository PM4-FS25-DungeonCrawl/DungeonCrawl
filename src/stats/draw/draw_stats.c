#include "draw_stats.h"

#include "../../character/level.h"
#include "../../logging/logger.h"

extern struct notcurses* nc;
extern struct ncplane* stdplane;

void render_stats_window(character_t* player) {
    if (player == NULL) {
        log_msg(ERROR, "Render Stats Window", "Player is NULL");
        return;
    }

    ncplane_erase(stdplane);
    int y = 0;
    int x = 0;

    // Display player stats
    ncplane_set_channels(stdplane, NCCHANNELS_INITIALIZER(0xff, 0xff, 0xff, 0, 0, 0));
    ncplane_putstr_yx(stdplane, y++, x, "Player Stats:");

    char stats_info[MAX_STRING_LENGTH];
    snprintf(stats_info, sizeof(stats_info), "Health: %4d / %-4d| Mana: %4d / %-4d | Stamina: %4d / %-4d",
             player->current_resources.health, player->max_resources.health,
             player->current_resources.mana, player->max_resources.mana,
             player->current_resources.stamina, player->max_resources.stamina);
    ncplane_putstr_yx(stdplane, y++, x, stats_info);

    snprintf(stats_info, sizeof(stats_info), "Strength: %-4d | Intelligence: %-4d | Dexterity: %-4d | Constitution: %-4d",
             player->base_stats.strength, player->base_stats.intelligence,
             player->base_stats.dexterity, player->base_stats.constitution);
    ncplane_putstr_yx(stdplane, y++, x, stats_info);

    snprintf(stats_info, sizeof(stats_info), "Current level: %-4d | XP: %4d / %-4d",
             player->level, player->xp, calculate_xp_for_next_level(player->level));
    ncplane_putstr_yx(stdplane, y++, x, stats_info);

    // Display equipped armor
    ncplane_putstr_yx(stdplane, y++, x, "Equipped Armor:");
    for (int i = 0; i < MAX_SLOT; i++) {
        if (player->equipment[i] != NULL) {
            snprintf(stats_info, sizeof(stats_info), "Armor: %s | Defense: Armor %-4d, Magic Resist %-4d",
                     player->equipment[i]->name, player->equipment[i]->defenses.armor,
                     player->equipment[i]->defenses.magic_resist);
            ncplane_putstr_yx(stdplane, y++, x, stats_info);
        } else {
            snprintf(stats_info, sizeof(stats_info), "No armor equipped in slot %d", i);
            ncplane_putstr_yx(stdplane, y++, x, stats_info);
        }
    }

    y += 2;// Add space

    snprintf(stats_info, sizeof(stats_info), "Available skillpoints: %d", player->skill_points);
    ncplane_putstr_yx(stdplane, y++, x, stats_info);
}

void draw_stats_menu(const char* title, const string_max_t options[], int option_count, int selected_index, const char* footer) {
    // Calculate position to place the menu below the stats window
    int y = 20;
    int x = 2;

    // Draw menu title
    ncplane_set_channels(stdplane, NCCHANNELS_INITIALIZER(0xff, 0xff, 0xff, 0, 0, 0));
    ncplane_putstr_yx(stdplane, y++, x, title);
    y++;// Add a space after the title

    // Draw options
    for (int i = 0; i < option_count; i++) {
        if (i == selected_index) {
            // Highlight selected option (white background, black text)
            ncplane_set_channels(stdplane, NCCHANNELS_INITIALIZER(0, 0, 0, 0xff, 0xff, 0xff));
        } else {
            // Normal option (white text, black background)
            ncplane_set_channels(stdplane, NCCHANNELS_INITIALIZER(0xff, 0xff, 0xff, 0, 0, 0));
        }
        ncplane_putstr_yx(stdplane, y++, x, options[i].characters);
    }

    // Draw footer at the bottom
    y++;
    ncplane_set_channels(stdplane, NCCHANNELS_INITIALIZER(0xff, 0xff, 0xff, 0, 0, 0));
    ncplane_putstr_yx(stdplane, y, x, footer);

    notcurses_render(nc);
}

void draw_stats_log(const char* message) {
    ncplane_set_channels(stdplane, NCCHANNELS_INITIALIZER(0xff, 0xff, 0xff, 0, 0, 0));
    ncplane_putstr_yx(stdplane, 1, 2, message);
    notcurses_render(nc);
}