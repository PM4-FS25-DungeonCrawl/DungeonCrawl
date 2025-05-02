#include "stats.h"
#include "../logging/logger.h"


void raise_skill(stats_t* stats, int stat, int value, int skillpoint) {
    if (stats == NULL) {
        log_msg(ERROR, "Stats", "Stats pointer is NULL");
        return;
    }
    if (value <= skillpoint) {
        skillpoint -= value;
        switch (stat) {
            case 0:
                stats->strength += value;
                break;
            case 1:
                stats->intelligence += value;
                break;
            case 2:
                stats->dexterity += value;
                break;
            case 3:
                stats->constitution += value;
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
