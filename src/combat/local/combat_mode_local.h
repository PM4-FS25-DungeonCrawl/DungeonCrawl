/**
 * @file combat_mode_local.h
 * @brief Exposes functions for working with the localization of the combat mode.
 */
#ifndef COMBAT_MODE_LOCAL_H
#define COMBAT_MODE_LOCAL_H

enum combat_mode_index {
    COMBAT_MODE_TITLE,
    MAIN_MENU_TITLE,
    ABILITY_MENU_TITLE,
    POTION_MENU_TITLE,
    MAIN_MENU_OPTION1,
    MAIN_MENU_OPTION2,
    ABILITY_FORMAT,
    POTION_FORMAT,
    PRESS_C_RETURN,
    NO_MORE_POTIONS,
    ATTACK_SUCCESS,
    ATTACK_MISS,
    ATTACK_FAIL,
    POTION_USE,

    HEALTH_STR,
    MANA_STR,
    STAMINA_STR,

    WON_COMBAT_MSG1,// part 1 of the message when combat is won
    WON_COMBAT_MSG2,// part 2 of the message when combat is won
    LOST_COMBAT_MSG,

    PRESS_ANY_CONTINUE,
    PRESS_ANY_EXIT,

    MAX_COMBAT_MODE_STRINGS
};


extern char** combat_mode_strings;

/**
 * @brief Updates the combat mode strings with localized versions.
 * @note This function should be called after initializing the combat mode.
 */
void update_combat_local(void);

#endif//COMBAT_MODE_LOCAL_H
