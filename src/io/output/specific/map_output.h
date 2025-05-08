#ifndef MAP_OUTPUT_H
#define MAP_OUTPUT_H

#include "../../input/input_types.h"
#include <stdbool.h>

// Forward declarations
typedef struct character_s character_t;
typedef struct vector2d_s vector2d_t;
typedef enum map_tile_e map_tile_t;

/**
 * @brief Initialize the map output handler
 * 
 * Sets up the map rendering system.
 * 
 * @return true on success, false on failure
 */
bool init_map_output(void);

/**
 * @brief Render the game map
 * 
 * Renders the map with player, monsters, items, etc.
 * 
 * @param map The map tiles to render
 * @param revealed_map The revealed tiles (for fog of war)
 * @param height The map height
 * @param width The map width
 * @param map_anchor The anchor point for map rendering (top-left corner)
 * @param player_pos The player's position
 * @param player The player character data
 * @return true on success, false on failure
 */
bool render_map(const map_tile_t* map, const map_tile_t* revealed_map,
               int height, int width, vector2d_t map_anchor, 
               vector2d_t player_pos, const character_t* player);

/**
 * @brief Update map state based on input
 * 
 * Processes player input to update the map state.
 * 
 * @param event The input event to process
 * @param player The player character to update
 * @param map The map tiles
 * @param revealed_map The revealed tiles (for fog of war)
 * @param height The map height
 * @param width The map width
 * @param player_pos The player's position (will be updated)
 * @return Result code indicating what happened (continue, combat, etc.)
 */
int update_map_state(const input_event_t* event, character_t* player,
                   map_tile_t* map, map_tile_t* revealed_map,
                   int height, int width, vector2d_t* player_pos);

/**
 * @brief Calculate visible tiles based on player position and light radius
 * 
 * Updates the revealed_map based on the player's position and light radius.
 * 
 * @param map The complete map
 * @param revealed_map The map of revealed tiles to update
 * @param height The map height
 * @param width The map width
 * @param player_pos The player's position
 * @param light_radius The radius of the player's light
 */
void calculate_visible_tiles(const map_tile_t* map, map_tile_t* revealed_map,
                          int height, int width, vector2d_t player_pos, int light_radius);

/**
 * @brief Shutdown the map output handler
 * 
 * Cleans up resources.
 */
void shutdown_map_output(void);

#endif // MAP_OUTPUT_H