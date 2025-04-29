#ifndef INVENTORY_MODE_H
#define INVENTORY_MODE_H

#include "../character/character_fw.h"

/**
 * @brief Internal states for the inventory mode.
 */
typedef enum {
    INVENTORY_MENU,
    INVENTORY_GEAR_MENU,
    INVENTORY_EQUIPMENT_MENU,
    INVENTORY_EXIT
} internal_inventory_state_t;

/**
 * @brief Initialize the inventory mode
 * @return int 0 on success, 1 on failure
 * @note This function must be called before using any other functions in this module.
 */
int init_inventory_mode();

/**
 * @brief Starts the inventory mode.
 * @param player The player character whose inventory will be managed.
 * @note This function handles the main loop for the inventory mode.
 */
void start_inventory_mode(character_t* player);


/**
 * @brief Displays the main inventory menu.
 * @param player The player character whose inventory is being managed.
 * @return internal_inventory_state_t The next state of the inventory mode.
 */
internal_inventory_state_t inventory_menu(character_t* player);

/**
 * @brief Displays the gear inventory menu.
 * @param player The player character whose inventory is being managed.
 * @return internal_inventory_state_t The next state of the inventory mode.
 */
internal_inventory_state_t inventory_gear_menu(character_t* player);

/**
 * @brief Displays the equipment menu.
 * @param player The player character whose inventory is being managed.
 * @return internal_inventory_state_t The next state of the inventory mode.
 */
internal_inventory_state_t inventory_equipment_menu(character_t* player);

/**
 * @brief Shuts down the inventory mode and frees allocated resources.
 * @note This function must be called to clean up resources used by the inventory mode.
 */
void shutdown_inventory_mode(void);

#endif // INVENTORY_MODE_H
