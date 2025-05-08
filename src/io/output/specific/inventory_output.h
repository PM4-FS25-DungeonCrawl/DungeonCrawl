#ifndef INVENTORY_OUTPUT_H
#define INVENTORY_OUTPUT_H

#include <stdbool.h>

// Forward declarations
typedef struct character_s character_t;
typedef struct gear_s gear_t;
typedef struct potion_s potion_t;

/**
 * @brief Initialize the inventory output handler
 * 
 * @return true on success, false on failure
 */
bool init_inventory_output(void);

/**
 * @brief Render the inventory screen
 * 
 * @param player The player character whose inventory to display
 * @return true on success, false on failure
 */
bool render_inventory(const character_t* player);

/**
 * @brief Display detailed information about a gear item
 * 
 * @param item The gear item to display details for
 * @param x The X coordinate for rendering
 * @param y The Y coordinate for rendering
 * @return true on success, false on failure
 */
bool display_gear_details(const gear_t* item, int x, int y);

/**
 * @brief Display detailed information about a potion
 * 
 * @param potion The potion to display details for
 * @param x The X coordinate for rendering
 * @param y The Y coordinate for rendering
 * @return true on success, false on failure
 */
bool display_potion_details(const potion_t* potion, int x, int y);

/**
 * @brief Run the inventory screen with input handling
 * 
 * @param player The player character
 * @return true if an item was used, false otherwise
 */
bool run_inventory(character_t* player);

/**
 * @brief Shutdown the inventory output handler
 */
void shutdown_inventory_output(void);

#endif // INVENTORY_OUTPUT_H