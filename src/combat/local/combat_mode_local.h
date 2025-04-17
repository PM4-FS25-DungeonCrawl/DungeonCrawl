#ifndef COMBAT_MODE_LOCAL_H
#define COMBAT_MODE_LOCAL_H


enum menu_titles {
    MAIN_MENU_INDEX,
    ABILITY_MENU_INDEX,
    POTION_MENU_INDEX,
    MAX_MENU_TITLES
};

enum option_formats {
    ABILITY_FORMAT_INDEX,
    POTION_FORMAT_INDEX,
    MAX_OPTION_FORMATS
};

enum combat_menu_options {
    COMBAT_MENU_OPTION1,
    COMBAT_MENU_OPTION2,
    MAX_COMBAT_MENU_OPTIONS
};

typedef struct {
    int idx;
    char* key;
} local_key_t;

//main menu
const local_key_t main_menu_title = {MAIN_MENU_INDEX, "COMBAT.MAIN.MENU.HEAD"};
const local_key_t main_menu_option1 = {COMBAT_MENU_OPTION1, "COMBAT.MAIN.MENU.OPTION1"};
const local_key_t main_menu_option2 = {COMBAT_MENU_OPTION2, "COMBAT.MAIN.MENU.OPTION2"};

//ability menu
const local_key_t ability_menu_title = {ABILITY_MENU_INDEX, "COMBAT.ABILITY.MENU.HEAD"};
const local_key_t ability_menu_option_format = {ABILITY_FORMAT_INDEX, "COMBAT.ABILITY.MENU.OPTION_FORMAT"};

//potion menu
const local_key_t potion_menu_title = {POTION_MENU_INDEX, "COMBAT.POTION.MENU.HEAD"};
const local_key_t potion_menu_option_format = {POTION_FORMAT_INDEX, "COMBAT.POTION.MENU.OPTION_FORMAT"};

const local_key_t menu_tail_message = {0, "COMBAT.ALL.MENU.TAIL"};


#endif //COMBAT_MODE_LOCAL_H
