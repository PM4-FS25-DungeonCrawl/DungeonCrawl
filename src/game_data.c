#include "game_data.h"

#include "character/monster.h"
#include "character/player.h"
#include "common.h"

#include <stddef.h>

// === External Global Variables ===
ability_table_t* ability_table;
character_t* goblin;
character_t* player;
potion_t* healing_potion;

int init_game_data(memory_pool_t* memory_pool) {
    NULL_PTR_HANDLER_RETURN(memory_pool, 1, "Game Data", "Memory pool is NULL");

    ability_table = init_ability_table(memory_pool);
    player = create_new_player(memory_pool);//initialize blank player
    healing_potion = init_potion(memory_pool, "Healing Potion", HEALING, 20);

    reset_goblin(memory_pool);

    if (ability_table == NULL || player == NULL || healing_potion == NULL) return 1;
    // add abilities to player
    add_ability(player, &ability_table->abilities[FIREBALL]);
    add_ability(player, &ability_table->abilities[SWORD_SLASH]);
    //add healing potion to a player
    add_potion(player, healing_potion);
    return 0;
}

int free_game_data(memory_pool_t* memory_pool) {
    free_ability_table(memory_pool, ability_table);
    free_character(memory_pool, player);
    free_character(memory_pool, goblin);
    free_potion(memory_pool, healing_potion);
    return 0;
}

int reset_goblin(memory_pool_t* memory_pool) {
    NULL_PTR_HANDLER_RETURN(memory_pool, 1, "Game Data", "Game Data was not initialized");
    memory_pool_free(memory_pool, goblin);
    goblin = create_new_goblin(memory_pool);
    if (goblin == NULL) {
        return 1;
    }
    add_ability(goblin, &ability_table->abilities[BITE]);
    return 0;
}