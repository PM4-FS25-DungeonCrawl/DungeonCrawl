#ifndef COMBAT_MODE_LOCAL_H
#define COMBAT_MODE_LOCAL_H

enum combat_mode_index {
    MAIN_MENU_TITLE,
    ABILITY_MENU_TITLE,
    POTION_MENU_TITLE,
    MAIN_MENU_OPTION1,
    MAIN_MENU_OPTION2,
    ABILITY_FORMAT,
    POTION_FORMAT,
    PRESS_ESC_RETURN,
    NO_MORE_POTIONS,
    ATTACK_SUCCESS,
    ATTACK_MISS,
    ATTACK_FAIL,
    POTION_USE,

    MAX_COMBAT_MODE_STRINGS
};


extern char** combat_mode_strings;

void update_combat_local(void);

#endif//COMBAT_MODE_LOCAL_H
