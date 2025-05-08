#ifndef STATS_OUTPUT_H
#define STATS_OUTPUT_H

#include <stdbool.h>

// Forward declarations
typedef struct character_s character_t;

/**
 * @brief Initialize the stats output handler
 * 
 * @return true on success, false on failure
 */
bool init_stats_output(void);

/**
 * @brief Render the character stats screen
 * 
 * @param player The player character to display stats for
 * @return true on success, false on failure
 */
bool render_stats_screen(const character_t* player);

/**
 * @brief Handle input for the stats screen
 * 
 * @param player The player character
 * @return true if the stats screen should remain open, false to exit
 */
bool handle_stats_input(character_t* player);

/**
 * @brief Run the stats screen with input handling
 * 
 * @param player The player character
 */
void run_stats_screen(character_t* player);

/**
 * @brief Shutdown the stats output handler
 */
void shutdown_stats_output(void);

#endif // STATS_OUTPUT_H