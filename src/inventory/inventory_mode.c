#include "inventory_mode.h"

#include "../../include/termbox2.h"
#include "../asciiart/ascii.h"
#include "../character/character.h"
#include "../common.h"
#include "../local/local.h"
#include "../local/local_strings.h"
#include "./draw/draw_inventory_mode.h"

#include <stdbool.h>

// === Internal Functions ===
void collect_inventory_gear_options(gear_t* gear_inventory[], const int count);
void collect_inventory_equipment_options(gear_t* equipment[]);

/**
 * @brief Updates the localized strings used in the inventory mode menus.
 *
 * This function populates the localized string buffers with values retrieved
 * from the localization system. These strings are used in various menus within
 * the inventory mode, such as the main menu, ability menu, and potion menu. It also
 * updates messages like the tail message and inventory-specific messages.
 *
 * @note This function is automatically called during initialization of inventory
 * mode to ensure that all strings are properly localized before use. It is
 * also registered as an observer to keep menu strings updated whenever the locale changes.
 */
void update_inventory_local(void);

// === Intern Global Variables ===
vector2d_t inventory_view_anchor = {1, 1};

string_max_t* inventory_gear_options;
string_max_t* inventory_equipment_options;

/**
 * @brief Initializes the inventory mode.
 */
int init_inventory_mode() {
    inventory_gear_options = memory_pool_alloc(main_memory_pool, sizeof(string_max_t) * MAX_GEAR_LIMIT);
    NULL_PTR_HANDLER_RETURN(inventory_gear_options, -1, "Inventory Mode", "Failed to allocate memory for inventory gear options");

    inventory_equipment_options = memory_pool_alloc(main_memory_pool, sizeof(string_max_t) * MAX_SLOT);
    NULL_PTR_HANDLER_RETURN(inventory_equipment_options, -1, "Inventory Mode", "Failed to allocate memory for inventory equipment options");

    //update local once, so the strings are initialized
    update_inventory_local();
    //add update local function to the observer list
    add_local_observer(update_inventory_local);
    return 0;
}

/**
 * @brief Starts the inventory mode.
 */
void start_inventory_mode(character_t* player) {
    internal_inventory_state_t inventory_state = INVENTORY_MENU;
    bool inventory_active = true;

    collect_inventory_gear_options(player->gear_inventory, player->gear_count);
    collect_inventory_equipment_options(player->equipment);

    while (inventory_active) {
        switch (inventory_state) {
            case INVENTORY_MENU:
                inventory_state = inventory_menu(player);
                break;
            case INVENTORY_GEAR_MENU:
                inventory_state = inventory_gear_menu(player);
                break;
            case INVENTORY_EQUIPMENT_MENU:
                inventory_state = inventory_equipment_menu(player);
                break;
            case INVENTORY_EXIT:
                inventory_active = false;
                break;
        }
    }
}

/**
 * @brief Displays the main inventory menu.
 */
internal_inventory_state_t inventory_menu(character_t* player) {
    const vector2d_t anchor = draw_inventory_view(inventory_view_anchor, player);
    int selected_index = 0;

    internal_inventory_state_t new_state = INVENTORY_MENU;
    bool submenu_selected = false;

    while (!submenu_selected) {
        draw_inventory_menu(anchor,
                            local_strings[como_gear_menu_title.idx].characters,
                            &local_strings[como_gear_menu_option1.idx],
                            MAX_COMO_GEAR_MENU_OPTION,
                            selected_index,
                            NULL);

        struct tb_event event;
        const int ret = tb_peek_event(&event, 10);

        if (ret == TB_OK) {
            if (event.key == TB_KEY_ARROW_UP) {
                selected_index = (selected_index - 1 + MAX_COMO_GEAR_MENU_OPTION) % MAX_COMO_GEAR_MENU_OPTION;
            } else if (event.key == TB_KEY_ARROW_DOWN) {
                selected_index = (selected_index + 1) % MAX_COMO_GEAR_MENU_OPTION;
            } else if (event.key == TB_KEY_ENTER) {
                if (selected_index == 0) {
                    new_state = INVENTORY_GEAR_MENU;
                } else if (selected_index == 1) {
                    new_state = INVENTORY_EQUIPMENT_MENU;
                }
                submenu_selected = true;
            } else if (event.key == TB_KEY_ESC) {
                new_state = INVENTORY_EXIT;
                submenu_selected = true;
            }
        }
    }
    return new_state;
}

/**
 * @brief Displays the gear inventory menu.
 */
