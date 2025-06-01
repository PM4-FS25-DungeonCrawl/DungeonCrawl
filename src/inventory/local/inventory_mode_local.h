/**
 * @file inventory_mode_local.h
 * @brief Exposes functions for localizing the inventory mode.
 */
#ifndef INVENTORY_MODE_LOCAL_H
#define INVENTORY_MODE_LOCAL_H

enum inventory_mode_index {
    MAIN_MENU_TITLE,
    SHOW_GEAR,
    SHOW_EQUIPPED_GEAR,
    SHOW_POTIONS,

    HEALTH_STR,
    MANA_STR,
    STAMINA_STR,

    INVENTORY_FULL_MSG,
    INVENTORY_EMPTY_MSG,
    INVENTORY_MENU_TITLE,
    INVENTORY_MENU_HEADER,
    INVENTORY_DROP_GEAR_MSG,
    INVENTORY_GEAR_FORMAT,
    INVENTORY_GEAR_FORMAT_EMPTY,

    EQUIPMENT_MENU_TITLE,
    EQUIPMENT_MENU_HEADER,
    EQUIPMENT_SLOT_FULL,
    EQUIPMENT_HANDS_SLOT_FULL,

    POTION_FORMAT,
    POTION_MENU_TITLE,
    POTION_MENU_HEADER,
    POTION_FULL_MSG,
    POTION_EMPTY_MSG,
    POTION_DROP_POTION_MSG,
    POTION_USE,

    LOOT_MAIN_MENU_TITLE,
    LOOT_OPTION_SHOW_GEAR,
    LOOT_OPTION_SHOW_GEAR_EQUIPPED,
    LOOT_OPTION_SHOW_POTIONS,
    LOOT_GEAR_MENU_HEADER,
    LOOT_EQUIPMENT_MENU_HEADER,
    LOOT_POTION_MENU_HEADER,
    FINISH_LOOTING_MSG,

    PRESS_C_RETURN,
    MAX_INVENTORY_STRINGS
};

extern char** inventory_mode_strings;

/**
 * @brief Updates the inventory mode strings with the actual local strings.
 * @note This function must be called after initializing the local handler.
 */
void update_inventory_local(void);

#endif//INVENTORY_MODE_LOCAL_H
