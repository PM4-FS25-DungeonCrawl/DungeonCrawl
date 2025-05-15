#include "stats_output.h"

#include "../../../character/character.h"
#include "../../../character/level.h"
#include "../../../local/local_handler.h"
#include "../../../logging/logger.h"
#include "../../../memory/memory_management.h"
#include "../../../stats/local/stats_mode_local.h"
#include "../../io_handler.h"
#include "../common/common_output.h"

// Store menu options for stats menu
static string_max_t* stats_menu_options;


int init_stats_mode() {
    stats_mode_strings = (char**) malloc(sizeof(char*) * MAX_STATS_STRINGS);
    RETURN_WHEN_NULL(stats_mode_strings, 1, "Stats Mode", "Failed to allocate memory for stats mode strings.")

    for (int i = 0; i < MAX_STATS_STRINGS; i++) {
        stats_mode_strings[i] = NULL;
    }

    // Allocate memory for menu options
    stats_menu_options = memory_pool_alloc(test_memory_pool, sizeof(string_max_t) * MAX_ABILITY_LIMIT);
    RETURN_WHEN_NULL(stats_menu_options, -1, "Stats Mode",
                     "Allocated memory for stats window options in memory pool is NULL");

    // Initialize localized strings
    update_stats_local();

    // Register for locale changes
    observe_local(update_stats_local);
    return 0;
}

/**
 * @brief Render the stats window
 *
 * @param player Pointer to the player character
 */
void render_stats_window(const character_t* player) {
    // Clear the screen
    //clear_screen();
    ncplane_erase(stdplane);


    int y = 0;
    int x = 0;
    char stats_info[MAX_STRING_LENGTH];

    // Display stats title
    print_text_default(y++, x, stats_mode_strings[PLAYER_MENU_TITLE]);

    // Display player resources
    snprintf(stats_info, sizeof(stats_info), "%s: %4d / %-4d| %s: %4d / %-4d | %s: %4d / %-4d",
             stats_mode_strings[HEALTH_STR], player->current_resources.health, player->max_resources.health,
             stats_mode_strings[MANA_STR], player->current_resources.mana, player->max_resources.mana,
             stats_mode_strings[STAMINA_STR], player->current_resources.stamina, player->max_resources.stamina);
    print_text_default(y++, x, stats_info);

    // Display player attributes
    snprintf(stats_info, sizeof(stats_info), "%s: %-4d | %s: %-4d | %s: %-4d | %s: %-4d",
             stats_mode_strings[STRENGTH_STR], player->base_stats.strength,
             stats_mode_strings[INTELLIGENCE_STR], player->base_stats.intelligence,
             stats_mode_strings[DEXTERITY_STR], player->base_stats.dexterity,
             stats_mode_strings[CONSTITUTION_STR], player->base_stats.constitution);
    print_text_default(y++, x, stats_info);

    // Display player level and XP
    snprintf(stats_info, sizeof(stats_info), "%s: %-4d | %s: %4d / %-4d",
             stats_mode_strings[LEVEL_STR], player->level,
             stats_mode_strings[EXP_STR], player->xp, calculate_xp_for_next_level(player->level));
    print_text_default(y++, x, stats_info);

    // Display equipment header
    snprintf(stats_info, sizeof(stats_info), "%s:", stats_mode_strings[INVENTORY_MENU_TITLE]);
    print_text_default(y++, x, stats_info);

    // Display equipped items
    for (int i = 0; i < MAX_SLOT; i++) {
        if (player->equipment[i] != NULL) {
            snprintf(stats_info, sizeof(stats_info), "%s: %s | %s: %-4d, %s: %-4d",
                     stats_mode_strings[EQUIPPED_ARMOR_STR], player->equipment[i]->local_key,
                     stats_mode_strings[ARMOR_STR], player->equipment[i]->defenses.armor,
                     stats_mode_strings[MAGIC_RESISTANCE_STR], player->equipment[i]->defenses.magic_resist);
        } else {
            snprintf(stats_info, sizeof(stats_info), "%s %d", stats_mode_strings[EMPTY_ARMOR_SLOT_STR], i);
        }
        print_text_default(y++, x, stats_info);
    }

    y += 2;// Add space

    // Display skill points
    snprintf(stats_info, sizeof(stats_info), "%s: %d", stats_mode_strings[AVAILABLE_SKILL_POINTS_STR], player->skill_points);
    print_text_default(y, x, stats_info);
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
void draw_stats_menu(const char* title, const char** options, int option_count, int selected_index, const char* footer) {
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

void shutdown_stats_mode() {
    if (stats_mode_strings != NULL) {
        for (int i = 0; i < MAX_STATS_STRINGS; i++) {
            if (stats_mode_strings[i] != NULL) {
                free(stats_mode_strings[i]);
            }
        }
        free(stats_mode_strings);
    }
}
