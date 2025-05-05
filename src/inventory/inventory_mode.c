#include "inventory_mode.h"

#include "../character/character.h"
#include "../common.h"
#include "../game.h"
#include "../local/local.h"
#include "../local/local_strings.h"
#include "../src/combat/combat_mode.h"
#include "./draw/draw_inventory_mode.h"

#include <notcurses/notcurses.h>
#include <stdbool.h>

// === Internal Functions ===
void collect_inventory_gear_options(gear_t* gear_inventory[], const int count);
void collect_inventory_equipment_options(gear_t* equipment[]);
void collect_inventory_potion_options(potion_t* potion_inventory[], const int count);

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
string_max_t* inventory_potion_options;

/**
 * @brief Initializes the inventory mode.
 */
int init_inventory_mode() {
    inventory_gear_options = memory_pool_alloc(main_memory_pool, sizeof(string_max_t) * MAX_GEAR_LIMIT);
    NULL_PTR_HANDLER_RETURN(inventory_gear_options, -1, "Inventory Mode", "Failed to allocate memory for inventory gear options");

    inventory_equipment_options = memory_pool_alloc(main_memory_pool, sizeof(string_max_t) * MAX_SLOT);
    NULL_PTR_HANDLER_RETURN(inventory_equipment_options, -1, "Inventory Mode", "Failed to allocate memory for inventory equipment options");

    inventory_potion_options = memory_pool_alloc(main_memory_pool, sizeof(string_max_t) * MAX_POTION_LIMIT);
    NULL_PTR_HANDLER_RETURN(inventory_potion_options, -1, "Inventory Mode", "Failed to allocate memory for inventory potion options");

    //update local once, so the strings are initialized
    update_inventory_local();
    //add update local function to the observer list
    add_local_observer(update_inventory_local);
    return 0;
}

/**
 * @brief Starts the inventory mode.
 */
