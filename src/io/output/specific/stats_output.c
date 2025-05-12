#include "stats_output.h"

#include "../../../character/character.h"
#include "../../../character/level.h"
#include "../../../local/local.h"
#include "../../../local/local_strings.h"
#include "../../../logging/logger.h"
#include "../../../memory/memory_management.h"
#include "../../io_handler.h"
#include "../common/common_output.h"

// Store menu options for stats menu
static string_max_t* stats_menu_options;

/**
 * @brief Initialize the stats mode
 *
 * Allocates memory for stats menu options and registers for localization updates.
 *
 * @return 0 on success, -1 on failure
 */
int init_stats_mode(void) {
    // Allocate memory for menu options
    stats_menu_options = memory_pool_alloc(main_memory_pool, sizeof(string_max_t) * MAX_ABILITY_LIMIT);
    NULL_PTR_HANDLER_RETURN(stats_menu_options, -1, "Stats Mode", "Allocated memory for stats window options in memory pool is NULL");

    // Initialize localized strings
    update_stats_local();

    // Register for locale changes
    add_local_observer(update_stats_local);

    return 0;
}

/**
 * @brief Render the stats window
 *
 * @param player Pointer to the player character
 */
void render_stats_window(character_t* player) {
    // Clear the screen
    //clear_screen();
    ncplane_erase(stdplane);


    int y = 0;
    int x = 0;
    char stats_info[MAX_STRING_LENGTH];

    // Display stats title
    print_text_default(y++, x, local_strings[stmo_main_menu_title.idx].characters);

    // Display player resources
    snprintf(stats_info, sizeof(stats_info), "%s: %4d / %-4d| %s: %4d / %-4d | %s: %4d / %-4d",
             local_strings[stmo_ability_hp.idx].characters,
             player->current_resources.health, player->max_resources.health,
             local_strings[stmo_ability_mp.idx].characters,
             player->current_resources.mana, player->max_resources.mana,
             local_strings[stmo_ability_sp.idx].characters,
             player->current_resources.stamina, player->max_resources.stamina);
    print_text_default(y++, x, stats_info);

    // Display player attributes
    snprintf(stats_info, sizeof(stats_info), "%s: %-4d | %s: %-4d | %s: %-4d | %s: %-4d",
             local_strings[stmo_ability_strength.idx].characters, player->base_stats.strength,
             local_strings[stmo_ability_intelligence.idx].characters, player->base_stats.intelligence,
             local_strings[stmo_ability_dexterity.idx].characters, player->base_stats.dexterity,
             local_strings[stmo_ability_constitution.idx].characters, player->base_stats.constitution);
    print_text_default(y++, x, stats_info);

    // Display player level and XP
    snprintf(stats_info, sizeof(stats_info), "%s: %-4d | %s: %4d / %-4d",
             local_strings[stmo_player_level.idx].characters, player->level,
             local_strings[stmo_player_xp.idx].characters, player->xp, calculate_xp_for_next_level(player->level));
    print_text_default(y++, x, stats_info);

    // Display equipment header
    snprintf(stats_info, sizeof(stats_info), "%s:", local_strings[stmo_option_inventory.idx].characters);
    print_text_default(y++, x, stats_info);

    // Display equipped items
    for (int i = 0; i < MAX_SLOT; i++) {
        if (player->equipment[i] != NULL) {
            snprintf(stats_info, sizeof(stats_info), "%s: %s | %s: %-4d, %s: %-4d",
                     local_strings[stmo_option_inventory.idx].characters, player->equipment[i]->name,
                     local_strings[stmo_player_armor.idx].characters, player->equipment[i]->defenses.armor,
                     local_strings[stmo_player_magic_resist.idx].characters, player->equipment[i]->defenses.magic_resist);
        } else {
            snprintf(stats_info, sizeof(stats_info), "%s %d", local_strings[stmo_option_no_armor.idx].characters, i);
        }
        print_text_default(y++, x, stats_info);
    }

    y += 2;// Add space

    // Display skill points
    snprintf(stats_info, sizeof(stats_info), "%s: %d", local_strings[stmo_option_skillpoints.idx].characters, player->skill_points);
    print_text_default(y++, x, stats_info);
}

/**
 * @brief Draw the stats menu
 *
 * @param title Menu title
 * @param options Array of option strings
 * @param option_count Number of options
 * @param selected_index Currently selected option index
 * @param footer Text to display at the bottom of the menu
 */
void draw_stats_menu(const char* title, const char* options[], int option_count, int selected_index, const char* footer) {
    // Set menu position
    int y = 20;
    int x = 2;

    // Draw menu options using centralized print_menu
    print_menu(title, options, option_count, selected_index, y, x,
               DEFAULT_COLORS, DEFAULT_COLORS, INVERTED_COLORS);

    // Draw footer at the bottom
    y += option_count + 1;
    print_text_default(y, x, footer);
}

/**
 * @brief Display a message in the stats log
 *
 * @param message The message to display
 */
void draw_stats_log(const char* message) {
    // Display message
    print_text_default(1, 2, message);

    // Render the frame
    render_io_frame();
}

/**
 * @brief Update localized strings for the stats mode
 */
void update_stats_local(void) {
    // Main menu title
    snprintf(local_strings[stmo_main_menu_title.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(stmo_main_menu_title.key));

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

/**
 * @brief Shutdown the stats mode
 *
 * Frees allocated resources.
 */
void shutdown_stats_mode(void) {
    // Free menu options
    memory_pool_free(main_memory_pool, stats_menu_options);
}