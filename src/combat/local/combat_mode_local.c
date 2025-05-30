/**
 * @file combat_mode_local.c
 * @brief Implementation for the localization of the combat mode.
 */
#include "combat_mode_local.h"

#include "../../local/local_handler.h"

#include <stdlib.h>

char** combat_mode_strings = NULL;

void update_combat_local(void) {
    for (int i = 0; i < MAX_COMBAT_MODE_STRINGS; i++) {
        if (combat_mode_strings[i] != NULL) {
            free(combat_mode_strings[i]);
        }
    }

    combat_mode_strings[MAIN_MENU_TITLE] = get_local_string("COMBAT.MAIN.MENU.TITLE");
    combat_mode_strings[ABILITY_MENU_TITLE] = get_local_string("COMBAT.ABILITY.MENU.TITLE");
    combat_mode_strings[POTION_MENU_TITLE] = get_local_string("COMBAT.POTION.MENU.TITLE");
    combat_mode_strings[MAIN_MENU_OPTION1] = get_local_string("COMBAT.USE.ABILITY");
    combat_mode_strings[MAIN_MENU_OPTION2] = get_local_string("COMBAT.USE.POTION");

    combat_mode_strings[ABILITY_FORMAT] = get_local_string("COMBAT.ABILITY.FORMAT");
    combat_mode_strings[POTION_FORMAT] = get_local_string("POTION.FORMAT");
    combat_mode_strings[PRESS_C_RETURN] = get_local_string("PRESS.C.RETURN");
    combat_mode_strings[NO_MORE_POTIONS] = get_local_string("POTION.EMPTY");

    combat_mode_strings[ATTACK_SUCCESS] = get_local_string("COMBAT.ATTACK.SUCCESS");
    combat_mode_strings[ATTACK_MISS] = get_local_string("COMBAT.ATTACK.MISS");
    combat_mode_strings[ATTACK_FAIL] = get_local_string("COMBAT.ATTACK.FAIL");
    combat_mode_strings[POTION_USE] = get_local_string("COMBAT.POTION.USE");

    combat_mode_strings[WON_COMBAT_MSG1] = get_local_string("COMBAT.WON.MESSAGE1");
    combat_mode_strings[WON_COMBAT_MSG2] = get_local_string("COMBAT.WON.MESSAGE2");
}
