#ifndef STATS_MODE_LOCAL_H
#define STATS_MODE_LOCAL_H

enum stats_mode_index {
    //strings that are updated via local
    PLAYER_MENU_TITLE,
    STATS_MENU_TITLE,
    INVENTORY_MENU_TITLE,
    HEALTH_STR,
    STAMINA_STR,
    MANA_STR,
    STRENGTH_STR,
    INTELLIGENCE_STR,
    DEXTERITY_STR,
    CONSTITUTION_STR,
    LEVEL_STR,
    EXP_STR,
    ARMOR_STR,
    MAGIC_RESISTANCE_STR,
    AVAILABLE_SKILL_POINTS_STR,
    EQUIPPED_ARMOR_STR,
    EMPTY_ARMOR_SLOT_STR,

    MAX_STATS_STRINGS
};

extern char** stats_mode_strings;

void update_stats_local(void);

#endif//STATS_MODE_LOCAL_H
