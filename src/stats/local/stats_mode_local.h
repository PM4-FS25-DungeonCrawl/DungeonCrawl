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

/**
 * Updates the localized strings used in the stats menu.
 *
 * This function refreshes the strings used to display various statistics
 * or menu titles by retrieving their updated localized values. It first frees
 * any previously allocated strings in the `stats_mode_strings` array and then
 * populates the array with fresh localized strings retrieved from the
 * `get_local_string` function.
 *
 * @note The function assumes that the `stats_mode_strings` array has
 *       sufficient capacity to hold all the required strings (defined by
 *       `MAX_STATS_STRINGS`). It also assumes that indices into the array
 *       correspond to specific keys for localization.
 */
void update_stats_local(void);

#endif//STATS_MODE_LOCAL_H
