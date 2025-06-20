/**
 * @file inventory_mode.c
 * @brief Implements functionality for the inventory mode.
 */
#include "inventory_mode.h"

#include "../character/character.h"
#include "../common.h"
#include "../io/input/input_handler.h"
#include "../io/output/specific/inventory_output.h"
#include "../item/gear.h"
#include "../item/local/gear_local.h"
#include "../item/local/potion_local.h"
#include "../local/local_handler.h"
#include "../src/combat/combat_mode.h"
#include "local/inventory_mode_local.h"

// === Internal Functions ===
/**
 * @brief Checks if the equipment can be equipped in case it occupies a hands slot.
 * @param player The player character.
 * @param gear The gear to check.
 * @return bool True if the gear can be equipped, false otherwise.
 */
bool can_equip_gear(character_t* player, gear_t* gear);
/**
 * @brief Checks which options are available in the inventory menu for display.
 * @param gear_inventory The gear inventory to check.
 * @param count The number of items in the gear inventory.
 */
void collect_inventory_gear_options(gear_t* gear_inventory[], int count);
/**
 * @brief Checks which options are available in the equipment menu for display.
 * @param equipment The equipment to check.
 */
void collect_inventory_equipment_options(gear_t* equipment[]);
/**
 * @brief Checks which options are available in the potion menu for display.
 * @param potion_inventory The potion inventory to check.
 * @param count The number of items in the potion inventory.
 */
void collect_inv_potion_options(potion_t* potion_inventory[], int count);

// === Intern Global Variables ===
vector2d_t inventory_view_anchor = {1, 1};
internal_inventory_state_t inventory_state = INVENTORY_MENU;

int inventory_gear_count = 0;
int inventory_potion_count = 0;

char** inventory_gear_options = NULL;
char** inventory_equipment_options = NULL;
char** inventory_potion_options = NULL;

int init_inventory_mode() {
    inventory_mode_strings = (char**) malloc(sizeof(char*) * MAX_INVENTORY_STRINGS);
    RETURN_WHEN_NULL(inventory_mode_strings, -1, "Inventory Mode", "Failed to allocate memory for inventory mode strings");

    inventory_equipment_options = (char**) malloc(sizeof(char*) * MAX_SLOT);
    if (inventory_equipment_options == NULL) {
        free(inventory_mode_strings);
        log_msg(ERROR, "Inventory Mode", "Failed to allocate memory for inventory equipment options");
        return -1;
    }

    // preset the strings to NULL
    for (int i = 0; i < MAX_INVENTORY_STRINGS; i++) {
        inventory_mode_strings[i] = NULL;
    }
    // preset the equipped gear options to NULL
    for (int i = 0; i < MAX_SLOT; i++) {
        inventory_equipment_options[i] = NULL;
    }

    //update local once, so the strings are initialized
    update_inventory_local();
    //add update local function to the observer list
    observe_local(update_inventory_local);
    return 0;
}

inventory_result_t start_inventory(character_t* player, character_t* monster) {
    if (monster != NULL) {
        collect_inventory_gear_options(monster->gear_inventory, monster->gear_count);
        collect_inventory_equipment_options(monster->equipment);
        collect_inv_potion_options(monster->potion_inventory, monster->potion_count);
    } else {
        collect_inventory_gear_options(player->gear_inventory, player->gear_count);
        collect_inventory_equipment_options(player->equipment);
        collect_inv_potion_options(player->potion_inventory, player->potion_count);
    }

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
            inventory_state = INVENTORY_MENU;
            return EXIT_TO_MAP;
    }
    return CONTINUE_INVENTORY;
}

