#include <stdlib.h>

#include "player.h"

character_t* create_new_player(void) {
    character_t* player = init_character(PLAYER, "Hero");
    NULL_PTR_HANDLER(player, "Player", "Failed to allocate memory for player");

    set_character_stats(player, 5, 5, 5, 5);
    return player;
}
