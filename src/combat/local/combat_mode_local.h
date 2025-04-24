#ifndef COMBAT_MODE_LOCAL_H
#define COMBAT_MODE_LOCAL_H

#include "../../local/local.h"

#define MAX_MENU_TITLES 3
#define MAX_OPTION_FORMATS 2
#define MAX_COMBAT_MENU_OPTIONS 2

enum combat_string_index {
    //titles
    MAIN_MENU_INDEX,
    ABILITY_MENU_INDEX,
    POTION_MENU_INDEX,
    //format options
    ABILITY_FORMAT_INDEX,
    POTION_FORMAT_INDEX,
    //options main
    COMBAT_MENU_OPTION1_INDEX,
    COMBAT_MENU_OPTION2_INDEX,
    //menu tail message
    TAIL_MSG_INDEX,
    //combat messages
    EMPTY_POTION_INVENTORY_INDEX,
    ATTACK_SUCCESS_INDEX,
    ATTACK_MISS_INDEX,
    ATTACK_FAIL_INDEX,
    POTION_USE_INDEX,

    MAX_COMBAT_LOCAL_STRINGS
};

//main menu
static const local_key_t main_menu_title = {MAIN_MENU_INDEX, "COMBAT.MAIN.MENU.HEAD"};
static const local_key_t main_menu_option1 = {COMBAT_MENU_OPTION1_INDEX, "COMBAT.MAIN.MENU.OPTION1"};
static const local_key_t main_menu_option2 = {COMBAT_MENU_OPTION2_INDEX, "COMBAT.MAIN.MENU.OPTION2"};

//ability menu
static const local_key_t ability_menu_title = {ABILITY_MENU_INDEX, "COMBAT.ABILITY.MENU.HEAD"};
static const local_key_t ability_menu_option_format = {ABILITY_FORMAT_INDEX, "COMBAT.ABILITY.MENU.OPTION_FORMAT"};

//potion menu
static const local_key_t potion_menu_title = {POTION_MENU_INDEX, "COMBAT.POTION.MENU.HEAD"};
static const local_key_t potion_menu_option_format = {POTION_FORMAT_INDEX, "COMBAT.POTION.MENU.OPTION_FORMAT"};

//tail message
static const local_key_t menu_tail_message = {TAIL_MSG_INDEX, "COMBAT.ALL.MENU.TAIL"};

//combat messages
static const local_key_t empty_potion_inventory = {EMPTY_POTION_INVENTORY_INDEX, "COMBAT.POTION.INVENTORY.EMPTY"};
static const local_key_t attack_success = {ATTACK_SUCCESS_INDEX, "COMBAT.ATTACK.SUCCESS.FORMAT"};
static const local_key_t attack_miss = {ATTACK_MISS_INDEX, "COMBAT.ATTACK.MISS.FORMAT"};
static const local_key_t attack_fail = {ATTACK_FAIL_INDEX, "COMBAT.ATTACK.FAIL.FORMAT"};
static const local_key_t potion_use = {POTION_USE_INDEX, "COMBAT.POTION.USE.FORMAT"};
#endif//COMBAT_MODE_LOCAL_H