internal_inventory_state_t inventory_menu(character_t* player, character_t* monster) {
    const character_t* target = (monster != NULL) ? monster : player;
    const vector2d_t anchor = draw_inventory_view(inventory_view_anchor, target);
    int selected_index = 0;

    internal_inventory_state_t new_state = INVENTORY_MENU;
    bool submenu_selected = false;

    while (!submenu_selected) {
        if (monster != NULL) {
            draw_inventory_menu(anchor,
                                inventory_mode_strings[LOOT_MAIN_MENU_TITLE],
                                NULL,
                                &inventory_mode_strings[LOOT_OPTION_SHOW_GEAR],
                                3,
                                selected_index,
                                NULL,
                                inventory_mode_strings[FINISH_LOOTING_MSG]);
        } else {
            draw_inventory_menu(anchor,
                                inventory_mode_strings[MAIN_MENU_TITLE],
                                NULL,
                                &inventory_mode_strings[SHOW_GEAR],
                                3,
                                selected_index,
                                NULL,
                                inventory_mode_strings[PRESS_C_RETURN]);
        }

        // check for input
        input_event_t input_event;
        if (!get_input_blocking(&input_event)) {
            continue;
        }

        // Handle input using logical input types
        switch (input_event.type) {
            case INPUT_UP:
                selected_index = (selected_index - 1 + 3) % 3;
                break;
            case INPUT_DOWN:
                selected_index = (selected_index + 1) % 3;
                break;
            case INPUT_CONFIRM:
                if (selected_index == 0) {
                    new_state = INVENTORY_GEAR_MENU;
                } else if (selected_index == 1) {
                    new_state = INVENTORY_EQUIPMENT_MENU;
                } else if (selected_index == 2) {
                    new_state = INVENTORY_POTION_MENU;
                }
                submenu_selected = true;
                break;
            case INPUT_CANCEL:
            case INPUT_INVENTORY:
                new_state = INVENTORY_EXIT;
                submenu_selected = true;
                break;
            default:
                break;
        }
    }
    return new_state;
}

internal_inventory_state_t inventory_gear_menu(character_t* player, character_t* monster) {
    const character_t* target = (monster != NULL) ? monster : player;
    vector2d_t anchor = draw_inventory_view(inventory_view_anchor, target);
    int selected_index = 0;

    if (target->gear_count == 0) {
        draw_inventory_log(anchor, inventory_mode_strings[INVENTORY_EMPTY_MSG]);
        return INVENTORY_MENU;
    }

    internal_inventory_state_t new_state = INVENTORY_GEAR_MENU;
    bool item_selected_or_esc = false;

    while (!item_selected_or_esc) {
        if (monster != NULL) {
            draw_inventory_menu(anchor,
                                inventory_mode_strings[INVENTORY_MENU_TITLE],
                                inventory_mode_strings[LOOT_GEAR_MENU_HEADER],
                                inventory_gear_options,// TODO
                                monster->gear_count,
                                selected_index,
                                NULL,
                                inventory_mode_strings[PRESS_C_RETURN]);
        } else {
            draw_inventory_menu(anchor,
                                inventory_mode_strings[INVENTORY_MENU_TITLE],
                                inventory_mode_strings[INVENTORY_MENU_HEADER],
                                inventory_gear_options,// TODO
                                player->gear_count,
                                selected_index,
                                inventory_mode_strings[INVENTORY_DROP_GEAR_MSG],
                                inventory_mode_strings[PRESS_C_RETURN]);
        }

        // check for input
        input_event_t input_event;
        if (!get_input_blocking(&input_event)) {
            continue;
        }

        // For character keys like 'd'/'D', we need to check the raw input value
        uint32_t key_id = input_event.raw_input.id;

        // Handle input using logical input types
        switch (input_event.type) {
            case INPUT_UP:
                selected_index = (selected_index - 1 + target->gear_count) % target->gear_count;
                break;
            case INPUT_DOWN:
                selected_index = (selected_index + 1) % target->gear_count;
                break;
            case INPUT_CONFIRM:
                if (monster != NULL) {
                    if (player->gear_count < MAX_GEAR_LIMIT) {
                        add_gear(player, monster->gear_inventory[selected_index]);
                        remove_gear(monster, monster->gear_inventory[selected_index]);
                        collect_inventory_gear_options(monster->gear_inventory, monster->gear_count);
                    } else {
                        anchor = draw_inventory_view(inventory_view_anchor, target);
                        draw_inventory_log(anchor, inventory_mode_strings[INVENTORY_FULL_MSG]);
                    }
                } else {
                    if (player->equipment[player->gear_inventory[selected_index]->slot] == NULL) {
                        if (can_equip_gear(player, player->gear_inventory[selected_index])) {
                            equip_gear(player, player->gear_inventory[selected_index]);
                            collect_inventory_gear_options(player->gear_inventory, player->gear_count);
                        } else {
                            anchor = draw_inventory_view(inventory_view_anchor, target);
                            draw_inventory_log(anchor, inventory_mode_strings[EQUIPMENT_HANDS_SLOT_FULL]);
                        }
                    } else {
                        anchor = draw_inventory_view(inventory_view_anchor, target);
                        draw_inventory_log(anchor, inventory_mode_strings[EQUIPMENT_SLOT_FULL]);
                    }
                }
                return INVENTORY_GEAR_MENU;
            case INPUT_CANCEL:
                new_state = INVENTORY_MENU;
                item_selected_or_esc = true;
                break;
            default:
                // Handle special character keys
                if ((key_id == 'd' || key_id == 'D') && monster == NULL) {
                    remove_gear(player, player->gear_inventory[selected_index]);
                    collect_inventory_gear_options(player->gear_inventory, player->gear_count);
                    return INVENTORY_GEAR_MENU;
                }
                break;
        }
    }
    return new_state;
}

