#include "inventory_mode_local.h"

#include "../../local/local_handler.h"

#include <stdlib.h>

char** inventory_mode_strings = NULL;

void update_inventory_local(void) {
    for (int i = 0; i < MAX_INVENTORY_STRINGS; i++) {
        if (inventory_mode_strings[i] != NULL) {
            free(inventory_mode_strings[i]);
        }
    }

    inventory_mode_strings[MAIN_MENU_TITLE] = get_local_string("INVENTORY.MAIN.MENU.TITLE");
    inventory_mode_strings[SHOW_GEAR] = get_local_string("INVENTORY.OPTION.SHOW.GEAR");
    inventory_mode_strings[SHOW_EQUIPPED_GEAR] = get_local_string("INVENTORY.OPTION.SHOW.GEAR.EQUIPPED");
    inventory_mode_strings[SHOW_POTIONS] = get_local_string("INVENTORY.OPTION.SHOW.POTIONS");
    inventory_mode_strings[INVENTORY_FULL_MSG] = get_local_string("INVENTORY.FULL");
    inventory_mode_strings[INVENTORY_EMPTY_MSG] = get_local_string("INVENTORY.EMPTY");
    inventory_mode_strings[INVENTORY_MENU_TITLE] = get_local_string("INVENTORY.MENU.TITLE");
    inventory_mode_strings[INVENTORY_MENU_HEADER] = get_local_string("INVENTORY.MENU.HEADER");
    inventory_mode_strings[INVENTORY_DROP_GEAR_MSG] = get_local_string("INVENTORY.DROP_GEAR");
    inventory_mode_strings[INVENTORY_GEAR_FORMAT] = get_local_string("INVENTORY.GEAR.FORMAT");
    inventory_mode_strings[INVENTORY_GEAR_FORMAT_EMPTY] = get_local_string("INVENTORY.GEAR.FORMAT.EMPTY");

    inventory_mode_strings[EQUIPMENT_MENU_TITLE] = get_local_string("EQUIPMENT.MENU.TITLE");
    inventory_mode_strings[EQUIPMENT_MENU_HEADER] = get_local_string("EQUIPMENT.MENU.HEADER");
    inventory_mode_strings[EQUIPMENT_SLOT_FULL] = get_local_string("EQUIPMENT.SLOT.FULL");

    inventory_mode_strings[POTION_FORMAT] = get_local_string("POTION.FORMAT");
    inventory_mode_strings[POTION_MENU_TITLE] = get_local_string("POTION.MENU.TITLE");
    inventory_mode_strings[POTION_MENU_HEADER] = get_local_string("POTION.MENU.HEADER");
    inventory_mode_strings[POTION_FULL_MSG] = get_local_string("POTION.FULL");
    inventory_mode_strings[POTION_EMPTY_MSG] = get_local_string("POTION.EMPTY");
    inventory_mode_strings[POTION_DROP_POTION_MSG] = get_local_string("POTION.DROP_POTION");
    inventory_mode_strings[POTION_USE] = get_local_string("COMBAT.POTION.USE");

    inventory_mode_strings[LOOT_MAIN_MENU_TITLE] = get_local_string("LOOT.MAIN.MENU.TITLE");
    inventory_mode_strings[LOOT_OPTION_SHOW_GEAR] = get_local_string("LOOT.OPTION.SHOW.GEAR");
    inventory_mode_strings[LOOT_OPTION_SHOW_GEAR_EQUIPPED] = get_local_string("LOOT.OPTION.SHOW.GEAR.EQUIPPED");
    inventory_mode_strings[LOOT_OPTION_SHOW_POTIONS] = get_local_string("LOOT.OPTION.SHOW.POTIONS");
    inventory_mode_strings[LOOT_GEAR_MENU_HEADER] = get_local_string("LOOT.GEAR.MENU.HEADER");
    inventory_mode_strings[LOOT_EQUIPMENT_MENU_HEADER] = get_local_string("LOOT.EQUIPMENT.MENU.HEADER");
    inventory_mode_strings[LOOT_POTION_MENU_HEADER] = get_local_string("LOOT.POTION.MENU.HEADER");
    inventory_mode_strings[FINISH_LOOTING_MSG] = get_local_string("LOOT.FINISH");

    inventory_mode_strings[PRESS_ESC_RETURN] = get_local_string("PRESS.ESC.RETURN");
}
