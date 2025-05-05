//
// Created by Nghia Dao on 26/04/2025.
//

#ifndef STATS_MODE_H
#define STATS_MODE_H
#include "../character/character.h"
#include "stats.h"

typedef enum {
    STATS_WINDOW,
    STATS_EXIT
} stats_result_t;
void stats_mode(character_t* player);


#endif//STATS_MODE_H