/**
 * @brief Checks wether the character can equip certain gear or not.
 *
 * @param player Pointer to the player.
 * @param gear Pointer to the gear to check.
 * @return true if player can equip the gear false if not.
 */
bool can_equip_gear(character_t* player, gear_t* gear) {
    if (gear->slot == SLOT_BOTH_HANDS) {
        if (player->equipment[SLOT_LEFT_HAND] != NULL || player->equipment[SLOT_RIGHT_HAND] != NULL) {
            return false;
        }
    } else if (gear->slot == SLOT_LEFT_HAND || gear->slot == SLOT_RIGHT_HAND) {
        if (player->equipment[SLOT_BOTH_HANDS] != NULL) {
            return false;
        }
    }
    return true;
}

internal_inventory_state_t inventory_equipment_menu(character_t* player, character_t* monster) {
    const character_t* target = (monster != NULL) ? monster : player;
    vector2d_t anchor = draw_inventory_view(inventory_view_anchor, target);
    int selected_index = 0;

    internal_inventory_state_t new_state = INVENTORY_EQUIPMENT_MENU;
    bool item_selected_or_esc = false;

    while (!item_selected_or_esc) {
        if (monster != NULL) {
            draw_inventory_menu(anchor,
                                inventory_mode_strings[EQUIPMENT_MENU_TITLE],
                                inventory_mode_strings[LOOT_EQUIPMENT_MENU_HEADER],
                                inventory_equipment_options,// TODO
                                MAX_SLOT,
                                selected_index,
                                NULL,
                                inventory_mode_strings[PRESS_C_RETURN]);
        } else {
            draw_inventory_menu(anchor,
                                inventory_mode_strings[EQUIPMENT_MENU_TITLE],
                                inventory_mode_strings[EQUIPMENT_MENU_HEADER],
                                inventory_equipment_options,// TODO
                                MAX_SLOT,
                                selected_index,
                                NULL,
                                inventory_mode_strings[PRESS_C_RETURN]);
        }

        // check for input
        input_event_t input_event;
        if (!get_input_blocking(&input_event)) {
            continue;
        }

        // Handle input using logical input types
        switch (input_event.type) {
            case INPUT_UP:
                selected_index = (selected_index - 1 + MAX_SLOT) % MAX_SLOT;
                break;
            case INPUT_DOWN:
                selected_index = (selected_index + 1) % MAX_SLOT;
                break;
            case INPUT_CONFIRM:
                if (monster != NULL) {
                    if (monster->equipment[selected_index] != NULL) {
                        if (player->gear_count < MAX_GEAR_LIMIT) {
                            add_gear(player, monster->equipment[selected_index]);
                            remove_equipped_gear(monster, (gear_slot_t) selected_index);
                            collect_inventory_equipment_options(monster->equipment);
                        } else {
                            anchor = draw_inventory_view(inventory_view_anchor, target);
                            draw_inventory_log(anchor, inventory_mode_strings[INVENTORY_FULL_MSG]);
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
                            draw_inventory_log(anchor, inventory_mode_strings[INVENTORY_FULL_MSG]);
                        }
                        return INVENTORY_EQUIPMENT_MENU;
                    }
                }
                break;
            case INPUT_CANCEL:
                new_state = INVENTORY_MENU;
                item_selected_or_esc = true;
                break;
            default:
                break;
        }
    }
    return new_state;
}

