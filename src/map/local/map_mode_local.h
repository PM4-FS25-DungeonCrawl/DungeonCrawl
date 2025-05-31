#ifndef MAP_MODE_LOCAL_H
#define MAP_MODE_LOCAL_H

enum map_mode_index {
    PRESS_KEY_MENU,
    PRESS_KEY_STATS,
    PRESS_KEY_INVENTORY,
    PLAYER_POSITION_STR,
    MAX_MAP_MODE_STRINGS
};

/**
 * @var map_mode_strings
 * Global array of local strings used by the map mode.
 */
extern char** map_mode_strings;

/**
 * @brief Initializes the local map mode.
 *
 * This function sets up any resources or state required for the local map mode.
 *
 * @return int Returns 0 on success, or a non-zero error code on failure.
 */
int init_map_mode_local(void);

/**
 * @brief Shuts down the local map mode.
 *
 * This function releases any resources or state associated with the local map mode.
 */
void shutdown_map_mode_local(void);

#endif//MAP_MODE_LOCAL_H
