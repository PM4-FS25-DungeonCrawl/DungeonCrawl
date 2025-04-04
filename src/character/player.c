#include <stdlib.h>

#include "player.h"
#include "../logging/logger.h"

character_t* create_new_player(void) {
    character_t* player = init_character(PLAYER, "Hero");
    if (player == NULL) {
        // memory for player could not be allocated
        log_msg(ERROR, "Player", "Failed to allocate memory for player");
        return NULL;
    }

    set_character_stats(player, 100, 10, 5, 5, 5, 5);
    return player;
}
