#ifndef LOCAL_STRINGS_H
#define LOCAL_STRINGS_H

#define MAX_COMO_MAIN_MENU_OPTION 2
#define MAX_INMO_MAIN_MENU_OPTION 2

typedef struct {
    int idx;
    char* key;
} local_key_t;

/**
 * Every Enum Starts with four different starting characters, based on the module they are used in.
 * 'MAME' = main menu
 * 'SAME' = save menu
 * 'LAME' = language menu
 * 'MAMO' = map mode
 * 'COMO' = combat mode
 */
enum local_string_index {
    //main menu index
    MAME_NEW_GAME_OPTION,
    MAME_CONTINUE_OPTION,
    MAME_SAVE_GAME_OPTION,
    MAME_LOAD_GAME_OPTION,
    MAME_CHANGE_LANGUAGE_OPTION,
    MAME_EXIT_OPTION,
    MAME_CONFIRMATION_CONTINUE,
    MAME_CONFIRMATION_EXIT,

    //language menu index
    LAME_LANGUAGE_ENGLISH,
    LAME_LANGUAGE_GERMAN,

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

    //inventory mode index
    INMO_MAIN_MENU_TITLE,
    INMO_INVENTORY_MENU_TITLE,
    INMO_EQUIPMENT_MENU_TITLE,
    INMO_MAIN_MENU_OPTION1,
    INMO_MAIN_MENU_OPTION2,
    INMO_INVENTORY_FORMAT,
    INMO_EQUIPMENT_FORMAT,
    INMO_EQUIPMENT_FORMAT_EMPTY,
    INMO_INVENTORY_HEADER_MESSAGE,
    INMO_EQUIPMENT_HEADER_MESSAGE,
    INMO_SUBMENU_KEY_MESSAGE,
    INMO_NO_MORE_GEAR,
    INMO_NO_MORE_GEAR_SLOT,
    INMO_NO_FREE_EQUIPMENT_SLOT,

    //loot mode index
    LOMO_MAIN_MENU_TITLE,
    LOMO_MAIN_MENU_OPTION1,
    LOMO_MAIN_MENU_OPTION2,
    LOMO_INVENTORY_HEADER_MESSAGE,
    LOMO_EQUIPMENT_HEADER_MESSAGE,
    LOMO_SUBMENU_TAIL_MESSAGE,

    MAX_LOCAL_STRINGS
};

//main menu strings
static const local_key_t mame_new_game_option = {MAME_NEW_GAME_OPTION, "MAIN.MENU.OPTION.NEW_GAME"};
static const local_key_t mame_continue_option = {MAME_CONTINUE_OPTION, "MAIN.MENU.OPTION.CONTINUE"};
static const local_key_t mame_save_game_option = {MAME_SAVE_GAME_OPTION, "MAIN.MENU.OPTION.SAVE_GAME"};
static const local_key_t mame_load_game_option = {MAME_LOAD_GAME_OPTION, "MAIN.MENU.OPTION.LOAD_GAME"};
static const local_key_t mame_change_language_option = {MAME_CHANGE_LANGUAGE_OPTION, "MAIN.MENU.OPTION.CHANGE_LANGUAGE"};
static const local_key_t mame_exit_option = {MAME_EXIT_OPTION, "MAIN.MENU.OPTION.EXIT"};
static const local_key_t mame_confirm_continue = {MAME_CONFIRMATION_CONTINUE, "MAIN.MENU.CONFIRMATION.CONTINUE"};
static const local_key_t mame_confirm_exit = {MAME_CONFIRMATION_EXIT, "MAIN.MENU.CONFIRMATION.EXIT"};

//language menu strings
static const local_key_t lame_language_english = {LAME_LANGUAGE_ENGLISH, "LANGUAGE.MENU.OPTION.ENGLISH"};
static const local_key_t lame_language_german = {LAME_LANGUAGE_GERMAN, "LANGUAGE.MENU.OPTION.GERMAN"};

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

//inventory mode strings
static const local_key_t inmo_main_menu_title = {INMO_MAIN_MENU_TITLE, "INVENTORY.MAIN.MENU.HEAD"};
static const local_key_t inmo_inventory_menu_title = {INMO_INVENTORY_MENU_TITLE, "INVENTORY.INVENTORY.MENU.HEAD"};
static const local_key_t inmo_equipment_menu_title = {INMO_EQUIPMENT_MENU_TITLE, "INVENTORY.EQUIPMENT.MENU.HEAD"};
static const local_key_t inmo_main_menu_option1 = {INMO_MAIN_MENU_OPTION1, "INVENTORY.MAIN.MENU.OPTION1"};
static const local_key_t inmo_main_menu_option2 = {INMO_MAIN_MENU_OPTION2, "INVENTORY.MAIN.MENU.OPTION2"};
static const local_key_t inmo_inventory_format = {INMO_INVENTORY_FORMAT, "INVENTORY.INVENTORY.MENU.OPTION_FORMAT"};
static const local_key_t inmo_equipment_format = {INMO_EQUIPMENT_FORMAT, "INVENTORY.EQUIPMENT.MENU.OPTION_FORMAT"};
static const local_key_t inmo_equipment_format_empty = {INMO_EQUIPMENT_FORMAT_EMPTY, "INVENTORY.EQUIPMENT.MENU.OPTION_FORMAT_EMPTY"};
static const local_key_t inmo_inventory_header_message = {INMO_INVENTORY_HEADER_MESSAGE, "INVENTORY.INVENTORY.HEAD.MESSAGE"};
static const local_key_t inmo_equipment_header_message = {INMO_EQUIPMENT_HEADER_MESSAGE, "INVENTORY.EQUIPMENT.HEAD.MESSAGE"};
static const local_key_t inmo_submenu_key_message = {INMO_SUBMENU_KEY_MESSAGE, "INVENTORY.SUB.MENU.MESSAGE"};
static const local_key_t inmo_no_more_gear = {INMO_NO_MORE_GEAR, "INVENTORY.INVENTORY.INVENTORY.EMPTY"};
static const local_key_t inmo_no_more_gear_slot = {INMO_NO_MORE_GEAR_SLOT, "INVENTORY.INVENTORY.INVENTORY.FULL"};
static const local_key_t inmo_no_free_equipment_slot = {INMO_NO_FREE_EQUIPMENT_SLOT, "INVENTORY.EQUIPMENT.INVENTORY.FULL"};

//loot mode strings
static const local_key_t lomo_main_menu_title = {LOMO_MAIN_MENU_TITLE, "LOOT.MAIN.MENU.HEAD"};
static const local_key_t lomo_main_menu_option1 = {LOMO_MAIN_MENU_OPTION1, "LOOT.MAIN.MENU.OPTION1"};
static const local_key_t lomo_main_menu_option2 = {LOMO_MAIN_MENU_OPTION2, "LOOT.MAIN.MENU.OPTION2"};
static const local_key_t lomo_inventory_header_message = {LOMO_INVENTORY_HEADER_MESSAGE, "LOOT.INVENTORY.HEAD.MESSAGE"};
static const local_key_t lomo_equipment_header_message = {LOMO_EQUIPMENT_HEADER_MESSAGE, "LOOT.EQUIPMENT.HEAD.MESSAGE"};
static const local_key_t lomo_submenu_tail_message = {LOMO_SUBMENU_TAIL_MESSAGE, "LOOT.SUB.MENU.TAIL"};

#endif//LOCAL_STRINGS_H
