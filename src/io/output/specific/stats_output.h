/**
 * @file stats_output.h
 * @brief Exposes functions for outputing to the screen in stats mode.
 */
#ifndef STATS_OUTPUT_H
#define STATS_OUTPUT_H

#include "../../../character/character.h"

/**
 * @brief Initialize the stats mode
 *
 * Allocates necessary resources and initializes the stats mode.
 *
 * @return 0 on success, negative value on failure
 */
int init_stats_mode(void);

/**
 * @brief Render the stats window
 *
 * Displays the player's statistics including health, mana, attributes,
 * level, experience, and equipment.
 *
 * @param player Pointer to the player character
 */
void render_stats_window(const character_t* player);

/**
 * @brief Draw the stats menu
 *
 * Renders a menu for the stats window with options and a selected item.
 *
 * @param title Menu title
 * @param options Array of option strings
 * @param option_count Number of options
 * @param selected_index Currently selected option index
 * @param footer Text to display at the bottom of the menu
 */
void draw_stats_menu(const char* title, const char** options, int option_count, int selected_index, const char* footer);

/**
 * @brief Display a message in the stats log
 *
 * Shows a message in the stats window log area.
 *
 * @param message The message to display
 */
void draw_stats_log(const char* message);

/**
 * @brief Update localized strings for the stats mode
 *
 * Updates all localized strings used in the stats window.
 */
void update_stats_local(void);

/**
 * @brief Shutdown the stats mode
 *
 * Cleans up resources used by the stats mode.
 */
void shutdown_stats_mode(void);

#endif// STATS_OUTPUT_H