void start_inventory_mode(character_t* player, character_t* monster) {
    internal_inventory_state_t inventory_state = INVENTORY_MENU;
    bool inventory_active = true;

    if (monster != NULL) {
        collect_inventory_gear_options(monster->gear_inventory, monster->gear_count);
        collect_inventory_equipment_options(monster->equipment);
        collect_inventory_potion_options(monster->potion_inventory, monster->potion_count);
    } else {
        collect_inventory_gear_options(player->gear_inventory, player->gear_count);
        collect_inventory_equipment_options(player->equipment);
        collect_inventory_potion_options(player->potion_inventory, player->potion_count);
    }

    while (inventory_active) {
        switch (inventory_state) {
            case INVENTORY_MENU:
                inventory_state = inventory_menu(player, monster);
                break;
            case INVENTORY_GEAR_MENU:
                inventory_state = inventory_gear_menu(player, monster);
                break;
            case INVENTORY_EQUIPMENT_MENU:
                inventory_state = inventory_equipment_menu(player, monster);
                break;
            case INVENTORY_POTION_MENU:
                inventory_state = inventory_potion_menu(player, monster);
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
internal_inventory_state_t inventory_menu(character_t* player, character_t* monster) {
    const character_t* target = (monster != NULL) ? monster : player;
    const vector2d_t anchor = draw_inventory_view(inventory_view_anchor, target);
    int selected_index = 0;

    internal_inventory_state_t new_state = INVENTORY_MENU;
    bool submenu_selected = false;

    while (!submenu_selected) {
        if (monster != NULL) {
            draw_inventory_menu(anchor,
                                local_strings[lomo_main_menu_title.idx].characters,
                                NULL,
                                &local_strings[lomo_main_menu_option1.idx],
                                MAX_INMO_MAIN_MENU_OPTION,
                                selected_index,
                                NULL,
                                local_strings[lomo_submenu_tail_message.idx].characters);
        } else {
            draw_inventory_menu(anchor,
                                local_strings[inmo_main_menu_title.idx].characters,
                                NULL,
                                &local_strings[inmo_main_menu_option1.idx],
                                MAX_INMO_MAIN_MENU_OPTION,
                                selected_index,
                                NULL,
                                local_strings[como_submenu_tail_message.idx].characters);
        }

        // check for input
        ncinput event;
        memset(&event, 0, sizeof(event));
        notcurses_get_blocking(nc, &event);

        // skip if key event is release
        if (!(event.evtype == NCTYPE_UNKNOWN || event.evtype == NCTYPE_PRESS)) { continue; }

        if (event.id == NCKEY_UP) {
            selected_index = (selected_index - 1 + MAX_INMO_MAIN_MENU_OPTION) % MAX_INMO_MAIN_MENU_OPTION;
        } else if (event.id == NCKEY_DOWN) {
            selected_index = (selected_index + 1) % MAX_INMO_MAIN_MENU_OPTION;
        } else if (event.id == NCKEY_ENTER) {
            if (selected_index == 0) {
                new_state = INVENTORY_GEAR_MENU;
            } else if (selected_index == 1) {
                new_state = INVENTORY_EQUIPMENT_MENU;
            } else if (selected_index == 2) {
                new_state = INVENTORY_POTION_MENU;
            }
            submenu_selected = true;
        } else if (event.id == NCKEY_ESC) {
            new_state = INVENTORY_EXIT;
            submenu_selected = true;
        }
    }
    return new_state;
}

/**
 * @brief Displays the gear inventory menu.
 */
internal_inventory_state_t inventory_gear_menu(character_t* player, character_t* monster) {
    const character_t* target = (monster != NULL) ? monster : player;
    vector2d_t anchor = draw_inventory_view(inventory_view_anchor, target);
    int selected_index = 0;

    if (target->gear_count == 0) {
        draw_inventory_log(anchor, local_strings[inmo_no_more_gear.idx].characters);
        return INVENTORY_MENU;
    }

    internal_inventory_state_t new_state = INVENTORY_MENU;
    bool item_selected_or_esc = false;

    while (!item_selected_or_esc) {
        if (monster != NULL) {
            draw_inventory_menu(anchor,
                                local_strings[inmo_inventory_menu_title.idx].characters,
                                local_strings[lomo_inventory_header_message.idx].characters,
                                inventory_gear_options,
                                monster->gear_count,
                                selected_index,
                                NULL,
                                local_strings[lomo_submenu_tail_message.idx].characters);
        } else {
            draw_inventory_menu(anchor,
                                local_strings[inmo_inventory_menu_title.idx].characters,
                                local_strings[inmo_inventory_header_message.idx].characters,
                                inventory_gear_options,
                                player->gear_count,
                                selected_index,
                                local_strings[inmo_submenu_key_message.idx].characters,
                                local_strings[como_submenu_tail_message.idx].characters);
        }

        // check for input
        ncinput event;
        memset(&event, 0, sizeof(event));
        notcurses_get_blocking(nc, &event);

        // skip if key event is release
        if (!(event.evtype == NCTYPE_UNKNOWN || event.evtype == NCTYPE_PRESS)) { continue; }

        if (event.id == NCKEY_UP) {
            selected_index = (selected_index - 1 + player->gear_count) % player->gear_count;
        } else if (event.id == NCKEY_DOWN) {
            selected_index = (selected_index + 1) % player->gear_count;
        } else if (event.id == NCKEY_ENTER) {
            if (monster != NULL) {
                if (player->gear_count < MAX_GEAR_LIMIT) {
                    add_gear(player, monster->gear_inventory[selected_index]);
                    remove_gear(monster, monster->gear_inventory[selected_index]);
                    collect_inventory_gear_options(monster->gear_inventory, monster->gear_count);
                } else {
                    anchor = draw_inventory_view(inventory_view_anchor, target);
                    draw_inventory_log(anchor, local_strings[inmo_no_more_gear_slot.idx].characters);
                }
            } else {
                if (player->equipment[player->gear_inventory[selected_index]->slot] == NULL) {
                    equip_gear(player, player->gear_inventory[selected_index]);
                    collect_inventory_gear_options(player->gear_inventory, player->gear_count);
                } else {
                    anchor = draw_inventory_view(inventory_view_anchor, target);
                    draw_inventory_log(anchor, local_strings[inmo_no_free_equipment_slot.idx].characters);
                }
            }
            return INVENTORY_GEAR_MENU;
        } else if ((event.id == 'd' || event.id == 'D') && monster == NULL) {
            remove_gear(player, player->gear_inventory[selected_index]);
            collect_inventory_gear_options(player->gear_inventory, player->gear_count);
            return INVENTORY_GEAR_MENU;
        } else if (event.id == NCKEY_ESC) {
            new_state = INVENTORY_MENU;
            item_selected_or_esc = true;
        }
    }
    return new_state;
}

/**
 * @brief Displays the equipment inventory menu.
 */
internal_inventory_state_t inventory_equipment_menu(character_t* player, character_t* monster) {
    const character_t* target = (monster != NULL) ? monster : player;
    vector2d_t anchor = draw_inventory_view(inventory_view_anchor, target);
    int selected_index = 0;

    internal_inventory_state_t new_state = INVENTORY_MENU;
    bool item_selected_or_esc = false;

    while (!item_selected_or_esc) {
        if (monster != NULL) {
            draw_inventory_menu(anchor,
                                local_strings[inmo_equipment_menu_title.idx].characters,
                                local_strings[lomo_equipment_header_message.idx].characters,
                                inventory_equipment_options,
                                MAX_SLOT,
                                selected_index,
                                NULL,
                                local_strings[lomo_submenu_tail_message.idx].characters);
        } else {
            draw_inventory_menu(anchor,
                                local_strings[inmo_equipment_menu_title.idx].characters,
                                local_strings[inmo_equipment_header_message.idx].characters,
                                inventory_equipment_options,
                                MAX_SLOT,
                                selected_index,
                                NULL,
                                local_strings[como_submenu_tail_message.idx].characters);
        }

        // check for input
        ncinput event;
        memset(&event, 0, sizeof(event));
        notcurses_get_blocking(nc, &event);

        // skip if key event is release
        if (!(event.evtype == NCTYPE_UNKNOWN || event.evtype == NCTYPE_PRESS)) { continue; }

        if (event.id == NCKEY_UP) {
            selected_index = (selected_index - 1 + MAX_SLOT) % MAX_SLOT;
        } else if (event.id == NCKEY_DOWN) {
            selected_index = (selected_index + 1) % MAX_SLOT;
        } else if (event.id == NCKEY_ENTER) {
            if (monster != NULL) {
                if (monster->equipment[selected_index] != NULL) {
                    if (player->gear_count < MAX_GEAR_LIMIT) {
                        add_gear(player, monster->equipment[selected_index]);
                        remove_equipped_gear(monster, (gear_slot_t) selected_index);
                        collect_inventory_equipment_options(monster->equipment);
                    } else {
                        anchor = draw_inventory_view(inventory_view_anchor, target);
                        draw_inventory_log(anchor, local_strings[inmo_no_more_gear_slot.idx].characters);
                    }
                    return INVENTORY_EQUIPMENT_MENU;
                }
            } else {
                if (player->equipment[selected_index] != NULL) {
                    if (player->gear_count < MAX_GEAR_LIMIT) {
                        unequip_gear(player, (gear_slot_t) selected_index);
                        collect_inventory_equipment_options(player->equipment);
                    } else {
                        anchor = draw_inventory_view(inventory_view_anchor, target);
                        draw_inventory_log(anchor, local_strings[inmo_no_more_gear_slot.idx].characters);
                    }
                    return INVENTORY_EQUIPMENT_MENU;
                }
            }
        } else if (event.id == NCKEY_ESC) {
            new_state = INVENTORY_MENU;
            item_selected_or_esc = true;
        }
    }
    return new_state;
}

/**
 * @brief Displays the potion inventory menu.
 */
internal_inventory_state_t inventory_potion_menu(character_t* player, character_t* monster) {
    const character_t* target = (monster != NULL) ? monster : player;
    vector2d_t anchor = draw_inventory_view(inventory_view_anchor, target);
    int selected_index = 0;

    if (target->potion_count == 0) {
        draw_inventory_log(anchor, local_strings[como_no_more_potions.idx].characters);
        return INVENTORY_MENU;
    }

    internal_inventory_state_t new_state = INVENTORY_MENU;
    bool item_selected_or_esc = false;

    while (!item_selected_or_esc) {
        if (monster != NULL) {
            draw_inventory_menu(anchor,
                                local_strings[inmo_potion_menu_title.idx].characters,
                                local_strings[lomo_potion_header_message.idx].characters,
                                inventory_potion_options,
                                monster->potion_count,
                                selected_index,
                                NULL,
                                local_strings[lomo_submenu_tail_message.idx].characters);
        } else {
            draw_inventory_menu(anchor,
                                local_strings[inmo_potion_menu_title.idx].characters,
                                local_strings[inmo_potion_header_message.idx].characters,
                                inventory_potion_options,
                                player->potion_count,
                                selected_index,
                                local_strings[inmo_submenu_key_message.idx].characters,
                                local_strings[como_submenu_tail_message.idx].characters);
        }

        // check for input
        ncinput event;
        memset(&event, 0, sizeof(event));
        notcurses_get_blocking(nc, &event);

        // skip if key event is release
        if (!(event.evtype == NCTYPE_UNKNOWN || event.evtype == NCTYPE_PRESS)) { continue; }

        if (event.id == NCKEY_UP) {
            selected_index = (selected_index - 1 + player->potion_count) % player->potion_count;
        } else if (event.id == NCKEY_DOWN) {
            selected_index = (selected_index + 1) % player->potion_count;
        } else if (event.id == NCKEY_ENTER) {
            if (monster != NULL) {
                if (player->potion_count < MAX_POTION_LIMIT) {
                    add_potion(player, monster->potion_inventory[selected_index]);
                    remove_potion(monster, monster->potion_inventory[selected_index]);
                    collect_inventory_potion_options(monster->potion_inventory, monster->potion_count);
                } else {
                    anchor = draw_inventory_view(inventory_view_anchor, target);
                    draw_inventory_log(anchor, local_strings[inmo_no_more_potion_slot.idx].characters);
                }
            } else {
                char message[MAX_STRING_LENGTH];
                snprintf(message, sizeof(message), local_strings[como_potion_use.idx].characters,//TODO: This method of using formats is not safe!
                         player->name,
                         player->potion_inventory[selected_index]->name,
                         player->potion_inventory[selected_index]->value,
                         potion_type_to_string(player->potion_inventory[selected_index]->effectType));

                invoke_potion_effect(player, player->potion_inventory[selected_index]);
                anchor = draw_inventory_view(inventory_view_anchor, target);
                draw_inventory_log(anchor, message);
                collect_inventory_potion_options(player->potion_inventory, player->potion_count);
            }
            return INVENTORY_POTION_MENU;
        } else if ((event.id == 'd' || event.id == 'D') && monster == NULL) {
            remove_potion(player, player->potion_inventory[selected_index]);
            collect_inventory_potion_options(player->potion_inventory, player->potion_count);
            return INVENTORY_POTION_MENU;
        } else if (event.id == NCKEY_ESC) {
            new_state = INVENTORY_MENU;
            item_selected_or_esc = true;
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
                 local_strings[inmo_inventory_format.idx].characters,
                 gear_inventory[i]->name,
                 gear_slot_to_string(gear_inventory[i]->slot));
    }
}

/**
 * @brief Collects equipment inventory options for display.
 */
void collect_inventory_equipment_options(gear_t* equipment[]) {
    for (int i = 0; i < MAX_SLOT; i++) {
        memset(inventory_equipment_options[i].characters, '\0', MAX_STRING_LENGTH);
    }

    for (int i = 0; i < MAX_SLOT; i++) {
        if (equipment[i] != NULL) {
            snprintf(inventory_equipment_options[i].characters, MAX_STRING_LENGTH,
                     local_strings[inmo_equipment_format.idx].characters,
                     equipment[i]->name,
                     gear_slot_to_string((gear_slot_t) i));
        } else {
            snprintf(inventory_equipment_options[i].characters, MAX_STRING_LENGTH,
                     local_strings[inmo_equipment_format_empty.idx].characters,
                     gear_slot_to_string((gear_slot_t) i));
        }
    }
}

/**
 * @brief Collects potion inventory options for display.
 */
void collect_inventory_potion_options(potion_t* potion_inventory[], const int count) {
    for (int i = 0; i < MAX_POTION_LIMIT; i++) {
        memset(inventory_potion_options[i].characters, '\0', MAX_STRING_LENGTH);
    }

    for (int i = 0; i < count; i++) {
        snprintf(inventory_potion_options[i].characters, MAX_STRING_LENGTH,
                 local_strings[inmo_potion_format.idx].characters,
                 potion_inventory[i]->name,
                 potion_type_to_string(potion_inventory[i]->effectType));
    }
}

void update_inventory_local(void) {
    //inventory menu
    snprintf(local_strings[inmo_main_menu_title.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(inmo_main_menu_title.key));
    snprintf(local_strings[inmo_main_menu_option1.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(inmo_main_menu_option1.key));
    snprintf(local_strings[inmo_main_menu_option2.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(inmo_main_menu_option2.key));
    snprintf(local_strings[inmo_main_menu_option3.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(inmo_main_menu_option3.key));

    //loot menu
    snprintf(local_strings[lomo_main_menu_title.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(lomo_main_menu_title.key));
    snprintf(local_strings[lomo_main_menu_option1.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(lomo_main_menu_option1.key));
    snprintf(local_strings[lomo_main_menu_option2.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(lomo_main_menu_option2.key));
    snprintf(local_strings[lomo_main_menu_option3.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(lomo_main_menu_option3.key));

    //inventory gear menu
    snprintf(local_strings[inmo_inventory_menu_title.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(inmo_inventory_menu_title.key));
    snprintf(local_strings[inmo_inventory_format.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(inmo_inventory_format.key));

    //inventory equipment menu
    snprintf(local_strings[inmo_equipment_menu_title.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(inmo_equipment_menu_title.key));
    snprintf(local_strings[inmo_equipment_format.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(inmo_equipment_format.key));
    snprintf(local_strings[inmo_equipment_format_empty.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(inmo_equipment_format_empty.key));

    //inventory potion menu
    snprintf(local_strings[inmo_potion_menu_title.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(inmo_potion_menu_title.key));
    snprintf(local_strings[inmo_potion_format.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(inmo_potion_format.key));

    //header messages
    snprintf(local_strings[inmo_inventory_header_message.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(inmo_inventory_header_message.key));
    snprintf(local_strings[inmo_equipment_header_message.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(inmo_equipment_header_message.key));
    snprintf(local_strings[inmo_potion_header_message.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(inmo_potion_header_message.key));
    snprintf(local_strings[lomo_inventory_header_message.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(lomo_inventory_header_message.key));
    snprintf(local_strings[lomo_equipment_header_message.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(lomo_equipment_header_message.key));
    snprintf(local_strings[lomo_potion_header_message.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(lomo_potion_header_message.key));

    //tail messages
    snprintf(local_strings[inmo_submenu_key_message.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(inmo_submenu_key_message.key));
    snprintf(local_strings[como_submenu_tail_message.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(como_submenu_tail_message.key));
    snprintf(local_strings[lomo_submenu_tail_message.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(lomo_submenu_tail_message.key));

    //inventory messages
    snprintf(local_strings[inmo_no_more_gear.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(inmo_no_more_gear.key));
    snprintf(local_strings[como_no_more_potions.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(como_no_more_potions.key));
    snprintf(local_strings[inmo_no_more_gear_slot.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(inmo_no_more_gear_slot.key));
    snprintf(local_strings[inmo_no_more_potion_slot.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(inmo_no_more_potion_slot.key));
    snprintf(local_strings[inmo_no_free_equipment_slot.idx].characters, MAX_STRING_LENGTH, "%s", get_local_string(inmo_no_free_equipment_slot.key));
}

/**
 * @brief Shuts down the inventory mode and frees allocated resources.
 */
void shutdown_inventory_mode(void) {
    memory_pool_free(main_memory_pool, inventory_gear_options);
    memory_pool_free(main_memory_pool, inventory_equipment_options);
    memory_pool_free(main_memory_pool, inventory_potion_options);
}