internal_inventory_state_t inventory_potion_menu(character_t* player, character_t* monster) {
    const character_t* target = (monster != NULL) ? monster : player;
    vector2d_t anchor = draw_inventory_view(inventory_view_anchor, target);
    int selected_index = 0;

    if (target->potion_count == 0) {
        draw_inventory_log(anchor, inventory_mode_strings[POTION_EMPTY_MSG]);
        return INVENTORY_MENU;
    }

    internal_inventory_state_t new_state = INVENTORY_MENU;
    bool item_selected_or_esc = false;

    while (!item_selected_or_esc) {
        if (monster != NULL) {
            draw_inventory_menu(anchor,
                                inventory_mode_strings[POTION_MENU_TITLE],
                                inventory_mode_strings[LOOT_POTION_MENU_HEADER],
                                inventory_potion_options,// TODO
                                monster->potion_count,
                                selected_index,
                                NULL,
                                inventory_mode_strings[PRESS_C_RETURN]);
        } else {
            draw_inventory_menu(anchor,
                                inventory_mode_strings[POTION_MENU_TITLE],
                                inventory_mode_strings[POTION_MENU_HEADER],
                                inventory_potion_options,// TODO
                                player->potion_count,
                                selected_index,
                                inventory_mode_strings[POTION_DROP_POTION_MSG],
                                inventory_mode_strings[PRESS_C_RETURN]);
        }

        // check for input
        input_event_t input_event;
        if (!get_input_blocking(&input_event)) {
            continue;
        }

        // For character keys like 'd'/'D', we need to check the raw input value
        uint32_t key_id = input_event.raw_input.id;

        // Handle input using logical input types
        switch (input_event.type) {
            case INPUT_UP:
                selected_index = (selected_index - 1 + target->potion_count) % target->potion_count;
                break;
            case INPUT_DOWN:
                selected_index = (selected_index + 1) % target->potion_count;
                break;
            case INPUT_CONFIRM:
                if (monster != NULL) {
                    if (player->potion_count < MAX_POTION_LIMIT) {
                        add_potion(player, monster->potion_inventory[selected_index]);
                        remove_potion(monster, monster->potion_inventory[selected_index]);
                        collect_inv_potion_options(monster->potion_inventory, monster->potion_count);
                    } else {
                        anchor = draw_inventory_view(inventory_view_anchor, target);
                        draw_inventory_log(anchor, inventory_mode_strings[POTION_FULL_MSG]);
                    }
                } else {
                    char message[MAX_STRING_LENGTH];
                    snprintf(message, sizeof(message), inventory_mode_strings[POTION_USE],//TODO: This method of using formats is not safe!
                             player->name,
                             potion_names[player->potion_inventory[selected_index]->effectType],
                             player->potion_inventory[selected_index]->value,
                             potion_type_to_string(player->potion_inventory[selected_index]->effectType));

                    invoke_potion_effect(player, player->potion_inventory[selected_index]);
                    anchor = draw_inventory_view(inventory_view_anchor, target);
                    draw_inventory_log(anchor, message);
                    collect_inv_potion_options(player->potion_inventory, player->potion_count);
                }
                return INVENTORY_POTION_MENU;
            case INPUT_CANCEL:
                new_state = INVENTORY_MENU;
                item_selected_or_esc = true;
                break;
            default:
                // Handle special character keys
                if ((key_id == 'd' || key_id == 'D') && monster == NULL) {
                    remove_potion(player, player->potion_inventory[selected_index]);
                    collect_inv_potion_options(player->potion_inventory, player->potion_count);
                    return INVENTORY_POTION_MENU;
                }
                break;
        }
    }
    return new_state;
}

/**
 * @brief Collects all the options for inventory gear for displaying.
 *
 * @param gear_inventory An array of gear.
 * @param count Ammount of items in the array.
 */
void collect_inventory_gear_options(gear_t* gear_inventory[], const int count) {
    if (inventory_gear_options != NULL) {
        for (int i = 0; i < inventory_gear_count; i++) {
            if (inventory_gear_options[i] != NULL) {
                free(inventory_gear_options[i]);
                inventory_gear_options[i] = NULL;
            }
        }
        free(inventory_gear_options);
        inventory_gear_options = NULL;
    }

    inventory_gear_count = count;
    inventory_gear_options = (char**) malloc(count * sizeof(char*));
    RETURN_WHEN_NULL(inventory_gear_options, , "Inventory Mode", "Failed to allocate memory for inventory gear options");

    for (int i = 0; i < count; i++) {
        inventory_gear_options[i] = (char*) malloc(MAX_STRING_LENGTH * sizeof(char));
        if (inventory_gear_options[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(inventory_gear_options[j]);
                inventory_gear_options[j] = NULL;
            }
            free(inventory_gear_options);
            inventory_gear_options = NULL;
            log_msg(ERROR, "Inventory Mode", "Failed to allocate memory for inventory gear options");
            return;
        }
    }

    for (int i = 0; i < count; i++) {
        snprintf(inventory_gear_options[i], MAX_STRING_LENGTH,
                 inventory_mode_strings[INVENTORY_GEAR_FORMAT],//TODO: This method of using formats is not safe!
                 gear_names[gear_inventory[i]->gear_identifier],
                 gear_slot_to_string(gear_inventory[i]->slot),
                 gear_inventory[i]->defenses.armor,
                 gear_inventory[i]->defenses.magic_resist);
    }
}

/**
 * @brief Collects equipment inventory options for display.
 *
 * @param equipment An array of equipment.
 */
