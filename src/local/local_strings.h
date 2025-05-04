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
 * 'LAME' = language menu
 * 'MAMO' = map mode
 * 'COMO' = combat mode
 * 'STMO' = stats mode
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

    //save menu index

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

    //stats mode index
    STMO_MAIN_MENU_TITLE,
    STMO_ABILITY_HP,
    STMO_ABILITY_MP,
    STMO_ABILITY_SP,
    STMO_ABILITY_STRENGTH,
    STMO_ABILITY_INTELLIGENCE,
    STMO_ABILITY_DEXTERITY,
    STMO_ABILITY_CONSTITUTION,
    STMO_PLAYER_LEVEL,
    STMO_PLAYER_XP,
    STMO_PLAYER_ARMOR,
    STMO_PLAYER_MAGIC_RESIST,
    STMO_OPTION_SKILLPOINTS,
    STMO_OPTION_INVENTORY,
    STMO_OPTION_NO_ARMOR,
    STMO_MENU_STATS_TITLE,

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

//stats mode strings
static const local_key_t stmo_main_menu_title = {STMO_MAIN_MENU_TITLE, "STATS.MENU.TITLE.HEAD"};
static const local_key_t stmo_ability_hp = {STMO_ABILITY_HP, "STATS.MENU.TITLE.HEALTH"};
static const local_key_t stmo_ability_mp = {STMO_ABILITY_MP, "STATS.MENU.TITLE.MANA"};
static const local_key_t stmo_ability_sp = {STMO_ABILITY_SP, "STATS.MENU.TITLE.STAMINA"};
static const local_key_t stmo_ability_strength = {STMO_ABILITY_STRENGTH, "STATS.MENU.TITLE.STRENGTH"};
static const local_key_t stmo_ability_intelligence = {STMO_ABILITY_INTELLIGENCE, "STATS.MENU.TITLE.INTELLIGENCE"};
static const local_key_t stmo_ability_dexterity = {STMO_ABILITY_DEXTERITY, "STATS.MENU.TITLE.DEXTERITY"};
static const local_key_t stmo_ability_constitution = {STMO_ABILITY_CONSTITUTION, "STATS.MENU.TITLE.CONSTITUTION"};
static const local_key_t stmo_player_level = {STMO_PLAYER_LEVEL, "STATS.MENU.TITLE.LEVEL"};
static const local_key_t stmo_player_xp = {STMO_PLAYER_XP, "STATS.MENU.TITLE.XP"};
static const local_key_t stmo_player_armor = {STMO_PLAYER_ARMOR, "STATS.MENU.TITLE.ARMOR"};
static const local_key_t stmo_player_magic_resist = {STMO_PLAYER_MAGIC_RESIST, "STATS.MENU.TITLE.MAGIC_RESIST"};
static const local_key_t stmo_option_skillpoints = {STMO_OPTION_SKILLPOINTS, "STATS.MENU.OPTION_FORMAT.LEVEL"};
static const local_key_t stmo_option_inventory = {STMO_OPTION_INVENTORY, "STATS.MENU.OPTION.ARMOR"};
static const local_key_t stmo_option_no_armor = {STMO_OPTION_NO_ARMOR, "STATS.MENU.OPTION2.ARMOR"};
static const local_key_t stmo_menu_stats_title = {STMO_MENU_STATS_TITLE, "STATS.MAIN.MENU.STATS"};

#endif//LOCAL_STRINGS_H
