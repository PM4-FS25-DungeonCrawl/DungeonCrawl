#include "draw_stats.h"
#include "../../character/level.h"
#include "../../logging/logger.h"

#include "../../local/local.h"
#include "../../local/local_strings.h"

extern struct notcurses* nc;
extern struct ncplane* stdplane;

string_max_t* stats_menu_options;// holds the ability menu options

void update_stats_local(void);

int init_stats_mode() {
    stats_menu_options = memory_pool_alloc(main_memory_pool, sizeof(string_max_t) * MAX_ABILITY_LIMIT);
    NULL_PTR_HANDLER_RETURN(stats_menu_options, -1, "Stats Mode", "Allocated memory for stats window options in memory pool is NULL");

    //update local once, so the strings are initialized
    update_stats_local();
    //add update local function to the observer list
    add_local_observer(update_stats_local);
    return 0;
}


void render_stats_window(character_t* player) {
    ncplane_erase(stdplane);
    int y = 0;
    int x = 0;

    char stats_info[MAX_STRING_LENGTH];
    // Display player stats
    ncplane_set_channels(stdplane, NCCHANNELS_INITIALIZER(0xff, 0xff, 0xff, 0, 0, 0));
    snprintf(stats_info, MAX_STRING_LENGTH, "%s:", local_strings[stmo_main_menu_title.idx].characters);
    ncplane_putstr_yx(stdplane, y++, x, stats_info);



    snprintf(stats_info, sizeof(stats_info), "%s: %4d / %-4d| %s: %4d / %-4d | %s: %4d / %-4d",
            local_strings[stmo_ability_hp.idx].characters,
             player->current_resources.health, player->max_resources.health,
             local_strings[stmo_ability_mp.idx].characters,
             player->current_resources.mana, player->max_resources.mana,
             local_strings[stmo_ability_sp.idx].characters,
             player->current_resources.stamina, player->max_resources.stamina);
    ncplane_putstr_yx(stdplane, y++, x, stats_info);

    snprintf(stats_info, sizeof(stats_info), "%s: %-4d | %s: %-4d | %s: %-4d | %s: %-4d",
            local_strings[stmo_ability_strength.idx].characters, player->base_stats.strength,
            local_strings[stmo_ability_intelligence.idx].characters, player->base_stats.intelligence,
            local_strings[stmo_ability_dexterity.idx].characters, player->base_stats.dexterity,
            local_strings[stmo_ability_constitution.idx].characters, player->base_stats.constitution);
    ncplane_putstr_yx(stdplane, y++, x, stats_info);

    snprintf(stats_info, sizeof(stats_info), "%s: %-4d | %s: %4d / %-4d",
    local_strings[stmo_player_level.idx].characters, player->level,
    local_strings[stmo_player_xp.idx].characters, player->xp, calculate_xp_for_next_level(player->level));
    ncplane_putstr_yx(stdplane, y++, x, stats_info);

    // Display equipped armor
    ncplane_putstr_yx(stdplane, y++, x, "Equipped Armor:");
    for (int i = 0; i < MAX_SLOT; i++) {
        if (player->equipment[i] != NULL) {
            snprintf(stats_info, sizeof(stats_info), "%s: %s | %s: %-4d, %s: %-4d",
                     local_strings[stmo_option_inventory.idx].characters, player->equipment[i]->name,
                     local_strings[stmo_player_armor.idx].characters, player->equipment[i]->defenses.armor,
                     local_strings[stmo_player_magic_resist.idx].characters,player->equipment[i]->defenses.magic_resist);
            ncplane_putstr_yx(stdplane, y++, x, stats_info);
        } else {
            snprintf(stats_info, sizeof(stats_info), "%s %d", local_strings[stmo_option_no_armor.idx].characters,i);
            ncplane_putstr_yx(stdplane, y++, x, stats_info);
        }
    }

    y += 2; // Add space

    snprintf(stats_info, sizeof(stats_info), "%s: %d",local_strings[stmo_option_skillpoints.idx].characters, player->skill_points);
    ncplane_putstr_yx(stdplane, y++, x, stats_info);
}

void draw_stats_menu(const char* title, const string_max_t options[], int option_count, int selected_index, const char* footer) {
    // Calculate position to place the menu below the stats window
    int y = 20;
    int x = 2;

    // Draw menu title
    ncplane_set_channels(stdplane, NCCHANNELS_INITIALIZER(0xff, 0xff, 0xff, 0, 0, 0));
    ncplane_putstr_yx(stdplane, y++, x, title);
    y++; // Add a space after the title

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

void update_stats_local(void) {
    // Main menu title
    snprintf(local_strings[stmo_main_menu_title.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(stmo_main_menu_title.key));

    // Ability menu title
    snprintf(local_strings[stmo_ability_menu_title.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(stmo_ability_menu_title.key));

    // Stats titles
    snprintf(local_strings[stmo_ability_hp.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(stmo_ability_hp.key));
    snprintf(local_strings[stmo_ability_mp.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(stmo_ability_mp.key));
    snprintf(local_strings[stmo_ability_sp.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(stmo_ability_sp.key));

    // Ability stats
    snprintf(local_strings[stmo_ability_strength.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(stmo_ability_strength.key));
    snprintf(local_strings[stmo_ability_intelligence.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(stmo_ability_intelligence.key));
    snprintf(local_strings[stmo_ability_dexterity.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(stmo_ability_dexterity.key));
    snprintf(local_strings[stmo_ability_constitution.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(stmo_ability_constitution.key));

    // Player stats
    snprintf(local_strings[stmo_player_level.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(stmo_player_level.key));
    snprintf(local_strings[stmo_player_xp.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(stmo_player_xp.key));
    snprintf(local_strings[stmo_player_armor.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(stmo_player_armor.key));
    snprintf(local_strings[stmo_player_magic_resist.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(stmo_player_magic_resist.key));

    // Options
    snprintf(local_strings[stmo_option_skillpoints.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(stmo_option_skillpoints.key));
    snprintf(local_strings[stmo_option_inventory.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(stmo_option_inventory.key));
    snprintf(local_strings[stmo_option_no_armor.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(stmo_option_no_armor.key));
}

void shutdown_stats_mode() {
    memory_pool_free(main_memory_pool, stats_menu_options);
}