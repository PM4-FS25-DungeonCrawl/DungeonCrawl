/**
 * @file stats_mode.h
 * @brief Exposes functions and enums for the stats_mode.
 */
#ifndef STATS_MODE_H
#define STATS_MODE_H
#include "../character/character.h"
#include "stats.h"

typedef enum {
    STATS_WINDOW,
    STATS_EXIT
} stats_result_t;

/**
 * @brief Enter the stat mode for a given character
 *
 * @param player Pointer to the player character.
 * @return A stats_result_t enum of the next state.
 */
stats_result_t stats_mode(character_t* player);


#endif//STATS_MODE_H