internal_inventory_state_t inventory_gear_menu(character_t* player) {
    const vector2d_t anchor = draw_inventory_view(inventory_view_anchor, player);
    int selected_index = 0;

    if (player->gear_count == 0) {
        draw_inventory_log(anchor, local_strings[como_no_more_gear.idx].characters);
        return INVENTORY_MENU;
    }

    internal_inventory_state_t new_state = INVENTORY_MENU;
    bool item_selected_or_esc = false;

    while (!item_selected_or_esc) {
        draw_inventory_menu(anchor,
                            local_strings[como_inventory_menu_title.idx].characters,
                            inventory_gear_options,
                            player->gear_count,
                            selected_index,
                            local_strings[como_submenu_tail_message.idx].characters);

        struct tb_event event;
        const int ret = tb_peek_event(&event, 10);

        if (ret == TB_OK) {
            if (event.key == TB_KEY_ARROW_UP) {
                selected_index = (selected_index - 1 + player->gear_count) % player->gear_count;
            } else if (event.key == TB_KEY_ARROW_DOWN) {
                selected_index = (selected_index + 1) % player->gear_count;
            } else if (event.key == TB_KEY_ENTER) {
                equip_gear(player, player->gear_inventory[selected_index]);
                collect_inventory_gear_options(player->gear_inventory, player->gear_count);
            } else if (event.key == TB_KEY_ESC) {
                new_state = INVENTORY_MENU;
                item_selected_or_esc = true;
            }
        }
    }
    return new_state;
}

/**
 * @brief Displays the equipment menu.
 */
internal_inventory_state_t inventory_equipment_menu(character_t* player) {
    const vector2d_t anchor = draw_inventory_view(inventory_view_anchor, player);
    int selected_index = 0;

    internal_inventory_state_t new_state = INVENTORY_MENU;
    bool item_selected_or_esc = false;

    while (!item_selected_or_esc) {
        draw_inventory_menu(anchor,
                            local_strings[como_equipment_menu_title.idx].characters,
                            inventory_equipment_options,
                            MAX_SLOT,
                            selected_index,
                            local_strings[como_submenu_tail_message.idx].characters);

        struct tb_event event;
        const int ret = tb_peek_event(&event, 10);

        if (ret == TB_OK) {
            if (event.key == TB_KEY_ARROW_UP) {
                selected_index = (selected_index - 1 + MAX_SLOT) % MAX_SLOT;
            } else if (event.key == TB_KEY_ARROW_DOWN) {
                selected_index = (selected_index + 1) % MAX_SLOT;
            } else if (event.key == TB_KEY_ENTER) {
                if (player->equipment[selected_index] != NULL) {
                    unequip_gear(player, (gear_slot_t)selected_index);
                    collect_inventory_equipment_options(player->equipment);
                }
            } else if (event.key == TB_KEY_ESC) {
                new_state = INVENTORY_MENU;
                item_selected_or_esc = true;
            }
        }
    }
    return new_state;
}

/**
 * @brief Collects gear inventory options for display.
 */
void collect_inventory_gear_options(gear_t* gear_inventory[], const int count) {
    for (int i = 0; i < MAX_GEAR_LIMIT; i++) {
        memset(inventory_gear_options[i].characters, '\0', MAX_STRING_LENGTH);
    }

    for (int i = 0; i < count; i++) {
        snprintf(inventory_gear_options[i].characters, MAX_STRING_LENGTH,
                 local_strings[como_inventory_format.idx].characters,
                 gear_inventory[i]->name,
                 gear_slot_to_string(gear_inventory[i]->slot));
    }
}

/**
 * @brief Collects equipment options for display.
 */
void collect_inventory_equipment_options(gear_t* equipment[]) {
    for (int i = 0; i < MAX_SLOT; i++) {
        memset(inventory_equipment_options[i].characters, '\0', MAX_STRING_LENGTH);
    }

    for (int i = 0; i < MAX_SLOT; i++) {
        if (equipment[i] != NULL) {
            snprintf(inventory_equipment_options[i].characters, MAX_STRING_LENGTH,
                     local_strings[como_equipment_format.idx].characters,
                     equipment[i]->name,
                     gear_slot_to_string((gear_slot_t)i));
        } else {
            snprintf(inventory_equipment_options[i].characters, MAX_STRING_LENGTH,
                     local_strings[como_equipment_format_empty.idx].characters,
                     gear_slot_to_string((gear_slot_t)i));
        }
    }
}

void update_inventory_local(void) {
    //inventory menu
    snprintf(local_strings[como_gear_menu_title.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(como_gear_menu_title.key));
    snprintf(local_strings[como_gear_menu_option1.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(como_gear_menu_option1.key));
    snprintf(local_strings[como_gear_menu_option2.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(como_gear_menu_option2.key));

    //inventory gear menu
    snprintf(local_strings[como_inventory_menu_title.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(como_inventory_menu_title.key));
    snprintf(local_strings[como_inventory_format.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(como_inventory_format.key));

    //inventory equipment menu
    snprintf(local_strings[como_equipment_menu_title.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(como_equipment_menu_title.key));
    snprintf(local_strings[como_equipment_format.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(como_equipment_format.key));
    snprintf(local_strings[como_equipment_format_empty.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(como_equipment_format_empty.key));

    //tail message
    snprintf(local_strings[como_submenu_tail_message.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(como_submenu_tail_message.key));

    //inventory messages
    snprintf(local_strings[como_no_more_gear.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(como_no_more_gear.key));
}

/**
 * @brief Shuts down the inventory mode and frees allocated resources.
 */
void shutdown_inventory_mode(void) {
    memory_pool_free(main_memory_pool, inventory_gear_options);
    memory_pool_free(main_memory_pool, inventory_equipment_options);
}
