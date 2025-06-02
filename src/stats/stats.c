/**
 * @file stats.c
 * @brief Implementation for stats.
 */
#include "stats.h"

#include "../logging/logger.h"


void raise_skill(stats_t* stats, stat_type_t stat, int skillpoint) {
    if (stats == NULL) {
        log_msg(ERROR, "Stats", "Stats pointer is NULL");
        return;
    }
    if (skillpoint > 0) {
        //skillpoint--; does not work because it is passed by value
        switch (stat) {
            case STRENGTH:
                stats->strength++;
                break;
            case INTELLIGENCE:
                stats->intelligence++;
                break;
            case DEXTERITY:
                stats->dexterity++;
                break;
            case CONSTITUTION:
                stats->constitution++;
                break;
            default:
                log_msg(ERROR, "Stats", "Invalid stat type");
                break;
        }
    } else {
        log_msg(ERROR, "Stats", "Not enough skill points to allocate");
        return;
    }
}
