#include "draw_stats.h"

#include "../../character/level.h"
#include "../../local/local_handler.h"
#include "../local/stats_mode_local.h"

extern struct notcurses* nc;
extern struct ncplane* stdplane;

string_max_t* stats_menu_options;// holds the ability menu options

int init_stats_mode() {
    stats_mode_strings = (char**) malloc(sizeof(char*) * MAX_STATS_STRINGS);
    RETURN_WHEN_NULL(stats_mode_strings, 1, "Stats Mode", "Failed to allocate memory for stats mode strings.")

    for (int i = 0; i < MAX_STATS_STRINGS; i++) {
        stats_mode_strings[i] = NULL;
    }

    //update local once, so the strings are initialized
    update_stats_local();
    //add update local function to the observer list
    observe_local(update_stats_local);
    return 0;
}


void render_stats_window(character_t* player) {
    ncplane_erase(stdplane);
    int y = 0;
    int x = 0;

    char stats_info[MAX_STRING_LENGTH];
    // Display player stats
    ncplane_set_channels(stdplane, NCCHANNELS_INITIALIZER(0xff, 0xff, 0xff, 0, 0, 0));
    snprintf(stats_info, MAX_STRING_LENGTH, "%s", stats_mode_strings[PLAYER_MENU_TITLE]);
    ncplane_putstr_yx(stdplane, y++, x, stats_info);


    snprintf(stats_info, sizeof(stats_info), "%s: %4d / %-4d| %s: %4d / %-4d | %s: %4d / %-4d",
             stats_mode_strings[HEALTH_STR], player->current_resources.health, player->max_resources.health,
             stats_mode_strings[MANA_STR], player->current_resources.mana, player->max_resources.mana,
             stats_mode_strings[STAMINA_STR], player->current_resources.stamina, player->max_resources.stamina);
    ncplane_putstr_yx(stdplane, y++, x, stats_info);

    snprintf(stats_info, sizeof(stats_info), "%s: %-4d | %s: %-4d | %s: %-4d | %s: %-4d",
             stats_mode_strings[STRENGTH_STR], player->base_stats.strength,
             stats_mode_strings[INTELLIGENCE_STR], player->base_stats.intelligence,
             stats_mode_strings[DEXTERITY_STR], player->base_stats.dexterity,
             stats_mode_strings[CONSTITUTION_STR], player->base_stats.constitution);
    ncplane_putstr_yx(stdplane, y++, x, stats_info);

    snprintf(stats_info, sizeof(stats_info), "%s: %-4d | %s: %4d / %-4d",
             stats_mode_strings[LEVEL_STR], player->level,
             stats_mode_strings[EXP_STR], player->xp, calculate_xp_for_next_level(player->level));
    ncplane_putstr_yx(stdplane, y++, x, stats_info);

    // Display equipped armor
    snprintf(stats_info, sizeof(stats_info), "%s:", stats_mode_strings[INVENTORY_MENU_TITLE]);
    ncplane_putstr_yx(stdplane, y++, x, stats_info);
    for (int i = 0; i < MAX_SLOT; i++) {
        if (player->equipment[i] != NULL) {
            snprintf(stats_info, sizeof(stats_info), "%s: %s | %s: %-4d, %s: %-4d",
                     stats_mode_strings[EQUIPPED_ARMOR_STR], player->equipment[i]->name,
                     stats_mode_strings[ARMOR_STR], player->equipment[i]->defenses.armor,
                     stats_mode_strings[MAGIC_RESISTANCE_STR], player->equipment[i]->defenses.magic_resist);
            ncplane_putstr_yx(stdplane, y++, x, stats_info);
        } else {
            snprintf(stats_info, sizeof(stats_info), "%s %d", stats_mode_strings[EMPTY_ARMOR_SLOT_STR], i);
            ncplane_putstr_yx(stdplane, y++, x, stats_info);
        }
    }

    y += 2;// Add space

    snprintf(stats_info, sizeof(stats_info), "%s: %d", stats_mode_strings[AVAILABLE_SKILL_POINTS_STR], player->skill_points);
    ncplane_putstr_yx(stdplane, y, x, stats_info);
}

void draw_stats_menu(const char* title, const char* options[], int option_count, int selected_index, const char* footer) {
    // Calculate position to place the menu below the stats window
    int y = 20;
    int x = 2;

    // Draw menu title
    //ncplane_set_channels(stdplane, NCCHANNELS_INITIALIZER(0xff, 0xff, 0xff, 0, 0, 0));
    ncplane_putstr_yx(stdplane, y, x, title);
    y++;// Add a space after the title

    // Draw options
    x++;
    for (int i = 0; i < option_count; i++) {
        if (i == selected_index) {
            // Highlight selected option (white background, black text)
            ncplane_set_channels(stdplane, NCCHANNELS_INITIALIZER(0, 0, 0, 0xff, 0xff, 0xff));
        } else {
            // Normal option (white text, black background)
            ncplane_set_channels(stdplane, NCCHANNELS_INITIALIZER(0xff, 0xff, 0xff, 0, 0, 0));
        }
        ncplane_putstr_yx(stdplane, y++, x, options[i]);
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

void shutdown_stats_mode() {
    if (stats_mode_strings != NULL) {
        for (int i = 0; i < MAX_STATS_STRINGS; i++) {
            if (stats_mode_strings[i] != NULL) {
                free(stats_mode_strings[i]);
            }
        }
        free(stats_mode_strings);
    }

    memory_pool_free(main_memory_pool, stats_menu_options);
}
