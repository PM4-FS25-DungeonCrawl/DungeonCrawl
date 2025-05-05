#include "stats.h"

#include "../logging/logger.h"


void raise_skill(stats_t* stats, stat_type_t stat, int skillpoint) {
    if (stats == NULL) {
        log_msg(ERROR, "Stats", "Stats pointer is NULL");
        return;
    }
    if (1 <= skillpoint) {
        skillpoint--;
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
    log_msg(INFO, "Stats", "Stats set to STR:%d INT:%d DEX:%d CON:%d",
            stats->strength, stats->intelligence, stats->dexterity, stats->constitution);
}
