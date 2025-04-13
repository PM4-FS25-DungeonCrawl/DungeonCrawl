#ifndef COMBAT_MODE_H
#define COMBAT_MODE_H

#include "../character/character_fw.h"

typedef enum {
    PLAYER_WON,
    PLAYER_LOST,
    EXIT_GAME
} combat_result_t;

void init_combat_mode(void);
combat_result_t start_combat(character_t* player, character_t* monster);
void shutdown_combat_mode(void);

#endif//COMBAT_MODE_H
