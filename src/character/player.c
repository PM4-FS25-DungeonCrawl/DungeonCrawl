/**
 * @file player.c
 * @brief This file contains functions for creating and initializing player characters
 */

#include "player.h"

#include "../local/local_handler.h"

character_t* create_new_player(memory_pool_t* memory_pool) {
    NULL_PTR_HANDLER_RETURN(memory_pool, NULL, "Player", "Memory pool is NULL");

    char* player_name = get_local_string("PLAYER.DEFAULT.NAME");
    character_t* player = init_character(memory_pool, PLAYER, player_name);
    free(player_name);
    NULL_PTR_HANDLER_RETURN(player, NULL, "Player", "Failed to allocate memory for player");

    set_character_stats(player, 5, 5, 5, 10);
    set_initial_xp(player, 0);
    set_level(player, 1);
    set_skill_points(player, 5);
    return player;
}
