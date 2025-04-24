#ifndef LOCAL_STRINGS_H
#define LOCAL_STRINGS_H

#define MAX_COMO_MAIN_MENU_OPTION 2

typedef struct {
    int idx;
    char* key;
} local_key_t;

/**
 * Every Enum Starts with four different starting characters, based on the module they are used in.
 * 'MAME' = main menu
 * 'SAME' = save menu
 * 'MAMO' = map mode
 * 'COMO' = combat mode
 */
enum local_string_index {
    //combat mode index
    COMO_MAIN_MENU_TITLE,
    COMO_ABILITY_MENU_TITLE,
    COMO_POTION_MENU_TITLE,
    COMO_MAIN_MENU_OPTION1,
    COMO_MAIN_MENU_OPTION2,
    COMO_ABILITY_FORMAT,
    COMO_POTION_FORMAT,
    COMO_SUBMENU_TAIL_MESSAGE,
    COMO_NO_MORE_POTIONS,
    COMO_ATTACK_SUCCESS,
    COMO_ATTACK_MISS,
    COMO_ATTACK_FAIL,
    COMO_POTION_USE,

    MAX_LOCAL_STRINGS
};

//combat mode strings
static const local_key_t como_main_menu_title = {COMO_MAIN_MENU_TITLE, "COMBAT.MAIN.MENU.HEAD"};
static const local_key_t como_ability_menu_title = {COMO_ABILITY_MENU_TITLE, "COMBAT.ABILITY.MENU.HEAD"};
static const local_key_t como_potion_menu_title = {COMO_POTION_MENU_TITLE, "COMBAT.POTION.MENU.HEAD"};
static const local_key_t como_main_menu_option1 = {COMO_MAIN_MENU_OPTION1, "COMBAT.MAIN.MENU.OPTION1"};
static const local_key_t como_main_menu_option2 = {COMO_MAIN_MENU_OPTION2, "COMBAT.MAIN.MENU.OPTION2"};
static const local_key_t como_ability_format = {COMO_ABILITY_FORMAT, "COMBAT.ABILITY.MENU.OPTION_FORMAT"};
static const local_key_t como_potion_format = {COMO_POTION_FORMAT, "COMBAT.POTION.MENU.OPTION_FORMAT"};
static const local_key_t como_submenu_tail_message = {COMO_SUBMENU_TAIL_MESSAGE, "COMBAT.SUB.MENU.TAIL"};
static const local_key_t como_no_more_potions = {COMO_NO_MORE_POTIONS, "COMBAT.POTION.INVENTORY.EMPTY"};
static const local_key_t como_attack_success = {COMO_ATTACK_SUCCESS, "COMBAT.ATTACK.SUCCESS.FORMAT"};
static const local_key_t como_attack_miss = {COMO_ATTACK_MISS, "COMBAT.ATTACK.MISS.FORMAT"};
static const local_key_t como_attack_fail = {COMO_ATTACK_FAIL, "COMBAT.ATTACK.FAIL.FORMAT"};
static const local_key_t como_potion_use = {COMO_POTION_USE, "COMBAT.POTION.USE.FORMAT"};

#endif //LOCAL_STRINGS_H
