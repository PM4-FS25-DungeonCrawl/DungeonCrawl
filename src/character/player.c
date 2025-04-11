#include "player.h"

#include <stdlib.h>

character_t* create_new_player(void) {
    character_t* player = init_character(PLAYER, "Hero");
    NULL_PTR_HANDLER(player, "Player", "Failed to allocate memory for player");

    set_character_stats(player, 5, 5, 5, 5);
    set_initial_xp(player, 0);
    set_level(player, 1);
    return player;
}
