#include "stats_mode_local.h"

#include "../../local/local_handler.h"

#include <stdlib.h>

char** stats_mode_strings = NULL;

void update_stats_local(void) {
    for (int i = 0; i < MAX_STATS_STRINGS; i++) {
        if (stats_mode_strings[i] != NULL) {
            free(stats_mode_strings[i]);
        }
    }

    stats_mode_strings[PLAYER_MENU_TITLE] = get_local_string("STATS.PLAYER.MENU.TITLE");
    stats_mode_strings[STATS_MENU_TITLE] = get_local_string("STATS.STATS.MENU.TITLE");
    stats_mode_strings[INVENTORY_MENU_TITLE] = get_local_string("STATS.INVENTORY.MENU.TITLE");
    stats_mode_strings[HEALTH_STR] = get_local_string("HEALTH");
    stats_mode_strings[STAMINA_STR] = get_local_string("STAMINA");
    stats_mode_strings[MANA_STR] = get_local_string("MANA");
    stats_mode_strings[STRENGTH_STR] = get_local_string("STRENGTH");
    stats_mode_strings[INTELLIGENCE_STR] = get_local_string("INTELLIGENCE");
    stats_mode_strings[DEXTERITY_STR] = get_local_string("DEXTERITY");
    stats_mode_strings[CONSTITUTION_STR] = get_local_string("CONSTITUTION");
    stats_mode_strings[LEVEL_STR] = get_local_string("LEVEL");
    stats_mode_strings[EXP_STR] = get_local_string("EXP_POINTS");
    stats_mode_strings[ARMOR_STR] = get_local_string("ARMOR");
    stats_mode_strings[MAGIC_RESISTANCE_STR] = get_local_string("MAGIC_RESISTANCE");
    stats_mode_strings[AVAILABLE_SKILL_POINTS_STR] = get_local_string("STATS.AV.SKILL_P");
    stats_mode_strings[EQUIPPED_ARMOR_STR] = get_local_string("STATS.EQUIPPED.ARMOR");
    stats_mode_strings[EMPTY_ARMOR_SLOT_STR] = get_local_string("STATS.EMPTY.ARMOR.SLOT");
}