void collect_inventory_equipment_options(gear_t* equipment[]) {
    if (inventory_equipment_options != NULL) {
        for (int i = 0; i < MAX_SLOT; i++) {
            if (inventory_equipment_options[i] != NULL) {
                free(inventory_equipment_options[i]);
                inventory_equipment_options[i] = NULL;
            }
        }
        for (int i = 0; i < MAX_SLOT; i++) {
            inventory_equipment_options[i] = (char*) malloc(MAX_STRING_LENGTH * sizeof(char));
            if (inventory_equipment_options[i] == NULL) {
                for (int j = 0; j < i; j++) {
                    free(inventory_equipment_options[j]);
                    inventory_equipment_options[j] = NULL;
                }
                log_msg(ERROR, "Inventory Mode", "Failed to allocate memory for inventory equipment options");
                return;
            }
        }
    }

    for (int i = 0; i < MAX_SLOT; i++) {
        if (equipment[i] != NULL) {
            snprintf(inventory_equipment_options[i], MAX_STRING_LENGTH,
                     inventory_mode_strings[INVENTORY_GEAR_FORMAT],//TODO: This method of using formats is not safe!
                     gear_names[equipment[i]->gear_identifier],
                     gear_slot_to_string((gear_slot_t) i),
                     equipment[i]->defenses.armor,
                     equipment[i]->defenses.magic_resist);
        } else {
            snprintf(inventory_equipment_options[i], MAX_STRING_LENGTH,
                     inventory_mode_strings[INVENTORY_GEAR_FORMAT_EMPTY],//TODO: This method of using formats is not safe!
                     gear_slot_to_string((gear_slot_t) i));
        }
    }
}

/**
 * @brief Collects all the options for inventory potions for displaying.
 *
 * @param potion_inventory An array of potions.
 * @param count Ammount of potions in the array.
 */
void collect_inv_potion_options(potion_t* potion_inventory[], int count) {
    if (inventory_potion_options != NULL) {
        for (int i = 0; i < inventory_potion_count; i++) {
            if (inventory_potion_options[i] != NULL) {
                free(inventory_potion_options[i]);
                inventory_potion_options[i] = NULL;
            }
        }
        free(inventory_potion_options);
        inventory_potion_options = NULL;
    }

    inventory_potion_count = count;
    inventory_potion_options = (char**) malloc(count * sizeof(char*));
    RETURN_WHEN_NULL(inventory_potion_options, , "Inventory Mode", "Failed to allocate memory for inventory potion options");

    for (int i = 0; i < count; i++) {
        inventory_potion_options[i] = (char*) malloc(MAX_STRING_LENGTH * sizeof(char));
        if (inventory_potion_options[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(inventory_potion_options[j]);
                inventory_potion_options[j] = NULL;
            }
            free(inventory_potion_options);
            inventory_potion_options = NULL;
            log_msg(ERROR, "Inventory Mode", "Failed to allocate memory for inventory potion options");
            return;
        }
    }

    for (int i = 0; i < count; i++) {
        snprintf(inventory_potion_options[i], MAX_STRING_LENGTH,
                 inventory_mode_strings[POTION_FORMAT],
                 potion_names[potion_inventory[i]->effectType],
                 potion_type_to_string(potion_inventory[i]->effectType),
                 potion_inventory[i]->value);
    }
}

void shutdown_inventory_mode(void) {
    // free the local strings
    if (inventory_mode_strings != NULL) {
        for (int i = 0; i < MAX_INVENTORY_STRINGS; i++) {
            if (inventory_mode_strings[i] != NULL) {
                free(inventory_mode_strings[i]);
            }
        }
        free(inventory_mode_strings);
    }
    // free the inventory gear options
    if (inventory_gear_options != NULL) {
        for (int i = 0; i < inventory_gear_count; i++) {
            if (inventory_gear_options[i] != NULL) {
                free(inventory_gear_options[i]);
            }
        }
        free(inventory_gear_options);
    }
    // free the inventory equipped gear options
    if (inventory_equipment_options != NULL) {
        for (int i = 0; i < MAX_SLOT; i++) {
            if (inventory_equipment_options[i] != NULL) {
                free(inventory_equipment_options[i]);
            }
        }
        free(inventory_equipment_options);
    }
    // free the inventory potion options
    if (inventory_potion_options != NULL) {
        for (int i = 0; i < inventory_potion_count; i++) {
            if (inventory_potion_options[i] != NULL) {
                free(inventory_potion_options[i]);
            }
        }
        free(inventory_potion_options);
    }
}
